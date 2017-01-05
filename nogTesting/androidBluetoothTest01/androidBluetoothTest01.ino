
int Received = 0;

void setup() {
  // put your setup code here, to run once:
  while (!Serial);
  Serial.begin(115200);
  Serial.println("Starting");
}

void loop() {

  // put your main code here, to run repeatedly:
  if (Serial.available() > 0)
  {
    Received = Serial.read();
    Serial.print(Received);
  }
}
