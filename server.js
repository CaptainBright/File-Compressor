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
  return text.replace(/"/g, '""').replace(/\r?\n/g, ' ');
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
  const tempInputFile = `temp_input_${Date.now()}.txt`;
  const tempOutputFile = `temp_output_${Date.now()}.dat`;
  
  try {
    await createTempFile(text, tempInputFile);
    
    // Execute C++ program
    exec(`${execPath} encode_file "${tempInputFile}" "${tempOutputFile}"`, async (error, stdout, stderr) => {
      if (error) {
        console.error("Encoding error:", error);
        await deleteFile(tempInputFile);
        return res.status(500).json({ 
          error: "Encoding failed", 
          details: error.message 
        });
      }

      // Check if encoding was successful
      if (!stdout.includes("SUCCESS:")) {
        console.error("Encoding failed:", stdout);
        await deleteFile(tempInputFile);
        return res.status(500).json({ 
          error: "Encoding failed", 
          details: stdout 
        });
      }

      try {
        // Read the encoded file to get the encoded text
        const encodedText = await new Promise((resolve, reject) => {
          fs.readFile(tempOutputFile, 'utf8', (err, data) => {
            if (err) reject(err);
            else resolve(data);
          });
        });

        const result = {
          encoded: encodedText,
          originalSize: text.length,
          encodedSize: encodedText.length
        };

        // Calculate compression ratio
        result.compressionRatio = (
          (result.encodedSize / (result.originalSize * 8)) * 100
        ).toFixed(1);

        // Store the original text for decoding
        const sessionId = Date.now().toString();
        tempStorage.set(sessionId, text);
        result.sessionId = sessionId;

        // Clean up temp files
        await deleteFile(tempInputFile);
        await deleteFile(tempOutputFile);
        await deleteFile(`${tempOutputFile}.tree`);
        
        res.json(result);
      } catch (readError) {
        console.error("Failed to read encoded file:", readError);
        await deleteFile(tempInputFile);
        await deleteFile(tempOutputFile);
        await deleteFile(`${tempOutputFile}.tree`);
        return res.status(500).json({ 
          error: "Failed to read encoded result", 
          details: readError.message 
        });
      }
    });
  } catch (err) {
    console.error("File creation error:", err);
    res.status(500).json({ error: "Failed to create temporary file" });
  }
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
        // Encoded text is binary string, no need to escape newlines
        const escapedEncoded = encoded.replace(/"/g, '""');
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

            // Parse the decoded output - the decoded text starts after "DECODED:" and may span multiple lines
            const decodedMatch = decodeStdout.match(/DECODED:(.*)/s);
            let decoded = "";
            
            if (decodedMatch && decodedMatch[1]) {
              decoded = decodedMatch[1].trim();
            }

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
