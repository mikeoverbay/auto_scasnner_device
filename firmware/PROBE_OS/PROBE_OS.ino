#include <FastLED.h>

#define USB_PRODUCT "AutoProbe"
#define USB_MANUFACTURER "Coffee CNC"

// --- LED SETUP ---
#define LED_PIN 5
#define NUM_LEDS 1
#define LED_TYPE WS2811
#define COLOR_ORDER RGB
#define BRIGHTNESS 128
CRGB leds[NUM_LEDS];

// --- OPTICAL SWITCH INPUTS ---
#define HOME_PIN 21
#define PROBE_PIN 20

#define fast 600
#define slow 8000
#define TOUCH_OVERSHOOT_STEPS 10

// --- STEPPER PINOUT ---
#define STEP_PIN 16  // A2
#define DIR_PIN 10
#define MS3_PIN 14  // A0

// --- Motion Control Variables ---
int stepDelay = fast;
bool dirForward = true;
bool ERROR = false;
bool zSet = false;
float current_position = 0.0;
const float step_pitch = (0.03937 * 16 * 3.141592) / 1600.0;  // inches per step
const float max_travel = 4.5;

// --- Absolute Machine Position ---
float machinePosition = 0.0;  // absolute Z height (0 = home, increases downward)
const float machineMin = 0.0;
const float machineMax = 4.5;
double zRetract = 1.000;  // default, inches (or whatever you use)

// --- State Machine ---
enum State {
  HOMING,
  INIT_PROBING,
  PROBE_LOCATION,
  IDLE,
  BUSY
};

State currentState = HOMING;


// ============================================================================
// SETUP: Initializes serial, LED, GPIO, driver configuration.
// ============================================================================
void setup() {
  Serial.begin(115200);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  setLED(CRGB::Black);

  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(MS3_PIN, OUTPUT);
  pinMode(HOME_PIN, INPUT_PULLUP);
  pinMode(PROBE_PIN, INPUT_PULLUP);

  digitalWrite(MS3_PIN, LOW);  // 1/8 step mode
  digitalWrite(DIR_PIN, HIGH);

  setLED(CRGB::Red);  // startup state
}


// ============================================================================
// MAIN LOOP: Dispatches state machine actions.
// ============================================================================
void loop() {
  handleSerial();

  switch (currentState) {
    case HOMING:
      setLED(ERROR ? CRGB::Red : CRGB::Blue);
      homingRoutine();
      zSet = false;
      break;

    case INIT_PROBING:
      setLED(CRGB::Blue);
      initProbingRoutine();
      break;

    case PROBE_LOCATION:
      if (!zSet) {
        currentState = INIT_PROBING;
        break;
      }
      setLED(CRGB::Blue);
      probeLocationRoutine();
      break;

    case IDLE:
      setLED(ERROR ? CRGB::Red : CRGB::White);
      break;

    case BUSY:
      setLED(CRGB::Green);
      break;
  }
}


// ============================================================================
// HOMING ROUTINE:
//  - Rapid moves downward until HOME switch activates
//  - Slowly moves upward until switch opens (precise home position)
//  - Resets machinePosition and current_position to zero
// ============================================================================
void homingRoutine() {
  stepDelay = fast;
  while (digitalRead(HOME_PIN) != HIGH) {
    stepBackward();
  }

  stepDelay = slow;
  while (digitalRead(HOME_PIN) == HIGH) {
    stepForward();
  }
  current_position = 0.0;
  machinePosition = 0.0;

  Serial.println("homed");
  currentState = IDLE;
}


// ============================================================================
// INIT PROBING ROUTINE:
//  - Used for initial reference touch
//  - Touches the part
//  - Backs off
//  - Retracts upward exactly 1.0 inch
//  - No delta output (reference only)
// ============================================================================
void initProbingRoutine() {
  ERROR = false;
  current_position = 0.0;

  stepDelay = fast;
  while (!digitalRead(PROBE_PIN)) {
    if (machinePosition >= machineMax) {
      setLED(CRGB::Red);
      currentState = HOMING;
      ERROR = true;
      Serial.println("ERROR");
      return;
    }
    stepForward();
  }
  for (int i = 0; i < TOUCH_OVERSHOOT_STEPS; i++) {
    if (machinePosition >= machineMax) {
      setLED(CRGB::Red);
      currentState = HOMING;
      ERROR = true;
      Serial.println("ERROR");
      return;
    }
    stepForward();
  }
  stepDelay = slow;
  while (digitalRead(PROBE_PIN)) {
    stepBackward();
  }

  // Retract 1.0 inch upward
  int retractSteps = (int)(zRetract / step_pitch);

  stepDelay = fast;
  for (int i = 0; i < retractSteps; i++) {
    stepBackward();
    if (machinePosition <= machineMin) break;
  }
  Serial.println("0.0");
  zSet = true;
  currentState = BUSY;
}


