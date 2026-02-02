#define LED_PIN 13

bool ledState = false;

void ledInit(void) {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    ledState = false;
}

void ledOn(void) {
    digitalWrite(LED_PIN, HIGH);
    ledState = true;
}

void ledOff(void) {
    digitalWrite(LED_PIN, LOW);
    ledState = false;
}

bool ledGetState(void) {
    return ledState;
}

#define BUFFER_SIZE 32
char commandBuffer[BUFFER_SIZE];
uint8_t bufferIndex = 0;

void processCommand(const char* command);

void setup() {
    Serial.begin(9600);
    ledInit();
    Serial.println("Comenzi: led on, led off, status");
}

void loop() {
    while (Serial.available() > 0) {
        char c = Serial.read();
        
        if (c == '\n' || c == '\r') {
            if (bufferIndex > 0) {
                commandBuffer[bufferIndex] = '\0';
                processCommand(commandBuffer);
                bufferIndex = 0;
            }
        } else if (bufferIndex < BUFFER_SIZE - 1) {
            commandBuffer[bufferIndex++] = c;
        }
    }
}

void processCommand(const char* command) {
    char cmd[BUFFER_SIZE];
    
    for (uint8_t i = 0; command[i] != '\0' && i < BUFFER_SIZE - 1; i++) {
        cmd[i] = tolower(command[i]);
        cmd[i + 1] = '\0';
    }
    
    if (strcmp(cmd, "led on") == 0) {
        ledOn();
        Serial.println("[OK] LED aprins!");
    } 
    else if (strcmp(cmd, "led off") == 0) {
        ledOff();
        Serial.println("[OK] LED stins!");
    }
    else if (strcmp(cmd, "status") == 0) {
        if (ledGetState()) {
            Serial.println("[INFO] LED-ul este APRINS");
        } else {
            Serial.println("[INFO] LED-ul este STINS");
        }
    }
    else {
        Serial.println("[EROARE] Comanda necunoscuta!");
    }
}