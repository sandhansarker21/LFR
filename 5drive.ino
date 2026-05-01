void drive(int left, int right)
{
  if (left > 0) {
   digitalWrite(lmf, 1);
   digitalWrite(lmb, 0);
  }
  else {
    left = -left;
   digitalWrite(lmf, 0);
   digitalWrite(lmb, 1);
  }
  if (right > 0) {
   digitalWrite(rmf, 1);
   digitalWrite(rmb, 0);
  }
  else {
    right = -right;
   digitalWrite(rmf, 0);
   digitalWrite(rmb, 1);
  }
  analogWrite(lmPWM, left);
  analogWrite(rmPWM, right);
}
