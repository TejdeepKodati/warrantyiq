#pragma once

#include "types.hpp"
#include <vector>

namespace warranty {

class WeibullMLEFitter {
public:
    WeibullMLEFitter() = default;

    // Fits 2-parameter Weibull distribution to failure times using Newton-Raphson MLE
    WeibullParameters fit(const std::vector<double>& failureTimes, int maxIterations = 100, double tolerance = 1e-7) const;

    // Evaluates Reliability R(t) = exp(-(t/eta)^beta)
    static double reliability(double t, double beta, double eta);

    // Evaluates Cumulative Failure Probability F(t) = 1 - R(t)
    static double cumulativeFailureProbability(double t, double beta, double eta);

    // Evaluates instantaneous Hazard Rate h(t) = (beta / eta) * (t / eta)^(beta - 1)
    static double hazardRate(double t, double beta, double eta);

    // Evaluates Mean Time To Failure (MTTF) = eta * Gamma(1 + 1/beta)
    static double computeMTTF(double beta, double eta);

    // Evaluates B-life (e.g. B10 life = time at which 10% fail, p = 0.10)
    static double computeBLife(double p, double beta, double eta);
};

} // namespace warranty
