#pragma once
#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <optional>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <assert.h>

// ==============================
// 2.1 Очередь с грубой блокировкой
// ==============================
template<typename T>
class CoarseQueue {
    std::mutex mu_;
    std::queue<T> q_;
    std::condition_variable cv_;
public:
    CoarseQueue() = default;

    void push(T value) {
        {
            std::lock_guard<std::mutex> lg(mu_);
            q_.push(std::move(value));
        }
        cv_.notify_one();
    }

    // Блокирующий pop
    T wait_pop() {
        std::unique_lock<std::mutex> ul(mu_);
        cv_.wait(ul, [&]{ return !q_.empty(); });
        T out = std::move(q_.front());
        q_.pop();
        return out;
    }

    // Неблокирующий pop (возвращает false если пусто)
    bool try_pop(T& out) {
        std::lock_guard<std::mutex> lg(mu_);
        if (q_.empty()) return false;
        out = std::move(q_.front());
        q_.pop();
        return true;
    }
};

// ==============================
// 2.1 Очередь с тонкой блокировкой
// (Michael & Scott с двумя мьютексами для head/tail)
// ==============================
template<typename T>
class FineGrainedQueue {
    struct Node {
        std::optional<T> data;       
        std::unique_ptr<Node> next;  // Владелец следующего узла
        Node() = default;
        explicit Node(T&& v) : data(std::move(v)) {}
    };

    // Две тонкие блокировки
    std::mutex head_mu_;
    std::mutex tail_mu_;
    std::condition_variable cv_;

    std::unique_ptr<Node> head_;     // владеем головой
    std::atomic<Node*> tail_;        // "сырый" хвост (для быстрых проверок в wait/try_pop)

    // Внутренние помощники
    bool empty_unsafe_() const noexcept {
        // Читаем атомарный хвост без мьютекса; защищено логикой MS-очереди
        return head_.get() == tail_.load(std::memory_order_acquire);
    }

    // Снять голову; вызывать при удержанном head_mu_ и когда очередь не пуста
    std::unique_ptr<Node> popHeadHeld_() {
        std::unique_ptr<Node> oldHead = std::move(head_);
        head_ = std::move(oldHead->next);
        return oldHead; // oldHead — бывший dummy
    }

public:
    FineGrainedQueue() {
        head_ = std::make_unique<Node>();       
        tail_.store(head_.get(), std::memory_order_relaxed);
    }

    // lock-free вне уровня списка не делаем — только тонкая блокировка
    void push(T value) {
        auto newNode = std::make_unique<Node>(std::move(value)); // узел с данными
        Node* const newTailRaw = newNode.get();

        {
            std::lock_guard<std::mutex> tk(tail_mu_);

            // Текущий хвост указывает на dummy-слот без next; подвешиваем новый
            Node* tailRaw = tail_.load(std::memory_order_acquire);
            tailRaw->next = std::move(newNode);

            // Сдвигаем хвост на новый dummy-узел
            tail_.store(newTailRaw, std::memory_order_release);
        }
        cv_.notify_one();
    }

    // Блокирующий pop: возвращает значение (перемещением)
    T wait_pop() {
        std::unique_lock<std::mutex> hk(head_mu_);
        cv_.wait(hk, [&]{ return !empty_unsafe_(); });

        // head_ — dummy, его next содержит узел с данными
        std::unique_ptr<Node> oldHead = popHeadHeld_();
        // Теперь новый head_ — это узел с данными; его data гарантированно есть
        T out = std::move(*(head_->data));
        head_->data.reset(); // превращаем текущий head_ обратно в dummy
        return out;
    }

    // Неблокирующий pop: false если пусто
    bool try_pop(T& out) {
        std::lock_guard<std::mutex> hk(head_mu_);
        if (empty_unsafe_()) return false;

        std::unique_ptr<Node> oldHead = popHeadHeld_();
        out = std::move(*(head_->data));
        head_->data.reset();
        return true;
    }
};



// ========================= Hazard Pointers (minimal) =========================
namespace hp {

// Параметры домена HP — при необходимости увеличьте
inline constexpr std::size_t MAX_THREADS = 128;
inline constexpr std::size_t HPS_PER_THREAD = 2; // для очереди Майкла–Скотта нужно 2

struct HazardRecord {
    std::atomic<void*> ptr{nullptr};
};

struct Domain {
    std::array<HazardRecord, MAX_THREADS * HPS_PER_THREAD> hazards{};
    std::atomic<std::size_t> registered{0};

    // Пул всех защищённых указателей для сканирования
    void collect_protected(std::vector<void*>& out) {
        out.clear();
        for (auto& h : hazards) {
            void* p = h.ptr.load(std::memory_order_acquire);
            if (p) out.push_back(p);
        }
    }

