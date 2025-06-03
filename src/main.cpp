#include <Arduino.h>

void setup()
{
  // put your setup code here, to run once:
  Serial0.begin(115200);
  Serial0.println("Hello from ESP32!");
}

void loop()
{
  // put your main code here, to run repeatedly:
  Serial0.println("Hello from ESP32!");
  delay(1000);
}
