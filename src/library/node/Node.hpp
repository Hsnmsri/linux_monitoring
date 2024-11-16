#pragma once

#include <iostream>
#include <vector>
#include <log/Log.hpp>
#include <node/NodeStructure.hpp>
#include <unordered_set>
#include <log/Log.hpp>

class Node
{
public:
    Node() = default;

    void setNodes(std::vector<NodeStructure> nodes) { appNodes = nodes; }
    void checkUniqueNodes();

private:
    std::vector<NodeStructure> appNodes;
    Log logger;

    // void Node::checkNodesConnectionStatus();
};