    static Domain& instance() {
        static Domain d;
        return d;
    }
};

struct Slot {
    HazardRecord* recs[HPS_PER_THREAD]{};
};

inline thread_local Slot tls_slot;
inline thread_local bool tls_registered = false;

// простая регистрация треда: выделяем HPS_PER_THREAD подряд и возвращаем ссылки
inline void register_thread() {
    if (tls_registered) return;
    Domain& d = Domain::instance();
    std::size_t base = d.registered.fetch_add(1, std::memory_order_acq_rel) * HPS_PER_THREAD;
    assert(base + HPS_PER_THREAD <= d.hazards.size() && "hp::MAX_THREADS too small");
    for (std::size_t i = 0; i < HPS_PER_THREAD; ++i) {
        tls_slot.recs[i] = &d.hazards[base + i];
    }
    tls_registered = true;
}

inline void clear_all() {
    for (std::size_t i = 0; i < HPS_PER_THREAD; ++i) {
        tls_slot.recs[i]->ptr.store(nullptr, std::memory_order_release);
    }
}

inline void protect(std::size_t idx, void* p) {
    tls_slot.recs[idx]->ptr.store(p, std::memory_order_release);
}
inline void unprotect(std::size_t idx) {
    tls_slot.recs[idx]->ptr.store(nullptr, std::memory_order_release);
}

// ——— упрощённый механизм отложенного удаления ———
template <class T>
struct RetireList {
    static_assert(std::is_trivially_destructible<T>::value || std::is_destructible<T>::value,
                  "T must be destructible");

    std::vector<T*> bag;
    std::size_t threshold = 64; // когда достигли — запускаем scan()

    void retire(T* p) {
        bag.push_back(p);
        if (bag.size() >= threshold) scan();
    }

    void scan() {
        Domain& d = Domain::instance();
        std::vector<void*> protected_ptrs;
        protected_ptrs.reserve(d.hazards.size());
        d.collect_protected(protected_ptrs);

        // переносим «небезопасные» обратно, безопасные — удаляем
        std::vector<T*> survivors;
        survivors.reserve(bag.size());
        for (T* node : bag) {
            bool protected_now = false;
            for (void* hpv : protected_ptrs) {
                if (hpv == node) { protected_now = true; break; }
            }
            if (protected_now) survivors.push_back(node);
            else delete node;
        }
        bag.swap(survivors);
    }

    ~RetireList() {
        for (T* p : bag) delete p;
    }
};

} // namespace hp

// ===================== Lock-free очередь (Michael & Scott) ===================

template <class T>
class LockFreeQueue {
    struct Node {
        std::atomic<Node*> next{nullptr};
        T value;

        // dummy ctor
        Node() : next(nullptr), value() {}
        // value ctor
        explicit Node(const T& v) : next(nullptr), value(v) {}
        explicit Node(T&& v) : next(nullptr), value(std::move(v)) {}
    };

    std::atomic<Node*> head;
    std::atomic<Node*> tail;

    // retire list на тред
    static hp::RetireList<Node>& retire_list() {
        static thread_local hp::RetireList<Node> rl;
        return rl;
    }

public:
    LockFreeQueue() {
        hp::register_thread();
        Node* dummy = new Node(); // фиктивный узел
        head.store(dummy, std::memory_order_relaxed);
        tail.store(dummy, std::memory_order_relaxed);
    }

    ~LockFreeQueue() {
        // Дренируем очередь
        T tmp;
        while (try_pop(tmp)) {}
        // Удаляем dummy
        Node* h = head.load(std::memory_order_relaxed);
        delete h;
    }

    // Не-блокирующая push
    void push(const T& v) { emplace(v); }
    void push(T&& v) { emplace(std::move(v)); }

    template <class U>
    void emplace(U&& v) {
        hp::register_thread();
        Node* node = new Node(std::forward<U>(v));
        for (;;) {
            Node* t = tail.load(std::memory_order_acquire);
            hp::protect(0, t);
            if (t != tail.load(std::memory_order_acquire)) continue; // ABA из-за перезагрузки

            Node* next = t->next.load(std::memory_order_acquire);
            if (next != nullptr) {
                // хвост отстаёт — подтянем
                tail.compare_exchange_weak(t, next, std::memory_order_acq_rel, std::memory_order_acquire);
                continue;
            }
            // попробуем пристегнуть
            if (t->next.compare_exchange_weak(next, node, std::memory_order_acq_rel, std::memory_order_acquire)) {
                // продвинем хвост
                tail.compare_exchange_weak(t, node, std::memory_order_acq_rel, std::memory_order_acquire);
                hp::unprotect(0);
                return;
            }
        }
    }

    // Неблокирующее извлечение: возвращает true если удалось
    bool try_pop(T& out) {
        hp::register_thread();
        for (;;) {
            Node* h = head.load(std::memory_order_acquire);
            hp::protect(0, h);
            if (h != head.load(std::memory_order_acquire)) continue;

            Node* t = tail.load(std::memory_order_acquire);
            Node* next = h->next.load(std::memory_order_acquire);
            hp::protect(1, next);

            if (h != head.load(std::memory_order_acquire)) continue;

            if (next == nullptr) {
                // пусто
                hp::clear_all();
                return false;
            }
            if (h == t) {
                // хвост отстаёт — подтянем
                tail.compare_exchange_weak(t, next, std::memory_order_acq_rel, std::memory_order_acquire);
                continue;
            }

            // читаем значение до смены указателей
            out = next->value;

            // сдвигаем head на next
            if (head.compare_exchange_weak(h, next, std::memory_order_acq_rel, std::memory_order_acquire)) {
                // старый dummy уходит в retire
                hp::unprotect(0); // head больше не нужен
                hp::unprotect(1); // next больше не нужен как hazard
                retire_list().retire(h);
                return true;
            }
            // иначе — повторить цикл
        }
    }

    // Удобный метод «поп без результата»: вернуть bool
    bool try_pop_discard() {
        T trash;
        return try_pop(trash);
    }

    // Запрещаем копирование
    LockFreeQueue(const LockFreeQueue&) = delete;
    LockFreeQueue& operator=(const LockFreeQueue&) = delete;
};