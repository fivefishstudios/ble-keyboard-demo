/**************************************************************************
 Firmware Name: BLE Keyboard Demo
*****************************************************************************************/
#include <Arduino.h>
#include <ezsbc_esp32.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fivefish_PGAChip.h>
#include <SPI.h>
#include <Wire.h>

#define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>

#include <WiFi.h>
#include <esp_now.h>
#include <BleKeyboard.h>

#define LED_HEARTBEAT 1
#if LED_HEARTBEAT
#define HB_LED LED1BLUE
#define HB_LED_TIME 1000 // in milliseconds
#endif
#define LOOP_DELAY 10

// Declaration for SSD1306 display connected using software SPI (default case):
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_CLK D27
#define OLED_MOSI D26
#define OLED_RESET D25
#define OLED_DC D32
#define OLED_CS D33
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
/* Comment out above, uncomment this block to use hardware SPI 
#define OLED_DC     D7
#define OLED_CS     D8
#define OLED_RESET  D6  
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  &SPI, OLED_DC, OLED_RESET, OLED_CS);
*/

// Bluetooth keyboard
BleKeyboard bleKeyboard;

// Change these two numbers to the pins connected to encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
#define Encoder_PinA D16
#define Encoder_PinB D17

#define Encoder_Switch D34
long oldPosition = -999;
long newPosition = -999;
Encoder myEnc(Encoder_PinA, Encoder_PinB);

unsigned long timeLastPBSwitchPress;
unsigned long PBSwitchDelay = 350;  // in milliseconds

// if we need to re-assign switch order, these need to be changed. 
#define SWITCH_YLW D36
#define SWITCH_RED D39
#define SWITCH_GRN D35

#define LED_YLW D21
#define LED_RED D22   
#define LED_GRN D23

bool Status_Ylw;
bool Status_Red;
bool Status_Grn;
bool Status_EncoderSwitch;
int EncoderValue = 0;

int EncoderSwitchCounter; // used by IRQ for encoder push button (for changing channel names)
int YlwSwitchCounter;     // used by IRQ for PAD push button (for toggling PAD)
int RedSwitchCounter;     // used by IRQ for 48V push button (for toggling 48V)
int GrnSwitchCounter;     // used by IRQ for REV push button (for toggling REV)

// for IRQ
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// IRQ handler for Encoder switch
void IRAM_ATTR EncoderSwitchHandler()
{
  portENTER_CRITICAL_ISR(&mux);
  EncoderSwitchCounter++;
  portEXIT_CRITICAL_ISR(&mux);
}

// IRQ handler for PAD switch
void IRAM_ATTR YlwSwitchHandler()
{
  portENTER_CRITICAL_ISR(&mux);
  YlwSwitchCounter++;  
  portEXIT_CRITICAL_ISR(&mux);
}

// IRQ handler for 48V switch
void IRAM_ATTR RedSwitchHandler()
{
  portENTER_CRITICAL_ISR(&mux);
  RedSwitchCounter++;  
  portEXIT_CRITICAL_ISR(&mux);
}

// IRQ handler for REV switch
void IRAM_ATTR GrnSwitchHandler()
{
  portENTER_CRITICAL_ISR(&mux);
  GrnSwitchCounter++;  
  portEXIT_CRITICAL_ISR(&mux);
}

void UpdateOLEDMonitor()
{
  int line1 = 8;
  int line2 = line1 + 20 + 1;
  int line3 = line2 + 20 + 1;

  display.setTextColor(WHITE);
  display.setTextSize(4);
  display.setCursor(0,10);
  int displayValue = map(EncoderValue, 0, 16, 0, 100); // map 0-16 to 0-100
  display.print(displayValue);  

  display.setTextSize(2);
  if (Status_EncoderSwitch){
    display.setCursor(0,50);
    display.print("PB ON");
  } else {
    display.setCursor(0,40);
    display.print("      ");
  }

  if (Status_Red)
  {
    display.setCursor(90, line1);
    // display.print("RED");
    display.print("FF ");
  }
  else
  {
    display.setCursor(90, line1);
    display.print("   ");
  }

  if (Status_Grn)
  {
    display.setCursor(90, line2);
    // display.print("GRN");
    display.print("   ");
  }
  else
  {
    display.setCursor(90, line2);
    display.print("   ");
  }

  if (Status_Ylw)
  {
    display.setCursor(90, line3);
    // display.print("YLW");
    display.print("REW");
  }
  else
  {
    display.setCursor(90, line3);
    display.print("   ");
  }
}

void DisplaySwitchLEDs()
{
  digitalWrite(LED_YLW, Status_Ylw);
  digitalWrite(LED_RED, Status_Red);
  digitalWrite(LED_GRN, Status_Grn);
}

void setup()
{
  Serial.begin(115200);  // for debugging

  // bluetooth keyboard
  bleKeyboard.begin();


  // setup LED heartbeat to know it's working
  #if LED_HEARTBEAT
    pinMode(HB_LED, OUTPUT);
    digitalWrite(HB_LED, LOW);
  #endif

  // setup PB switches
  pinMode(SWITCH_YLW, INPUT);
  pinMode(SWITCH_RED, INPUT);
  pinMode(SWITCH_GRN, INPUT);

  // setup Push Button LEDs
  pinMode(LED_YLW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GRN, OUTPUT);

  // setup Encoder switch IRQ
  pinMode(Encoder_Switch, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(Encoder_Switch), EncoderSwitchHandler, RISING);

  // setup PAD switch IRQ
  pinMode(SWITCH_YLW, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(SWITCH_YLW), YlwSwitchHandler, RISING);

  // setup P48 switch IRQ
  pinMode(SWITCH_RED, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(SWITCH_RED), RedSwitchHandler, RISING);

  // setup REV switch IRQ
  pinMode(SWITCH_GRN, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(SWITCH_GRN), GrnSwitchHandler, RISING);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC))
  {
    for (;;)
      ; // Don't proceed, loop forever
  }

  // rotate 90-degrees
  // display.setRotation(1);  // 0..3, where 1=90-degrees
  display.clearDisplay(); // Immediately clear the buffer, so we don't display default splash logo

}



