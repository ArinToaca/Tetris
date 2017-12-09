#include <tetris.h>
tetris *t;
void setup() {
  Serial.begin(115200);
  t =  new tetris();
  t->setPins();
  t->setGame();

} 
void loop() {

  t->update();
  
}
