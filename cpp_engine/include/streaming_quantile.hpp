#pragma once

#include "types.hpp"
#include <vector>
#include <array>

namespace warranty {

// Jain & Chlamtac P-Square Algorithm for dynamic single-pass streaming quantile estimation in O(1) space
class PSquareQuantileEstimator {
public:
    explicit PSquareQuantileEstimator(double targetQuantile = 0.95);

    // Ingests next metric value from streaming telemetry
    void update(double value);

    // Returns current estimated quantile value
    double getQuantile() const;

    // Returns number of processed observations
    uint64_t count() const;

    // Resets estimator state
    void reset();

private:
    double m_p;
    uint64_t m_count;
    std::array<double, 5> m_q;  // Marker heights (values)
    std::array<double, 5> m_n;  // Actual marker positions
    std::array<double, 5> m_np; // Desired marker positions
    std::array<double, 5> m_dn; // Desired position increments

    double parabolic(int i, double d);
    double linear(int i, double d);
};

} // namespace warranty
