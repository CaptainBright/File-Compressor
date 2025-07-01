#include "HuffmanTree.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <memory>
#include <stdexcept>

HuffmanTree::HuffmanTree() : root(nullptr) {
}

HuffmanTree::~HuffmanTree() {
    // Shared pointers will handle cleanup automatically
}

void HuffmanTree::buildTree(const std::string& text) {
    if (text.empty()) {
        throw std::invalid_argument("Input text cannot be empty");
    }

    frequencies.clear();
    for (char ch : text) {
        frequencies[ch]++;
    }

    buildTree(frequencies);
}

void HuffmanTree::buildTree(const std::unordered_map<char, int>& freqMap) {
    if (freqMap.empty()) {
        throw std::invalid_argument("Frequency map cannot be empty");
    }

    frequencies = freqMap;

    // Create priority queue with NodeComparator for deterministic tie-breaking
    std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, NodeComparator> pq;

    // Sort characters to ensure deterministic processing order
    std::vector<std::pair<char, int>> sortedFreqs(frequencies.begin(), frequencies.end());
    std::sort(sortedFreqs.begin(), sortedFreqs.end());

    // Add all characters as leaf nodes to the priority queue in sorted order
    for (const auto& pair : sortedFreqs) {
        pq.push(std::make_shared<Node>(pair.first, pair.second));
    }

    // Special case: only one unique character
    if (pq.size() == 1) {
        auto single = pq.top(); 
        pq.pop();
        root = std::make_shared<Node>(single->frequency, single, nullptr);
    } else {
        // Build the tree by merging nodes
        while (pq.size() > 1) {
            auto right = pq.top(); pq.pop();
            auto left = pq.top(); pq.pop();

            auto merged = std::make_shared<Node>(left->frequency + right->frequency, left, right);
            pq.push(merged);
        }
        root = pq.top();
    }

    // Generate codes
    codes.clear();
    if (root->isLeaf()) {
        // Special case: single character gets code "0"
        codes[root->character] = "0";
    } else {
        buildCodes(root, "");
    }
}

void HuffmanTree::buildCodes(std::shared_ptr<Node> node, const std::string& code) {
    if (!node) return;

    if (node->isLeaf()) {
        codes[node->character] = code.empty() ? "0" : code;
        return;
    }

    if (node->left) buildCodes(node->left, code + "0");
    if (node->right) buildCodes(node->right, code + "1");
}

std::string HuffmanTree::getCode(char ch) const {
    auto it = codes.find(ch);
    if (it != codes.end()) {
        return it->second;
    }
    throw std::invalid_argument("Character not found in Huffman tree: " + std::string(1, ch));
}

const std::unordered_map<char, std::string>& HuffmanTree::getCodes() const {
    return codes;
}

std::string HuffmanTree::encode(const std::string& text) const {
    if (codes.empty()) {
        throw std::runtime_error("Tree not built - no codes available");
    }

    std::string encoded;
    encoded.reserve(text.length() * 8); // Reserve space for efficiency

    for (char ch : text) {
        encoded += getCode(ch);
    }

    return encoded;
}

std::string HuffmanTree::decodeHelper(const std::string& encoded, int& pos, std::shared_ptr<Node> node) const {
    if (!node) throw std::runtime_error("Invalid tree structure");

    if (node->isLeaf()) {
        return std::string(1, node->character);
    }

    if (pos >= static_cast<int>(encoded.length())) {
        throw std::runtime_error("Incomplete encoded string");
    }

    if (encoded[pos] == '0' && node->left) {
        pos++;
        return decodeHelper(encoded, pos, node->left);
    } else if (encoded[pos] == '1' && node->right) {
        pos++;
        return decodeHelper(encoded, pos, node->right);
    } else {
        throw std::runtime_error("Invalid encoded string or tree structure at position " + std::to_string(pos));
    }
}

std::string HuffmanTree::decode(const std::string& encoded) const {
    if (!root) throw std::runtime_error("Tree not built");
    if (encoded.empty()) return "";

    std::string decoded;
    int pos = 0;
    
    while (pos < static_cast<int>(encoded.length())) {
        decoded += decodeHelper(encoded, pos, root);
    }
    
    // Ensure all bits were consumed
    if (pos != static_cast<int>(encoded.length())) {
        throw std::runtime_error("Invalid encoded string: extra bits remaining");
    }
    
    return decoded;
}

const std::unordered_map<char, int>& HuffmanTree::getFrequencies() const {
    return frequencies;
}

void HuffmanTree::printTree() const {
    if (!root) {
        std::cout << "Tree is empty." << std::endl;
        return;
    }
    std::cout << "Huffman Tree Structure:" << std::endl;
    printTreeHelper(root, "", true);
}

void HuffmanTree::printTreeHelper(std::shared_ptr<Node> node, const std::string& prefix, bool isLast) const {
    if (!node) return;

    std::cout << prefix;
    std::cout << (isLast ? "└── " : "├── ");

    if (node->isLeaf()) {
        char ch = node->character;
        if (ch == ' ') std::cout << "'SPACE'";
        else if (ch == '\n') std::cout << "'NEWLINE'";
        else if (ch == '\t') std::cout << "'TAB'";
        else if (ch == '\r') std::cout << "'CARRIAGE_RETURN'";
        else if (std::isprint(ch)) std::cout << "'" << ch << "'";
        else std::cout << "'ASCII_" << static_cast<int>(ch) << "'";
        std::cout << " (freq: " << node->frequency << ")" << std::endl;
    } else {
        std::cout << "Internal (freq: " << node->frequency << ")" << std::endl;
        if (node->right)
            printTreeHelper(node->right, prefix + (isLast ? "    " : "│   "), !node->left);
        if (node->left)
            printTreeHelper(node->left, prefix + (isLast ? "    " : "│   "), true);
    }
}

