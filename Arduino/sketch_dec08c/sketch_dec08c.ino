#include <aia-tft.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  aiaTft t = aiaTft();
  int a = t.begin();
  Serial.print(a);
}

void loop() {
  // put your main code here, to run repeatedly:

}
