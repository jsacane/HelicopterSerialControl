/* 
   Controls Syma S107G helicopter through Serial Monitor commands.
   Written by Jack Sacane, based off of code and research by Kerry D. Wong and Andrew Barry (Not affiliated).
   
   Commands:
   ---------
   t = Take off
   h = Hover
   l = Land
   w/s = Pitch forward/backward
   a/d = Pitch left/right
   q/e = Trim left/right
   u = Increase throttle
   j = Decrease throttle
   0 = Kill throttle
   1-9 = Throttle "settings" (Scales throttle by magnitude of number)
   r = Reset yaw, pitch, and trim
   
   Note: Multiple commands can be sent at the same time, but they will be read individually left to right. For example
         sending wwwww sends the w command 5 times, increasing forward pitch by 15.
*/


#include <TimerOne.h>

const int IR_PIN = 3;
const unsigned long DURATION = 180000;
const int HEADER_DURATION = 2000;
const int HIGH_DURATION = 380;
const int ZERO_LOW_DURATION = 220;
const int ONE_LOW_DURATION = 600;

byte yawCmd, pitchCmd, throttleCmd, trimCmd;



void setup() {

  Serial.begin(9600);
  
  pinMode(IR_PIN, OUTPUT);
  digitalWrite(IR_PIN, LOW);
  
  // Initialize globals to be neutral
  yawCmd = 63;
  pitchCmd = 63;
  throttleCmd = 0;
  trimCmd = 52;
  
  // Setup interrupt interval: 180ms
  Timer1.initialize(DURATION);
  Timer1.attachInterrupt(sendCommand);
  
  // Setup PWM: f=38Khz PWM=0.5  
  byte v = 8000 / 38;
  TCCR2A = _BV(WGM20);
  TCCR2B = _BV(WGM22) | _BV(CS20); 
  OCR2A = v;
  OCR2B = v / 2;

}

// Gets called every time serial data is received
void serialEvent() {

  char cmd = Serial.read();
  Serial.println();
  Serial.print("Command received is: ");
  Serial.println(cmd);
  
  switch(cmd) {
  
    case 't': // Take off and hover
      Serial.println("Taking off...");
      throttleCmd = 127;
      delay(1000);
      throttleCmd = 73; // Approximate hover throttle
      break;
      
    case 'h': // Hover
      Serial.println("Hovering...");
      throttleCmd = 73;
      yawCmd = 63;
      pitchCmd = 63;
      break;
      
    case 'l': // Land
      Serial.println("Landing...");
      yawCmd = 63;
      pitchCmd = 63;
      while (throttleCmd > 0) {
        throttleCmd--;
        delay(50);
      }
      break;
      
    case 'w': // Pitch forward
      Serial.print("Pitch is: ");
      pitchCmd -= 5;
      Serial.println(pitchCmd);
      break;
      
    case 'a': // Turn left
      Serial.print("Yaw is: ");
      yawCmd -= 5;
      Serial.println(yawCmd);
      break;
      
    case 's': // Pitch backward
      Serial.print("Pitch is: ");
      pitchCmd += 5;
      Serial.println(pitchCmd);
      break;
     
    case 'd': // Turn right
      Serial.print("Yaw is: ");
      yawCmd += 5;
      Serial.println(yawCmd);
      break;
    
    case 'q': // Trim left
      Serial.print("Trim is: ");
      trimCmd -= 5;
      Serial.println(trimCmd);
      break;
      
    case 'e': // Trim right
      Serial.print("Trim is: ");
      trimCmd += 5;
      Serial.println(trimCmd);
      break;
      
    case 'r': // Reset yaw, pitch, and trim
      Serial.println("Resetting yaw, pitch, and trim");
      yawCmd = 63;
      pitchCmd = 63;
      trimCmd = 52;
      break;
    
    case 'u':  // Increase throttle
      Serial.print("Throttle is ");
      if (throttleCmd < 127-3) {
        throttleCmd += 3;
      }
      Serial.println(throttleCmd);
      break;

    case 'j':  // Decrease throttle
      Serial.print("Trottle is ");
      if (throttleCmd > 3) {
        throttleCmd -= 3;
      }
      Serial.println(throttleCmd);
      break;
    
    case '0': // Kill throttle
      Serial.println("Killing throttle");
      throttleCmd = 0;
      yawCmd = 63;
      pitchCmd = 63;
      break;
      
    case '1': // Throttle "settings" - product of char and 14 (9 * 14 = 126)
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      Serial.print("Throttle is: ");
      throttleCmd = atoi(&cmd) * 14; // Just converts char to const char*
                                     // then scales appropriately
      Serial.println(throttleCmd);   
      break;
      
    default:
      Serial.println("Invalid command. Check command reference.");
      break;
      
  }

}

// "Header" sequence
void sendHeader() {

  TCCR2A |= _BV(COM2B1);
  delayMicroseconds(HEADER_DURATION);
  TCCR2A &= ~_BV(COM2B1);
  delayMicroseconds(HEADER_DURATION);
  TCCR2A |= _BV(COM2B1);
  delayMicroseconds(HIGH_DURATION);
  TCCR2A &= ~_BV(COM2B1);

}

// "Zero" sequence
void sendZero() {
  
  delayMicroseconds(ZERO_LOW_DURATION);
  TCCR2A |= _BV(COM2B1);
  delayMicroseconds(HIGH_DURATION);
  TCCR2A &= ~_BV(COM2B1);

}

// "One" sequence
void sendOne() {

  delayMicroseconds(ONE_LOW_DURATION);
  TCCR2A |= _BV(COM2B1);
  delayMicroseconds(HIGH_DURATION);
  TCCR2A &= ~_BV(COM2B1);

}

void sendCommand() {
  
  byte b;
  
  sendHeader();
  
  // Check each bit in binary value of yaw/pitch/throttle/trim 
  // against each bit in binary value of 1 shifted left by i 
  // with & operator to see if the current binary value of 
  // yaw/pitch/throttle/trim is a 0 or 1. Then shift it back to least
  // significant bit (to keep consistency with b = 1 or b = 0)
  // If it's a 1, send 1-sequence. If it's a 0, send 0-sequence
  
  for (int i=7; i>=0; i--) { // Note: Start at 8th bit and go down
                             //       because helicopter reads byte 
                             //       left to right

    b = (yawCmd & (1 << i)) >> i;
    if (b > 0) sendOne(); else sendZero();
  
  }
  
  for (int i=7; i>=0; i--) {
  
    b = (pitchCmd & (1 << i)) >> i;
    if (b > 0) sendOne(); else sendZero();
  
  }
  
  for (int i=7; i>=0; i--) {
  
    b = (throttleCmd & (1 << i)) >> i;
    if (b > 0) sendOne(); else sendZero();
  
  }
  
  for (int i=7; i>=0; i--) {
  
    b = (trimCmd & (1 << i)) >> i;
    if (b > 0) sendOne(); else sendZero();
  
  }

}

void loop() {}
