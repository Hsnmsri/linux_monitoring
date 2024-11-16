#include <Node.hpp>

void Node::checkUniqueNodes()
{
    std::unordered_set<std::string> uniqueNodes;

    for (const auto &node : appNodes)
    {
        // Generate a unique identifier by concatenating key properties
        std::string nodeId = node.name + "_" + node.ip + "_" + node.port + "_" + node.secret;

        if (uniqueNodes.find(nodeId) != uniqueNodes.end())
        {
            logger.logToConsole("Duplicate node detected with ID: " + nodeId);
        }
        else
        {
            uniqueNodes.insert(nodeId);
        }
    }
}
