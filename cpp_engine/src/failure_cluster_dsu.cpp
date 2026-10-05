#include "../include/failure_cluster_dsu.hpp"
#include <algorithm>

namespace warranty {

void FailureClusterDSU::addComponent(const std::string& componentId, double baseCost) {
    if (m_nodes.find(componentId) == m_nodes.end()) {
        m_nodes[componentId] = {componentId, baseCost};
        m_parent[componentId] = componentId;
        m_rank[componentId] = 0;
    }
}

void FailureClusterDSU::recordCoFailure(const std::string& compA, const std::string& compB, double repairCost) {
    addComponent(compA, repairCost * 0.5);
    addComponent(compB, repairCost * 0.5);

    std::string u = std::min(compA, compB);
    std::string v = std::max(compA, compB);

    auto& edge = m_edges[u][v];
    edge.compA = u;
    edge.compB = v;
    edge.frequency++;
    edge.accumulatedCost += repairCost;
}

std::string FailureClusterDSU::findSet(const std::string& u) {
    if (m_parent.find(u) == m_parent.end()) {
        m_parent[u] = u;
        m_rank[u] = 0;
        return u;
    }
    if (m_parent[u] != u) {
        m_parent[u] = findSet(m_parent[u]); // Path compression
    }
    return m_parent[u];
}

void FailureClusterDSU::unionSets(const std::string& u, const std::string& v) {
    std::string rootU = findSet(u);
    std::string rootV = findSet(v);

    if (rootU != rootV) {
        // Union by rank
        if (m_rank[rootU] < m_rank[rootV]) {
            m_parent[rootU] = rootV;
        } else if (m_rank[rootU] > m_rank[rootV]) {
            m_parent[rootV] = rootU;
        } else {
            m_parent[rootV] = rootU;
            m_rank[rootU]++;
        }
    }
}

void FailureClusterDSU::buildClusters(int minCoOccurrences) {
    // Reset parents
    for (const auto& [id, _] : m_nodes) {
        m_parent[id] = id;
        m_rank[id] = 0;
    }

    for (const auto& [u, adj] : m_edges) {
        for (const auto& [v, edge] : adj) {
            if (edge.frequency >= minCoOccurrences) {
                unionSets(u, v);
            }
        }
    }
}

std::vector<ComponentCluster> FailureClusterDSU::getClusters() const {
    // Group nodes by root
    // Const cast or mutable helper to perform path compressed lookup
    FailureClusterDSU* nonConstThis = const_cast<FailureClusterDSU*>(this);

    std::unordered_map<std::string, std::vector<std::string>> groups;
    for (const auto& [id, _] : m_nodes) {
        std::string root = nonConstThis->findSet(id);
        groups[root].push_back(id);
    }

    std::vector<ComponentCluster> result;
    int clusterCounter = 1;

    for (const auto& [root, members] : groups) {
        ComponentCluster cluster;
        cluster.clusterId = clusterCounter++;
        cluster.rootComponent = root;
        cluster.memberComponents = members;
        cluster.totalWarrantyExposureUsd = 0.0;
        cluster.claimCount = 0;

        for (const auto& m : members) {
            cluster.totalWarrantyExposureUsd += m_nodes.at(m).individualCost;
        }

        // Add edge costs between members
        for (size_t i = 0; i < members.size(); ++i) {
            for (size_t j = i + 1; j < members.size(); ++j) {
                std::string u = std::min(members[i], members[j]);
                std::string v = std::max(members[i], members[j]);
                auto itU = m_edges.find(u);
                if (itU != m_edges.end()) {
                    auto itV = itU->second.find(v);
                    if (itV != itU->second.end()) {
                        cluster.totalWarrantyExposureUsd += itV->second.accumulatedCost;
                        cluster.claimCount += itV->second.frequency;
                    }
                }
            }
        }

        result.push_back(cluster);
    }

    // Sort by descending liability
    std::sort(result.begin(), result.end(), [](const ComponentCluster& a, const ComponentCluster& b) {
        return a.totalWarrantyExposureUsd > b.totalWarrantyExposureUsd;
    });

    return result;
}

std::string FailureClusterDSU::findRoot(const std::string& componentId) {
    return findSet(componentId);
}

} // namespace warranty
