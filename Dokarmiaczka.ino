#include <Arduino.h>
#include <TM1637Display.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <iostream>   // std::cout
#include <string>
#include <sstream>

#define BLYNK_PRINT Serial

SimpleTimer timer;
int newTimer = 1;

char pass[] = "elektron";
char ssid[] = "Casa Nostra";
char auth[] = "259452416c1b4ef9bf87cd824cabf9ad";
const long interval = 2000; //interval how many miliseconds button should be pushed
int feed_x_times = 10;
int on_off_state, readiness;
int dispense_delay = 2000;

const uint8_t DONE [ ] = {
    SEG_B | SEG_C | SEG_D | SEG_E | SEG_G, //d
    SEG_C | SEG_D | SEG_E | SEG_G, //  o
    SEG_C | SEG_E | SEG_G, //n
    SEG_A | SEG_D | SEG_E | SEG_F | SEG_G, // E
};

const uint8_t DOIN [ ] = {
    SEG_B | SEG_C | SEG_D | SEG_E | SEG_G, //d
    SEG_C | SEG_D | SEG_E | SEG_G, //  o
    SEG_C, //i
    SEG_C | SEG_E | SEG_G, // n
};

const uint8_t REDY [ ] = {
    SEG_E | SEG_G,// r
    SEG_A | SEG_D | SEG_E | SEG_F | SEG_G, // E
    SEG_B | SEG_C | SEG_D | SEG_E | SEG_G, //d
    SEG_B | SEG_C | SEG_D | SEG_F | SEG_G,  //i
};

const uint8_t R15 [ ] = {
    SEG_E | SEG_G,// r
    0x00,
    SEG_B | SEG_C, // 1
    SEG_A | SEG_C | SEG_D | SEG_F | SEG_G,  // 5
};

const uint8_t R4 [ ] = {
    SEG_E | SEG_G,// r
    0x00,
    0x00,
    SEG_B | SEG_C | SEG_F | SEG_G,  //4
};

const int CLK = D1; //Set the CLK pin connection to the display
const int DIO = D2; //Set the DIO pin connection to the display
const int UP = D6;  // Set input pin for physical button "+ 1 PORTION"
const int DOWN = D5;  // Set input pin for physical button "- 1 PORTION"
const int FEED = D0; // Set input pin for physical button FEED

int upstate = digitalRead(UP);
int downstate = digitalRead(DOWN);
int feedstate = 0;

TM1637Display display(CLK, DIO); //set up the 4-Digit Display.

int value = 1244;
uint8_t segto = 0x80 | display.encodeDigit((value / 100)%10);

void feed(int times);
void function();
//void combine(int i, int times);

WidgetLCD lcd(V5);

BLYNK_WRITE(V3)
{
    ::feed_x_times = param.asInt(); // assigning incoming value from pin V3 to a variable
    display.showNumberDec(feed_x_times); //Display the numCounter value;
}

