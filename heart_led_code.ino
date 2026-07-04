// IG @diaawastaken LED heart Arduino code
// Please follow for future projects

// --- PIN DEFINITIONS ---
const int buttonPin = 2; // Interrupt pin for the button
const int topLed = A5;
const int bottomLed = A2; // MOVED FROM D3 TO A2

// Symmetrical sides (7 LEDs each)
const int leftSide[] = {11, 7, 8, 9, 6, 5, 12};
const int rightSide[] = {13, A4, A3, 10, 4, A1, A0};

// Full clockwise rotation: Top -> Right Side -> Bottom -> Left Side (Bottom to Top)
const int clockwisePath[] = {A5, 13, A4, A3, 10, 4, A1, A0, A2, 12, 5, 6, 9, 8, 7, 11};

// Array of all 16 LEDs for bulk on/off operations
const int allLeds[] = {A2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0, A1, A3, A4, A5};
const int numTotalLeds = 16;

// --- STATE VARIABLES ---
volatile int stage = 1;
volatile bool stageChanged = false;
volatile unsigned long lastButtonPress = 0;

// Animation speeds (milliseconds) - SPLIT FOR INDEPENDENT CONTROL
const int stage2Speed = 130;    // Tuned for the top-to-bottom drop
const int stage3Speed = 250;    // Slower for the binary alternating flicker
const int clockwiseSpeed = 90; 
const int blinkSpeed = 600;    

void setup() {
  // Initialize all LEDs as outputs and turn them off
  for (int i = 0; i < numTotalLeds; i++) {
    pinMode(allLeds[i], OUTPUT);
    digitalWrite(allLeds[i], LOW);
  }

  // Initialize button with internal pull-up
  pinMode(buttonPin, INPUT_PULLUP);

  // Attach interrupt to Pin 2 for instant button response
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonISR, FALLING);
}

void loop() {
  stageChanged = false; // Reset the flag before entering a stage

  switch (stage) {
    case 1: stage1(); break;
    case 2: stage2(); break;
    case 3: stage3(); break;
    case 4: stage4(); break;
    case 5: stage5(); break;
    case 6: stage6(); break;
  }
}

// --- INTERRUPT SERVICE ROUTINE (Debounce & State Change) ---
void buttonISR() {
  unsigned long currentMillis = millis();
  // 200ms debounce window
  if (currentMillis - lastButtonPress > 200) {
    stage++;
    if (stage > 6) {
      stage = 1;
    }
    lastButtonPress = currentMillis;
    stageChanged = true; // Signal to break out of current animation loop
  }
}

// --- SMART DELAY ---
// Allows animations to pause, but breaks out IMMEDIATELY if the button is pressed
void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    if (stageChanged) return;
  }
}

// --- HELPER FUNCTIONS ---
void turnAllOff() {
  for (int i = 0; i < numTotalLeds; i++) {
    digitalWrite(allLeds[i], LOW);
  }
}

void turnAllOn() {
  for (int i = 0; i < numTotalLeds; i++) {
    digitalWrite(allLeds[i], HIGH);
  }
}

// Maps a step (0 to 8) to the corresponding horizontal "level" of LEDs
void lightLevel(int level) {
  if (level == 0) {
    digitalWrite(topLed, HIGH); // Level 0: Top
  } 
  else if (level >= 1 && level <= 7) {
    int idx = level - 1;
    digitalWrite(leftSide[idx], HIGH);  // Level 1-7: Symmetrical sides
    digitalWrite(rightSide[idx], HIGH);
  } 
  else if (level == 8) {
    digitalWrite(bottomLed, HIGH); // Level 8: Bottom
  }
}

// --- STAGE LOGIC ---

// Stage 1: All OFF
void stage1() {
  turnAllOff();
  while (!stageChanged) {
    delay(10); // Idle, waiting for interrupt
  }
}

// Stage 2: Top to bottom pairs
void stage2() {
  while (!stageChanged) {
    for (int i = 0; i <= 8; i++) { // 9 total vertical levels (0 to 8)
      turnAllOff();
      lightLevel(i);
      smartDelay(stage2Speed); 
      if (stageChanged) return;
    }
  }
}

// Stage 3: Alternating Binary Flow
void stage3() {
  while (!stageChanged) {
    // Frame 1: Turn ON all "Even" levels (0, 2, 4, 6, 8)
    turnAllOff();
    for (int i = 0; i <= 8; i += 2) {
      lightLevel(i);
    }
    smartDelay(stage3Speed); 
    if (stageChanged) return;

    // Frame 2: Turn ON all "Odd" levels (1, 3, 5, 7)
    turnAllOff();
    for (int i = 1; i <= 8; i += 2) {
      lightLevel(i);
    }
    smartDelay(stage3Speed); 
    if (stageChanged) return;
  }
}

// Stage 4: Chaser going clockwise around the heart
void stage4() {
  while (!stageChanged) {
    for (int i = 0; i < numTotalLeds; i++) {
      turnAllOff();
      digitalWrite(clockwisePath[i], HIGH);
      smartDelay(clockwiseSpeed); 
      if (stageChanged) return;
    }
  }
}

// Stage 5: All ON / All OFF (Swapped from Stage 6)
void stage5() {
  while (!stageChanged) {
    turnAllOn();
    smartDelay(blinkSpeed);
    if (stageChanged) return;
    
    turnAllOff();
    smartDelay(blinkSpeed);
    if (stageChanged) return;
  }
}

// Stage 6: All ON steady (Swapped from Stage 5)
void stage6() {
  turnAllOn();
  while (!stageChanged) {
    delay(10);
  }
}