#include <atomic>
#include <climits>
#include <cstdint>
#include <mutex>
#include <utility>
#include <vector>
#include <thread>
#include <functional>
#include <algorithm>
#include <cassert>

// -----------------------------
//   Реализация HP 
// -----------------------------
namespace hp {

inline constexpr int  kMaxThreads        = 128; 
inline constexpr int  kPerThreadSlots    = 2;   
inline constexpr size_t kReclaimThreshold = 64; 

// Глобальная таблица Hazard Pointers
static std::atomic<void*> g_hp[kMaxThreads * kPerThreadSlots];

// Выдача tid для текущего потока
inline int acquire_tid() {
    static std::atomic<int> next{0};
    thread_local int tid = [] {
        int id = next.fetch_add(1, std::memory_order_relaxed);
        assert(id < kMaxThreads && "Too many threads for hazard pointers");
        return id;
    }();
    return tid;
}

// Очистка HP-слотов потока
inline void clear_thread_hp(int tid) {
    for (int s = 0; s < kPerThreadSlots; ++s) {
        g_hp[tid * kPerThreadSlots + s].store(nullptr, std::memory_order_release);
    }
}

// Узел в списке "retired"
struct retired_record {
    void* ptr;
    void (*deleter)(void*);
};

// Пер-нитевой retired-список
inline std::vector<retired_record>& retired_list() {
    thread_local std::vector<retired_record> list;
    return list;
}

// Удалитель по умолчанию
template <class T>
inline void default_delete(void* p) { delete static_cast<T*>(p); }

// Пометить объект на утилизацию
template <class T>
inline void retire(T* p, void (*del)(void*) = &default_delete<T>) {
    if (!p) return;
    auto& rl = retired_list();
    rl.push_back({p, del});
    if (rl.size() >= kReclaimThreshold) {
        // 1) снимок всех HP
        std::vector<void*> hazards;
        hazards.reserve(kMaxThreads * kPerThreadSlots);
        for (int i = 0; i < kMaxThreads * kPerThreadSlots; ++i) {
            if (void* h = g_hp[i].load(std::memory_order_acquire)) hazards.push_back(h);
        }
        // 2) удалить те, кого нет в hazards
        auto it = rl.begin();
        while (it != rl.end()) {
            if (std::find(hazards.begin(), hazards.end(), it->ptr) == hazards.end()) {
                it->deleter(it->ptr);
                it = rl.erase(it);
            } else {
                ++it;
            }
        }
    }
}

// Финальная зачистка
inline void drain_all() {
    bool progress;
    int spins = 0;
    do {
        progress = false;

        std::vector<void*> hazards;
        hazards.reserve(kMaxThreads * kPerThreadSlots);
        for (int i = 0; i < kMaxThreads * kPerThreadSlots; ++i) {
            if (void* h = g_hp[i].load(std::memory_order_acquire)) hazards.push_back(h);
        }

        auto& rl = retired_list();
        auto it = rl.begin();
        while (it != rl.end()) {
            if (std::find(hazards.begin(), hazards.end(), it->ptr) == hazards.end()) {
                it->deleter(it->ptr);
                it = rl.erase(it);
                progress = true;
            } else {
                ++it;
            }
        }
    } while (progress && ++spins <= 8);
}

// Установка/сброс HP слота
inline void set(int slot, void* p) {
    int tid = acquire_tid();
    assert(slot >= 0 && slot < kPerThreadSlots);
    g_hp[tid * kPerThreadSlots + slot].store(p, std::memory_order_release);
}
inline void clear(int slot) {
    int tid = acquire_tid();
    assert(slot >= 0 && slot < kPerThreadSlots);
    g_hp[tid * kPerThreadSlots + slot].store(nullptr, std::memory_order_release);
}

// RAII 
struct Guard {
    int slot;
    explicit Guard(int s) : slot(s) {
        (void)acquire_tid(); // регистрация при первом обращении
        clear(slot);
    }
    void set(void* p) { hp::set(slot, p); }
    ~Guard() { clear(slot); }
};

} // namespace hp