void loop()
{
  // blink our heartbeat LED
  #if LED_HEARTBEAT
    static uint32_t timeLast = 0;
    if (millis() - timeLast >= HB_LED_TIME)
    {
      digitalWrite(HB_LED, digitalRead(HB_LED) == LOW ? HIGH : LOW);
      timeLast = millis();
    }
  #endif

  // some ble functions
  if(bleKeyboard.isConnected()) {
    // Serial.println("BLE connected...");
    // bleKeyboard.print("Hello world");
    // delay(100);
  } else {
    // Serial.println("BLE Keyboard not connected... doing nothing ");
  }


  display.clearDisplay();

  // ISR - Rotary Encoder handler (CW, CCW)
  newPosition = myEnc.read();
  if (newPosition != oldPosition)
  {
    if (newPosition > oldPosition)
    {
      // do something if encoder is rotated CW
      EncoderValue++;
      if (EncoderValue > 16) {EncoderValue = 16;};
      if(bleKeyboard.isConnected()) {bleKeyboard.write(KEY_MEDIA_VOLUME_UP); delay(20);}
    }
    else
    {
      // do something if encoder is rotated CCW
      EncoderValue--;
      if (EncoderValue < 0) {EncoderValue = 0;};
      if(bleKeyboard.isConnected()) {bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN); delay(20);}
    }
    oldPosition = newPosition;
    
    // **********************
    // ** IMPORTANT **
    // ** Modify Encoder.h and use only case (1) and case (4) to avoid multiple increment and decrements
    // **********************
  }

  // check for Encoder PB Switch pressed
  if (EncoderSwitchCounter > 0)
  {
    // but only update if last PB press was more than xxx milliseconds ago, so we can ignore rapid switch bounces
    if (millis() - timeLastPBSwitchPress > PBSwitchDelay){ 
      timeLastPBSwitchPress = millis();  // store new time of last PB switch press
      // do something if encoder switch pressed 
      Status_EncoderSwitch = !Status_EncoderSwitch;
      if(bleKeyboard.isConnected()) {bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);}
    }
    

    // clear EncoderSwitch counter
    portENTER_CRITICAL(&mux);
    // EncoderSwitchCounter--;
    EncoderSwitchCounter = 0; // to combat switch bounce, reset immediately to zero. 
    portEXIT_CRITICAL(&mux);
  }  

  // check if Pad Switch Pressed
  if (YlwSwitchCounter > 0)
  { 
    // but only take action if last PB press was more than xxx milliseconds ago, so we can ignore rapid switch bounces
    if (millis() - timeLastPBSwitchPress > PBSwitchDelay){ 
      // do something if Pad switch pressed 
      if(bleKeyboard.isConnected()) {bleKeyboard.write(KEY_MEDIA_PREVIOUS_TRACK); delay(50);}
      digitalWrite(LED_YLW, 1);
      delay(100);
      digitalWrite(LED_YLW, 0);
      timeLastPBSwitchPress = millis();  // store new time of last PB switch press
    }
    // clear PadSwitchCounter
    portENTER_CRITICAL(&mux);
    YlwSwitchCounter--;
    portEXIT_CRITICAL(&mux);
  }


  // check if P48 Switch Pressed
  if (RedSwitchCounter > 0)
  { 
    // but only take action if last PB press was more than xxx milliseconds ago, so we can ignore rapid switch bounces
    if (millis() - timeLastPBSwitchPress > PBSwitchDelay){ 
      // do something if P48V switch pressed 
      if(bleKeyboard.isConnected()) {bleKeyboard.write(KEY_MEDIA_NEXT_TRACK); delay(50);}
      digitalWrite(LED_RED, 1);
      delay(100);
      digitalWrite(LED_RED, 0);
      timeLastPBSwitchPress = millis();  // store new time of last PB switch press
    }
    // clear P48SwitchCounter
    portENTER_CRITICAL(&mux);
    RedSwitchCounter--;
    portEXIT_CRITICAL(&mux);
  }


  // check if Rev Switch Pressed
  if (GrnSwitchCounter > 0)
  { 
    // but only take action if last PB press was more than xxx milliseconds ago, so we can ignore rapid switch bounces
    if (millis() - timeLastPBSwitchPress > PBSwitchDelay){ 
      // do something if POL switch pressed 
      if(bleKeyboard.isConnected()) {bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE); delay(50);}
      digitalWrite(LED_GRN, 1);
      delay(100);
      digitalWrite(LED_GRN, 0);
      timeLastPBSwitchPress = millis();  // store new time of last PB switch press
    }
    // clear RevSwitchCounter
    portENTER_CRITICAL(&mux);
    GrnSwitchCounter--;
    portEXIT_CRITICAL(&mux);
  }

  // display current statuses
  UpdateOLEDMonitor();
  // DisplaySwitchLEDs();

  display.display(); // refresh display with updated data

}