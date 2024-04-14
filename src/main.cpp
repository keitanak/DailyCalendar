#include <Arduino.h>

#include "TFT_eSPI.h"
#include "OpenFontRender.h"
#include <Wire.h>
#include "RTC8564.h"
#include <BH1750.h>
#include "jpcalendar.h"
#include "binaryttf.h"

#define TTF_FONT binaryttf // The font is referenced with the array name

OpenFontRender ofr;
TFT_eSPI tft;
BH1750 lightMeter;

const int LED_PIN = 15;

int interruptPin = 12;
int curentweekday = 8;
int knumber;

void setup() {

  struct dateTime dt = {0, 27, 20, 6, 4, 24, 6};

  Serial.begin(115200);

  pinMode(interruptPin, INPUT_PULLUP);
  RTC8564.begin(&dt);

  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(TFT_WHITE);

  ofr.setDrawer(tft); // Link drawing object to tft instance (so font will be rendered on TFT)

// for debug specify date
//  RTC8564.setDateTime(&dt);

  lightMeter.begin();
  Serial.println(F("BH1750 Test begin"));
 // analogWriteFreq(25000);
 // analogWriteRange(1024);
 // pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN,255);

  //randome seed for kotowaza seletion
  randomSeed(lightMeter.readLightLevel());

}

void loop() {
  struct dateTime dt;
  int wareki,jpphnum;
  bool jpph;


  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  int a= max(log(lux)*100, 100);
  Serial.println(a);
  analogWrite(LED_PIN, min(255,a));


  RTC8564.getDateTime(&dt);

  // only draw if day change detected.
  if (curentweekday != dt.weekday)
  {
    curentweekday = dt.weekday;
    jpph= false;

    tft.fillScreen(TFT_WHITE);

  if (dt.weekday == 0)
  {
    ofr.setFontColor(TFT_RED);
  } else if (dt.weekday == 6)
  {
    ofr.setFontColor(TFT_BLUE);
  } else {
    ofr.setFontColor(TFT_BLACK);
  }
  for ( int i = 0; i < sizeof(jpphlist); i++ )
  {
    if(jpphlist[i].dateval == (dt.year*10000+dt.month*100+dt.day) )
      {
        jpph=true;
        jpphnum=i;
        ofr.setFontColor(TFT_RED);
      }
  }

  ofr.setBackgroundColor(TFT_WHITE);

  // Load the font and check it can be read OK
  if (ofr.loadFont(TTF_FONT, sizeof(TTF_FONT))) {
    Serial.println("Initialise error");
    return;
  }

  // Set the cursor to top left
  ofr.setCursor(0, 0);

  // A neat feature is that line spacing can be tightend up (by a factor of 0.7 here)
  ofr.setLineSpaceRatio(0.7);

  // Aligment left start
  ofr.setAlignment(Align::Left);

  // Year with 4 digit
  ofr.setCursor(20, 20);
  ofr.setFontSize(35);
  ofr.printf("%d\n",dt.year+2000);

  // Aligment center start
  ofr.setAlignment(Align::Center);

  // Month
  ofr.setCursor(160, 20);
  ofr.setFontSize(40);
  ofr.printf("%d月\n",dt.month);

  // Japanese way of month
  ofr.setCursor(160, 70);
  ofr.setFontSize(20);
  ofr.printf("%s\n",_wafugetumei[dt.month-1]);

  // Day
  ofr.setCursor(160, 60);
  ofr.setFontSize(220);
  ofr.printf("%d\n",dt.day);

  // Weekday in Japanese
  ofr.setCursor(160, 320);
  ofr.setFontSize(40);
  ofr.printf("%s\n",_jpday[dt.weekday]);

  // If public holoiday, name of holiday
  if (jpph) {
    ofr.setCursor(160, 380);
    ofr.setFontSize(30);
    ofr.printf("%s\n",jpphlist[jpphnum].label);
    }

  // Right alignement start
  ofr.setAlignment(Align::Right);

  // Wareki
  ofr.setCursor(300, 20);
  ofr.setFontSize(25);
  wareki= dt.year - 18;
  ofr.printf("令和%d年\n",wareki);

  // eto
  ofr.setCursor(300, 60);
  ofr.setFontSize(20);
  ofr.printf("%s\n",_junishi[dt.year % 12]);

  // Rokuyo
  ofr.setCursor(300, 320);
  ofr.setFontSize(25);
  ofr.printf("%s\n",_rokuyostr[Rokuyo[dt.year-24][dt.month-1][dt.day-1]]);

  // Aligment center start
  ofr.setAlignment(Align::Center);

  // kotowaza
  ofr.setCursor(160, 400);
  ofr.setFontSize(25);
  knumber = random(513);
  ofr.printf("%s\n",Kotowaza[knumber].kotowazak);
  ofr.setCursor(160, 430);
  ofr.setFontSize(20);
  ofr.printf("%s\n",Kotowaza[knumber].kotowazah);

  // Unload font
  ofr.unloadFont();
  }

  delay(5000);
}
