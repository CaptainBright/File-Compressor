#ifndef HUFFMANTREE_H
#define HUFFMANTREE_H

#include <string>
#include <unordered_map>
#include <memory>
#include <queue>
#include <vector>
#include <fstream>

struct Node {
    char character;
    int frequency;
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;

    // Constructor for leaf nodes
    Node(char ch, int freq) : character(ch), frequency(freq), left(nullptr), right(nullptr) {}
    
    // Constructor for internal nodes
    Node(int freq, std::shared_ptr<Node> l, std::shared_ptr<Node> r) 
        : character(0), frequency(freq), left(l), right(r) {}

    bool isLeaf() const {
        return left == nullptr && right == nullptr;
    }
};

struct NodeComparator {
    bool operator()(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) const {
        if (a->frequency != b->frequency) {
            return a->frequency > b->frequency; // Min heap based on frequency
        }
        
        // Tie-breaking rules for deterministic behavior
        // 1. Leaf nodes have priority over internal nodes
        if (a->isLeaf() && !b->isLeaf()) return false;
        if (!a->isLeaf() && b->isLeaf()) return true;
        
        // 2. If both are leaf nodes, compare characters
        if (a->isLeaf() && b->isLeaf()) {
            return a->character > b->character;
        }
        
        // 3. If both are internal nodes, compare by total character count and then by minimum character
        if (!a->isLeaf() && !b->isLeaf()) {
            // Compare by minimum character in the subtree
            char minA = getMinChar(a);
            char minB = getMinChar(b);
            if (minA != minB) return minA > minB;
            
            // If still tied, compare by node creation order (pointer value)
            return a.get() > b.get();
        }
        
        return false; // Should never reach here
    }
    
private:
    char getMinChar(const std::shared_ptr<Node>& node) const {
        if (!node) return 127; // Max char value
        if (node->isLeaf()) return node->character;
        
        char leftMin = node->left ? getMinChar(node->left) : 127;
        char rightMin = node->right ? getMinChar(node->right) : 127;
        return std::min(leftMin, rightMin);
    }
};

class HuffmanTree {
private:
    std::shared_ptr<Node> root;
    std::unordered_map<char, std::string> codes;
    std::unordered_map<char, int> frequencies;

    void buildCodes(std::shared_ptr<Node> node, const std::string& code);
    void printTreeHelper(std::shared_ptr<Node> node, const std::string& prefix, bool isLast) const;
    std::string decodeHelper(const std::string& encoded, int& pos, std::shared_ptr<Node> node) const;
    int calculateHeight(std::shared_ptr<Node> node) const;
    void serializeTree(std::ofstream& file, std::shared_ptr<Node> node) const;
    std::shared_ptr<Node> deserializeTree(std::ifstream& file);

public:
    HuffmanTree();
    ~HuffmanTree();

    // Build tree from text or frequency map
    void buildTree(const std::string& text);
    void buildTree(const std::unordered_map<char, int>& freqMap);

    // Encoding and decoding
    std::string encode(const std::string& text) const;
    std::string decode(const std::string& encoded) const;

    // Utility functions
    std::string getCode(char ch) const;
    const std::unordered_map<char, std::string>& getCodes() const;
    const std::unordered_map<char, int>& getFrequencies() const;

    // Display functions
    void printTree() const;
    void printCodes() const;

    // File I/O
    bool saveTreeToFile(const std::string& filename) const;
    bool loadTreeFromFile(const std::string& filename);

    // Analysis functions
    double getAverageCodeLength() const;
    int getTreeHeight() const;
};

#endif // HUFFMANTREE_H