#include <LowPower.h>

// Button pins
#define BTN1 15
#define BTN2 16
#define BTN3 17
#define BTN4 18

// LED pins
#define LED_blue 5
#define LED_red 6
#define LED_green 9
#define LED_yellow 10

#define SRV 2   // Servo
#define BZR 3   // Piezzo buzzer
#define FLM 14  // IR flame sensor

int answers[] = {2, 1, 1, 3};
int LED_pins[] = {LED_blue, LED_red, LED_green, LED_yellow};
int BTN_states[4];
int current_question = 0;
int pulseWidth;
int buttons_pressed = 0;
int flame_threshold = 800;

// Arrays with melodies. The first element in the subarray is the frequency (0 is a rest), the second element is the duration.
double correct_melody[11][2] = {{659.3, 0.5}, {0, 0.05}, {659.3, 0.5}, {0, 0.5}, {659.3, 0.5}, {0, 0.5}, {554.4, 0.5}, {0, 0.05}, {659.3, 0.5}, {0, 0.5}, {783.9, 0.5}};
double happy_birthday_melody[25][2] = {{1046.5, 0.75}, {1046.5, 0.5}, {1174.7, 1}, {1046.5, 1}, {1396.9, 1}, {1318.5, 2}, {1046.5, 0.75}, {1046.5, 0.5}, {1174.7, 1}, {1046.5, 1}, {1568, 1}, {1396.9, 2}, {1046.5, 0.75}, {1046.5, 0.5}, {2093, 1}, {1760, 1}, {1396.9, 1}, {1318.5, 1}, {1174.7, 2}, {1975.5, 0.75}, {1975.5, 0.5}, {1760, 1}, {1396.9, 1}, {1568, 1}, {1396.9, 2}};
double congratulations_melody[25][2] = {{0, 1}, {349.2, 1}, {0, 1}, {493.88, 1}, {0, 0.5}, {392, 1}, {0, 0.5}, {329.63, 1}, {0, 0.5}, {440, 1}, {493.88, 1}, {466.16, 0.5}, {440, 1}, {392, 0.5}, {659.26, 0.5}, {783.99, 0.5}, {880, 1}, {698.46, 0.5}, {783.99, 0.5}, {0, 0.5}, {659.26, 1}, {523.25, 0.5}, {587.33, 0.5}, {493.88, 1}, {0, 0.5}};

// Secret directions in morse code. 0 - pause, 1 - dot, 2 - dash
int morse_code[38] = {2, 2, 2, 2, 1, 0, 2, 2, 1, 1, 1, 0, 1, 1, 1, 2, 2, 0, 1, 2, 2, 2, 2, 0, 2, 2, 2, 1, 1, 0, 2, 2, 1, 1, 1};


void setup() {
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);
  pinMode(BTN4, INPUT_PULLUP);

  pinMode(LED_red, OUTPUT);
  pinMode(LED_green, OUTPUT);
  pinMode(LED_yellow, OUTPUT);
  pinMode(LED_blue, OUTPUT);

  pinMode(BZR, OUTPUT);
  pinMode(SRV, OUTPUT);
  pinMode(FLM, INPUT);


  // Initialize servo angle to 90 (closed)
  setServoAngle(160);
  delay(5000);
  setServoAngle(91);

  // Sleep for a while
  for (int i = 0; i < 225; i++) {
     LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
  
  intro();

}

void loop() {
  BTN_states[0] = digitalRead(BTN1);
  BTN_states[1] = digitalRead(BTN2);
  BTN_states[2] = digitalRead(BTN3);
  BTN_states[3] = digitalRead(BTN4);

  buttons_pressed = 4 - (BTN_states[0] + BTN_states[1] + BTN_states[2] + BTN_states[3]);
  
  if (buttons_pressed == 1) { // If only one button pressed
    
    if (BTN_states[answers[current_question]] == 0) {
      
      fadein(LED_pins[current_question], 255);
      play(correct_melody, 300, 0, 11, 0);
      current_question++;
      
      if (current_question == 4) {
        play(congratulations_melody, 300, 0, 25, 1);
        play(congratulations_melody, 300, 3, 25, 1);
        setServoAngle(160);
        play(happy_birthday_melody, 480, 0, 25, 0);

        waitForFlame();
        playMorse();

        // Do nothing
        while(1){}
      }
    } else {
      wrong(LED_pins[current_question]);
    }
    
  }
}

void setServoAngle(int angle) {
  // Sets the servo to the specified angle
  
  pulseWidth = (angle * 10) + 600;
  
  for(int i=0; i<50; i++)
   {
   digitalWrite(SRV, HIGH);
   delayMicroseconds(pulseWidth);
   digitalWrite(SRV, LOW);
   delay(20);
 }
}


void intro () {
  // Executes the intro sequence
  
  play(happy_birthday_melody, 480, 0, 24, 0);
  
  for (int i = happy_birthday_melody[25]; i > 300; i--) {
    tone(BZR, i);
    delay(10);
  }

  for (int i = 0; i < 3; i++) {
    tone(BZR, 300);
    delay(200);
    noTone(BZR);
    delay(200);
  }
  
  noTone(BZR);
}

void fadein(int LED, int strength) { 
  // Fades in an LED to the specified strength

  for (int i = 0; i < strength; i++) {
    analogWrite(LED, i);
    delay(500/strength);
  }
}

void play(double notes[][2], double full_note_length, int from, int to, int flash_leds) {
  // Plays the sequence providedd in the notes array
  
  for (int i = from; i < to; i++) {

    if (flash_leds == 1) {
      for (int i = 0; i < 4; i++) {
        analogWrite(LED_pins[i], random(0, 255));
      }
    }
    
    if (notes[i][0] == 0) {
      noTone(BZR);
    } else {
      tone(BZR, notes[i][0]);
    }

    delay(full_note_length * notes[i][1]);

    if (i != to & notes[i+1][0] != 0) {
      noTone(BZR);
      delay(full_note_length * 0.05);
    }
  }

  // Turn off the buzzer and LEDs
  noTone(BZR);
  if (flash_leds == 1) {
    for (int i = 0; i < 4; i++) {
      analogWrite(LED_pins[i], 0);
    }
  }
}

void wrong(int LED) {
  // Beeps three times
  
  for (int i = 0; i < 3; i++) {
    tone(BZR, 400);
    analogWrite(LED, 100);
    delay(200);
    analogWrite(LED, 0);
    noTone(BZR);
    delay(200);
  }
}

void waitForFlame() {
  // Waits for the IR sensor to detect flame
  
  while (true) {
    if (analogRead(FLM) < flame_threshold)
      break;
  }
}

void playMorse() {
  // Plays the Morse sequence
  
  int duration = 800;
  int freq = 600;

  int i = 0;
  while (true) {
    
    if (morse_code[i] == 0) {
      noTone(BZR);
      delay(duration);
    } else if (morse_code[i] == 1) {
      tone(BZR, freq);
      delay(duration / 3);
    } else {
      tone(BZR, freq);
      delay(duration);
    }
    
    noTone(BZR);
    delay(100);
    i++;
    i %= 38;

    if (i == 0) 
      delay(3000);

    // Breaks the sequence
    if (digitalRead(BTN1) == 0) 
      break;
    
  }
}

  
