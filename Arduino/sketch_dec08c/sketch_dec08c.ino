#include <aia-tft.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  //aiaTft(uint8_t scl, uint8_t sda, uint8_t cd, uint8_t cs, uint8_t reset)
  aiaTft t = aiaTft(/*sclk=*/ 13, /*data=*/ 11, /*cd=*/ 9 , /*cs=*/ 10, /*reset=*/8);
  int a = t.begin();
  Serial.print(a);
}

void loop() {
  // put your main code here, to run repeatedly:

}