// -----------------------------
//   CoarseListSet 
// -----------------------------
class CoarseListSet {
    struct Node {
        int   key;
        Node* next;
        explicit Node(int k, Node* n = nullptr) : key(k), next(n) {}
    };

    Node* head_;                 // sentinel: -∞
    Node* tail_;                 // sentinel: +∞
    mutable std::mutex m_;       // один мьютекс на всю структуру

public:
    CoarseListSet() {
        tail_ = new Node(INT_MAX, nullptr);
        head_ = new Node(INT_MIN, tail_);
    }

    ~CoarseListSet() {
        // разбор списка (параллельных операций тут уже нет)
        Node* p = head_;
        while (p) {
            Node* n = p->next;
            delete p;
            p = n;
        }
    }

    bool find(int key) const {
        std::lock_guard<std::mutex> lk(m_);
        Node* curr = head_;
        while (curr->key < key) {
            curr = curr->next;
        }
        return curr->key == key;
    }

    bool push(int key) {
        std::lock_guard<std::mutex> lk(m_);
        Node* pred = head_;
        Node* curr = pred->next;
        while (curr->key < key) {
            pred = curr;
            curr = curr->next;
        }
        if (curr->key == key) return false;        // уже есть
        Node* node = new Node(key, curr);          // вставка между pred и curr
        pred->next = node;
        return true;
    }

    bool remove(int key) {
        std::lock_guard<std::mutex> lk(m_);
        Node* pred = head_;
        Node* curr = pred->next;
        while (curr->key < key) {
            pred = curr;
            curr = curr->next;
        }
        if (curr->key != key) return false;        // нет такого ключа
        pred->next = curr->next;                   // «перешиваем» ссылки
        hp::retire(curr);
        return true;
    }
};


// -----------------------------
//   LockFreeListSet + HP
// -----------------------------
class LockFreeListSet {
    struct Node {
        int key;
        std::atomic<Node*> next; // младший бит — mark (логически удалён)
        explicit Node(int k, Node* n=nullptr) : key(k), next(n) {}
    };

    Node* head_;
    Node* tail_;

    // Помощники для «помеченных» указателей (используем младший бит)
    static Node* mark(Node* p) {
        return reinterpret_cast<Node*>(reinterpret_cast<uintptr_t>(p) | 1u);
    }
    static Node* unmark(Node* p) {
        return reinterpret_cast<Node*>(reinterpret_cast<uintptr_t>(p) & ~uintptr_t(1));
    }
    static bool  is_marked(Node* p) {
        return (reinterpret_cast<uintptr_t>(p) & 1u) != 0;
    }

    // Lock-free find: возвращает (pred, curr) такие, что curr->key >= key,
    // curr не помечен; по пути best-effort вырезаем помеченные.
    // Важно: защищаем pred/curr HP-указателями чтобы их не утилизировали.
    std::pair<Node*, Node*> find_window(int key) {
        hp::Guard gp(0); // HP слот для pred
        hp::Guard gc(1); // HP слот для curr

    retry:
        Node* pred = head_;
        gp.set(pred);

        Node* cur0 = pred->next.load(std::memory_order_acquire);
        cur0 = unmark(cur0);
        Node* curr = cur0;
        gc.set(curr);

        // Проверим, что pred->next всё ещё тот же (защитили curr — но могли промаркировать/поменять)
        if (unmark(pred->next.load(std::memory_order_acquire)) != curr) {
            // неконсистентность — начнём заново
            goto retry;
        }

        while (true) {
            Node* succ = curr->next.load(std::memory_order_acquire);

            // Удаляем подряд все логически помеченные curr
            while (is_marked(succ)) {
                Node* unmarked_succ = unmark(succ);

                // Пытаемся физически вырезать curr: pred->next CAS(curr -> unmarked_succ)
                Node* expected = curr;
                if (!pred->next.compare_exchange_weak(
                        expected, unmarked_succ,
                        std::memory_order_acq_rel, std::memory_order_acquire)) {
                    // pred изменился — перезапускам
                    goto retry;
                }

                // retire curr (он логически помечен и физически вырезан)
                hp::retire(curr);

                // Двигаемся дальше: curr := unmarked_succ (обязательно защитить HP)
                curr = unmarked_succ;
                gc.set(curr);

                succ = curr->next.load(std::memory_order_acquire);
            }

            if (curr->key >= key) {
                // Окно найдено
                return { pred, curr };
            }

            // Шагаем вперёд: pred := curr; curr := succ
            pred = curr;
            gp.set(pred);
            curr = succ; // здесь succ гарантированно не помечен
            gc.set(curr);

            // Убеждаемся, что между установкой HP и повторной загрузкой ссылки ничего не "изменилось":
            // (легкий best-effort, допускаем перезапуск через goto retry при рассинхронизации)
            Node* check = unmark(pred->next.load(std::memory_order_acquire));
            if (check != curr) goto retry;
        }
    }

public:
    LockFreeListSet() {
        tail_ = new Node(INT_MAX, nullptr);
        head_ = new Node(INT_MIN, tail_);
    }
    ~LockFreeListSet() {
        // В учебных целях: безопасная утилизация вне конкуренции.
        Node* p = head_;
        while (p) {
            Node* n = unmark(p->next.load(std::memory_order_relaxed));
            delete p;
            p = n;
        }
        hp::drain_all();
    }

