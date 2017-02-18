#include <IRremote.h>
#include <elapsedMillis.h>
#include "Keyboard.h"

int recvPin = 18;
unsigned long lastread = 0;
//mode 0 = premiere pro
//mode 1 = audition
short mode = 0;
short modes = 2;
char shuttle = 0;
IRrecv irrecv(recvPin);

//Constants
unsigned char colors[2][3] = {{86, 25, 189}, {31, 198, 140}};
unsigned short ctrl = 0x100;
unsigned short alt = 0x200;
unsigned short shift = 0x400;
unsigned short mapping[1][81] = {
  { //Mode 0
    ctrl|'s', //eject
    '\0', //power_tv
    '\0', //power_vcr
    '\0', //vol up
    '\0', //vol down
    '\0', //mute
    '\0', //q.v. tv
    '\0', //q.v. vcr
    '\0', //channel up tv
    '\0', //channel up vcr
    '\0', //channel down tv
    '\0', //channel down vcr
    '\0', //input_tv
    '\0', //input_vcr
    '\0', //display_tv
    '\0', //display_vcr
    '\0', //record
    ctrl|'z', //index-
    ctrl|shift|'z', //index+
    '\0', //vcr plus+
    KEY_LEFT_ARROW, //rew
    KEY_RIGHT_ARROW, //ff
    '\0', //stop
    ' ', //play
    '\0', //menu_tv
    ctrl|'k', //menu_vcr
    'k', //pause
    '\0', //outer_left_tv
    '\0', //outer_left_vcr_1
    '\0', //outer_left_vcr_2
    '\0', //outer_left_vcr_3
    '\0', //outer_left_vcr_4
    '\0', //outer_left_vcr_5
    '\0', //outer_left_vcr_6
    '\0', //outer_left_vcr_7
    'k', //outer_centered_vcr
    '\0', //outer_right_tv
    '\0', //outer_right_vcr_1
    '\0', //outer_right_vcr_2
    '\0', //outer_right_vcr_3
    '\0', //outer_right_vcr_4
    '\0', //outer_right_vcr_5
    '\0', //outer_right_vcr_6
    '\0', //outer_right_vcr_7
    '\0', //inner_left_tv
    KEY_UP_ARROW, //inner_left_vcr
    '\0', //inner_right_tv
    KEY_DOWN_ARROW, //inner_right_vcr
    '\0', //pip_source
    '\0', //pip
    '1', //1_tv
    '1', //1_vcr
    '2', //2_tv
    '2', //2_vcr
    '3', //3_tv
    '3', //3_vcr
    '\0', //otr_tv
    '\0', //otr_vcr
    '4', //4_tv
    '4', //4_vcr
    '5', //5_tv
    '5', //5_vcr
    '6', //6_tv
    '6', //6_vcr
    '\0', //vcrtv
    '7', //7_tv
    '7', //7_vcr
    '8', //8_tv
    '8', //8_vcr
    '9', //9_tv
    '9', //9_vcr
    '\0', //cancel_tv
    '\0', //cancel_vcr
    '\0', //audio_tv
    '\0', //audio_vcr
    '0', //0_tv
    '0', //0_vcr
    '\0', //video_tv
    '\0', //video_vcr
    '\0'  //enter_vcr
  }
};

void setup() {
  Serial.begin(250000);   // Status message will be sent to PC at 9600 baud
  irrecv.enableIRIn();  // Start the receiver
  Keyboard.begin();
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
}

void loop () {
  analogWrite(9, colors[mode][2]);
  analogWrite(10, colors[mode][1]);
  analogWrite(11, colors[mode][0]);
  digitalWrite(13, LOW);
  unsigned short reading = decodeCode();
  digitalWrite(13, HIGH);
  if (millis() - lastread > 200) {
    lastread = millis();
    Serial.println(reading, HEX);
    action(reading);
  }
}

unsigned short decodeCode() {
  decode_results results;
  while (!irrecv.decode(&results)) {}
  //We're going to make a number based on the code we got using pulse width encoding
  unsigned short out = 0; //Initialize the number
  //There are 16 bits in this number, and it all depends on the even numbered values in the rawbuf starting from 2.
  for (int i = 2; i < results.rawlen; i += 2) {
    if (results.rawbuf[i] > 30) out |= (1 << ((34 - i) / 2 - 1));
  }
  irrecv.resume();
  return out;
}

