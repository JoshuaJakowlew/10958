#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <string>
#include <cassert>

enum class NodeType : int
{
    VAL,
    ADD,
    SUB,
    MUL,
    DIV,
    EXP
};

struct Node
{
    double getValue()
    {
        switch (type)
        {
            case NodeType::VAL: return value;
            case NodeType::ADD: return left->getValue() + right->getValue();
            case NodeType::SUB: return left->getValue() - right->getValue();
            case NodeType::MUL: return left->getValue() * right->getValue();
            case NodeType::DIV: return left->getValue() / right->getValue();
            case NodeType::EXP: return std::pow(left->getValue(), right->getValue());
            default: assert(!"Unreachable"); return 0;
        }
    }

    friend std::ostream& operator<< (std::ostream& out, Node const& node)
    {
        out.precision(18);

        switch (node.type)
        {
            case NodeType::VAL:
                out << node.digits;
                break;
            case NodeType::ADD:
                out << "(" << *node.left << "+" << *node.right << ")";
                break;
            case NodeType::SUB:
                out << "(" << *node.left << "-" << *node.right << ")";
                break;
            case NodeType::MUL:
                out << "(" << *node.left << "*" << *node.right << ")";
                break;
            case NodeType::DIV:
                out << "(" << *node.left << "/" << *node.right << ")";
                break;
            case NodeType::EXP:
                out << "(" << *node.left << "^" << *node.right << ")";
                break;
            default:
                assert(!"Unreachable");
        }

        return out;
    }

    NodeType type;
    std::string digits;
    int digitsOffset;

    // valid only in non-leaf nodes
    Node *left, *right;

    // valid only for leaf nodes
    double value;
};

struct Solution
{
    Solution(std::string const & digits, int maxsplits, double goal)
        : digits(digits)
        , maxsplits(maxsplits)
        , goal(goal)
        , nodes(digits.size() * 2 - 1)
        , root(nodes.front())
    {
    }

    auto split()
    {
        // int length, leftLength;

        double v = root.getValue();
        double e = std::fabs(v - goal);
        if (e <= bestError)
        {
            bestError = e;
            std::cout << root << " = " << root.getValue() << std::endl;
        }

        if (0 == maxsplits) return;

        /*
        Try each leaf node with more than 1 digit
        that is not left of the last split point
        */
        // int saveLastSplitOffset;        
        // Node *node;
        for (int splitNode = 0; splitNode < nodesUsed; ++splitNode)
        {
            auto node = &nodes[splitNode];
            
            if (node->type != NodeType::VAL ||
                node->digits.length() <= 1 ||
                node->digitsOffset < lastSplitOffset
            ) continue;

            /* Record the node being split, and remember the previous one */
            int saveLastSplitOffset = lastSplitOffset;
            lastSplitOffset = node->digitsOffset;

            /* Attach children */
            node->left = &nodes[nodesUsed++];
            node->left->type = NodeType::VAL;

            node->right = &nodes[nodesUsed++];
            node->right->type = NodeType::VAL;

            /* Try each split point */
            int length = node->digits.length();
            node->left->digits = std::string(
                node->digits.begin(),
                node->digits.begin() + length - 1
            );
            node->left->digitsOffset = node->digitsOffset;

            node->right->digitsOffset = node->digitsOffset + length + 1;

            for (int leftLength = length - 1; leftLength > 0; --leftLength)
            {
                node->left->digits = std::string(
                    node->digits.begin(),
                    node->digits.begin() + leftLength
                );
                node->left->value = std::stod(node->left->digits);
                
                node->right->digits = std::string(
                    node->digits.begin() + leftLength,
                    node->digits.end()
                );
                node->right->value = std::stod(node->right->digits);
                
                --node->right->digitsOffset;

                for(node->type = NodeType::ADD; node->type <= NodeType::EXP; node->type = static_cast<NodeType>(1 + static_cast<int>(node->type)))
                {
                    maxsplits -= 1;
                    split();
                }
            }

            /* Unsplit: free children and revert to leaf. node->digits is still good. */
            nodesUsed -= 2;
            node->type = NodeType::VAL;

            /* Restore remembered stuff */
            lastSplitOffset = saveLastSplitOffset;
        }
    }

    auto solve()
    {
        auto root = Node {
            .type = NodeType::VAL,
            .digits = digits,
            .digitsOffset = 0,
            .value = std::stod(digits)
        };
        nodes.front() = root;
        nodesUsed = 1;

        lastSplitOffset = 0;
        split();

        assert(nodesUsed == 1);
        nodesUsed = 0;
    }

    std::string digits;
    int maxsplits;
    double goal;
    double bestError = std::numeric_limits<double>::max();

    std::vector<Node> nodes;
    Node & root;
    int lastSplitOffset;
    int nodesUsed;
};

int main()
{
    std::string digits = "123456789";
    int splits = 8;
    double goal = 10958;

    std::cout << "Digits: "; std::cin >> digits;
    std::cout << "Splits: "; std::cin >> splits;
    std::cout << "Goal: "  ; std::cin >> goal;

    Solution solution(digits, splits, goal);
    solution.solve();
}