    bool push(int key) {
        while (true) {
            auto [pred, curr] = find_window(key);
            if (curr->key == key) return false; // уже существует

            Node* node = new Node(key, curr);

            Node* expected = curr;
            if (pred->next.compare_exchange_weak(
                    expected, node,
                    std::memory_order_acq_rel, std::memory_order_acquire)) {
                return true; // вставили между pred и curr
            }
            // иначе кто-то вмешался — повторим
            delete node; // узел ещё не виден никому => можно удалить
        }
    }

    bool remove(int key) {
        while (true) {
            auto [pred, curr] = find_window(key);
            if (curr->key != key) return false;

            // 1) логическая пометка curr->next (mark)
            Node* succ = curr->next.load(std::memory_order_acquire);
            if (!curr->next.compare_exchange_weak(
                    succ, mark(succ),
                    std::memory_order_acq_rel, std::memory_order_acquire)) {
                continue; // не удалось пометить — попробуем снова
            }

            // 2) физическое вырезание (best-effort)
            Node* expected = curr;
            if (pred->next.compare_exchange_strong(
                    expected, succ,
                    std::memory_order_acq_rel, std::memory_order_acquire)) {
                // Полностью вырезан — можно в retire
                hp::retire(curr);
            } else {
                // Кто-то другой уже перешил — всё равно помечен; будет удалён позже
                hp::retire(curr);
            }
            return true;
        }
    }

    bool find(int key) {
        hp::Guard g0(0), g1(1);

        Node* curr = head_;
        g0.set(curr);

        while (true) {
            Node* next = curr->next.load(std::memory_order_acquire);
            Node* un = unmark(next);
            g1.set(un);

            // Проверка согласованности: curr->next мог измениться после установки HP.
            if (unmark(curr->next.load(std::memory_order_acquire)) != un) {
                // рассинхрон — попробуем ещё раз от head
                curr = head_;
                g0.set(curr);
                continue;
            }

            curr = un;
            g0.set(curr); // теперь curr в g0, следующий шаг — защитим следующее звено g1

            if (curr->key >= key) {
                Node* succ = curr->next.load(std::memory_order_acquire);
                bool ok = (curr->key == key) && !is_marked(succ);
                return ok;
            }
        }
    }
};

// -----------------------------
//   FineListSet 
// -----------------------------
class FineListSet {
    struct alignas(64) Node {
        int key;
        std::atomic<Node*> next;
        std::atomic<bool>  marked;   
        std::mutex m;
        explicit Node(int k, Node* n=nullptr)
            : key(k), next(n), marked(false) {}
        void lock()   { m.lock(); }
        void unlock() { m.unlock(); }
    };

    Node* head_;
    Node* tail_;

