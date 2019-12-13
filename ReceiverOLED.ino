// Arduino9x_RX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (receiver)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Arduino9x_TX


 //Tested OK in 128X32 OLED,Not OK in 128X64 OLED
#include <SPI.h>
#include <RH_RF95.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32//64 // OLED display height, in pixels

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
 
// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 433.0
 
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
 
// Blinky on receipt
//#define LED 5

void(* resetFunc) (void) = 0; //declare reset function at address 0


// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void testdrawchar(void) {
  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  // Not all the characters will fit on the display. This is normal.
  // Library will draw what it can and the rest will be clipped.
  for(int16_t i=0; i<256; i++) {
    if(i == '\n') display.write(' ');
    else          display.write(i);
  }

  display.display();
  delay(2000);
}

void setup() 
{
//  pinMode(A5, OUTPUT);//SDA
//  pinMode(A4, OUTPUT);//SCL
 
  //while (!Serial);
  Serial.begin(9600);
    delay(100);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address or 0x3D for
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
    {
        Serial.println("Now reset the Maduino Lora");
        delay(1000);
        resetFunc();//restart 
        delay(20);
    }
  }
  Serial.println("Maduino LoRa Receive Test!");
  //pinMode(LED, OUTPUT);     
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("www.makerfabs.com");
  display.println("MakePython ESP8266!");
  display.display();
  delay(2000);
  

  #if 1
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");
 
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
 
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
 
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
  #endif
  display.clearDisplay();

  testdrawchar();
  delay(2000);
}
 
void loop()
{
  if(rf95.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    //Serial.println("Length="+String(RH_RF95_MAX_MESSAGE_LEN));//Length=251
    if (rf95.recv(buf, &len))
    {
      //digitalWrite(LED, HIGH);
      //RH_RF95::printBuffer("Received: ", buf, len);
      Serial.print("Got: ");
      Serial.println((char*)buf);
      //Serial.print("length: ");
      //Serial.println((String)len);
      //Serial.println((String)sizeof(buf));
      String getPacketStr="";//Arrays.toString(buf);
      int ci=0;
      //for(ci=0;buf[ci]!='\0';ci++)
      for(ci=0;ci<16;ci++)
      {
        //getPacketStr += String(buf[ci]);
      }
      Serial.println(getPacketStr);
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);      
      display.setCursor(8, 0);
      
      //display.println(getPacketStr);
      //display.display();
      display.println("Makerfabs MakePython");
      display.setCursor(0, 16);
      for(int16_t i=0; i< len; i++) {
        //if(i == '\n') display.write(' ');
        //else          display.write(buf[i]);
        display.write(buf[i]);
      }
      display.display();
       
  
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
      
      // Send a reply
      uint8_t data[] = "And hello back to you";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      Serial.println("Sent a reply");
      //digitalWrite(LED, LOW);
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
}
