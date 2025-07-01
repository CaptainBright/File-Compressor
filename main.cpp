#include "HuffmanTree.h"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

void compressFile(const std::string& inputPath, const std::string& encodedPath, const std::string& treePath) {
    std::ifstream inFile(inputPath);
    if (!inFile) {
        std::cerr << "Failed to open input file: " << inputPath << std::endl;
        return;
    }

    std::string text((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    inFile.close();

    std::cout << "=== File Compression ===" << std::endl;
    std::cout << "Input file: " << inputPath << std::endl;
    std::cout << "Original size: " << text.length() * 8 << " bits" << std::endl;

    HuffmanTree huffman;
    huffman.buildTree(text);

    std::string encoded = huffman.encode(text);

    std::ofstream encodedFile(encodedPath, std::ios::binary);
    if (!encodedFile) {
        std::cerr << "Failed to open encoded output file: " << encodedPath << std::endl;
        return;
    }
    encodedFile << encoded;
    encodedFile.close();

    if (huffman.saveTreeToFile(treePath)) {
        std::cout << "Huffman tree saved to: " << treePath << std::endl;
    } else {
        std::cerr << "Failed to save Huffman tree." << std::endl;
        return;
    }

    std::cout << "Encoded size: " << encoded.length() << " bits" << std::endl;
    double ratio = static_cast<double>(encoded.length()) / (text.length() * 8) * 100.0;
    std::cout << "Compression ratio: " << std::fixed << std::setprecision(2) << ratio << "%" << std::endl;
}

void decompressFile(const std::string& encodedPath, const std::string& treePath, const std::string& outputPath) {
    std::ifstream encodedFile(encodedPath, std::ios::binary);
    if (!encodedFile) {
        std::cerr << "Failed to open encoded file: " << encodedPath << std::endl;
        return;
    }

    std::string encoded((std::istreambuf_iterator<char>(encodedFile)), std::istreambuf_iterator<char>());
    encodedFile.close();

    HuffmanTree huffman;
    if (!huffman.loadTreeFromFile(treePath)) {
        std::cerr << "Failed to load Huffman tree from: " << treePath << std::endl;
        return;
    }

    std::string decoded = huffman.decode(encoded);

    std::ofstream outFile(outputPath);
    if (!outFile) {
        std::cerr << "Failed to open output file: " << outputPath << std::endl;
        return;
    }
    outFile << decoded;
    outFile.close();

    std::cout << "Decoded text written to: " << outputPath << std::endl;
    std::cout << "Decoded size: " << decoded.length() * 8 << " bits" << std::endl;
}

void printUsage() {
    std::cout << "Usage:\n";
    std::cout << "  huffman encode <input_text> - Encode text directly\n";
    std::cout << "  huffman decode <encoded_text> <tree_file> - Decode text using tree file\n";
    std::cout << "  huffman encode_file <input_file> <output_file> - Encode file\n";
    std::cout << "  huffman decode_file <input_file> <output_file> - Decode file\n";
    std::cout << "  huffman (no args) - Run original compression demo\n";
}

int main(int argc, char* argv[]) {
    // If no arguments, run the original demo
    if (argc == 1) {
        const std::string inputFile = "input.txt";
        const std::string encodedFile = "encoded.txt";
        const std::string treeFile = "huffman_tree.dat";
        const std::string outputFile = "output.txt";

        compressFile(inputFile, encodedFile, treeFile);
        decompressFile(encodedFile, treeFile, outputFile);

        // Validate the result
        std::ifstream in1(inputFile), in2(outputFile);
        std::string orig((std::istreambuf_iterator<char>(in1)), std::istreambuf_iterator<char>());
        std::string dec((std::istreambuf_iterator<char>(in2)), std::istreambuf_iterator<char>());
        std::cout << "Verification: " << (orig == dec ? "SUCCESS" : "FAIL") << std::endl;

        return 0;
    }

    // Handle command line arguments for web interface
    std::string command = argv[1];

    try {
        if (command == "encode" && argc == 3) {
            std::string text = argv[2];
            
            HuffmanTree huffman;
            huffman.buildTree(text);
            
            std::string encoded = huffman.encode(text);
            
            // Output in format expected by web server
            std::cout << "ENCODED:" << encoded << std::endl;
            std::cout << "ORIGINAL_SIZE:" << text.length() << std::endl;
            std::cout << "ENCODED_SIZE:" << encoded.length() << std::endl;
            
        } else if (command == "decode" && argc == 4) {
            std::string encoded = argv[2];
            std::string treeFile = argv[3];
            
            HuffmanTree huffman;
            if (!huffman.loadTreeFromFile(treeFile)) {
                std::cout << "ERROR:Cannot load tree file" << std::endl;
                return 1;
            }
            
            std::string decoded = huffman.decode(encoded);
            std::cout << "DECODED:" << decoded << std::endl;
            
        } else if (command == "encode_file" && argc == 4) {
            std::string inputFile = argv[2];
            std::string outputFile = argv[3];
            
            // Check if input file exists
            std::ifstream inFile(inputFile);
            if (!inFile) {
                std::cout << "ERROR:Cannot open input file" << std::endl;
                return 1;
            }
            
            std::string text((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
            inFile.close();
            
            HuffmanTree huffman;
            huffman.buildTree(text);
            
            std::string encoded = huffman.encode(text);
            
            // Save encoded data
            std::ofstream encodedFile(outputFile, std::ios::binary);
            if (!encodedFile) {
                std::cout << "ERROR:Cannot create output file" << std::endl;
                return 1;
            }
            encodedFile << encoded;
            encodedFile.close();
            
            // Save tree file (same name as output but with .tree extension)
            std::string treeFile = outputFile + ".tree";
            if (!huffman.saveTreeToFile(treeFile)) {
                std::cout << "ERROR:Cannot save tree file" << std::endl;
                return 1;
            }
            
            std::cout << "SUCCESS:File encoded successfully. Tree saved as " << treeFile << std::endl;
            
        } else if (command == "decode_file" && argc == 4) {
            std::string inputFile = argv[2];
            std::string outputFile = argv[3];
            
            // Check if input file exists
            std::ifstream encodedFile(inputFile, std::ios::binary);
            if (!encodedFile) {
                std::cout << "ERROR:Cannot open encoded file" << std::endl;
                return 1;
            }
            
            std::string encoded((std::istreambuf_iterator<char>(encodedFile)), std::istreambuf_iterator<char>());
            encodedFile.close();
            
            // Look for tree file (same name as input but with .tree extension)
            std::string treeFile = inputFile + ".tree";
            
            HuffmanTree huffman;
            if (!huffman.loadTreeFromFile(treeFile)) {
                std::cout << "ERROR:Cannot load tree file " << treeFile << std::endl;
                return 1;
            }
            
            std::string decoded = huffman.decode(encoded);
            
            // Save decoded data
            std::ofstream outFile(outputFile);
            if (!outFile) {
                std::cout << "ERROR:Cannot create output file" << std::endl;
                return 1;
            }
            outFile << decoded;
            outFile.close();
            
            std::cout << "SUCCESS:File decoded successfully" << std::endl;
            
        } else {
            printUsage();
            return 1;
        }
    } catch (const std::exception& e) {
        std::cout << "ERROR:" << e.what() << std::endl;
        return 1;
    }

    return 0;
}