#pragma once

#include "types.hpp"
#include <string>
#include <vector>
#include <unordered_map>

namespace warranty {

class FailureClusterDSU {
public:
    FailureClusterDSU() = default;

    // Registers a component in the universe of tracked parts
    void addComponent(const std::string& componentId, double baseCost = 0.0);

    // Records a co-occurring failure or dependency edge between two components
    void recordCoFailure(const std::string& compA, const std::string& compB, double repairCost);

    // Runs clustering where edges with co-occurrence frequency >= minCoOccurrences are merged
    void buildClusters(int minCoOccurrences = 2);

    // Returns identified component failure clusters sorted by total warranty liability
    std::vector<ComponentCluster> getClusters() const;

    // Finds representative root component for a given part ID
    std::string findRoot(const std::string& componentId);

private:
    struct ComponentNode {
        std::string id;
        double individualCost;
    };

    struct Edge {
        std::string compA;
        std::string compB;
        int frequency;
        double accumulatedCost;
    };

    std::unordered_map<std::string, ComponentNode> m_nodes;
    std::unordered_map<std::string, std::unordered_map<std::string, Edge>> m_edges;

    // DSU structures
    std::unordered_map<std::string, std::string> m_parent;
    std::unordered_map<std::string, int> m_rank;

    std::string findSet(const std::string& u);
    void unionSets(const std::string& u, const std::string& v);
};

} // namespace warranty
