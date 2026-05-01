#define portOfPin(P) \
  (((P) >= 0 && (P) < 8) ? &PORTD : (((P) > 7 && (P) < 14) ? &PORTB : &PORTC))
#define ddrOfPin(P) \
  (((P) >= 0 && (P) < 8) ? &DDRD : (((P) > 7 && (P) < 14) ? &DDRB : &DDRC))
#define pinOfPin(P) \
  (((P) >= 0 && (P) < 8) ? &PIND : (((P) > 7 && (P) < 14) ? &PINB : &PINC))
#define pinIndex(P) ((uint8_t)(P > 13 ? P - 14 : P & 7))
#define pinMask(P) ((uint8_t)(1 << pinIndex(P)))

#define pinAsInput(P) *(ddrOfPin(P)) &= ~pinMask(P)
#define pinAsInputPullUp(P) \
  *(ddrOfPin(P)) &= ~pinMask(P); \
  digitalHigh(P)
#define pinAsOutput(P) *(ddrOfPin(P)) |= pinMask(P)
#define digitalLow(P) *(portOfPin(P)) &= ~pinMask(P)
#define digitalHigh(P) *(portOfPin(P)) |= pinMask(P)
#define isHigh(P) ((*(pinOfPin(P)) & pinMask(P)) > 0)
#define isLow(P) ((*(pinOfPin(P)) & pinMask(P)) == 0)
#define digitalState(P) ((uint8_t)isHigh(P))

#define led 13

byte channel_adress_table[14][4] = {
  { 0, 0, 0, 0 },  //0
  { 0, 0, 0, 1 },  //1
  { 0, 0, 1, 0 },  //2
  { 0, 0, 1, 1 },  //3
  { 0, 1, 0, 0 },  //4
  { 0, 1, 0, 1 },  //5
  { 0, 1, 1, 0 },  //6
  { 0, 1, 1, 1 },  //7
  { 1, 0, 0, 0 },  //8
  { 1, 0, 0, 1 },  //9
  { 1, 0, 1, 0 },  //10
  { 1, 0, 1, 1 },  //11
  { 1, 1, 0, 0 },  //12
  { 1, 1, 0, 1 },  //13
};
//mux variables
#define S0 A3
#define S1 A2
#define S2 A1
#define S3 2

//========button pins========
//#define cal 4
//#define runbot 5

//=====motor pins=======
//MOTOR 1
#define lmf 8
#define lmb 7
#define lmPWM 6
//MOTOR 2
#define rmf 10
#define rmb 9
#define rmPWM 11

//PID variables
float Kp, Ki, Kd;
int error;
int lastError = 0;

int baseSpeed = 75, maxSpeed = 100;
int brkSpeed, minSpeed;
int lmSpeed, rmSpeed;
byte flag = 0, mode = 0;
//=======IR variables=======
#define numSensor 14
int rawSensor[numSensor], maximum[numSensor], minimum[numSensor], mid[numSensor], sensor[numSensor];
uint16_t position = 0;
byte on_line = 0;
int maxTh = 500, minTh = 400;
#include <EEPROM.h>

void setup() {
  // put your setup code here, to runbot once:
  Serial.begin(9600);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(A0, INPUT);

  //pinMode(cal, INPUT_PULLUP);
//  pinMode(runbot, INPUT_PULLUP);

  pinMode(lmf, OUTPUT);
  pinMode(lmb, OUTPUT);

  pinMode(rmf, OUTPUT);
  pinMode(rmb, OUTPUT);

  pinMode(lmPWM, OUTPUT);
  pinMode(rmPWM, OUTPUT);

  analogReadSetup();
  // calibration();
  readCalibration();
}

void loop() {
  //  // put your main code here, to runbot repeatedly:

  long st = micros();
  position = readLine();
  //printReading(0);
  if (!on_line)
  {
    if (!on_line) //if all sensor on white(dotted line) it'll go forward 80ms (tune this value)
    {
      drive(lmSpeed, rmSpeed);
      delay(80);
    }
  }
  else if(on_line == numSensor) {
    drive(0,0);
    delay(1000);
  }

  lineFollow(
    position,
    baseSpeed = 50,
    maxSpeed = 75,
    minSpeed = -40,
    Kp = 0.2,
    Kd = 0,
    Ki = 0);

  // drive(200, 200);
}

/*byte push(byte button_number) {
  uint32_t b_timer = 0;
  if (!digitalRead(button_number)) {  //if button is pressed
    delay(50);                        //debounce delay;
    while (!digitalRead(button_number)) {
      delay(50);
      b_timer += 50;
      if (b_timer > 1000) digitalWrite(led, 1);
    }
    digitalWrite(led, 0);
    if (b_timer > 1000) return 2;    //long press
    else if (b_timer > 0) return 1;  //short press
  }
  return 0;
}*/
