#include <bits/stdc++.h>
#include "lists.hpp"
using namespace std;

struct WorkloadCfg {
    size_t total_ops;
    int readers = 0;                 // потоки только find
    int writers = 1;                 // потоки push/remove
    int key_range = 1'000'000;
    int warmup_ms = 200;
    double p_insert = 0.50;          // доля вставок у писателей (остальное remove)
};

static bool parse_int_arg(char const* flag, int& out, int argc, char** argv) {
    for (int i = 1; i + 1 < argc; ++i) {
        if (std::string(argv[i]) == flag) { out = std::max(0, atoi(argv[i+1])); return true; }
    }
    return false;
}

static void maybe_read_interactive(int& readers, int& writers) {
    if (readers <= 0) {
        cerr << "Readers? ";
        if (!(cin >> readers)) readers = 1;
        if (readers < 0) readers = 0;
    }
    if (writers < 0) writers = 0;
    if (readers == 0 && writers == 0) writers = 1; 
}

template <class SetT>
static pair<double,double> run_bench_rw(const char* name, SetT& set, const WorkloadCfg& cfg) {
    const int R = cfg.readers;
    const int W = cfg.writers;
    const int T = R + W;
    if (T == 0) return {0,0};

    {
        atomic<bool> start{false};
        vector<thread> th;
        th.reserve(T);

        auto warm_reader = [&]{
            std::mt19937_64 rng(std::random_device{}() ^ (uint64_t)std::hash<std::thread::id>{}(std::this_thread::get_id()));
            std::uniform_int_distribution<int> keyd(0, cfg.key_range-1);
            while (!start.load(std::memory_order_acquire)) std::this_thread::yield();
            auto t0 = chrono::high_resolution_clock::now();
            while (chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now()-t0).count() < cfg.warmup_ms) {
                (void)set.find(keyd(rng));
            }
        };
        auto warm_writer = [&]{
            std::mt19937_64 rng(std::random_device{}() ^ (uint64_t)std::hash<std::thread::id>{}(std::this_thread::get_id()));
            std::uniform_int_distribution<int> keyd(0, cfg.key_range-1);
            std::uniform_real_distribution<double> pd(0.0,1.0);
            while (!start.load(std::memory_order_acquire)) std::this_thread::yield();
            auto t0 = chrono::high_resolution_clock::now();
            while (chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now()-t0).count() < cfg.warmup_ms) {
                int k = keyd(rng);
                if (pd(rng) < cfg.p_insert) (void)set.push(k);
                else (void)set.remove(k);
            }
        };

        for (int i=0;i<R;++i) th.emplace_back(warm_reader);
        for (int i=0;i<W;++i) th.emplace_back(warm_writer);
        start.store(true, memory_order_release);
        for (auto& t: th) t.join();
        hp::drain_all();
    }

    // --- Основной прогон: равномерно делим total_ops поровну между потоками
    size_t ops_per_thread = (cfg.total_ops + T - 1) / T;
    atomic<size_t> done{0};
    vector<thread> th; th.reserve(T);
    atomic<bool> go{false};

    auto reader = [&]{
        std::mt19937_64 rng(std::random_device{}() ^ (uint64_t)std::hash<std::thread::id>{}(std::this_thread::get_id()));
        std::uniform_int_distribution<int> keyd(0, cfg.key_range-1);
        while (!go.load(memory_order_acquire)) std::this_thread::yield();
        size_t local=0;
        while (local < ops_per_thread) {
            (void)set.find(keyd(rng));
            ++local;
        }
        done.fetch_add(local, memory_order_relaxed);
    };
    auto writer = [&]{
        std::mt19937_64 rng(std::random_device{}() ^ (uint64_t)std::hash<std::thread::id>{}(std::this_thread::get_id()));
        std::uniform_int_distribution<int> keyd(0, cfg.key_range-1);
        std::uniform_real_distribution<double> pd(0.0,1.0);
        while (!go.load(memory_order_acquire)) std::this_thread::yield();
        size_t local=0;
        while (local < ops_per_thread) {
            int k = keyd(rng);
            if (pd(rng) < cfg.p_insert) (void)set.push(k);
            else (void)set.remove(k);
            ++local;
        }
        done.fetch_add(local, memory_order_relaxed);
    };

    for (int i=0;i<R;++i) th.emplace_back(reader);
    for (int i=0;i<W;++i) th.emplace_back(writer);

    auto t0 = chrono::high_resolution_clock::now();
    go.store(true, memory_order_release);
    for (auto& t: th) t.join();
    auto t1 = chrono::high_resolution_clock::now();

    double sec = chrono::duration<double>(t1 - t0).count();
    size_t executed = std::min(done.load(), cfg.total_ops ? cfg.total_ops : done.load());
    double ops_per_sec = executed / sec;

    hp::drain_all();

    cout << setw(16) << left << name
         << " | readers=" << setw(3) << R
         << " writers=" << setw(3) << W
         << " | ops=" << setw(8) << cfg.total_ops
         << " | time=" << fixed << setprecision(3) << setw(7) << sec << " s"
         << " | " << fixed << setprecision(0) << ops_per_sec << " ops/s"
         << "\n";

    return {sec, ops_per_sec};
}

int main(int argc, char** argv) {
    // Параметры чтения/записи из аргументов или stdin
    int readers = 0, writers = 1;
    parse_int_arg("--readers", readers, argc, argv);
    parse_int_arg("--writers", writers, argc, argv);
    maybe_read_interactive(readers, writers);

    vector<size_t> sizes = {100'000, 200'000, 300'000};

    cout << "CPU threads detected: " << thread::hardware_concurrency() << "\n";
    cout << "Readers=" << readers << ", Writers=" << writers
         << " | key_range=1e6; warmup=200ms; writer mix: "
         << "insert=" << 60 << "%, remove=" << 40 << "%\n\n";

    for (size_t ops : sizes) {
        WorkloadCfg cfg;
        cfg.total_ops = ops;
        cfg.readers = readers;
        cfg.writers = writers;

        {
            CoarseListSet s;
            run_bench_rw("CoarseListSet", s, cfg);
        }
        {
            FineListSet s;
            run_bench_rw("FineListSet", s, cfg);
        }
        {
            LockFreeListSet s;
            run_bench_rw("LockFreeListSet", s, cfg);
        }

        cout << "\n";
    }
    return 0;
}
