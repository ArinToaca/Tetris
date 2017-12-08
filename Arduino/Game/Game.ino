#define game_size 21

char disp[game_size][game_size];

uint8_t i,j;

void setup() {
  Serial.begin(9600);
  
  // put your setup code here, to run once:
  for(i=0;i<game_size;i++)
    for(j=0;j<game_size;j++)
      disp[i][j]=' ';

  for(j=0;j<game_size;j++)
  {
    disp[j][6]='|';
    disp[j][16]='|';
  }

  Serial.println("Game ready!");
}

void update_tick()
{
  
}

void display_tick()
{
  Serial.println(" ");
  for(i=0;i<game_size;i++)
    Serial.print('-');
  Serial.println(" ");
  for(i=0;i<game_size;i++)
  {
    for(j=0;j<game_size;j++)
      Serial.print(disp[i][j]);
    Serial.println(" ");
  }
  for(i=0;i<game_size;i++)
    Serial.print('-');
}

void loop() {
  // put your main code here, to run repeatedly:
  update_tick();
  display_tick();

  delay(1000);
}
