#include "../include/weibull_mle.hpp"
#include <cmath>
#include <numeric>
#include <algorithm>
#include <stdexcept>

namespace warranty {

double WeibullMLEFitter::reliability(double t, double beta, double eta) {
    if (t <= 0.0) return 1.0;
    if (eta <= 0.0 || beta <= 0.0) return 0.0;
    return std::exp(-std::pow(t / eta, beta));
}

double WeibullMLEFitter::cumulativeFailureProbability(double t, double beta, double eta) {
    return 1.0 - reliability(t, beta, eta);
}

double WeibullMLEFitter::hazardRate(double t, double beta, double eta) {
    if (t <= 0.0 || eta <= 0.0 || beta <= 0.0) return 0.0;
    return (beta / eta) * std::pow(t / eta, beta - 1.0);
}

double WeibullMLEFitter::computeMTTF(double beta, double eta) {
    if (beta <= 0.0 || eta <= 0.0) return 0.0;
    return eta * std::tgamma(1.0 + (1.0 / beta));
}

double WeibullMLEFitter::computeBLife(double p, double beta, double eta) {
    if (p <= 0.0 || p >= 1.0 || beta <= 0.0 || eta <= 0.0) return 0.0;
    // B_p = eta * (-ln(1 - p))^(1 / beta)
    return eta * std::pow(-std::log(1.0 - p), 1.0 / beta);
}

WeibullParameters WeibullMLEFitter::fit(const std::vector<double>& failureTimes, int maxIterations, double tolerance) const {
    WeibullParameters params{};
    params.beta = 1.0;
    params.eta = 1.0;
    params.mttf = 0.0;
    params.b10Life = 0.0;
    params.logLikelihood = 0.0;
    params.converged = false;
    params.iterations = 0;

    std::vector<double> validTimes;
    validTimes.reserve(failureTimes.size());
    for (double t : failureTimes) {
        if (t > 1e-6) validTimes.push_back(t);
    }

    if (validTimes.empty()) {
        return params;
    }

    const double n = static_cast<double>(validTimes.size());

    // Precompute sum of ln(t_i)
    double sumLnT = 0.0;
    for (double t : validTimes) {
        sumLnT += std::log(t);
    }
    const double meanLnT = sumLnT / n;

    // Initial estimate for beta using Menon's estimator:
    // var(ln T) ~ pi^2 / (6 * beta^2)  =>  beta ~ pi / (sqrt(6) * s_{ln T})
    double varLnT = 0.0;
    for (double t : validTimes) {
        double d = std::log(t) - meanLnT;
        varLnT += d * d;
    }
    varLnT /= std::max(1.0, n - 1.0);
    double initialBeta = 1.0;
    if (varLnT > 1e-4) {
        initialBeta = 1.28254983 / std::sqrt(varLnT);
    }
    if (initialBeta < 0.1) initialBeta = 0.1;
    if (initialBeta > 20.0) initialBeta = 20.0;

    double beta = initialBeta;

    // Newton-Raphson optimization
    for (int iter = 0; iter < maxIterations; ++iter) {
        params.iterations = iter + 1;

        double sumTB = 0.0;
        double sumTB_LnT = 0.0;
        double sumTB_LnT2 = 0.0;

        for (double t : validTimes) {
            double lnT = std::log(t);
            double tB = std::pow(t, beta);
            sumTB += tB;
            sumTB_LnT += tB * lnT;
            sumTB_LnT2 += tB * lnT * lnT;
        }

        if (sumTB <= 1e-12) break;

        // g(beta) = (1 / beta) + meanLnT - (sumTB_LnT / sumTB)
        double g = (1.0 / beta) + meanLnT - (sumTB_LnT / sumTB);

        if (std::abs(g) < tolerance) {
            params.converged = true;
            break;
        }

        // Analytical derivative g'(beta):
        // g'(beta) = -1 / beta^2 - (sumTB_LnT2 * sumTB - (sumTB_LnT)^2) / (sumTB)^2
        double num = (sumTB_LnT2 * sumTB) - (sumTB_LnT * sumTB_LnT);
        double gPrime = -(1.0 / (beta * beta)) - (num / (sumTB * sumTB));

        if (std::abs(gPrime) < 1e-12) break;

        double step = g / gPrime;
        double nextBeta = beta - step;

        // Damped step if nextBeta <= 0
        if (nextBeta <= 0.0) {
            nextBeta = beta * 0.5;
        }

        if (std::abs(nextBeta - beta) < tolerance) {
            beta = nextBeta;
            params.converged = true;
            break;
        }

        beta = nextBeta;
    }

    // Scale parameter eta = ( (1 / n) * sum(t_i^beta) )^(1 / beta)
    double sumTB = 0.0;
    for (double t : validTimes) {
        sumTB += std::pow(t, beta);
    }
    double eta = std::pow(sumTB / n, 1.0 / beta);

    // Compute log-likelihood
    double logL = n * std::log(beta) - n * beta * std::log(eta);
    for (double t : validTimes) {
        logL += (beta - 1.0) * std::log(t) - std::pow(t / eta, beta);
    }

    params.beta = beta;
    params.eta = eta;
    params.mttf = computeMTTF(beta, eta);
    params.b10Life = computeBLife(0.10, beta, eta);
    params.logLikelihood = logL;

    return params;
}

} // namespace warranty
