#include <stdio.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

//===========================================
// LCD MODULE
//===========================================
LiquidCrystal_I2C lcd(0x27, 20, 4);  // Adresa I2C, 20 coloane, 4 randuri

void lcdInit(void) {
    lcd.init();
    lcd.backlight();
    lcd.clear();
}

void lcdPrint(const char* text, int col, int row) {
    lcd.setCursor(col, row);
    lcd.print(text);
}

void lcdClear(void) {
    lcd.clear();
}

//===========================================
// LED MODULE
//===========================================
#define LED_VALID_PIN 12
#define LED_INVALID_PIN 2

void ledInit(void) {
    pinMode(LED_VALID_PIN, OUTPUT);
    pinMode(LED_INVALID_PIN, OUTPUT);
    digitalWrite(LED_VALID_PIN, LOW);
    digitalWrite(LED_INVALID_PIN, LOW);
}

void ledValidOn(void) {
    digitalWrite(LED_VALID_PIN, HIGH);
    digitalWrite(LED_INVALID_PIN, LOW);
}

void ledInvalidOn(void) {
    digitalWrite(LED_VALID_PIN, LOW);
    digitalWrite(LED_INVALID_PIN, HIGH);
}

void ledAllOff(void) {
    digitalWrite(LED_VALID_PIN, LOW);
    digitalWrite(LED_INVALID_PIN, LOW);
}

//===========================================
// KEYPAD MODULE
//===========================================
const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}
};

byte rowPins[ROWS] = {22, 24, 26, 28};
byte colPins[COLS] = {30, 32, 34};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

char keypadGetKey(void) {
    return keypad.getKey();
}

//===========================================
// STDIO MODULE
//===========================================
static int lcdPutchar(char c, FILE *stream) {
    lcd.write(c);
    return c;
}

FILE lcdStream;

void stdioInit(void) {
    fdev_setup_stream(&lcdStream, lcdPutchar, NULL, _FDEV_SETUP_WRITE);
    stdout = &lcdStream;
}

//===========================================
// MAIN MODULE
//===========================================
#define CODE_LENGTH 4
#define CORRECT_CODE "2222"

char enteredCode[CODE_LENGTH + 1] = "";
uint8_t codeIndex = 0;

void resetCode(void) {
    memset(enteredCode, 0, sizeof(enteredCode));
    codeIndex = 0;
    ledAllOff();
    lcdClear();
    lcd.setCursor(0, 0);
    printf("Enter PIN:");
    lcd.setCursor(0, 1);
}

void checkCode(void) {
    if (strcmp(enteredCode, CORRECT_CODE) == 0) {
        lcdClear();
        lcd.setCursor(0, 0);
        printf("Access Granted!");
        ledValidOn();
    } else {
        lcdClear();
        lcd.setCursor(0, 0);
        printf("Access Denied!");
        ledInvalidOn();
    }
    delay(2000);
    resetCode();
}

void setup() {
    Serial.begin(9600);
    lcdInit();
    ledInit();
    stdioInit();
    resetCode();
}

void loop() {
    char key = keypadGetKey();
    
    if (key) {
        if (key == '#') {
            // Verificare cod
            checkCode();
        } 
        else if (key == '*') {
            // Sterge codul
            resetCode();
        } 
        else if (codeIndex < CODE_LENGTH) {
            // Adauga cifra
            enteredCode[codeIndex] = key;
            codeIndex++;
            enteredCode[codeIndex] = '\0';
            
            // Afiseaza * pentru fiecare cifra
            lcd.setCursor(0, 1);
            for (int i = 0; i < codeIndex; i++) {
                printf("*");
            }
        }
    }
}