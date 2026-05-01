int lastValue = 0;

uint32_t readLine()
{
  unsigned long avg;  // this is for the weighted total, which is long
  // before division
  unsigned int sum;  // this is for the denominator which is <= 64000

  byte avgTimes = 1;
  for (int i = 0; i < numSensor; i++)
  {
    rawSensor[i] = 0;
  }

  for (int i = 13; i >= 0; i--)
  {
    Mux_Addr(i);
    delay(0.5);
    rawSensor[i] = analogRd(0);
    Serial.print(rawSensor[i]);
    Serial.print(" ");
  }
  Serial.println();

  on_line = 0;
  avg = 0;
  sum = 0;

  for (int i = 0; i < numSensor; i++)
  {
    sensor[i] = mapC(rawSensor[i], minimum[i], maximum[i], 0, 2026);
    if(sensor[i] < 0) sensor[i] = 0;
    if(sensor[i] > 1023) sensor[i] = 1023;
    // keep track of whether we see the line at all
    if (sensor[i] > maxTh)
    {
      on_line += 1;
    }

    // only average in values that are above a noise threshold
    if (sensor[i] > 50 && sensor[i] < 1100)
    {
      avg += (long)(sensor[i]) * (i * 1000);
      sum += sensor[i];
    }
  }

  if (!on_line) {
    // If it last read to the left of center, return 0.
    if (lastValue < (numSensor - 1) * 1000 / 2)
    {
      return 0;
    }
    // If it last read to the right of center, return the max.
    else
    {
      return (numSensor - 1) * 1000;
    }
  }

  lastValue = avg / sum;
  return lastValue;
}


int mapC(int x, int in_min, int in_max, int out_min, int out_max) 
{
  return (x - in_min) * ((out_max - out_min) / (in_max - in_min));
}


void Mux_Addr(int ch_Addr) {
  if (channel_adress_table[ch_Addr][0]) {
    digitalHigh(S3);
  } else {
    digitalLow(S3);
  }

  if (channel_adress_table[ch_Addr][1]) {
    digitalHigh(S2);
  } else {
    digitalLow(S2);
  }

  if (channel_adress_table[ch_Addr][2]) {
    digitalHigh(S1);
  } else {
    digitalLow(S1);
  }

  if (channel_adress_table[ch_Addr][3]) {
    digitalHigh(S0);
  } else {
    digitalLow(S0);
  }
}