    static Node* load_unmarked(std::atomic<Node*>& a) {
        return reinterpret_cast<Node*>(
            reinterpret_cast<uintptr_t>(a.load(std::memory_order_acquire)) & ~uintptr_t(1));
    }

    // Оптимистичный поиск окна без замков.
    // Возвращает пару (pred, curr) такие, что curr->key >= key.
    // Помеченные пропускаем по пути.
    std::pair<Node*, Node*> find_window_nolock(int key) {
    retry:
        Node* pred = head_;
        Node* curr = head_->next.load(std::memory_order_acquire);

        // согласовать pred->next==curr (между чтениями могло измениться)
        if (head_->next.load(std::memory_order_acquire) != curr) goto retry;

        while (true) {
            if (!curr) return {pred, tail_}; 

            Node* succ = curr->next.load(std::memory_order_acquire);

            // пропускаем логически удалённые
            if (curr->marked.load(std::memory_order_acquire)) {
                // физически вырезать не пытаемся (это делают писатели под замками)
                curr = succ;
                continue;
            }

            if (curr->key >= key) return {pred, curr};

            pred = curr;
            curr = succ;

            if (pred->next.load(std::memory_order_acquire) != curr) goto retry;
        }
    }

    // Валидация для писателей: pred не помечен, curr не помечен, pred->next == curr.
    static bool validate(Node* pred, Node* curr) {
        if (pred->marked.load(std::memory_order_acquire)) return false;
        if (curr && curr->marked.load(std::memory_order_acquire)) return false;
        return pred->next.load(std::memory_order_acquire) == curr;
    }

public:
    FineListSet() {
        tail_ = new Node(INT_MAX, nullptr);
        head_ = new Node(INT_MIN, tail_);
    }
    ~FineListSet() {
        // вне конкуренции: утилизируем всё, что осталось в списке
        Node* p = head_;
        while (p) {
            Node* n = p->next.load(std::memory_order_relaxed);
            delete p;
            p = n;
        }
        // узлы, которые ранее были удалены во время работы,
        // тут не освобождаются (см. remove)
    }

    bool find(int key) {
        Node* pred = head_;
        Node* curr = pred->next.load(std::memory_order_acquire);

        // согласование стартового шага
        if (head_->next.load(std::memory_order_acquire) != curr) {
            pred = head_;
            curr = pred->next.load(std::memory_order_acquire);
        }

        while (curr->key < key) {
            Node* succ = curr->next.load(std::memory_order_acquire);
            pred = curr;
            curr = succ;
            if (!curr) return false;
        }

        // существует и не помечен
        return (curr->key == key) && !curr->marked.load(std::memory_order_acquire);
    }

    bool push(int key) {
        while (true) {
            auto [pred, curr] = find_window_nolock(key);

            // Лочим в порядке адресов/ключей, чтобы исключить дедлоки
            pred->lock();
            if (curr) curr->lock();

            if (!validate(pred, curr)) {
                if (curr) curr->unlock();
                pred->unlock();
                continue; // retry
            }

            if (curr->key == key) {
                if (curr) curr->unlock();
                pred->unlock();
                return false;
            }

            Node* node = new Node(key, curr);
            // публикация вставки
            pred->next.store(node, std::memory_order_release);

            if (curr) curr->unlock();
            pred->unlock();
            return true;
        }
    }

    bool remove(int key) {
        while (true) {
            auto [pred, curr] = find_window_nolock(key);

            pred->lock();
            if (curr) curr->lock();

            if (!validate(pred, curr)) {
                if (curr) curr->unlock();
                pred->unlock();
                continue; // retry
            }

            if (!curr || curr->key != key) {
                if (curr) curr->unlock();
                pred->unlock();
                return false;
            }

            // Логическое удаление
            curr->marked.store(true, std::memory_order_release);
            // Физическое вырезание
            Node* succ = curr->next.load(std::memory_order_acquire);
            pred->next.store(succ, std::memory_order_release);

            if (curr) curr->unlock();
            pred->unlock();

            return true;
        }
    }
};
