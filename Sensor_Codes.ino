void reading()
{
  for(int i=0;i<6;i++)
  {
    s[i]=analogRead(i);
    //Serial.print(s[i]);
    //Serial.print(" ");
    (s[i]>350) ? s[i]=1 :s[i]=0;
    sensor_value += s[i] * sensor_position[i];
    sensor_count+=s[i];
  }
  //Serial.println();
  if(sensor_count) avg= sensor_value/sensor_count;
}