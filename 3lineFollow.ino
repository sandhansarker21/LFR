void lineFollow(unsigned int position,int baseSpeedC,int maxSpeed, int minSpeed,
float Kp, float Kd, float Ki)
{
  error = 6500 - position;
  
  float adjustPosition = Kp * error + Kd * (error - lastError);
  lmSpeed = baseSpeed + adjustPosition;
  rmSpeed = baseSpeed - adjustPosition;

  if (lmSpeed >= maxSpeed) lmSpeed = maxSpeed;
  else if (lmSpeed < minSpeed) lmSpeed = minSpeed;
  if (rmSpeed >= maxSpeed) rmSpeed = maxSpeed;
  else if (rmSpeed < minSpeed) rmSpeed = minSpeed;

  drive(lmSpeed, rmSpeed);
  lastError = error;
}
