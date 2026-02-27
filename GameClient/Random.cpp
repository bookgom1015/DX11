#include "Random.h"

float Random::Rand01() {
    static thread_local std::mt19937 rng{ std::random_device{}() };
    static thread_local std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(rng); // [0.0f, 1.0f)
}