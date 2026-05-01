void semi_pid()
{
  int lbasic=72, rbasic=72, lmtr, LMtr, rmtr, maxSpeed=120, minSpeed=-100;
  int kp=50, kd=1000, ki;
  float PID, error, latest_error;
  char turn='s';
  reading();
  error = 3.5-avg;
  PID = error*kp + (latest_error-error)*kd + (latest_error+error)*ki;
  latest_error = error;
  lmtr = lbasic + PID;
  rmtr = rbasic - PID;
  if (lmtr >= maxSpeed) lmtr = maxSpeed;
  else if (lmtr < minSpeed) lmtr = minSpeed;
  if (rmtr >= maxSpeed) rmtr = maxSpeed;
  else if (rmtr < minSpeed) rmtr = minSpeed;
  LMtr = 1.32 * lmtr;
  mtr(LMtr,rmtr);
  if(s[0]==1 && s[5]==0) turn='r';
  if(s[0]==0 && s[5]==1) turn='l';
  if(sensor_count==0)
  {
    if(turn!='s')
    {
      delay(50);
      (turn=='r') ? mtr(250,-250) : mtr(-250,250);
      while(s[2]==0 && s[3]==0) reading();
      turn='s';
    }
  }
  if(sensor_count==6)
  {
    delay(50);
    reading();
    if(sensor_count==0) turn='r';
    else if(sensor_count==6)
    {
      mtr(0,0);
      while(sensor_count==6) reading();
    }
  }
}