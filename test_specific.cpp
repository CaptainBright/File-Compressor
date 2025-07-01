#include "HuffmanTree.h"
#include <iostream>
#include <string>

int main() {
    std::string text = "hi this is teja testing out his new project";
    
    std::cout << "Testing input: \"" << text << "\"" << std::endl;
    std::cout << "Length: " << text.length() << std::endl;
    
    try {
        HuffmanTree huffman;
        huffman.buildTree(text);
        
        std::cout << "\nFrequencies:" << std::endl;
        const auto& frequencies = huffman.getFrequencies();
        for (const auto& pair : frequencies) {
            char ch = pair.first;
            if (ch == ' ') std::cout << "SPACE";
            else if (ch == '\n') std::cout << "NEWLINE";
            else if (ch == '\t') std::cout << "TAB";
            else std::cout << "'" << ch << "'";
            std::cout << ": " << pair.second << std::endl;
        }
        
        std::cout << "\nCodes:" << std::endl;
        const auto& codes = huffman.getCodes();
        for (const auto& pair : codes) {
            char ch = pair.first;
            if (ch == ' ') std::cout << "SPACE";
            else if (ch == '\n') std::cout << "NEWLINE";
            else if (ch == '\t') std::cout << "TAB";
            else std::cout << "'" << ch << "'";
            std::cout << ": " << pair.second << std::endl;
        }
        
        std::string encoded = huffman.encode(text);
        std::cout << "\nOriginal: " << text << std::endl;
        std::cout << "Encoded:  " << encoded << std::endl;
        std::cout << "Encoded length: " << encoded.length() << std::endl;
        
        std::string decoded = huffman.decode(encoded);
        std::cout << "Decoded:  " << decoded << std::endl;
        std::cout << "Decoded length: " << decoded.length() << std::endl;
        
        std::cout << "\nMatch: " << (text == decoded ? "YES" : "NO") << std::endl;
        
        if (text != decoded) {
            std::cout << "\nDETAILED COMPARISON:" << std::endl;
            std::cout << "Expected length: " << text.length() << std::endl;
            std::cout << "Got length: " << decoded.length() << std::endl;
            
            size_t minLen = std::min(text.length(), decoded.length());
            for (size_t i = 0; i < minLen; i++) {
                if (text[i] != decoded[i]) {
                    std::cout << "First mismatch at position " << i << std::endl;
                    std::cout << "Expected: '" << text[i] << "' (ASCII " << (int)text[i] << ")" << std::endl;
                    std::cout << "Got: '" << decoded[i] << "' (ASCII " << (int)decoded[i] << ")" << std::endl;
                    break;
                }
            }
            
            if (text.length() != decoded.length()) {
                std::cout << "Length mismatch!" << std::endl;
                if (text.length() > decoded.length()) {
                    std::cout << "Decoded is shorter by " << (text.length() - decoded.length()) << " characters" << std::endl;
                } else {
                    std::cout << "Decoded is longer by " << (decoded.length() - text.length()) << " characters" << std::endl;
                }
            }
        }
        
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
