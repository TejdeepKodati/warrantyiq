#include "../include/streaming_quantile.hpp"
#include <algorithm>
#include <cmath>

namespace warranty {

PSquareQuantileEstimator::PSquareQuantileEstimator(double targetQuantile)
    : m_p(targetQuantile), m_count(0) {
    reset();
}

void PSquareQuantileEstimator::reset() {
    m_count = 0;
    m_q.fill(0.0);
    m_n.fill(0.0);
    m_np.fill(0.0);
    m_dn = {0.0, m_p / 2.0, m_p, (1.0 + m_p) / 2.0, 1.0};
}

double PSquareQuantileEstimator::parabolic(int i, double d) {
    double q_i = m_q[i];
    double n_i = m_n[i];
    double n_plus = m_n[i + 1];
    double n_minus = m_n[i - 1];

    double term1 = d / (n_plus - n_minus);
    double term2 = (n_i - n_minus + d) * (m_q[i + 1] - q_i) / (n_plus - n_i);
    double term3 = (n_plus - n_i - d) * (q_i - m_q[i - 1]) / (n_i - n_minus);

    return q_i + term1 * (term2 + term3);
}

double PSquareQuantileEstimator::linear(int i, double d) {
    int next_idx = (d > 0) ? (i + 1) : (i - 1);
    return m_q[i] + d * (m_q[next_idx] - m_q[i]) / (m_n[next_idx] - m_n[i]);
}

void PSquareQuantileEstimator::update(double value) {
    if (m_count < 5) {
        m_q[m_count] = value;
        m_count++;
        if (m_count == 5) {
            std::sort(m_q.begin(), m_q.end());
            for (int i = 0; i < 5; ++i) {
                m_n[i] = i + 1;
            }
            m_np[0] = 1.0;
            m_np[1] = 1.0 + 2.0 * m_p;
            m_np[2] = 1.0 + 4.0 * m_p;
            m_np[3] = 3.0 + 2.0 * m_p;
            m_np[4] = 5.0;
        }
        return;
    }

    m_count++;

    // Find cell k such that q_k <= value < q_{k+1}
    int k = -1;
    if (value < m_q[0]) {
        m_q[0] = value;
        k = 0;
    } else if (value < m_q[1]) {
        k = 0;
    } else if (value < m_q[2]) {
        k = 1;
    } else if (value < m_q[3]) {
        k = 2;
    } else if (value <= m_q[4]) {
        k = 3;
    } else {
        m_q[4] = value;
        k = 3;
    }

    // Increment positions of markers k+1 through 4
    for (int i = k + 1; i < 5; ++i) {
        m_n[i]++;
    }

    // Update desired positions
    for (int i = 0; i < 5; ++i) {
        m_np[i] += m_dn[i];
    }

    // Adjust heights of markers 1, 2, 3
    for (int i = 1; i <= 3; ++i) {
        double d = m_np[i] - m_n[i];
        if ((d >= 1.0 && (m_n[i + 1] - m_n[i] > 1.0)) ||
            (d <= -1.0 && (m_n[i - 1] - m_n[i] < -1.0))) {
            double signD = (d >= 0.0) ? 1.0 : -1.0;
            double q_prime = parabolic(i, signD);
            if (m_q[i - 1] < q_prime && q_prime < m_q[i + 1]) {
                m_q[i] = q_prime;
            } else {
                m_q[i] = linear(i, signD);
            }
            m_n[i] += signD;
        }
    }
}

double PSquareQuantileEstimator::getQuantile() const {
    if (m_count == 0) return 0.0;
    if (m_count <= 5) {
        std::vector<double> copy(m_q.begin(), m_q.begin() + m_count);
        std::sort(copy.begin(), copy.end());
        size_t idx = static_cast<size_t>(std::round(m_p * (m_count - 1)));
        return copy[idx];
    }
    return m_q[2];
}

uint64_t PSquareQuantileEstimator::count() const {
    return m_count;
}

} // namespace warranty
