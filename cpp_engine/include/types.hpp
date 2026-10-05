#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace warranty {

// Warranty claim data point from after-market service records
struct WarrantyClaim {
    std::string claimId;
    std::string vinOrSerial;
    std::string componentId;
    std::string failureMode;
    double operatingHoursOrMileage; // Time to failure
    double repairCostUsd;
    std::string dealerCode;
    uint64_t claimTimestampEpoch;
    bool isCensored; // False = actual failure, True = right-censored (survived without failure)
};

// Estimated Weibull reliability parameters
struct WeibullParameters {
    double beta;       // Shape parameter (beta < 1 infant mortality, beta = 1 exponential/random, beta > 1 wear-out)
    double eta;        // Scale / Characteristic Life parameter
    double mttf;       // Mean Time To Failure (hrs / miles)
    double b10Life;    // 10% unreliability threshold (90% survival)
    double logLikelihood;
    bool converged;
    int iterations;
};

// Telemetry observation for streaming quantile anomaly detection
struct FieldSensorReading {
    std::string componentId;
    uint64_t timestampEpoch;
    double primaryMetric; // e.g. Operating Temperature, Pressure, Vibration
    double ambientTemperature;
};

// Component cluster generated via Disjoint Set Union (DSU)
struct ComponentCluster {
    int clusterId;
    std::string rootComponent;
    std::vector<std::string> memberComponents;
    double totalWarrantyExposureUsd;
    size_t claimCount;
};

// Projected warranty liability reserve
struct WarrantyReserveForecast {
    int forecastMonth;
    double projectedFailures;
    double projectedLiabilityUsd;
    double requiredSparePartsBuffer;
};

} // namespace warranty
