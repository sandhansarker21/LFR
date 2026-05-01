void analogReadSetup() {
  ADMUX = (1 << REFS0);  //REFS1 REFS0 ADLAR – MUX3 MUX2 MUX1 MUX0---> reference voltage selected is Vcc
  ADCSRA = (1 << ADEN);
  ADCSRA |= (1 << ADPS2) | (1 << ADPS0 );  //pre scaler 32 is better for both time and accuracy
                                          //ADCSRA = ADEN ADSC ADATE ADIF ADIE ADPS2 ADPS1 ADPS0---> no need to consider ADATE ADIF ADIE for our application
                                          // ADPS2 ADPS1 ADPS0 = 101 means pre scaler 1/32
}

int analogRd(byte pin) {
  //We read A1 (MUX0)
  ADMUX = (ADMUX & 0xF0) | (pin & 0x0F);  //pin number is received here
                                          //MUX2 MUX1 MUX0 = 000, 001, 010 etc selects the pin

  // Start AD conversion
  ADCSRA |= B11000000;
  // Detect end-of-conversion
  while (bit_is_set(ADCSRA, ADSC))
    ;
  int val = ADC;
  return val;
}

//void analogWriteSetup() {
//  //DDRD |= (1 << DDD6) | (1 << DDD5);  //pinmode output for pin 6, pin 5
//  //DDRB |= (1 << DDB1) | (1 << DDB2);  //pinmode output for pin 9, pin 10
//  TCCR0A = B10100011;                 //COM0A1 COM0A0 COM0B1 COM0B0 - - WGM01 WGM00 --->WGM02 WGM01 WGM00 = 011 means fast pwm
//  TCCR0B = B00000011;                 //X X - - WGM02 CS02 CS01 CS00 ---> CS02 CS01 CS00 = 011 means 1/64
//
//  TCCR1A = B10100001;  //COM1A1 COM1A0 COM1B1 COM1B0 - - WGM11 WGM10
//  TCCR1B = B00001011;  //X X - WGM13 WGM12 CS12 CS11 CS10 ----> WGM13 WGM12 WGM11 WGM10 = 0101 means fast pwm
//}


void printReading(byte a) {
  if (a == 1) {
    printPosition();
    Serial.println();
  } else if (a == 2) {
    printSpeed();
    Serial.println();
  } else if (a == 3) {
    printPosition();
    Serial.print("\t\t");
    printSpeed();
    Serial.println();
  }
}
void printPosition() {
  for (int i = 0; i < numSensor; i++) {
    Serial.print(sensor[i]);
    Serial.print("\t");
  }
  Serial.print(position);
  Serial.print("\t");
  Serial.print(on_line);
  Serial.print("\t");
  Serial.print(flag);
}
void printSpeed() {
  Serial.print(Kp);
  Serial.print("\t");
  Serial.print(baseSpeed);
  Serial.print("\t");
  Serial.print(lmSpeed);
  Serial.print("\t");
  Serial.print(rmSpeed);
}
