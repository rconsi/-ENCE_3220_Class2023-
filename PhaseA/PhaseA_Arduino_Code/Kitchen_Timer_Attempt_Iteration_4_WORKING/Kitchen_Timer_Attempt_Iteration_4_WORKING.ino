#define BUTTON_1  3
#define BUTTON_2  2
#define GREEN_LED 4
#define RED_LED   5
#define BUZZER    6

#define DATA      9
#define LATCH     8
#define CLOCK     7

#define DIGIT_4   10
#define DIGIT_3   11
#define DIGIT_2   12
#define DIGIT_1   13

//Debounce Variables

unsigned char table[]=
{0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c
,0x39,0x5e,0x79,0x71,0x00};
byte gCurrentDigit;

volatile unsigned char count = 0;
volatile unsigned char countTwo = 0;

//buzzer flag
volatile unsigned char gISRFlag1 = 0;
volatile unsigned char gBuzzerFlag = 0;
// Timer Variables
#define DEF_COUNT 30
volatile int gCount = DEF_COUNT;
unsigned char gTimerRun = 0;
unsigned int gReloadTimer1 = 62500;//corresponds to 1 second
byte gReloadTimer2 = 10; //refresh display timer
/**
* @brief Setup Peripherals and GPIO
* @param 
* @return
*/
void setup() {
   // LEDs Pins
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  // Button Pins
  pinMode(BUTTON_1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_1), Button_1_ISR, RISING);
  pinMode(BUTTON_2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_2), Button_2_ISR, RISING);

  // Buzer Pins
  pinMode(BUZZER, OUTPUT);

  //Buzzer off
  digitalWrite(BUZZER,LOW);

  // 7-Seg Display
  pinMode(DIGIT_1, OUTPUT);
  pinMode(DIGIT_2, OUTPUT);
  pinMode(DIGIT_3, OUTPUT);
  pinMode(DIGIT_4, OUTPUT);

  // Shift Register Pins
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);
  dispOff();//turns display off

  //Set up timers
  //intialize timer1 (16bit)
  //initialize timer2 (8bit)
  TCCR2A = 0;
  TCCR2B = 0;
  OCR2A = gReloadTimer2;                     // max value 2^8 - 1 = 255
  TCCR2A |= (1<<WGM21);
  TCCR2B = (1<<CS22) | (1<<CS21) | (1<<CS20); // 1204 prescaler
  TIMSK2 |= (1<<OCIE2A);
  interrupts();                               // enable all interrupts

}




void display(unsigned char num, unsigned char shift)
{
  //disp_on(10);
  //disp_on(11);
  digitalWrite(LATCH, LOW);
  shiftOut(DATA, CLOCK, MSBFIRST, table[num] | (shift<<7));
  digitalWrite(LATCH, HIGH);
}

void dispOff(){
  digitalWrite(DIGIT_1, HIGH);
  digitalWrite(DIGIT_2, HIGH);
  digitalWrite(DIGIT_3, HIGH);
  digitalWrite(DIGIT_4, HIGH);
}

void Button_1_ISR()
{ //set ISR flag to one
 gISRFlag1 = 1;
}
void Button_2_ISR()
{ //increment clock
  gCount++;
}
ISR(TIMER2_COMPA_vect){
  dispOff(); //turn off display
  OCR2A = gReloadTimer2; //load timer
  switch(gCurrentDigit){
    case 1: //0x:xx
      display(int((gCount / 60)/ 10) % 6, 0);
      digitalWrite(DIGIT_1,LOW);
      break;
    case 2: //x0:xx
      display(int(gCount / 60) % 10, 1);
      digitalWrite(DIGIT_2,LOW);
      break;
    case 3: //xx:0x
      display( (gCount / 10) % 6, 0);
      digitalWrite(DIGIT_3,LOW);
      break;
    case 4:
      display(gCount % 10, 0);
      digitalWrite(DIGIT_4,LOW);
      break;
    default:
      break;
  }
  gCurrentDigit = (gCurrentDigit % 4 ) + 1;
}
ISR(TIMER1_COMPA_vect){
  gCount--;
  OCR1A = gReloadTimer1;
  if(gCount == 0){
    //Stop Timer
    stopTimer1();
    //raise alarm
    gBuzzerFlag = 1;
    gTimerRun = 0;
  }
}
void stopTimer1(){
  // Stop Timer
  TCCR1B = 0; // stop clock
  TIMSK1 = 0; // cancel clock timer interrupt
}
void startTimer1(){
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = gReloadTimer1; // compare match register 16MHz/256
  TCCR1B |= (1<<WGM12);   // CTC mode
  TCCR1B |= (1<<CS12);    // 256 prescaler 
  TIMSK1 |= (1<<OCIE1A);  // enable timer compare interrupt
  interrupts();
}

void Active_Buzzer(){
  //buzzer interupt, when the timer reaches zero after being set, turn on the buzzer
  unsigned char i;
  unsigned char sleep_time = 1;
  
  for(i=0;i<100;i++ ){
    digitalWrite(BUZZER,HIGH);
    delay(sleep_time);
    digitalWrite(BUZZER,LOW);
    delay(sleep_time);
  }  
  
  
    
}

void loop() {
  
  
  //if increment button is pressed, check if the count is zero, if it is activate the buzzer
  //if the buzz flag = 1, check if count = 1, activate buzzer
  // Attend Button 2 ISR
  if(gISRFlag1 == 1)
  {
    // Reset ISR Flag
    gISRFlag1 = 0;

    if(gTimerRun == 0)
    {
      // Start Timer
      gTimerRun = 1;

      if(gCount == 0)
        gCount = DEF_COUNT;

      if(gBuzzerFlag == 1)
      {
        gBuzzerFlag = 0;

        // LEDs -> Timer Stopped
        digitalWrite(RED_LED, HIGH);
        digitalWrite(GREEN_LED, HIGH);
      }
      else
      {
        startTimer1();
        // LEDs -> Timer Running
        digitalWrite(RED_LED, LOW);
        digitalWrite(GREEN_LED, HIGH);
      }
    }
    else
    {
      // Stop Timer
      stopTimer1();
      gTimerRun = 0;

      // LEDs -> Timer Running
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, HIGH);
    }
  }

  // Attend gBuzzerFlag
  if(gBuzzerFlag == 1)
  {
    // Make Noise...
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    Active_Buzzer();
  }
    
  
}