void action(unsigned short reading) {
  short x = '\0';
  switch (reading) {
    case 0xEA6C: x = mapping[mode][0]; break;
    case 0xE240: 
    case 0xEA40:
      mode = (++mode)%modes;
      break;
    case 0xE244: x = mapping[mode][3]; break;
    case 0xE254: x = mapping[mode][4]; break;
    case 0xE24C: x = mapping[mode][5]; break;
    case 0xE2F0: x = mapping[mode][6]; break;
    case 0xEAA4: x = mapping[mode][7]; break;
    case 0xE250: x = mapping[mode][8]; break;
    case 0xEA50: x = mapping[mode][9]; break;
    case 0xE248: x = mapping[mode][10]; break;
    case 0xEA48: x = mapping[mode][11]; break;
    case 0xE25C: x = mapping[mode][12]; break;
    case 0xEA5C: x = mapping[mode][13]; break;
    case 0xE2D8: x = mapping[mode][14]; break;
    case 0xEAE4: x = mapping[mode][15]; break;
    case 0xEA30: x = mapping[mode][16]; break;
    case 0xEA68: x = mapping[mode][17]; break;
    case 0xEA70: x = mapping[mode][18]; break;
    case 0xEACC: x = mapping[mode][19]; break;
    case 0xEA34: x = mapping[mode][20]; break;
    case 0xEA24: x = mapping[mode][21]; break;
    case 0xEA28: x = mapping[mode][22]; break;
    case 0xEA20: x = mapping[mode][23]; break;
    case 0xE24A: x = mapping[mode][24]; break;
    case 0xEA4A: x = mapping[mode][25]; break;
    case 0xEA38: x = mapping[mode][26]; break;
    case 0xE2E0: x = mapping[mode][27]; break;
    case 0xEAD2:
      switch (mode) {
        case 0:
          if (shuttle < -1) Keyboard.write('l');
          if (shuttle > -1) Keyboard.write('j');
          shuttle = -1;
          break;
        default: x = mapping[mode][28]; break;
      }
      break;
    case 0xEADA:
      switch (mode) {
        case 0:
          if (shuttle < -2) Keyboard.write('l');
          if (shuttle > -2) Keyboard.write('j');
          shuttle = -2;
          break;
        default: x = mapping[mode][29]; break;
      }
      break;
    case 0xEAD6:
      switch (mode) {
        case 0:
          if (shuttle < -3) Keyboard.write('l');
          if (shuttle > -3) Keyboard.write('j');
          shuttle = -3;
          break;
        default: x = mapping[mode][30]; break;
      }
      break;
    case 0xEAB2:
      switch (mode) {
        case 0:
          if (shuttle < -4) Keyboard.write('l');
          if (shuttle > -4) Keyboard.write('j');
          shuttle = -4;
          break;
        default: x = mapping[mode][31]; break;
      }
      break;
    case 0xEA72:
      switch (mode) {
        case 0:
          if (shuttle < -5) Keyboard.write('l');
          if (shuttle > -5) Keyboard.write('j');
          shuttle = -5;
          break;
        default: x = mapping[mode][32]; break;
      }
      break;
    case 0xEAB6:
      switch (mode) {
        case 0:
          if (shuttle < -6) Keyboard.write('l');
          if (shuttle > -6) Keyboard.write('j');
          shuttle = -6;
          break;
        default: x = mapping[mode][33]; break;
      }
      break;
    case 0xEABE:
      switch (mode) {
        case 0:
          if (shuttle < -7) Keyboard.write('l');
          if (shuttle > -7) Keyboard.write('j');
          shuttle = -7;
          break;
        default: x = mapping[mode][34]; break;
      }
      break;
    case 0xEA22: x = mapping[mode][35]; break;
    case 0xE2AC: x = mapping[mode][36]; break;
    case 0xEA32:
      switch (mode) {
        case 0:
          if (shuttle < 1) Keyboard.write('l');
          if (shuttle > 1) Keyboard.write('j');
          shuttle = 1;
          break;
        default: x = mapping[mode][37]; break;
      }
      break;
    case 0xEA3A:
      switch (mode) {
        case 0:
          if (shuttle < 2) Keyboard.write('l');
          if (shuttle > 2) Keyboard.write('j');
          shuttle = 2;
          break;
        default: x = mapping[mode][38]; break;
      }
      break;
    case 0xEA36:
      switch (mode) {
        case 0:
          if (shuttle < 3) Keyboard.write('l');
          if (shuttle > 3) Keyboard.write('j');
          shuttle = 3;
          break;
        default: x = mapping[mode][39]; break;
      }
      break;
    case 0xEAA2:
      switch (mode) {
        case 0:
          if (shuttle < 4) Keyboard.write('l');
          if (shuttle > 4) Keyboard.write('j');
          shuttle = 4;
          break;
        default: x = mapping[mode][40]; break;
      }
      break;
    case 0xEAAA:
      switch (mode) {
        case 0:
          if (shuttle < 5) Keyboard.write('l');
          if (shuttle > 5) Keyboard.write('j');
          shuttle = 5;
          break;
        default: x = mapping[mode][41]; break;
      }
      break;
    case 0xEAA6:
      switch (mode) {
        case 0:
          if (shuttle < 6) Keyboard.write('l');
          if (shuttle > 6) Keyboard.write('j');
          shuttle = 6;
          break;
        default: x = mapping[mode][42]; break;
      }
      break;
    case 0xEAAE:
      switch (mode) {
        case 0:
          if (shuttle < 7) Keyboard.write('l');
          if (shuttle > 7) Keyboard.write('j');
          shuttle = 7;
          break;
        default: x = mapping[mode][43]; break;
      }
      break;
    case 0xE292: x = mapping[mode][44]; break;
    case 0xEACE: x = mapping[mode][45]; break;
    case 0xE282: x = mapping[mode][46]; break;
    case 0xEA2E: x = mapping[mode][47]; break;
    case 0xE2FA: x = mapping[mode][48]; break;
    case 0xE2EE: x = mapping[mode][49]; break;
    case 0xE200: x = mapping[mode][50]; break;
    case 0xEA01: x = mapping[mode][51]; break;
    case 0xE210: x = mapping[mode][52]; break;
    case 0xEA11: x = mapping[mode][53]; break;
    case 0xE208: x = mapping[mode][54]; break;
    case 0xEA09: x = mapping[mode][55]; break;
    case 0xE2E8: x = mapping[mode][56]; break;
    case 0xEAF5: x = mapping[mode][57]; break;
    case 0xE218: x = mapping[mode][58]; break;
    case 0xEA19: x = mapping[mode][59]; break;
    case 0xE204: x = mapping[mode][60]; break;
    case 0xEA05: x = mapping[mode][61]; break;
    case 0xE214: x = mapping[mode][62]; break;
    case 0xEA15: x = mapping[mode][63]; break;
    case 0xEAA1: x = mapping[mode][64]; break;
    case 0xE20C: x = mapping[mode][65]; break;
    case 0xEA0D: x = mapping[mode][66]; break;
    case 0xE21C: x = mapping[mode][67]; break;
    case 0xEA1D: x = mapping[mode][68]; break;
    case 0xE280: x = mapping[mode][69]; break;
    case 0xEA81: x = mapping[mode][70]; break;
    case 0xEAD1: x = mapping[mode][72]; break;
    case 0xE20A: x = mapping[mode][73]; break;
    case 0xEA0B: x = mapping[mode][74]; break;
    case 0xE290: x = mapping[mode][75]; break;
    case 0xEA91: x = mapping[mode][76]; break;
    case 0xE202: x = mapping[mode][77]; break;
    case 0xEA03: x = mapping[mode][78]; break;
    case 0xEAAD: x = mapping[mode][80]; break;
  }
  //Find shift, ctrl, and alt
  if(x&ctrl) {
    Keyboard.press(KEY_RIGHT_CTRL);
  }
  if(x&alt) {
    Keyboard.press(KEY_RIGHT_ALT);
  }
  if(x&shift) {
    Keyboard.press(KEY_RIGHT_SHIFT);
  }
  if(x!=0) Keyboard.write((char)x);
  Keyboard.releaseAll();
}

