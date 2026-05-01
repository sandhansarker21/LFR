//MOTOR 1
#define lmf 8
#define lmb 7
#define lmPWM 6
//MOTOR 2
#define rmf 9
#define rmb 10
#define rmPWM 11
int s[6];
int sensor_position[6]={1,2,3,4,5,6};
int sensor_value;
int sensor_count;
float avg;

void setup() 
{
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);

  pinMode(lmf, OUTPUT);
  pinMode(lmb, OUTPUT);

  pinMode(rmf, OUTPUT);
  pinMode(rmb, OUTPUT);

  pinMode(lmPWM, OUTPUT);
  pinMode(rmPWM, OUTPUT);

}

void loop() 
{
  semi_pid();
}
