int out1 = 5;
int out2 = 6;
float outVolt = .7;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(out1, OUTPUT);
  pinMode(out2, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  float fitted = .0487*pow(outVolt,3) - .5527*pow(outVolt,2) + 2.3975*outVolt - 1.4343;
  analogWrite(out1, fitted*51);
  analogWrite(out2, 1*51);
  Serial.println(outVolt);
  delay(3000);
  outVolt += .1;
}
