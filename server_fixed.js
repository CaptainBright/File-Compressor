const express = require("express");
const { exec } = require("child_process");
const path = require("path");
const fs = require("fs");
const os = require("os");

const app = express();
const PORT = 3000;

// Middleware
app.use(express.json());
app.use(express.static("public")); // Serve static files from 'public' directory

// Store encoded data and tree files temporarily
const tempStorage = new Map();

// Helper function to get proper executable path
function getExecutablePath() {
  return process.platform === "win32" ? ".\\huffman.exe" : "./huffman";
}

// Helper function to escape text for Windows command line
function escapeForWindows(text) {
  return text.replace(/"/g, '""');
}

// Helper function to create temp file with content
function createTempFile(content, filename) {
  return new Promise((resolve, reject) => {
    fs.writeFile(filename, content, 'utf8', (err) => {
      if (err) reject(err);
      else resolve();
    });
  });
}

// Helper function to delete file
function deleteFile(filename) {
  return new Promise((resolve) => {
    fs.unlink(filename, () => resolve()); // Ignore errors
  });
}

// Encode text endpoint
app.post("/api/encode", async (req, res) => {
  const { text } = req.body;

  if (!text) {
    return res.status(400).json({ error: "Text is required" });
  }

  const execPath = getExecutablePath();
  const escapedText = escapeForWindows(text);
  
  // Execute C++ program
  exec(`${execPath} encode "${escapedText}"`, (error, stdout, stderr) => {
    if (error) {
      console.error("Encoding error:", error);
      return res.status(500).json({ 
        error: "Encoding failed", 
        details: error.message 
      });
    }

    // Parse the output from C++ program
    const lines = stdout.trim().split(/\r?\n/);
    const result = {};

    lines.forEach((line) => {
      if (line.startsWith("ENCODED:")) {
        result.encoded = line.substring(8);
      } else if (line.startsWith("ORIGINAL_SIZE:")) {
        result.originalSize = parseInt(line.substring(14));
      } else if (line.startsWith("ENCODED_SIZE:")) {
        result.encodedSize = parseInt(line.substring(13));
      }
    });

    // Calculate compression ratio
    if (result.originalSize && result.encodedSize) {
      result.compressionRatio = (
        (result.encodedSize / (result.originalSize * 8)) * 100
      ).toFixed(1);
    }

    // Store the original text for decoding
    const sessionId = Date.now().toString();
    tempStorage.set(sessionId, text);
    result.sessionId = sessionId;

    res.json(result);
  });
});

// Decode text endpoint
app.post("/api/decode", async (req, res) => {
  const { encoded, sessionId } = req.body;

  if (!encoded) {
    return res.status(400).json({ error: "Encoded text is required" });
  }

  if (!sessionId || !tempStorage.has(sessionId)) {
    return res.status(400).json({
      error: "Invalid session or missing tree data. Please encode text first."
    });
  }

  const originalText = tempStorage.get(sessionId);
  const tempInputFile = `temp_input_${sessionId}.txt`;
  const tempEncodedFile = `temp_encoded_${sessionId}.dat`;
  const execPath = getExecutablePath();

  try {
    // Create temporary input file
    await createTempFile(originalText, tempInputFile);

    // First, create the tree by encoding the original text
    exec(
      `${execPath} encode_file "${tempInputFile}" "${tempEncodedFile}"`,
      (error, stdout, stderr) => {
        if (error) {
          console.error("Tree creation error:", error);
          return res.status(500).json({
            error: "Failed to create tree file",
            details: error.message
          });
        }

        // Now decode using the tree file
        const escapedEncoded = escapeForWindows(encoded);
        exec(
          `${execPath} decode "${escapedEncoded}" "${tempEncodedFile}.tree"`,
          (decodeError, decodeStdout, decodeStderr) => {
            if (decodeError) {
              console.error("Decoding error:", decodeError);
              return res.status(500).json({
                error: "Decoding failed",
                details: decodeError.message
              });
            }

            const lines = decodeStdout.trim().split(/\r?\n/);
            let decoded = "";

            lines.forEach((line) => {
              if (line.startsWith("DECODED:")) {
                decoded = line.substring(8);
              }
            });

            // Clean up temp files
            Promise.all([
              deleteFile(tempInputFile),
              deleteFile(tempEncodedFile),
              deleteFile(`${tempEncodedFile}.tree`)
            ]).then(() => {
              tempStorage.delete(sessionId);
              res.json({ decoded });
            });
          }
        );
      }
    );
  } catch (err) {
    console.error("File creation error:", err);
    res.status(500).json({ error: "Failed to create temporary file" });
  }
});

// Encode file endpoint
app.post("/api/encode-file", (req, res) => {
  const { filename } = req.body;

  if (!filename) {
    return res.status(400).json({ error: "Filename is required" });
  }

  const outputFile = `encoded_${filename}`;
  const execPath = getExecutablePath();

  exec(
    `${execPath} encode_file "${filename}" "${outputFile}"`,
    (error, stdout, stderr) => {
      if (error) {
        console.error("File encoding error:", error);
        return res.status(500).json({ 
          error: "File encoding failed", 
          details: error.message 
        });
      }

      if (stdout.includes("SUCCESS:")) {
        res.json({
          success: true,
          message: `File encoded successfully as ${outputFile}`,
          outputFile: outputFile
        });
      } else {
        res.status(500).json({ error: "Encoding failed", details: stdout });
      }
    }
  );
});

// Decode file endpoint
app.post("/api/decode-file", (req, res) => {
  const { filename } = req.body;

  if (!filename) {
    return res.status(400).json({ error: "Filename is required" });
  }

  const outputFile = `decoded_${filename}`;
  const execPath = getExecutablePath();

  exec(
    `${execPath} decode_file "${filename}" "${outputFile}"`,
    (error, stdout, stderr) => {
      if (error) {
        console.error("File decoding error:", error);
        return res.status(500).json({ 
          error: "File decoding failed", 
          details: error.message 
        });
      }

      if (stdout.includes("SUCCESS:")) {
        res.json({
          success: true,
          message: `File decoded successfully as ${outputFile}`,
          outputFile: outputFile
        });
      } else {
        res.status(500).json({ error: "Decoding failed", details: stdout });
      }
    }
  );
});

// View file endpoint
app.get("/api/view-file/:filename", (req, res) => {
  const filename = req.params.filename;

  fs.readFile(filename, "utf8", (err, data) => {
    if (err) {
      console.error("File read error:", err);
      return res.status(404).json({ error: "File not found" });
    }

    res.json({ content: data });
  });
});

// Health check endpoint
app.get("/api/health", (req, res) => {
  res.json({ 
    status: "OK", 
    platform: process.platform,
    executable: getExecutablePath()
  });
});

app.listen(PORT, () => {
  console.log(`Server running at http://localhost:${PORT}`);
  console.log(`Platform: ${process.platform}`);
  console.log(`Executable path: ${getExecutablePath()}`);
});
