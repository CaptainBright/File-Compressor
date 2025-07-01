#include "HuffmanTree.h"
#include <iostream>
#include <string>
#include <fstream>

int main() {
    std::string originalText = "hi this is teja testing out his new project";
    
    std::cout << "=== Simulating Web Server Process ===" << std::endl;
    std::cout << "Original text: \"" << originalText << "\"" << std::endl;
    
    // Step 1: Encode (simulating the encode endpoint)
    HuffmanTree huffman1;
    huffman1.buildTree(originalText);
    std::string encoded = huffman1.encode(originalText);
    
    std::cout << "Step 1 - Encoded: " << encoded << std::endl;
    std::cout << "Encoded length: " << encoded.length() << std::endl;
    
    // Step 2: Save original text to temp file (simulating server decode process)
    std::string tempInputFile = "temp_input_sim.txt";
    std::string tempEncodedFile = "temp_encoded_sim.dat";
    
    std::ofstream tempFile(tempInputFile);
    tempFile << originalText;
    tempFile.close();
    
    // Step 3: Create tree file by encoding the original text again
    HuffmanTree huffman2;
    huffman2.buildTree(originalText);
    std::string encoded2 = huffman2.encode(originalText);
    
    std::ofstream encodedFile(tempEncodedFile, std::ios::binary);
    encodedFile << encoded2;
    encodedFile.close();
    
    if (!huffman2.saveTreeToFile(tempEncodedFile + ".tree")) {
        std::cout << "ERROR: Failed to save tree file" << std::endl;
        return 1;
    }
    
    std::cout << "Step 2 - Tree encoded: " << encoded2 << std::endl;
    std::cout << "Tree encoded length: " << encoded2.length() << std::endl;
    std::cout << "Encodings match: " << (encoded == encoded2 ? "YES" : "NO") << std::endl;
    
    // Step 4: Load tree and decode the original encoded string
    HuffmanTree huffman3;
    if (!huffman3.loadTreeFromFile(tempEncodedFile + ".tree")) {
        std::cout << "ERROR: Failed to load tree file" << std::endl;
        return 1;
    }
    
    try {
        std::string decoded = huffman3.decode(encoded);
        std::cout << "Step 3 - Decoded: \"" << decoded << "\"" << std::endl;
        std::cout << "Decoded length: " << decoded.length() << std::endl;
        std::cout << "Final match: " << (originalText == decoded ? "YES" : "NO") << std::endl;
        
        if (originalText != decoded) {
            std::cout << "\nDETAILED ANALYSIS:" << std::endl;
            std::cout << "Original: ";
            for (char c : originalText) std::cout << (int)c << " ";
            std::cout << std::endl;
            std::cout << "Decoded:  ";
            for (char c : decoded) std::cout << (int)c << " ";
            std::cout << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "ERROR during decoding: " << e.what() << std::endl;
        
        // Try to decode the second encoded string instead
        try {
            std::string decoded2 = huffman3.decode(encoded2);
            std::cout << "Decoding with tree's own encoding works: \"" << decoded2 << "\"" << std::endl;
            std::cout << "Tree self-decode match: " << (originalText == decoded2 ? "YES" : "NO") << std::endl;
        } catch (const std::exception& e2) {
            std::cout << "Even tree self-decode failed: " << e2.what() << std::endl;
        }
    }
    
    // Clean up
    remove(tempInputFile.c_str());
    remove(tempEncodedFile.c_str());
    remove((tempEncodedFile + ".tree").c_str());
    
    return 0;
}
