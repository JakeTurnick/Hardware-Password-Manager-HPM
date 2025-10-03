// ESP32 uses HardwareSerial (Serial1 or Serial2) for additional UART ports.

#define FINGERPRINT_SERIAL Serial2
#define BAUD_RATE 19200
#define FP_WAKE_PIN 22
#define FP_RST_PIN 23

const byte led_gpio = 33;

bool FP_ready = false;

// --- Command Definitions ---
// Command 0x24: Acquire image and upload
const byte CMD_GET_IMAGE[] = {0xF5, 0x24, 0x00, 0x00, 0x00, 0x00, 0x24, 0xF5};
// Command 0x23: Acquire image and upload eigenvalues
const byte CMD_GET_EIGEN[] = {0xF5, 0x23, 0x00, 0x00, 0x00, 0x00, 0x23, 0xF5};
const byte HANDSHAKE[] = {0xF5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF5};

// A simple, fast hash function (like FNV-1a)
constexpr unsigned int hash(const char* str, unsigned int h = 2166136261U) {
    return *str ? hash(str + 1, (h ^ *str) * 16777619U) : h;
}

void setup() {
    Serial.begin(115500); // For debugging output to the PC monitor
    
    pinMode(led_gpio, OUTPUT);

    pinMode(FP_RST_PIN, OUTPUT);
    digitalWrite(FP_RST_PIN, HIGH);
    delay(500);
    // Initialize the UART for the fingerprint sensor on specific pins (e.g., 16/17)
    FINGERPRINT_SERIAL.begin(BAUD_RATE, SERIAL_8N1, 17, 16); 
    Serial.println("UART Fingerprint Sensor Test Ready.");

/*
    Serial.println("--- Testing Communication with Handshake (0x00) ---");
    byte responseHead[8];
    
    if (sendCommand(HANDSHAKE, responseHead)) {
      Serial.println("✅ Handshake");
      FP_ready = true;
    } else {
      // If it returns false, the error from sendCommand() is already printed.
      Serial.println("❌ Handshake");
      FP_ready = false;
    }*/
}

void loop() {
  // Check for commands from companion app
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    unsigned int commandHash = hash(command.c_str());

    switch (commandHash) {
      case hash("green_light_on"):
        digitalWrite(led_gpio, HIGH); // Turn the LED on
        break;
      case hash("green_light_off"):
        digitalWrite(led_gpio, LOW);
        break;
      default:
        Serial.println("Unknown or unassigned command: ");
        Serial.print(command);
        break;
    }

    /*if (command.equals("green_light_on")) {
      digitalWrite(led_gpio, HIGH); // Turn the LED on
    } else if (command.equals("green_light_off")){
      digitalWrite(led_gpio, LOW);
    }*/
  }
}



// Function to calculate the Checksum (CHK) for an 8-byte command frame
byte calculateChecksum(const byte* command) {
    // Checksum is the XOR sum of bytes 2 through 6 (CMD, P1, P2, P3, 0)
    byte chk = 0;
    for (int i = 1; i <= 5; i++) {
        chk ^= command[i];
    }
    return chk;
}