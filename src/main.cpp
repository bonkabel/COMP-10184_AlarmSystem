// COMP-10184 – Mohawk College
/**
 * Amy Abel, 000749165, October 17th 2022
 * I completed stage 5
 * */
#include <Arduino.h>
// digital input pin definitions

#define PIN_PIR D5
#define PIN_BUTTON D6
// The duration the light should be blinking for
#define TIMER_DURATION 10
// The interval between blinks
#define BLINK_INTERVAL 0.25

// The current state of the light true = blinking, false = not blinking
bool lightBlinking = false;
// The current state of the light true = solid on, false = not solid on
bool lightOn = false;
// When the light is first starts blinking
float startTime;
// The last time the light blinked
// Used to check if it should blink again
float lastBlink;
// If we should stop the light from doing anything
bool stopLight;
// If the button has already been pressed
// This is to prevent multiple actions in one button press
bool alreadyPressed;
// if the button is pressed, assume starting not pressed
bool buttonPressed = 0;
// If motion was detected, assume starting with no motion
bool motionDetected = false;

// converts millis to seconds
// makes timing stuff simple and readable
float millisToSeconds(long millis) {
	return float(millis / 1000.0);
}

void tryBlink() {
	if (millisToSeconds(millis()) > lastBlink + BLINK_INTERVAL) {
		lastBlink = millisToSeconds(millis());
		digitalWrite(LED_BUILTIN, false);
		delay(1);
		digitalWrite(LED_BUILTIN, true);
	}
}

// *************************************************************
void setup() {
	// configure the USB serial monitor
	Serial.begin(115200);
	// configure the LED output
	pinMode(LED_BUILTIN, OUTPUT);
	// PIR sensor is an INPUT
	pinMode(PIN_PIR, INPUT);
	// Button is an INPUT
	pinMode(PIN_BUTTON, INPUT_PULLUP);

	// make the light start off
	// unsure why this is needed but it works
	digitalWrite(LED_BUILTIN, true);
}

// Gets the inputs from the PIUR sensor and the button
void getInputs() {
	// read PIR sensor (true = Motion detected!). As long as there
	// is motion, this signal will be true. About 2.5 seconds after
	// motion stops, the PIR signal will become false.
	motionDetected = digitalRead(PIN_PIR);
	buttonPressed = !digitalRead(PIN_BUTTON);
}

// Handles the actual alarm code and decides what to do
void handleAlarm() {
	// Sets alreadyPressed to false when the button is let go
	if (alreadyPressed && !buttonPressed) {
		alreadyPressed = false;
	}

	// if there is motion detected and the light isn't already on, turn it on
	if (motionDetected && !lightBlinking && !stopLight) {
		lightBlinking = true;
		tryBlink();
		startTime = millisToSeconds(millis());
	}
	
	// if the light should be blinking, try to blink
	if (lightBlinking) {
		// Stop the light when the button is pressed
		if (buttonPressed) {
			lightBlinking = false;
			stopLight = true;
			alreadyPressed = true;
		}

		tryBlink();
	}

	// if the light is blinking and it has been longer than TIMER_DURATION since it started blinking
	// set lightOn to true
	if (lightBlinking && ( millisToSeconds(millis()) > (startTime + TIMER_DURATION) ) ) {
		lightBlinking = false;
		
		digitalWrite(LED_BUILTIN, true);
		lightOn = true;
	}
	
	// if the light should be solid on, make it solid on
	if (lightOn) {
		digitalWrite(LED_BUILTIN, false);
	}

	// Re arms the alarm
	if (stopLight && buttonPressed && !alreadyPressed) {
		stopLight = false;
	}
}

// *************************************************************
void loop() {
	getInputs();

	handleAlarm();
}

