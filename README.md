# WarrantyIQ: Predictive Warranty & After-Market Telemetry Intelligence Platform

WarrantyIQ is an enterprise after-market intelligence platform designed to address the key technical and strategic domains of **Accenture Strategy & Consulting (After-Market Service Operations, Warranty Management, and Spare Parts Analytics)**.

It pairs high-performance **Modern C++ Statistical Computing** and **Data Structures & Algorithms** with a modern **Next.js & Tailwind CSS** analytics platform.

---

## Core Capabilities & Architecture

```
                    +------------------------------------------+
                    |  Dealer Warranty Claims & IoT Telemetry  |
                    +--------------------+---------------------+
                                         |
                                         v
+-----------------------------------------------------------------------------------+
|                           WARRANTYIQ STATISTICAL CORE                             |
|                                                                                   |
|  +---------------------------+   +------------------------+   +----------------+  |
|  |     C++ Weibull MLE       |   |  P-Square Quantiles    |   |   DSU Graph    |  |
|  |  Newton-Raphson Solver    |-->|  O(1) Streaming Anomaly|-->|   Cascading    |  |
|  |  MTTF & B10 Life Engine   |   |  Threshold Detector    |   |   Failure Union|  |
|  +---------------------------+   +------------------------+   +----------------+  |
|                                                                        |          |
|  +---------------------------------------------------------------+     |          |
|  |     Warranty Reserve Liabilities & Spare Parts Forecaster     |<----+          |
|  |     12-Month Population Cohort Degradation Simulation         |                |
|  +---------------------------------------------------------------+                |
+----------------------------------------+------------------------------------------+
                                         |
                                         v
               +----------------------------------------------------+
               |    Interactive Next.js & Tailwind Web Dashboard    |
               |    Reliability Curves, SQL Console & GenAI PoVs    |
               +----------------------------------------------------+
```

### 1. C++ Weibull Maximum Likelihood Estimation (MLE)
- Models component reliability $R(t)$ and bathtub hazard rate $h(t)$:
  $$f(t) = \frac{\beta}{\eta} \left(\frac{t}{\eta}\right)^{\beta - 1} e^{-(t/\eta)^\beta}$$
- Solves the non-linear likelihood equation for shape $\beta$ using **Newton-Raphson optimization** with exact analytical first derivatives:
  $$g(\beta) = \frac{1}{\beta} + \frac{1}{n} \sum_{i=1}^n \ln(t_i) - \frac{\sum_{i=1}^n t_i^\beta \ln(t_i)}{\sum_{i=1}^n t_i^\beta} = 0$$
- Computes Mean Time To Failure (MTTF) via the Gamma function $\Gamma(1 + 1/\beta)$ and calculates $B_{10}$ life (the operational timestamp where 10% of field components fail).

### 2. Jain & Chlamtac $P^2$ Dynamic Streaming Quantile Estimator
- Continuously maintains dynamic 95th and 99th percentile anomaly thresholds over streaming field sensor telemetry (thermal, pressure, vibration).
- Operates in strict **$O(1)$ auxiliary space** and **$O(1)$ update time per sample** without storing or sorting historical observations.

### 3. Disjoint Set Union (DSU) Co-Failure Cascades
- Constructs component dependency graphs from co-occurring service records.
- Applies **weighted union-by-rank with path compression** ($O(\alpha(V))$ amortized complexity) to cluster interrelated component breakdowns (e.g., alternator diodes triggering secondary battery pack BMS degradation).

### 4. SQL Telemetry Console & GenAI Root-Cause Synthesis
- Interactive SQL querying over claims datasets.
- GenAI executive failure briefing synthesis generating actionable recommendations for warranty adjudication and supplier chargebacks.

---

## Repository Structure

```
warrantyiq/
├── cpp_engine/                     # Modern C++ statistical reliability core
│   ├── include/
│   │   ├── types.hpp               # Domain models (WarrantyClaim, Telemetry, Clusters)
│   │   ├── weibull_mle.hpp         # Newton-Raphson Weibull MLE fitter
│   │   ├── streaming_quantile.hpp  # P-Square dynamic streaming quantile estimator
│   │   └── failure_cluster_dsu.hpp # Disjoint Set Union co-failure clustering
│   ├── src/
│   │   ├── weibull_mle.cpp
│   │   ├── streaming_quantile.cpp
│   │   └── failure_cluster_dsu.cpp
│   ├── tests/
│   │   └── test_runner.cpp         # Complete C++ test suite and benchmarks
│   └── CMakeLists.txt
├── app/                            # Next.js App Router full-stack web application
│   ├── globals.css
│   ├── layout.tsx
│   └── page.tsx                    # Interactive dashboard, Weibull curves & SQL
├── lib/
│   └── engine.ts                   # Algorithm mirrors for client/server execution
├── package.json
├── tailwind.config.js
├── tsconfig.json
└── README.md
```

---

## Building and Testing Locally

### 1. Compile and Run C++ Engine
```bash
cd cpp_engine
g++ -std=c++17 -O3 -Iinclude src/*.cpp tests/test_runner.cpp -o test_runner.exe
./test_runner.exe
```

Expected output:
```
====================================================
  WARRANTYIQ: C++20 STATISTICAL ENGINE TEST SUITE   
====================================================
[TEST 1] Testing Weibull Maximum Likelihood Estimation (MLE)... PASS
[TEST 2] Testing P-Square Streaming Quantile Estimator (O(1) Space)... PASS
[TEST 3] Testing Disjoint Set Union (DSU) Component Clustering... PASS
====================================================
  ALL 3 STATISTICAL ENGINES PASSED SUCCESSFULLY!
====================================================
```

### 2. Run Next.js Dashboard
```bash
npm install
npm run dev
```
Open [http://localhost:3000](http://localhost:3000) in your browser.

**Live Production Deployment**: [https://warrantyiq-phi.vercel.app](https://warrantyiq-phi.vercel.app)  
**GitHub Repository**: [https://github.com/TejdeepKodati/warrantyiq](https://github.com/TejdeepKodati/warrantyiq)

---

## Author
**Kodati Tejdeep**  
B.Tech in Computer Science and Engineering  
Indian Institute of Technology (Indian School of Mines) Dhanbad
