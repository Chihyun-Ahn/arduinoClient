int pin = 2;
unsigned long duration;

void setup() {
  Serial.begin(9600);
  pinMode(pin, INPUT_PULLUP);
}

void loop() {
  duration = pulseIn(pin, LOW);
  Serial.println(duration);
}
