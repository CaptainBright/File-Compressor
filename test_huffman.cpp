#include "HuffmanTree.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>

void testBasicEncoding() {
    std::cout << "=== Testing Basic Encoding/Decoding ===" << std::endl;
    
    HuffmanTree huffman;
    std::string text = "hello world";
    
    huffman.buildTree(text);
    std::string encoded = huffman.encode(text);
    std::string decoded = huffman.decode(encoded);
    
    std::cout << "Original: " << text << std::endl;
    std::cout << "Encoded:  " << encoded << std::endl;
    std::cout << "Decoded:  " << decoded << std::endl;
    std::cout << "Match: " << (text == decoded ? "YES" : "NO") << std::endl;
    
    if (text != decoded) {
        std::cout << "ERROR: Basic encoding/decoding failed!" << std::endl;
    }
    std::cout << std::endl;
}

void testSingleCharacter() {
    std::cout << "=== Testing Single Character ===" << std::endl;
    
    HuffmanTree huffman;
    std::string text = "aaaaa";
    
    huffman.buildTree(text);
    std::string encoded = huffman.encode(text);
    std::string decoded = huffman.decode(encoded);
    
    std::cout << "Original: " << text << std::endl;
    std::cout << "Encoded:  " << encoded << std::endl;
    std::cout << "Decoded:  " << decoded << std::endl;
    std::cout << "Match: " << (text == decoded ? "YES" : "NO") << std::endl;
    
    if (text != decoded) {
        std::cout << "ERROR: Single character encoding/decoding failed!" << std::endl;
    }
    std::cout << std::endl;
}

