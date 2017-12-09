volatile byte button_DOWN = LOW;
volatile byte button_UP = LOW;
volatile byte button_LEFT = LOW;
volatile byte button_RIGHT = LOW;

#define DEBOUNCE_TIMEOUT_MS   25

void setup() {
  noInterrupts(); // disable all interrupts
  TCCR3A = 0;
  TCCR3B = 0;

  OCR3A = 3125; //16 MHZ / 256 / 2HZ
  TCNT3 = 0;
  TCCR3B |= (1 << WGM12);
  TCCR3B |= (1 << CS12); 
  TIMSK3 |= (1 << OCIE3A); 
  interrupts(); 
  
  Serial.begin(9600);
  
  uint8_t i;
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(18, INPUT);
  pinMode(20, INPUT);
    
  attachInterrupt(digitalPinToInterrupt(2), button_down_interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), button_up_interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(18), button_left_interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(20), button_right_interrupt, CHANGE);

  interrupts(); 
  Serial.println("Ready!");
}

void loop() {
  /*uint8_t i;
  Serial.print("Down button state: ");
  Serial.println(button_DOWN);

  Serial.print("Up button state: ");
  Serial.println(button_UP);

  Serial.print("Left button state: ");
  Serial.println(button_LEFT);

  Serial.print("Right button state: ");
  Serial.println(button_RIGHT);

  Serial.print("-----------: ");
  Serial.println(millis());

  delay(1000);*/
}

ISR(TIMER3_COMPA_vect)// interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
  Serial.println("Timer event!");
}

void button_down_interrupt() {
  sei();
  static unsigned long last_interrupt_time=0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > DEBOUNCE_TIMEOUT_MS) 
  {
    button_DOWN = ~button_DOWN;
  }
  last_interrupt_time = interrupt_time;
}

void button_up_interrupt() {
  sei();
  static unsigned long last_interrupt_time=0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > DEBOUNCE_TIMEOUT_MS) 
  {
    button_UP = ~button_UP;
  }
  last_interrupt_time = interrupt_time;
}

void button_left_interrupt() {
  sei();
  static unsigned long last_interrupt_time=0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > DEBOUNCE_TIMEOUT_MS) 
  {
    button_LEFT = ~button_LEFT;
  }
  last_interrupt_time = interrupt_time;
}

void button_right_interrupt() {
  sei();
  static unsigned long last_interrupt_time=0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > DEBOUNCE_TIMEOUT_MS) 
  {
    button_RIGHT = ~button_RIGHT;
  }
  last_interrupt_time = interrupt_time;
}

