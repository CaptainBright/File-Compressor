# ByteShrinker - File Compressor

ByteShrinker is a file compression tool using Huffman coding algorithm. It provides efficient text and file encoding and decoding mechanisms, enhancing data storage and transmission capabilities.

## Features

- **Text Compression**: Encode and decode text data efficiently.
- **File Operations**: Compress and decompress text files.
- **User-friendly Web Interface**: Supports file operations.

## Project Structure

- **C++ Core**: Implements the main Huffman coding algorithm.
  - `main.cpp`: Main application logic for file operations.
  - `HuffmanTree.h` and `HuffmanTree.cpp`: Implement the Huffman tree and encoding/decoding processes.

- **Local Web Server**: Handles API requests and serves the front-end.
  - `server.js`: Node.js server to handle HTTP requests.
  - `public/index.html`: Front-end UI built with a clean, modern design.

- **Executable**: Precompiled executable for encoding/decoding using CLI.
  - `huffman.exe`: Executable for file compression tasks.

## Installation

1. **Clone the repository**:
  
  - git clone [https://github.com/your-username/byte-shrinker.git](https://github.com/CaptainBright/File-Compressor.git)
  - Navigate to project directory
  - Install dependencies: npm install
  - npm start

## How to Use the Web Interface

-  Access the web UI at http://localhost:3000
-  Encode/Decode Text: Enter text and get the encoded/decoded result.
-  File Operations: Upload a .txt file for compression or choose an encoded file to decompress.
-  (Note : Make sure all the corresponding txt files are in the same directory as of this project.)

## Command Line Interface (CLI)

-  Encode Text:   huffman encode "your text here"
-  Decode Text:   huffman decode "encoded_text" "tree.dat"
-  Encode File:   huffman encode_file input.txt encoded.dat
-  Decode File:   huffman decode_file encoded.dat output.txt
