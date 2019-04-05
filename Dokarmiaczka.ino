#include <Arduino.h>
#include <TM1637Display.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
SimpleTimer timer;
int newTimer = 1;

char pass[] = "";
char ssid[] = "Casa Nostra";
char auth[] = "";
const long interval = 2000; //interval how many miliseconds button should be pushed
int pinValue1;
int pinValue2;

const uint8_t DONE [ ] = {
SEG_B | SEG_C | SEG_D | SEG_E | SEG_G, //d
SEG_C | SEG_D | SEG_E | SEG_G, //  o
SEG_C | SEG_E | SEG_G, //n
SEG_A | SEG_D | SEG_E | SEG_F | SEG_G, // E
};
 
const int CLK = D1; //Set the CLK pin connection to the display
const int DIO = D2; //Set the DIO pin connection to the display 
TM1637Display display(CLK, DIO); //set up the 4-Digit Display.

BLYNK_WRITE(V3)
{
  ::pinValue1 = param.asInt(); // assigning incoming value from pin V3 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  display.showNumberDec(pinValue1); //Display the numCounter value;
}

BLYNK_WRITE(V4)
{
  int startTime;
  pinValue2 = param.asInt(); // assigning incoming value from pin V4 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("V4 Slider value is: ");
  Serial.println(String(pinValue2));
  if (pinValue2 == 1){
   newTimer = timer.setTimeout(2000, function);
  } else {
    timer.disable(newTimer);
  }
}

 
void setup()
{
 Serial.begin(115200);
 Serial.println("Script initialization...");
 Blynk.begin(auth, ssid, pass);
 display.setBrightness(0x0a); //set the diplay to maximum brightness
}
 
 
void loop()
{
 Blynk.run();
 timer.run();
}

void function() {
    Serial.print("V3 Slider value is: ");
    Serial.println(String(::pinValue1));
    delay(500);
    display.setSegments(DONE);
}
