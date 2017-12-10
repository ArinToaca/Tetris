#include <tzeny-draw.h>
#include <tzeny-game.h>

volatile byte button_DOWN = LOW;
volatile byte button_UP = LOW;
volatile byte button_LEFT = LOW;
volatile byte button_RIGHT = LOW;

#define DEBOUNCE_TIMEOUT_MS   25

tzeny_draw *draw;
tzeny_game *game;

void setup() {
  Serial.begin(9600);
  setup_interrupts();
  // put your setup code here, to run once:
  draw = new tzeny_draw();
  game = new tzeny_game(draw); 
  
  game->start_game();
}

void loop() {
  // put your main code here, to run repeatedly:
  
}

ISR(TIMER4_COMPA_vect)// interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
  game->game_tick();
  //Serial.println("Tick!");
}

void setup_interrupts()
{
  noInterrupts(); // disable all interrupts
  TCCR4A = 0;
  TCCR4B = 0;

  OCR4A = 3125; //16 MHZ / 256 / 2HZ
  TCNT4 = 0;
  TCCR4B |= (1 << WGM12);
  TCCR4B |= (1 << CS12); 
  TIMSK4 |= (1 << OCIE4A); 
  
  
  uint8_t i;
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(18, INPUT);
  pinMode(19, INPUT);
    
  attachInterrupt(digitalPinToInterrupt(3), button_down_interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(19), button_up_interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(18), button_left_interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(2), button_right_interrupt, CHANGE);

  interrupts(); 
  Serial.println("Installed interrupts!");
}

void button_down_interrupt() {
  static unsigned long last_interrupt_time=0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > DEBOUNCE_TIMEOUT_MS) 
  {
    button_DOWN = ~button_DOWN;
    if(button_DOWN)
    {
      game->back_button_pressed();
    }
    else
    {
      game->back_button_released();
    }
  }
  last_interrupt_time = interrupt_time;
}

void button_up_interrupt() {
  static unsigned long last_interrupt_time=0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > DEBOUNCE_TIMEOUT_MS) 
  {
    button_UP = ~button_UP;
    if(button_UP)
    {
      game->top_button_pressed();
    }
  }
  last_interrupt_time = interrupt_time;
}

void button_left_interrupt() {
  static unsigned long last_interrupt_time=0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > DEBOUNCE_TIMEOUT_MS) 
  {
    button_LEFT = ~button_LEFT;
    if(button_LEFT)
    {
      game->left_button_pressed();
    }
  }
  last_interrupt_time = interrupt_time;
}
void button_right_interrupt() {
  static unsigned long last_interrupt_time=0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > DEBOUNCE_TIMEOUT_MS) 
  {
    button_RIGHT = ~button_RIGHT;
    if(button_RIGHT)
    {
      game->right_button_pressed();
    }
  }
  last_interrupt_time = interrupt_time;
}