void testSpecialCharacters() {
    std::cout << "=== Testing Special Characters ===" << std::endl;
    
    HuffmanTree huffman;
    std::string text = "hello\nworld\ttab\rcarriage return!@#$%^&*()";
    
    huffman.buildTree(text);
    std::string encoded = huffman.encode(text);
    std::string decoded = huffman.decode(encoded);
    
    std::cout << "Original length: " << text.length() << std::endl;
    std::cout << "Encoded length:  " << encoded.length() << std::endl;
    std::cout << "Decoded length:  " << decoded.length() << std::endl;
    std::cout << "Match: " << (text == decoded ? "YES" : "NO") << std::endl;
    
    if (text != decoded) {
        std::cout << "ERROR: Special character encoding/decoding failed!" << std::endl;
        std::cout << "Expected: ";
        for (char c : text) {
            std::cout << (int)c << " ";
        }
        std::cout << std::endl;
        std::cout << "Got:      ";
        for (char c : decoded) {
            std::cout << (int)c << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void testEmptyString() {
    std::cout << "=== Testing Empty String ===" << std::endl;
    
    HuffmanTree huffman;
    try {
        huffman.buildTree("");
        std::cout << "ERROR: Empty string should throw exception!" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Correctly caught exception: " << e.what() << std::endl;
    }
    std::cout << std::endl;
}

void testLargeText() {
    std::cout << "=== Testing Large Text ===" << std::endl;
    
    HuffmanTree huffman;
    std::string text = "";
    
    // Create a larger text with varying frequencies
    for (int i = 0; i < 100; i++) {
        text += "the quick brown fox jumps over the lazy dog ";
    }
    
    huffman.buildTree(text);
    std::string encoded = huffman.encode(text);
    std::string decoded = huffman.decode(encoded);
    
    std::cout << "Original length: " << text.length() << std::endl;
    std::cout << "Encoded length:  " << encoded.length() << std::endl;
    std::cout << "Decoded length:  " << decoded.length() << std::endl;
    std::cout << "Match: " << (text == decoded ? "YES" : "NO") << std::endl;
    
    if (text != decoded) {
        std::cout << "ERROR: Large text encoding/decoding failed!" << std::endl;
        std::cout << "First mismatch at position: ";
        for (size_t i = 0; i < std::min(text.length(), decoded.length()); i++) {
            if (text[i] != decoded[i]) {
                std::cout << i << " (expected '" << text[i] << "' got '" << decoded[i] << "')" << std::endl;
                break;
            }
        }
    }
    std::cout << std::endl;
}

void testCodeConsistency() {
    std::cout << "=== Testing Code Consistency ===" << std::endl;
    
    HuffmanTree huffman;
    std::string text = "abcdefg";
    
    huffman.buildTree(text);
    
    // Print codes for debugging
    std::cout << "Generated codes:" << std::endl;
    const auto& codes = huffman.getCodes();
    for (const auto& pair : codes) {
        std::cout << "'" << pair.first << "' -> " << pair.second << std::endl;
    }
    
    // Test each character individually
    bool allMatch = true;
    for (char c : text) {
        std::string singleChar(1, c);
        std::string encoded = huffman.encode(singleChar);
        std::string decoded = huffman.decode(encoded);
        
        if (singleChar != decoded) {
            std::cout << "ERROR: Character '" << c << "' failed round-trip" << std::endl;
            std::cout << "  Encoded: " << encoded << std::endl;
            std::cout << "  Decoded: " << decoded << std::endl;
            allMatch = false;
        }
    }
    
    if (allMatch) {
        std::cout << "All individual characters passed round-trip test" << std::endl;
    }
    std::cout << std::endl;
}

void testPartialDecoding() {
    std::cout << "=== Testing Partial/Invalid Decoding ===" << std::endl;
    
    HuffmanTree huffman;
    std::string text = "test";
    
    huffman.buildTree(text);
    std::string encoded = huffman.encode(text);
    
    std::cout << "Full encoded: " << encoded << std::endl;
    
    // Test with truncated encoded string
    if (encoded.length() > 1) {
        std::string truncated = encoded.substr(0, encoded.length() - 1);
        std::cout << "Truncated:    " << truncated << std::endl;
        
        try {
            std::string decoded = huffman.decode(truncated);
            std::cout << "Truncated decoded successfully (might be incomplete): " << decoded << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Truncated decoding correctly failed: " << e.what() << std::endl;
        }
    }
    
    // Test with invalid bits
    std::string invalid = encoded + "10101";
    std::cout << "Invalid:      " << invalid << std::endl;
    
    try {
        std::string decoded = huffman.decode(invalid);
        std::cout << "Invalid decoded: " << decoded << std::endl;
        std::cout << "WARNING: Invalid string decoded without error!" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Invalid decoding correctly failed: " << e.what() << std::endl;
    }
    std::cout << std::endl;
}

void testFileOperations() {
    std::cout << "=== Testing File Save/Load ===" << std::endl;
    
    HuffmanTree huffman1, huffman2;
    std::string text = "file test content with various characters!@#$";
    
    // Build tree and save
    huffman1.buildTree(text);
    std::string encoded1 = huffman1.encode(text);
    
    if (huffman1.saveTreeToFile("test_tree.dat")) {
        std::cout << "Tree saved successfully" << std::endl;
    } else {
        std::cout << "ERROR: Failed to save tree" << std::endl;
        return;
    }
    
    // Load tree and test
    if (huffman2.loadTreeFromFile("test_tree.dat")) {
        std::cout << "Tree loaded successfully" << std::endl;
        
        std::string encoded2 = huffman2.encode(text);
        std::string decoded1 = huffman1.decode(encoded1);
        std::string decoded2 = huffman2.decode(encoded2);
        
        std::cout << "Original encoding matches: " << (encoded1 == encoded2 ? "YES" : "NO") << std::endl;
        std::cout << "Cross-decoding works: " << (huffman2.decode(encoded1) == text ? "YES" : "NO") << std::endl;
        std::cout << "Both decoded correctly: " << (decoded1 == text && decoded2 == text ? "YES" : "NO") << std::endl;
        
        if (decoded1 != text || decoded2 != text) {
            std::cout << "ERROR: File save/load caused decoding issues!" << std::endl;
        }
    } else {
        std::cout << "ERROR: Failed to load tree" << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "Running comprehensive Huffman Tree tests..." << std::endl << std::endl;
    
    testBasicEncoding();
    testSingleCharacter();
    testSpecialCharacters();
    testEmptyString();
    testLargeText();
    testCodeConsistency();
    testPartialDecoding();
    testFileOperations();
    
    std::cout << "All tests completed." << std::endl;
    return 0;
}