// ============================================================================
// PROBE LOCATION ROUTINE:
//  - Assumes the machine ALWAYS starts probing exactly 1.0" above the part
//  - Moves down rapidly until probe closes
//  - Raises slowly until probe opens (true surface point)
//  - Calculates deltaZ = (surface height - start height)
//  - Prints ONLY deltaZ (in inches, 4 decimal places)
//  - Retracts back to EXACT start height (1.0 above part)
// ============================================================================
void probeLocationRoutine() {

  float startHeight = machinePosition;  // starting Z height (1.0" above part)

  // FAST SEEK DOWN
  stepDelay = fast;
  while (!digitalRead(PROBE_PIN)) {

    if (machinePosition >= machineMax) {
      setLED(CRGB::Red);
      currentState = HOMING;
      ERROR = true;
      Serial.println("ERROR");
      return;
    }

    stepForward();
  }
  for (int i = 0; i < TOUCH_OVERSHOOT_STEPS; i++) {
    if (machinePosition >= machineMax) {
      setLED(CRGB::Red);
      currentState = HOMING;
      ERROR = true;
      Serial.println("ERROR");
      return;
    }
    stepForward();
  }
  // SLOW BACKUP UNTIL SWITCH OPENS
  stepDelay = slow;
  while (digitalRead(PROBE_PIN)) {
    stepBackward();
    if (machinePosition <= machineMin) break;
  }

  // DELTA CALCULATION
  float deltaZ = machinePosition - startHeight;


  // RETRACT BACK TO EXACT START HEIGHT
  stepDelay = fast;
  while (machinePosition > startHeight) {
    stepBackward();
    if (machinePosition <= machineMin) break;
  }

  // Print ONLY the probing delta
  Serial.println(zRetract - deltaZ - 0.001, 3);
  currentState = BUSY;
}


// ============================================================================
// STEPPER CONTROL:
//  stepForward(): moves Z DOWN and increments absolute position
//  stepBackward(): moves Z UP and decrements absolute position
//  pulseStep(): generates one pulse at the configured delay rate
// ============================================================================
void stepForward() {
  digitalWrite(DIR_PIN, HIGH);
  pulseStep();
  machinePosition += step_pitch;
}

void stepBackward() {
  digitalWrite(DIR_PIN, LOW);
  pulseStep();
  machinePosition -= step_pitch;
  if (machinePosition < machineMin) machinePosition = machineMin;
}

void pulseStep() {
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(stepDelay);
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(stepDelay);
}


// ============================================================================
// LED HELPER: Sets LED color for visual state indication.
// ============================================================================
void setLED(const CRGB &color) {
  leds[0] = color;
  FastLED.show();
}


// ============================================================================
// SERIAL COMMAND HANDLER (CASE-INSENSITIVE):
//  Commands:
//    H → Homing
//    I → Init-Probing
//    P → Probe-Location
//    X → Idle
//  Lowercase and uppercase both work.
// ============================================================================

void handleSerial() {
  static char line[48];
  static uint8_t idx = 0;

  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\r') continue;

    if (c == '\n') {
      line[idx] = 0;  // null terminate
      idx = 0;
      processLine(line);
      continue;
    }

    if (idx < sizeof(line) - 1) {
      line[idx++] = c;
    } else {
      // overflow: reset buffer
      idx = 0;
    }
  }
}

void processLine(char *s) {
  // trim leading spaces
  while (*s == ' ' || *s == '\t') s++;

  // uppercase the command portion (up to ':'), keep number as-is
  for (char *p = s; *p && *p != ':'; p++) {
    if (*p >= 'a' && *p <= 'z') *p -= 32;
  }

  // 1) Single-letter commands
  if (s[0] && s[1] == 0) {
    switch (s[0]) {
      case 'H':
        currentState = HOMING;
        ERROR = false;
        break;
      case 'I': currentState = INIT_PROBING; break;
      case 'P': currentState = PROBE_LOCATION; break;
      case 'X': currentState = IDLE; break;
    }
    return;
  }

  // 2) Key/value commands like "ZR:0.750"
  if (strncmp(s, "ZR:", 3) == 0) {
    double v = atof(s + 3);  // parse number after "ZR:"
    zRetract = v;

    // optional ack
    Serial.print("OK ZR=");
    Serial.println(zRetract, 3);
    return;
  }

  // unknown command (optional)
  Serial.print("ERR ");
  Serial.println(s);
}