BLYNK_WRITE(V4)
{
    int startTime;
    on_off_state = param.asInt(); // assigning incoming value from pin V4 to a variable
    // You can also use:
    // String i = param.asStr();
    // double d = param.asDouble();
    Serial.print("V4 Slider value is: ");
    Serial.println(String(on_off_state));
    if (on_off_state == 1){
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
    display.setBrightness(0x0a); //set the diplay to maximum brightnes
    display.setSegments(REDY);
    delay(1000);
    display.showNumberDec(::feed_x_times); //Display the numCounter value;
    lcd.clear(); //Use it to clear the LCD Widget lcd.print(0, 0, ; "Skonczone" );
    lcd.print(0,0, "Dokarmiaczka");
    lcd.print(0,1, "gotowa");
}


void loop()
{
    Blynk.run();
    timer.run();
    if( (digitalRead(UP) == 1) && (::feed_x_times < 15)){
        upstate = 1;
        Serial.println("UP");
    } else if ((digitalRead(UP) == 1) && (::feed_x_times == 15)) {
        display.setSegments(R15);
    } else if ((digitalRead(UP) == 0) && (::feed_x_times == 15)) {
        display.showNumberDec(::feed_x_times); //
    } else if ((digitalRead(UP) == 0) && (::feed_x_times < 15) && (upstate == 0)) {
        display.showNumberDec(::feed_x_times);
    } else if ((digitalRead(UP) == 0) && (::feed_x_times < 15) && (upstate == 1)) {
        ::feed_x_times += 1;
        display.showNumberDec(::feed_x_times); //
        upstate = 0;
    }
    if ((digitalRead(DOWN) == 1) && (::feed_x_times > 4)){
        downstate = 1;
        Serial.println("DOWN");
    } else if ((digitalRead(DOWN) == 1) && (::feed_x_times == 4 )) {
        display.setSegments(R4);
    } else if ((digitalRead(DOWN) == 0) && (::feed_x_times == 4 )) {
        display.showNumberDec(::feed_x_times);
    } else if ((digitalRead(DOWN) == 0) && (::feed_x_times > 4) && (downstate == 0)){
        display.showNumberDec(::feed_x_times);
    } else if ((digitalRead(DOWN) == 0) && (::feed_x_times > 4) && (downstate == 1)){
        display.showNumberDec(::feed_x_times);
        ::feed_x_times -= 1;
        display.showNumberDec(::feed_x_times);
        downstate = 0;
    }
    if ((digitalRead(FEED) == LOW) && feedstate == 0){
        feedstate = 1;
    }
    if((digitalRead(FEED) == HIGH) && feedstate == 1){
        feedstate = 0;
        function();
    }
}

void function()
{
    Serial.print("V3 Slider value is: ");
    Serial.println(String(::feed_x_times));
    display.setSegments(DOIN);
    lcd.clear(); //Use it to clear the LCD Widget lcd.print(0, 0, "zaczynam" );
    lcd.print(0,0, "Zaczynam");
    lcd.print(0,1, "karmienie..");
    // startup music
    delay(3000);
    lcd.clear(); //Use it to clear the LCD Widget lcd.print(0, 0, "zaczynam" );
    feed(feed_x_times);
}

void feed(int times)
{
    uint8_t segto; // COLCON PART 
    int value = 1244; // COLCON PART
    display.setSegments(&segto, 1, 1);
    for(int i=1; i <= times; ++i){
        display.showNumberDecEx(0,64);
        segto = 0x80 | display.encodeDigit((value / 100)%10);// COLCON PART
        display.setSegments(&segto, 1, 1); // COLON PART
        int d  = combine(times, i);
        display.showNumberDec(d); //Display the numCounter value;
        lcd.print (0, 0, "Wydawanie"); //Use it to clear the LCD Widget lcd.print(0, 0, d);
        lcd.print (0, 1, (String(i) + " z " + String(times) + "  " + String(dispense_delay/1000) + " sek")); //Use it to clear the LCD Widget lcd.print(0, 0, d);
        //dispense food
        delay(dispense_delay);// delay for slow food eating
        lcd.clear(); //Use it to clear the LCD Widget lcd.print(0, 0, d);

    }
    // beep done
    display.setSegments(DONE);
    Blynk.notify(("Zakończono karmienie. Czas: " + String(times*dispense_delay/1000) + " sek."));
    lcd.clear(); //Use it to clear the LCD Widget lcd.print(0, 0, ; "Skonczone" );
    lcd.print(0,0, "Zakończono");
    lcd.print(0,1, "karmienie");
    delay(2000);
    ///set display to none
    lcd.clear();
    display.setSegments(REDY);
    lcd.print(0,0, "Dokarmiaczka");
    lcd.print(0,1, "gotowa");
    readiness = 1;

}

int combine(int a, int b) {
    return a*100 + b;
}
