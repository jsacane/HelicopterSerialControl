/* 
   To add:
   Throttle graphics
   List of commands
   Readout of "cmd" values
*/

import processing.serial.*;
Serial arduinoPort;
PImage gauge_img;

int r_up, g_up, b_up, r_down, g_down, b_down, r_left, 
    g_left, b_left, r_right, g_right, b_right;
    
byte yawCmd, pitchCmd, throttleCmd;
    
float gauge_angle = 133;

void setup() {

  size(600,600);
  println(Serial.list());
  arduinoPort = new Serial(this, "/dev/cu.usbmodem1421", 9600); //Use correct port
  
  gauge_img = loadImage("gauge.png");
  
  // Colors of arrows
  r_up = 255;
  g_up = 0;
  b_up = 0;
  
  r_down = 0;
  g_down = 255;
  b_down = 0;
  
  r_left = 255;
  g_left = 0;
  b_left = 255;
  
  r_right = 0;
  g_right = 0;
  b_right = 255;
  
  // Yaw/Pitch/Throttle values
  yawCmd = 63;
  pitchCmd = 63;
  throttleCmd = 0;

}

void draw() {
  
  background(255);
  
  // Draw arrows for WASD control
  strokeWeight(10);
  stroke(r_up,g_up,b_up);
  drawArrow(150,100,75,270);
  stroke(r_down,g_down,b_down);
  drawArrow(150,150,75,90);
  stroke(r_right,g_right,b_right);
  drawArrow(175,125,75,0);
  stroke(r_left,g_left,b_left);
  drawArrow(125,125,75,180);
  
  // Draw throttle gauge
  image(gauge_img, 325, 20, 225, 225);
  strokeWeight(5);
  stroke(0, 0, 0);
  drawArrow(437, 132, 75, gauge_angle);
  
  // Draw readout of current yaw/pitch/throttle values
  textSize(32);
  fill(0, 102, 153);
  text("Pitch Value", 50, 350);
  text("=", 300, 350);
  text(str(pitchCmd), 350, 350);
  text("Yaw Value", 50, 400);
  text("=", 300, 400);
  text(str(yawCmd), 350, 400);
  text("Throttle Value", 50, 450);
  text("=", 300, 450);
  text(str(throttleCmd), 350, 450);
  
}

void keyPressed() {

  switch(key) {
    // WASD arrows, controls pitch and yaw (directional movement)
    case 'w':
    case 'W':
      r_up = 255;
      g_up = 153;
      b_up = 153;
      if (pitchCmd >= 5) pitchCmd -= 5;
      else pitchCmd = pitchCmd;
      arduinoPort.write('w');
      break;
    case 'a':
    case 'A':
      r_left = 255;
      g_left = 153;
      b_left = 255;
      if (yawCmd >= 5) yawCmd -= 5;
      else yawCmd = yawCmd;
      arduinoPort.write('a');
      break; 
    case 's':
    case 'S':
      r_down = 153;
      g_down = 255;
      b_down = 153;
      if (pitchCmd <= 122) pitchCmd += 5;
      else pitchCmd = pitchCmd;
      arduinoPort.write('s');
      break; 
    case 'd':
    case 'D':
      r_right = 153;
      g_right = 153;
      b_right = 255;
      if (yawCmd <= 122) yawCmd += 5;
      else yawCmd = yawCmd;
      arduinoPort.write('d');
      break;
    
    case 't': // Take off
    case 'T':
      gauge_angle = 257.78;
      throttleCmd = 80;
      arduinoPort.write('t');
      break;
    case 'h': // Hover
    case 'H':
      gauge_angle = 257.78;
      throttleCmd = 80;
      arduinoPort.write('h');
      break;
    case 'l': // Land
    case 'L':
      gauge_angle = 133;
      throttleCmd = 0;
      arduinoPort.write('l');
      break;
    case 'r': // Reset yaw and pitch
    case 'R':
      yawCmd = 63;
      pitchCmd = 63;
      arduinoPort.write('r');
      break;
    case '0': // Kill throttle
      gauge_angle = 133;
      throttleCmd = 0;
      yawCmd = 63;
      pitchCmd = 63;
      arduinoPort.write('0');
      break;
      
    // Throttle "settings"
    case '1':
      arduinoPort.write('1');
      break;
    case '2':
      arduinoPort.write('2');
      break;
    case '3':
      arduinoPort.write('3');
      break;
    case '4':
      arduinoPort.write('4');
      break;
    case '5':
      arduinoPort.write('5');
      break;
    case '6':
      arduinoPort.write('6');
      break;
    case '7':
      arduinoPort.write('7');
      break;
    case '8':
      arduinoPort.write('8');
      break;
    case '9':
      arduinoPort.write('9');
      break;
    
    default:
      break;
  
  }
  
  switch(keyCode) {
    //Arrow keys, increases/decreases throttle
    case UP:
      arduinoPort.write('u');
      if (gauge_angle < 406) {
        gauge_angle += 6.837;
        throttleCmd += 3;
      }
      else gauge_angle = gauge_angle;
      break;
    case DOWN:
      arduinoPort.write('j');
      if (gauge_angle > 133) {
        gauge_angle -= 6.837;
        throttleCmd -= 3;
      }
      else gauge_angle = gauge_angle;
      break;
      
    default:
      break;
  
  }

}

void keyReleased() {

  switch(key) {
  // WASD arrows, resets colors
  case 'w':
  case 'W':
    r_up = 255;
    g_up = 0;
    b_up = 0;
    break;
  case 'a':
  case 'A':
    r_left = 255;
    g_left = 0;
    b_left = 255;
    break;  
  case 's':
  case 'S':
    r_down = 0;
    g_down = 255;
    b_down = 0;
    break; 
  case 'd':
  case 'D':
    r_right = 0;
    g_right = 0;
    b_right = 255;
    break;
      
  default:
    break;  
    
  }

}

// Helper function for drawing arrows
void drawArrow(int cx, int cy, int len, float angle) {
  
  pushMatrix();
  translate(cx, cy);
  rotate(radians(angle));
  line(0,0,len, 0);
  line(len, 0, len - 8, -8);
  line(len, 0, len - 8, 8);
  popMatrix();
  
}