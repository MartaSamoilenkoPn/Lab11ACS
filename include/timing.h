
#ifndef LAB2_PRODUCTIVITY_COMPARISON_OLES_TEAM_COMMON_H
#define LAB2_PRODUCTIVITY_COMPARISON_OLES_TEAM_COMMON_H
#include <chrono>
#include <atomic>
#include <string>
inline std::chrono::high_resolution_clock::time_point
get_current_time_fenced()
{
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::high_resolution_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

template<class D>
inline long long to_ms(const D& d)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
}

#endif
