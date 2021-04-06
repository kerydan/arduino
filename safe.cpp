#include "Display.h"
//These are the values that work for me
enum lightness
{
  const int CLOSED = 90,
  const int OPEN = 620
}
enum color
{
  RED = 4,
  YELLOW = 7,
  GREEN = 5   // AK: Add empty lines between logical groups of constants. 
}            // AK: as improvement for future, enum Color (); can be used instead of constants. 
const int BUZZER = 3;
const int LDR_IN = 16;
const int INPUT_KEY = 9;
const int CONFIRM_KEY = 8;

const String CODE = "1234";

void alarm();   // AK: separate functions bodies with 1 empty line
void setup() {
  // put your setup code here, to run once:
  pinMode(color::RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(INPUT_KEY, INPUT_PULLUP);
  pinMode(CONFIRM_KEY, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  
  Serial.begin(9600);        // AK: It should be const instead of "magic number" 
  Display.clear();
}

// The sounds I use here are pretty random, I got all of them here: https://www.arduino.cc/en/Tutorial/BuiltInExamples/toneKeyboard in the pitches
void loop() {
  // variables           
  
  int safeState = analogRead(LDR_IN);

  // I have recently learned about the static keyword and decided to move all the
  // variables that would otherwise have been global here
  static bool passed = false;
  static bool locked = false;
  static String pass = "----";
  static int mode = 0;
  static int k = 0;
  
  if(safeState < CLOSED)
  {
    if(!passed && !locked)
    {
    locked = true; tone(BUZZER, 98, 40);} // AK: this line should be shifted to the right. Nested code
    if(locked)                            // AK: this nested code block should be shifted right.
    {
      digitalWrite(YELLOW, HIGH);
      Display.show(pass);
      static int tried = 0;
      if(test_for_press_only(INPUT_KEY))
      {
        tone(BUZZER, 208, 50);
        digitalWrite(GREEN, HIGH);
        mode++;                  // AK: 5 lines can be replaced with 1 line:     mode = (mode % 4) + 1;
                                 // AK: variable mode, maybe you can think of better self-explanatory name.
        if (mode%5 == 0)
        {
          mode++;
        }
        pass[(k%4)] = '0'+(mode%5);
      }
      else{
        digitalWrite(GREEN, LOW);
      }
      if(test_for_press_only(CONFIRM_KEY))
      {
        digitalWrite(YELLOW, HIGH);
        tone(BUZZER, 208, 70);
        if((k%4) == 3)                 // Should be const. 
        {
          static int digitsCorrect = 0;
          for(int i = 0; i < 4; i++)
          {
            if(pass[i] == CODE[i])
            {
              digitsCorrect++;
            }
          }
          if(tried > 2)
          {
            Display.show("8888");
            alarm();
            tried = 0;
          }
          if(digitsCorrect == 4){       // AK Bracket should be on the next line   . 
          digitalWrite(YELLOW, LOW);   // AK this block should be indented to the right.
          tone(BUZZER, 31, 65);
          k++;
          Display.show("Open");
          digitalWrite(GREEN, HIGH);
          locked = !locked;
          passed = true;
          digitsCorrect = 0;
          delay(5000);
          digitalWrite(GREEN, LOW);
          Display.clear();}
          else
          {
            tried++;
            tone(BUZZER, 294, 80);
            Display.show("no");
            delay(1000);
            pass = "----";         // AK: should be const
            k++;
          }
        }
        else{
          k++;
        }
        // a jump segment: my display jumps to the second digit for some reason. If yours doesn`t, remove this segment
        static bool jump = true;
        if(k==1 && jump)
        {
          k =0;
          pass = "----";   // AK: should be const
          jump = false;
          mode = 0;
        }
        // end of jump segment
      }
    }
    else{digitalWrite(YELLOW, LOW);}
  }
  
  if (safeState >= lightness::OPEN)
  {
  if(locked){alarm();}
  else{passed = false; pass = "----";}
  }
  }

// taken from https://hackaday.com/2015/12/10/embed-with-elliot-debounce-your-noisy-buttons-part-ii/
bool test_for_press_only(int key){
 
    static uint8_t button_history[20];
    bool pressed = false;
 
    button_history[key] = button_history[key] <<1;
    button_history[key] |= digitalRead(key);
    if ((button_history[key] & 0b11000111) == 0b00000111){ 
        pressed = true;
        button_history[key] = 0b11111111;
    }
    return pressed;
};

void alarm()
{
  Serial.println("Alarm safe #2");
  for(int i = 0; i < 5; i++)
  {
    tone(BUZZER, 220, 500);
    digitalWrite(RED, HIGH);
    delay(499);
    tone(BUZZER, 262, 500);
    digitalWrite(RED, LOW);
    delay(499);
  }
};
