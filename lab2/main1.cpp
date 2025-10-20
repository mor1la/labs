#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstring>
#include <functional>
#include <iostream>
#include <mutex>
#include <numeric>
#include <thread>
#include <vector>

#include "queues.hpp"

using Clock = std::chrono::high_resolution_clock;
static std::mutex stdoutMutex;


template<class Q>
static void consumer_loop(Q& q, std::atomic<uint64_t>& consumed, uint64_t target) {
    int value{};
    while (consumed.load(std::memory_order_relaxed) < target) {
        if (q.try_pop(value)) {
            consumed.fetch_add(1, std::memory_order_relaxed);
        }
        // без yield и без пауз: чистый busy-spin
    }
}


template<class Q>
long long run_single_measurement(Q& q,
                                 int producers,
                                 int consumers,
                                 uint64_t total_ops)
{
    std::atomic<uint64_t> produced{0};
    std::atomic<uint64_t> consumed{0};

    // каждому продюсеру по равной доле
    uint64_t per_producer = total_ops / producers;
    uint64_t remainder = total_ops % producers;

    std::vector<std::thread> prodThreads, consThreads;
    prodThreads.reserve(producers);
    consThreads.reserve(consumers);

    // Барьер на запуск
    std::atomic<int> ready{0};
    auto start_barrier = [&](){
        ready.fetch_add(1);
        while (ready.load() < producers + consumers) { /* spin */ }
    };

    // Запуск потребителей
    for (int i = 0; i < consumers; ++i) {
        consThreads.emplace_back([&q, &consumed, total_ops, &start_barrier](){
            start_barrier();
            consumer_loop(q, consumed, total_ops);
        });
    }

    // Запуск производителей
    for (int i = 0; i < producers; ++i) {
        uint64_t count = per_producer + (i < static_cast<int>(remainder) ? 1 : 0);
        prodThreads.emplace_back([&q, count, &produced, &start_barrier, i](){
            start_barrier();
            for (uint64_t k = 0; k < count; ++k) {
                q.push(static_cast<int>(k + i*1315423911u)); // чуть рандомизируем значения
                produced.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    auto t0 = Clock::now();

    for (auto& t : prodThreads) t.join();
    for (auto& t : consThreads) t.join();

    auto t1 = Clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    return static_cast<long long>(ms);
}

template<class Q>
void benchmark_queue(const char* name, int producers, int consumers,
                     const std::vector<uint64_t>& targets)
{
    std::lock_guard<std::mutex> lg(stdoutMutex);
    std::cout << "\n=== " << name << " ===\n";
}

int main() {

    int producers, consumers;
    std::cout << "Укажите количество потоков (Писателей Читателей): ";
    std::cin >> producers >> consumers;

    const std::vector<uint64_t> targets = {100000, 200000, 5000000};

    auto run_suite = [&](auto& q, const char* label){
        {
            std::lock_guard<std::mutex> lg(stdoutMutex);
            std::cout << "\n=== " << label << " ===\n";
        }
        for (uint64_t target : targets) {
            std::vector<long long> samples; samples.reserve(5);
            for (int i = 0; i < 5; ++i) {
                long long ms = run_single_measurement(q, producers, consumers, target);
                samples.push_back(ms);
                std::lock_guard<std::mutex> lg(stdoutMutex);
                std::cout << "[Порог " << target << "] Замер " << (i+1) << ": "
                          << ms << " мс\n";
            }
            long long sum = std::accumulate(samples.begin(), samples.end(), 0LL);
            double avg = static_cast<double>(sum) / samples.size();
            std::lock_guard<std::mutex> lg(stdoutMutex);
            std::cout << "[Порог " << target << "] Среднее за 5 замеров: "
                      << avg << " мс\n";
        }
    };

    // 1) CoarseQueue
    {
        CoarseQueue<int> q;
        run_suite(q, "CoarseQueue (один мьютекс)");
    }

    // 2) FineGrainedQueue
    {
        FineGrainedQueue<int> q;
        run_suite(q, "FineGrainedQueue (два мьютекса)");
    }

    // 3) LockFreeQueue (hazard pointers)
    {
        LockFreeQueue<int> q;
        run_suite(q, "LockFreeQueue (lock-free, hazard pointers)");
    }

    return 0;
}

