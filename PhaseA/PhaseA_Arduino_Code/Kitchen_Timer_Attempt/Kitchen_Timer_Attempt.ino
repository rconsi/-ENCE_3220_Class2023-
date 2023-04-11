#define BUTTON_1  2
#define BUTTON_2  3
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

volatile unsigned char count = 0;
volatile unsigned char countTwo = 0;
//buzzer flag
volatile unsigned char buzz_flag = 0;
volatile unsigned char buzz_flag2 = 0;

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

  // 7-Seg Display
  pinMode(DIGIT_1, OUTPUT);
  pinMode(DIGIT_2, OUTPUT);
  pinMode(DIGIT_3, OUTPUT);
  pinMode(DIGIT_4, OUTPUT);

  // 7-Seg Display
  pinMode(DIGIT_1, OUTPUT);
  pinMode(DIGIT_2, OUTPUT);
  pinMode(DIGIT_3, OUTPUT);
  pinMode(DIGIT_4, OUTPUT);

  // Shift Register Pins
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);

  disp_on(10);
  //disp_on(11);
}
  //clockDown function
void countDown(){
  if(count != 0){
    count--;
    delay(150000);
    Display_Numbers(10);
   //Display_Numbers(11);
  }
}



void displayDigit(unsigned char num)
{
  //disp_on(10);
  //disp_on(11);
  digitalWrite(LATCH, LOW);
  shiftOut(DATA, CLOCK, MSBFIRST, table[num]);
  digitalWrite(LATCH, HIGH);
}

void Display_Numbers(int digit)
{
  displayDigit(count);
  //displayDigit(countTwo,digitTwo);
  //displayDigit(count);
  //displayDigit(count, 10);
  //displayDigit(countTwo,11);
  if(count >= 9)
  {
    count = 0;
  }
}
/* Attempt to use different function for each digit
void Display_NumberTwo(int digit)
{
  displayDigit(countTwo,digit);
  //if the count reaches 9, reset the number back to zero
  if(countTwo > 9)
  {
    countTwo = 0;
  }
}*/

void disp_on(int digit)
{
   digitalWrite(DIGIT_1, HIGH);
   digitalWrite(DIGIT_2, HIGH);
   digitalWrite(DIGIT_3, HIGH);
   digitalWrite(DIGIT_4, HIGH);
   digitalWrite(digit,LOW);
   //digitalWrite(digitTwo,LOW);
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

bool debounceDone;
unsigned long pressTime = 0;
int delayPeriod = 500;

void Button_1_ISR()
{
  unsigned int buttonState = 0;  // variable for reading the pushbutton status
  //buzz_flag = 0;
  buttonState = digitalRead(BUTTON_1);
  
    if(buttonState == HIGH){
    digitalWrite(RED_LED, buttonState);
    count++;
   
    //Display_Numbers(10);  
    Display_Numbers(10);
    }
    /*if (count = 0){
      countTwo++;
    }*/
  
  /*digitalWrite(RED_LED, buttonState);
  count++;
  Display_Numbers();*/
  
}


void Button_2_ISR()
{ 
  unsigned int buttonState = 0;  // variable for reading the pushbutton status
  
  buttonState = digitalRead(BUTTON_2);
  digitalWrite(GREEN_LED, buttonState);
  if( buttonState == HIGH ){
    buzz_flag2 = 1;
  }
  
  countDown();
  
  
}


void loop() {
  
  
  //if increment button is pressed, check if the count is zero, if it is activate the buzzer
  //if the buzz flag = 1, check if count = 1, activate buzzer
  if (buzz_flag == 1 && buzz_flag2 == 1){
    if(count == 0){
    Active_Buzzer();   
  }
  }
    
  
}
