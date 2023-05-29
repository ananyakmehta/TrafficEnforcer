#include <SoftwareSerial.h>

#define RX_PIN_C2S1     2
#define TX_PIN_C2S1     3
#define RX_PIN_C2S2     4
#define TX_PIN_C2S2     5

enum {
  SCANNER_1,
  SCANNER_2,
};

enum {
  SPEED_INDEX = 2,
  LIGHT_INDEX = 4,
  BUZZER_INDEX = 6,
  TIME_INDEX = 8,
};

SoftwareSerial BTserial1(RX_PIN_C2S1, TX_PIN_C2S1); // RX | TX
SoftwareSerial BTserial2(RX_PIN_C2S2, TX_PIN_C2S2); // RX | TX

int S_A = 10;  //speed motor a
int M_A1 = 6; //motor a = +
int M_A2 = 7; //motor a = -
int M_B1 = 8; //motor b = -
int M_B2 = 12; //motor b = +
int S_B = 9;  //speed motor b

// 11 -> 9 (S_B)
// 9 -> 12 (M_B2)

int R_S = A0; //sincer R
int S_S = A1; //sincer S
int L_S = A2; //sincer L

int Pin = 13; //pin for buzzer

uint8_t rules[9];
unsigned long currentMillis;
unsigned long stoptime;
bool fline = true;
bool scan = SCANNER_1;

void setup() 
{
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(M_B1, OUTPUT);
  pinMode(M_B2, OUTPUT);
  pinMode(M_A1, OUTPUT);
  pinMode(M_A2, OUTPUT);
  pinMode(S_B, OUTPUT);
  pinMode(S_A, OUTPUT);

  pinMode(L_S, INPUT);
  pinMode(S_S, INPUT);
  pinMode(R_S, INPUT);

  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);

  analogWrite(S_A, 100); 
  analogWrite(S_B, 100); 
  delay(200);
  
  Serial.begin(9600);
  Serial.println("Arduino with HC-06 is ready");

  BTserial1.begin(9600);  
  Serial.println("BTserial1 started at 9600");

  BTserial2.begin(9600);  
  Serial.println("BTserial2 started at 9600");
}

void read_BT1(void)
{
  if (scan == SCANNER_2) return;    
  BTserial1.listen();
  delay(1);
  uint8_t index = 0;
  bool start = false;
  while (BTserial1.available() > 0) {
    char inByte = BTserial1.read();
    if (inByte == 'A') {
      if (start) {
        start = false;
        // Display the rules
        rules[TIME_INDEX] = 12; // TODO
        Serial.print("Car 2 rules: ");
        for (int i=0; i<sizeof(rules); i++) {
          Serial.print(rules[i]);
          Serial.print(" ");
        }
        Serial.println();
        scan = SCANNER_2;
        analogWrite(S_A, 130); 
        analogWrite(S_B, 130); 
        digitalWrite(A3, HIGH);
        tone(Pin, 1000, 0);
#if 0
        Stop();
        fline = false;
#endif
        currentMillis = millis();
        break;
      } else {
        start = true;
      }
    }
    if (start) {
      if ((!index) && rules[TIME_INDEX]) {
        Stop();
        fline = false;
        unsigned long blah = millis();
        stoptime = ((rules[TIME_INDEX] * 1000) - (blah - currentMillis));
        Serial.print("rules: ");
        Serial.println(rules[TIME_INDEX]);
        Serial.print("stoptime: ");
        Serial.println(stoptime);
        Serial.print("current: ");
        Serial.println(currentMillis);
        Serial.print("blah: ");
        Serial.println(blah);
      }
      // Start reading the rules
      rules[index] = inByte;
      index++;
    }
    //digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level
    delay(1);
  }

  // Read the remaining bytes from BT and discard them
  while (BTserial1.available() > 0) {
    char inByte = BTserial1.read();
  }
}

void read_BT2(void)
{
  if (scan == SCANNER_1) return;    
  BTserial2.listen();
  delay(1);
  uint8_t index = 0;
  bool start = false;
  while (BTserial2.available() > 0) {
    char inByte = BTserial2.read();
    if (inByte == 'A') {
      if (start) {
        start = false;
        // Display the rules
        rules[TIME_INDEX] = 12; // TODO
        Serial.print("Car 2 rules: ");
        for (int i=0; i<sizeof(rules); i++) {
          Serial.print(rules[i]);
          Serial.print(" ");
        }
        Serial.println();
        scan = SCANNER_1;
        analogWrite(S_A, 100); 
        analogWrite(S_B, 100);
        digitalWrite(A3, LOW);
        noTone(Pin);
#if 0
        fline = false;
        Stop();
#endif
        currentMillis = millis();
        break;
      } else {
        start = true;
      }
    }
    if (start) {
      if ((!index) && rules[TIME_INDEX]) {
        Stop();
        fline = false;
        unsigned long blah = millis();
        stoptime = ((rules[TIME_INDEX] * 1000) - (blah - currentMillis));
        Serial.print("rules: ");
        Serial.println(rules[TIME_INDEX]);
        Serial.print("stoptime: ");
        Serial.println(stoptime);
        Serial.print("current: ");
        Serial.println(currentMillis);
        Serial.print("blah: ");
        Serial.println(blah);
      }
      // Start reading the rules
      rules[index] = inByte;
      index++;
    }
    //digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level
    delay(1);
  }

  // Read the remaining bytes from BT and discard them
  while (BTserial2.available() > 0) {
    char inByte = BTserial2.read();
  }
}

void follow_line(void)
{
  if ((digitalRead(L_S) == 0) && (digitalRead(S_S) == 1) && (digitalRead(R_S) == 0)) {
    forward();
  }

  if ((digitalRead(L_S) == 1) && (digitalRead(S_S) == 1) && (digitalRead(R_S) == 0)) {
    turnLeft();
  }

  if ((digitalRead(L_S) == 1) && (digitalRead(S_S) ==0) && (digitalRead(R_S) == 0)) {
    turnLeft();
  }

  if ((digitalRead(L_S) == 0) && (digitalRead(S_S) == 1) && (digitalRead(R_S) == 1)) {
    turnRight();
  }

  if ((digitalRead(L_S) == 0) && (digitalRead(S_S) == 0) && (digitalRead(R_S) == 1)) {
    turnRight();
  }

  if ((digitalRead(L_S) == 1) && (digitalRead(S_S) == 1) && (digitalRead(R_S) == 1)) {
    forward();
  }  
}


void loop()
{
  if (fline) {
    follow_line();
  }

  read_BT1();
  read_BT2();
  if ((fline == false) && ((millis() - currentMillis) >= stoptime)) {
    fline = true;
    forward();
    Serial.println("resume");
    currentMillis = millis();
  }
}

void forward(){
  digitalWrite(M_A1, LOW);
  digitalWrite(M_A2, HIGH);
  digitalWrite(M_B1, HIGH);
  digitalWrite(M_B2, LOW);  
}

void turnLeft(){
  digitalWrite(M_A1, LOW);
  digitalWrite(M_A2, LOW);
  digitalWrite(M_B1, HIGH);
  digitalWrite(M_B2, LOW); 
}

void turnRight(){
  digitalWrite(M_A1, LOW);
  digitalWrite(M_A2, HIGH);
  digitalWrite(M_B1, LOW);
  digitalWrite(M_B2, LOW);
}

void Stop(){
  digitalWrite(M_A1, LOW);
  digitalWrite(M_A2, LOW);
  digitalWrite(M_B1, LOW);
  digitalWrite(M_B2, LOW);
}
