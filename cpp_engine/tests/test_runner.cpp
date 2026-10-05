#include "../include/weibull_mle.hpp"
#include "../include/streaming_quantile.hpp"
#include "../include/failure_cluster_dsu.hpp"
#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <random>

using namespace warranty;

void testWeibullMLE() {
    std::cout << "\n[TEST 1] Testing Weibull Maximum Likelihood Estimation (MLE)...\n";
    WeibullMLEFitter fitter;

    // Simulated component wear-out failure data (hours to failure)
    std::vector<double> failureTimes = {
        420.5, 680.2, 850.1, 910.4, 1120.0, 1250.3, 1340.8,
        1420.2, 1490.5, 1580.4, 1630.1, 1750.6, 1820.0, 1980.5, 2150.2
    };

    WeibullParameters params = fitter.fit(failureTimes);

    std::cout << "  - Convergence: " << (params.converged ? "YES" : "NO") << " in " << params.iterations << " iterations\n";
    std::cout << "  - Estimated Shape (Beta): " << std::fixed << std::setprecision(3) << params.beta << "\n";
    std::cout << "  - Estimated Characteristic Life (Eta): " << std::fixed << std::setprecision(1) << params.eta << " hrs\n";
    std::cout << "  - Mean Time To Failure (MTTF): " << params.mttf << " hrs\n";
    std::cout << "  - B10 Life (10% unreliability threshold): " << params.b10Life << " hrs\n";
    std::cout << "  - Log-Likelihood: " << params.logLikelihood << "\n";

    assert(params.converged);
    assert(params.beta > 1.5 && params.beta < 4.0); // Clear wear-out failure mode
    assert(params.eta > 1200.0 && params.eta < 1800.0);
    assert(params.b10Life < params.mttf);

    // Verify reliability function R(t) monotonic decay
    double r500 = WeibullMLEFitter::reliability(500.0, params.beta, params.eta);
    double r1500 = WeibullMLEFitter::reliability(1500.0, params.beta, params.eta);
    assert(r500 > r1500);
    std::cout << "  - R(500h): " << (r500 * 100.0) << "% | R(1500h): " << (r1500 * 100.0) << "%\n";
    std::cout << "  - Weibull MLE Parameter Estimation: PASS\n";
}

void testStreamingQuantile() {
    std::cout << "\n[TEST 2] Testing P-Square Streaming Quantile Estimator (O(1) Space)...\n";
    PSquareQuantileEstimator p95(0.95);

    // Feed 2000 numbers uniformly distributed between 0 and 100
    // Theoretical 95th percentile is 95.0
    for (int i = 1; i <= 2000; ++i) {
        double val = (i % 100) + ((i * 17) % 100) * 0.01;
        p95.update(val);
    }

    double estimatedP95 = p95.getQuantile();
    std::cout << "  - Ingested observations: " << p95.count() << "\n";
    std::cout << "  - Estimated 95th Percentile: " << std::fixed << std::setprecision(2) << estimatedP95 << " (Theoretical ~95.0)\n";

    assert(std::abs(estimatedP95 - 95.0) < 6.0); // Within 6% error bound for P-Square single pass
    std::cout << "  - Streaming quantile estimation: PASS\n";
}

void testFailureClusterDSU() {
    std::cout << "\n[TEST 3] Testing Disjoint Set Union (DSU) Component Clustering...\n";
    FailureClusterDSU dsu;

    // Cluster 1: Electrical charging system
    dsu.recordCoFailure("ALT-48V", "BAT-AGM", 450.0);
    dsu.recordCoFailure("ALT-48V", "VLT-REG-01", 320.0);
    dsu.recordCoFailure("ALT-48V", "BAT-AGM", 450.0); // Second co-occurrence

    // Cluster 2: Forced induction system
    dsu.recordCoFailure("TRB-VGT-02", "OIL-SNS-P", 1250.0);
    dsu.recordCoFailure("TRB-VGT-02", "OIL-SNS-P", 1250.0);

    // Unrelated part: Brake pad
    dsu.addComponent("BRK-CER-PAD", 180.0);

    dsu.buildClusters(2); // Require >= 2 co-occurrences

    auto clusters = dsu.getClusters();
    std::cout << "  - Identified Failure Clusters: " << clusters.size() << "\n";

    for (const auto& c : clusters) {
        std::cout << "    * Cluster #" << c.clusterId << " (Root: " << c.rootComponent << "): "
                  << c.memberComponents.size() << " parts, $"
                  << std::fixed << std::setprecision(2) << c.totalWarrantyExposureUsd << " exposure\n";
        for (const auto& p : c.memberComponents) {
            std::cout << "        - " << p << "\n";
        }
    }

    assert(clusters.size() >= 2);
    // Verify that ALT-48V and BAT-AGM belong to same root
    assert(dsu.findRoot("ALT-48V") == dsu.findRoot("BAT-AGM"));
    std::cout << "  - DSU co-failure clustering verification: PASS\n";
}

int main() {
    std::cout << "====================================================\n";
    std::cout << "  WARRANTYIQ: C++20 STATISTICAL ENGINE TEST SUITE   \n";
    std::cout << "====================================================\n";

    auto start = std::chrono::high_resolution_clock::now();

    testWeibullMLE();
    testStreamingQuantile();
    testFailureClusterDSU();

    auto end = std::chrono::high_resolution_clock::now();
    double elapsedMs = std::chrono::duration<double, std::milli>(end - start).count();

    std::cout << "\n====================================================\n";
    std::cout << "  ALL 3 STATISTICAL ENGINES PASSED SUCCESSFULLY!\n";
    std::cout << "  Total Execution Time: " << std::fixed << std::setprecision(2) << elapsedMs << " ms\n";
    std::cout << "====================================================\n";

    return 0;
}
