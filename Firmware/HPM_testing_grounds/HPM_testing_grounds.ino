// ESP32 uses HardwareSerial (Serial1 or Serial2) for additional UART ports.

#define FINGERPRINT_SERIAL Serial2
#define BAUD_RATE 19200
#define FP_WAKE_PIN 22
#define FP_RST_PIN 23

bool FP_ready = false;

// --- Command Definitions ---
// Command 0x24: Acquire image and upload
const byte CMD_GET_IMAGE[] = {0xF5, 0x24, 0x00, 0x00, 0x00, 0x00, 0x24, 0xF5};
// Command 0x23: Acquire image and upload eigenvalues
const byte CMD_GET_EIGEN[] = {0xF5, 0x23, 0x00, 0x00, 0x00, 0x00, 0x23, 0xF5};
const byte HANDSHAKE[] = {0xF5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF5};

// Function to calculate the Checksum (CHK) for an 8-byte command frame
byte calculateChecksum(const byte* command) {
    // Checksum is the XOR sum of bytes 2 through 6 (CMD, P1, P2, P3, 0)
    byte chk = 0;
    for (int i = 1; i <= 5; i++) {
        chk ^= command[i];
    }
    return chk;
}

// Function to send command and wait for the 8-byte ACK response head
bool sendCommand(const byte* command, byte* responseHead) {
    // The command array must be 8 bytes long.
    if (command[6] != calculateChecksum(command)) {
        Serial.println("Error: Pre-calculated checksum is incorrect.");
        return false;
    }

    FINGERPRINT_SERIAL.write(command, 8);
    FINGERPRINT_SERIAL.flush(); // Wait for transmission to complete

    // The module is expected to respond with a data head (8 bytes) for 0x24 and 0x23
    int bytesRead = FINGERPRINT_SERIAL.readBytes(responseHead, 8);
    
    if (bytesRead == 8) {
        // The 5th byte (index 4) of the ACK data head is the status (Q3)
        // ACK_SUCCESS is 0x00
        if (responseHead[4] == 0x00) {
            Serial.println("Command acknowledged successfully.");
            return true;
        } else {
            Serial.print("Command failed, response status Q3: 0x");
            Serial.println(responseHead[4], HEX);
            return false;
        }
    } else {
        Serial.print("Failed to get 8-byte response head. Read: ");
        Serial.println(bytesRead);
        return false;
    }
}

// Function to read the large data packet
void readDataPacket(byte* responseHead) {
    // Length is stored in bytes 3 and 4 (Hi(Len) and Low(Len)) of the data head
    // Note: The data packet length itself is (Len + 3) bytes
    unsigned int len = (responseHead[2] << 8) | responseHead[3]; 

    // For 0x24 (Image) the data packet is fixed at 9800 bytes of image data, plus 3 header/footer bytes [cite: 378]
    // For 0x23 (Eigenvalue) the data packet is 193 bytes of eigenvalues, plus 3 header/footer bytes [cite: 389]
    unsigned int packetLength = len + 3; 

    Serial.print("Data packet expected length (Len + 3): ");
    Serial.println(packetLength);

    // Dynamic allocation for large data is generally best practice on an ESP32
    // For a 9803-byte image, this might require a significant buffer.
    byte* dataPacket = new byte[packetLength];
    if (!dataPacket) {
        Serial.println("Failed to allocate memory for data packet.");
        return;
    }
    
    int bytesRead = FINGERPRINT_SERIAL.readBytes(dataPacket, packetLength);

    if (bytesRead == packetLength) {
        Serial.println("Data packet received successfully.");
        // Process dataPacket here (e.g., store to SPIFFS, transmit over WiFi, etc.)
    } else {
        Serial.print("Error: Read ");
        Serial.print(bytesRead);
        Serial.print(" bytes, expected ");
        Serial.print(packetLength);
        Serial.println(" bytes.");
    }

    delete[] dataPacket; // Free the memory
}

void setup() {
    Serial.begin(115500); // For debugging output to the PC monitor

    pinMode(FP_RST_PIN, OUTPUT);
    digitalWrite(FP_RST_PIN, HIGH);
    delay(500);
    // Initialize the UART for the fingerprint sensor on specific pins (e.g., 16/17)
    FINGERPRINT_SERIAL.begin(BAUD_RATE, SERIAL_8N1, 17, 16); 
    Serial.println("UART Fingerprint Sensor Test Ready.");


    Serial.println("--- Testing Communication with Handshake (0x00) ---");
    byte responseHead[8];
    
    if (sendCommand(HANDSHAKE, responseHead)) {
      Serial.println("✅ Handshake");
      FP_ready = true;
    } else {
      // If it returns false, the error from sendCommand() is already printed.
      Serial.println("❌ Handshake");
      FP_ready = false;
    }
}

void loop() {
  byte responseHead[8];

  if (!FP_ready) {
    Serial.println("retrying Handshake");
    delay(2000);
    // retry handshake
   // digitalWrite(FP_RST_PIN, HIGH);
    delay(500);
    if (sendCommand(HANDSHAKE, responseHead)) {
      Serial.println("✅ Handshake");
      FP_ready = true;
    } else {
      // If it returns false, the error from sendCommand() is already printed.
      Serial.println("❌ Handshake");
      FP_ready = false;
    }
    return;
  } else {
    if (sendCommand(HANDSHAKE, responseHead)) {
      Serial.println("✅ Handshake");
      FP_ready = true;
    } else {
      // If it returns false, the error from sendCommand() is already printed.
      Serial.println("❌ Handshake");
      FP_ready = false;
    }
  }
    

    Serial.println("\n--- Requesting Eigenvalues (0x23) ---");
    // Ensure finger is placed before this command is sent
    if (sendCommand(CMD_GET_EIGEN, responseHead)) {
        readDataPacket(responseHead);
    }

    // Delay before the next test
    delay(5000); 

    Serial.println("\n--- Requesting Raw Image Data (0x24) ---");
    // Ensure finger is placed before this command is sent
    if (sendCommand(CMD_GET_IMAGE, responseHead)) {
        readDataPacket(responseHead);
    }
    
    delay(10000); // Wait for a new cycle
}