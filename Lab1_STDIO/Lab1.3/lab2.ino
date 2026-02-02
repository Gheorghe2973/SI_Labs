#include <stdio.h>

//===========================================
// GLOBAL SIGNALS (Provider/Consumer)
//===========================================
volatile bool led1State = false;        // Task1 provides, Task2 consumes
volatile int blinkInterval = 500;       // Task3 provides, Task2 consumes
volatile int blinkCount = 0;            // Task2 provides, Idle consumes
volatile bool led2State = false;        // Task2 provides, Idle consumes

//===========================================
// PIN DEFINITIONS
//===========================================
#define LED1_PIN 13
#define LED2_PIN 12
#define BUTTON1_PIN 4
#define BUTTON2_PIN 3
#define BUTTON3_PIN 2

//===========================================
// TIMING CONFIGURATION
//===========================================
#define TASK1_PERIOD 50
#define TASK2_PERIOD 10
#define TASK3_PERIOD 50
#define IDLE_PERIOD 1000

unsigned long task1LastRun = 0;
unsigned long task2LastRun = 0;
unsigned long task3LastRun = 0;
unsigned long idleLastRun = 0;
unsigned long led2LastToggle = 0;

//===========================================
// STDIO MODULE
//===========================================
int serialPutchar(char c, FILE *stream) {
    Serial.write(c);
    return c;
}

FILE serialStream;

void stdioInit(void) {
    fdev_setup_stream(&serialStream, serialPutchar, NULL, _FDEV_SETUP_WRITE);
    stdout = &serialStream;
}

//===========================================
// LED MODULE
//===========================================
void ledInit(void) {
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
}

void led1On(void) {
    digitalWrite(LED1_PIN, HIGH);
    led1State = true;
}

void led1Off(void) {
    digitalWrite(LED1_PIN, LOW);
    led1State = false;
}

void led1Toggle(void) {
    if (led1State) {
        led1Off();
    } else {
        led1On();
    }
}

void led2On(void) {
    digitalWrite(LED2_PIN, HIGH);
    led2State = true;
}

void led2Off(void) {
    digitalWrite(LED2_PIN, LOW);
    led2State = false;
}

void led2Toggle(void) {
    if (led2State) {
        led2Off();
    } else {
        led2On();
    }
    blinkCount++;
}

//===========================================
// BUTTON MODULE
//===========================================
bool button1Pressed = true;
bool button2Pressed = true;
bool button3Pressed = true;

void buttonInit(void) {
    pinMode(BUTTON1_PIN, INPUT_PULLUP);
    pinMode(BUTTON2_PIN, INPUT_PULLUP);
    pinMode(BUTTON3_PIN, INPUT_PULLUP);
}

bool button1IsPressed(void) {
    return digitalRead(BUTTON1_PIN) == LOW;
}

bool button2IsPressed(void) {
    return digitalRead(BUTTON2_PIN) == LOW;
}

bool button3IsPressed(void) {
    return digitalRead(BUTTON3_PIN) == LOW;
}

//===========================================
// TASK 1: Button LED Toggle
// Provider: led1State
//===========================================
void task1_ButtonLED(void) {
    unsigned long currentTime = millis();
    
    if (currentTime - task1LastRun >= TASK1_PERIOD) {
        task1LastRun = currentTime;
        
        bool currentState = button1IsPressed();
        
        // Detect rising edge (button just pressed)
        if (currentState && !button1Pressed) {
            led1Toggle();
        }
        
        button1Pressed = currentState;
    }
}

//===========================================
// TASK 2: Blinking LED
// Consumer: led1State, blinkInterval
// Provider: blinkCount, led2State
//===========================================
void task2_BlinkLED(void) {
    unsigned long currentTime = millis();
    
    if (currentTime - task2LastRun >= TASK2_PERIOD) {
        task2LastRun = currentTime;
        
        // LED2 blinks only when LED1 is OFF
        if (!led1State) {
            if (currentTime - led2LastToggle >= blinkInterval) {
                led2LastToggle = currentTime;
                led2Toggle();
            }
        } else {
            // LED1 is ON, so turn off LED2
            if (led2State) {
                led2Off();
            }
        }
    }
}

//===========================================
// TASK 3: Blink Interval Control
// Provider: blinkInterval
//===========================================
void task3_IntervalControl(void) {
    unsigned long currentTime = millis();
    
    if (currentTime - task3LastRun >= TASK3_PERIOD) {
        task3LastRun = currentTime;
        
        bool btn2State = button2IsPressed();
        bool btn3State = button3IsPressed();
        
        // Button 2: Increment interval
        if (btn2State && !button2Pressed) {
            blinkInterval += 100;
            if (blinkInterval > 2000) {
                blinkInterval = 2000;
            }
        }
        
        // Button 3: Decrement interval
        if (btn3State && !button3Pressed) {
            blinkInterval -= 100;
            if (blinkInterval < 100) {
                blinkInterval = 100;
            }
        }
        
        button2Pressed = btn2State;
        button3Pressed = btn3State;
    }
}

//===========================================
// IDLE TASK: Reporting via STDIO
// Consumer: led1State, led2State, blinkCount, blinkInterval
//===========================================
void idleTask_Report(void) {
    unsigned long currentTime = millis();
    
    if (currentTime - idleLastRun >= IDLE_PERIOD) {
        idleLastRun = currentTime;
        
        printf("===== SYSTEM STATUS =====\n");
        printf("LED1 (Main): %s\n", led1State ? "ON" : "OFF");
        printf("LED2 (Blink): %s\n", led2State ? "ON" : "OFF");
        printf("Blink Interval: %d ms\n", blinkInterval);
        printf("Blink Count: %d\n", blinkCount);
        printf("=========================\n\n");
    }
}

//===========================================
// SETUP
//===========================================
void setup() {
    Serial.begin(9600);
    stdioInit();
    ledInit();
    buttonInit();
    
    printf("Sequential Task System Started\n");
    printf("Button 1: Toggle LED1\n");
    printf("Button 2: Increase blink interval\n");
    printf("Button 3: Decrease blink interval\n\n");
}

//===========================================
// MAIN LOOP - Sequential Execution
//===========================================
void loop() {
    task1_ButtonLED();
    task2_BlinkLED();
    task3_IntervalControl();
    idleTask_Report();
}