void HuffmanTree::printCodes() const {
    if (codes.empty()) {
        std::cout << "No codes available." << std::endl;
        return;
    }

    std::cout << "\nHuffman Codes:" << std::endl;
    std::cout << std::setw(12) << "Character" << std::setw(12) << "Frequency" << std::setw(15) << "Code" << std::setw(10) << "Length" << std::endl;
    std::cout << std::string(49, '-') << std::endl;

    // Sort by frequency (descending) for better readability
    std::vector<std::pair<char, std::string>> sortedCodes(codes.begin(), codes.end());
    std::sort(sortedCodes.begin(), sortedCodes.end(),
        [this](const std::pair<char, std::string>& a, const std::pair<char, std::string>& b) {
            return frequencies.at(a.first) > frequencies.at(b.first);
        });

    for (const auto& pair : sortedCodes) {
        char ch = pair.first;
        std::string displayChar;
        
        if (ch == ' ') displayChar = "SPACE";
        else if (ch == '\n') displayChar = "NEWLINE";
        else if (ch == '\t') displayChar = "TAB";
        else if (ch == '\r') displayChar = "CR";
        else if (std::isprint(ch)) displayChar = std::string(1, ch);
        else displayChar = "ASCII_" + std::to_string(static_cast<int>(ch));

        std::cout << std::setw(12) << displayChar
                  << std::setw(12) << frequencies.at(ch)
                  << std::setw(15) << pair.second 
                  << std::setw(10) << pair.second.length() << std::endl;
    }
}

bool HuffmanTree::saveTreeToFile(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;

    try {
        // Save the actual tree structure to preserve exact encoding
        serializeTree(file, root);
        
        // Also save frequency map for compatibility
        size_t freqSize = frequencies.size();
        file.write(reinterpret_cast<const char*>(&freqSize), sizeof(freqSize));
        
        for (const auto& pair : frequencies) {
            file.write(&pair.first, sizeof(char));
            file.write(reinterpret_cast<const char*>(&pair.second), sizeof(int));
        }

        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

bool HuffmanTree::loadTreeFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;

    try {
        // First, try to deserialize the tree structure
        root = deserializeTree(file);
        
        // Read frequency map size
        size_t freqSize;
        file.read(reinterpret_cast<char*>(&freqSize), sizeof(freqSize));

        // Read frequency data
        frequencies.clear();
        for (size_t i = 0; i < freqSize; ++i) {
            char ch;
            int freq;
            file.read(&ch, sizeof(char));
            file.read(reinterpret_cast<char*>(&freq), sizeof(int));
            frequencies[ch] = freq;
        }

        // Rebuild codes from the loaded tree
        codes.clear();
        if (root && root->isLeaf()) {
            // Special case: single character gets code "0"
            codes[root->character] = "0";
        } else if (root) {
            buildCodes(root, "");
        }

        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

double HuffmanTree::getAverageCodeLength() const {
    if (codes.empty() || frequencies.empty()) return 0.0;

    double totalBits = 0.0;
    int totalChars = 0;

    for (const auto& pair : codes) {
        char ch = pair.first;
        int freq = frequencies.at(ch);
        int codeLength = static_cast<int>(pair.second.length());
        totalBits += freq * codeLength;
        totalChars += freq;
    }

    return totalChars > 0 ? totalBits / totalChars : 0.0;
}

int HuffmanTree::getTreeHeight() const {
    return calculateHeight(root);
}

int HuffmanTree::calculateHeight(std::shared_ptr<Node> node) const {
    if (!node) return -1;
    if (node->isLeaf()) return 0;

    int leftHeight = calculateHeight(node->left);
    int rightHeight = calculateHeight(node->right);
    return 1 + std::max(leftHeight, rightHeight);
}

void HuffmanTree::serializeTree(std::ofstream& file, std::shared_ptr<Node> node) const {
    if (!node) {
        char marker = 0; // Null node marker
        file.write(&marker, sizeof(char));
        return;
    }

    if (node->isLeaf()) {
        char marker = 1; // Leaf node marker
        file.write(&marker, sizeof(char));
        file.write(&node->character, sizeof(char));
        file.write(reinterpret_cast<const char*>(&node->frequency), sizeof(int));
    } else {
        char marker = 2; // Internal node marker
        file.write(&marker, sizeof(char));
        file.write(reinterpret_cast<const char*>(&node->frequency), sizeof(int));
        serializeTree(file, node->left);
        serializeTree(file, node->right);
    }
}

std::shared_ptr<Node> HuffmanTree::deserializeTree(std::ifstream& file) {
    char marker;
    file.read(&marker, sizeof(char));

    if (marker == 0) {
        return nullptr;
    } else if (marker == 1) {
        // Leaf node
        char ch;
        int freq;
        file.read(&ch, sizeof(char));
        file.read(reinterpret_cast<char*>(&freq), sizeof(int));
        return std::make_shared<Node>(ch, freq);
    } else if (marker == 2) {
        // Internal node
        int freq;
        file.read(reinterpret_cast<char*>(&freq), sizeof(int));
        auto left = deserializeTree(file);
        auto right = deserializeTree(file);
        return std::make_shared<Node>(freq, left, right);
    }

    throw std::runtime_error("Invalid tree file format");
}