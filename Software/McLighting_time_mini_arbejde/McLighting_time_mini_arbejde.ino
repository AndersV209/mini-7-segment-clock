//now with rtc ds3231
// ***************************************************************************
// Load libraries for: WebServer / WiFiManager / WebSockets
// ***************************************************************************
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

// needed for library WiFiManager
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <FS.h>

#include <WebSockets.h>           //https://github.com/Links2004/arduinoWebSockets
#include <WebSocketsServer.h>

//needed for this clock code
#include <WiFiUdp.h>
#include <Time.h>
#include <TimeLib.h>
#include <Timezone.h>    //https://github.com/JChristensen/Timezone

#include <ArduinoOTA.h>

// ***************************************************************************
// Inclode all definitions 
// ***************************************************************************
#include "definitions.h"


// ***************************************************************************
// Instanciate HTTP(80) / WebSockets(81) Server
// ***************************************************************************
ESP8266WebServer server ( 80 );
WebSocketsServer webSocket = WebSocketsServer(websocketport);


// ***************************************************************************
// Load libraries / Instanciate Neopixel
// ***************************************************************************
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMLEDS, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

//reset reason 
#if defined(debugging)
const char * const RST_REASONS[] =
{
    "REASON_DEFAULT_RST",       /* normal startup by power on */
    "REASON_WDT_RST",           /* hardware watch dog reset */
    "REASON_EXCEPTION_RST",     /* exception reset, GPIO status won’t change */
    "REASON_SOFT_WDT_RST",      /* software watch dog reset, GPIO status won’t change */
    "REASON_SOFT_RESTART",      /* software restart ,system_restart , GPIO status won’t change */
    "REASON_DEEP_SLEEP_AWAKE",  /* wake up from deep-sleep */
    "REASON_EXT_SYS_RST"        /* external system reset */
};
#endif

// ***************************************************************************
// Callback for WiFiManager library when config mode is entered
// ***************************************************************************
//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}



// ***************************************************************************
// Include: Webserver
// ***************************************************************************
#include "spiffs_webserver.h"

// ***************************************************************************
// Include: Request handlers
// ***************************************************************************
#include "request_handlers.h"

#include "RTClib.h"

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};



bool use_rtc = false;


// ***************************************************************************
// MAIN
// ***************************************************************************
void setup() {
  Serial.begin(115200);

  // ***************************************************************************
  // Setup: Neopixel
  // ***************************************************************************
  pixels.begin();
  pixels.setBrightness(brightness);
  pixels.setPixelColor(colon[0], pixels.Color(0,150,0)); // Moderately bright green color.
  pixels.setPixelColor(colon[1], pixels.Color(0,150,0)); // Moderately bright green color.
  pixels.show(); // Initialize all pixels to 'off'

  // ***************************************************************************
  // Setup: WiFiManager
  // ***************************************************************************
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  wifiManager.setTimeout(60);

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect(AP_NAME)) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    //ESP.reset();
    use_rtc = true;
    
    delay(1000);
  }

  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }



  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  pixels.setPixelColor(colon[0], pixels.Color(0,0,150)); // Moderately bright green color.
  pixels.setPixelColor(colon[1], pixels.Color(0,0,150)); // Moderately bright green color.
  pixels.show(); // Initialize all pixels to 'off'
  // ***************************************************************************
  // Setup: MDNS responder
  // ***************************************************************************
  //String hostname(HOSTNAME);
  MDNS.begin(HOSTNAME);
  Serial.print("Open http://");
  Serial.print(HOSTNAME);
  Serial.println(".local/edit to see the file browser");


  // ***************************************************************************
  // Setup: WebSocket server
  // ***************************************************************************
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);


  // ***************************************************************************
  // Setup: SPIFFS
  // ***************************************************************************
  SPIFFS.begin();
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    Serial.printf("\n");
  }

  // ***************************************************************************
  // Setup: SPIFFS Webserver handler
  // ***************************************************************************
  //list directory
  server.on("/list", HTTP_GET, handleFileList);
  //load editor
  server.on("/edit", HTTP_GET, []() {
    if (!handleFileRead("/edit.htm")) server.send(404, "text/plain", "FileNotFound");
  });
  //create file
  server.on("/edit", HTTP_PUT, handleFileCreate);
  //delete file
  server.on("/edit", HTTP_DELETE, handleFileDelete);
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  server.on("/edit", HTTP_POST, []() {
    server.send(200, "text/plain", "");
  }, handleFileUpload);
  //get heap status, analog input value and all GPIO statuses in one json call
  server.on("/esp_status", HTTP_GET, []() {
    String json = "{";
    json += "\"heap\":" + String(ESP.getFreeHeap());
    json += ", \"analog\":" + String(analogRead(A0));
    json += ", \"gpio\":" + String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16)));
    json += "}";
    server.send(200, "text/json", json);
    json = String();
  });


  //called when the url is not defined here
  //use it to load content from SPIFFS
  server.onNotFound([]() {
    if (!handleFileRead(server.uri()))
      handleNotFound();
  });

  server.on("/upload", handleMinimalUpload);

  server.on("/restart", []() {
    Serial.printf("/restart:\n");
    server.send(200, "text/plain", "restarting..." );
    ESP.restart();
  });

  server.on("/reset_wlan", []() {
    Serial.printf("/reset_wlan:\n");
    server.send(200, "text/plain", "Resetting WLAN and restarting..." );
    WiFiManager wifiManager;
    wifiManager.resetSettings();
    ESP.restart();
  });


  // ***************************************************************************
  // Setup: SPIFFS Webserver handler
  // ***************************************************************************
  server.on("/set_brightness", []() {
    if (server.arg("c").toInt() > 0) {
      brightness = (int) server.arg("c").toInt() * 2.55;
    } else {
      brightness = server.arg("p").toInt();
    }
    if (brightness > 255) {
      brightness = 255;
    }
    if (brightness < 0) {
      brightness = 0;
    }
    pixels.setBrightness(brightness);

    getStatusJSON();
  });

  server.on("/get_brightness", []() {
    String str_brightness = String((int) (brightness / 2.55));
    server.send(200, "text/plain", str_brightness );
    Serial.print("/get_brightness: ");
    Serial.println(str_brightness);
  });

  server.on("/get_switch", []() {
    server.send(200, "text/plain", (mode == OFF) ? "0" : "1" );
    Serial.printf("/get_switch: %s\n", (mode == OFF) ? "0" : "1");
  });

  server.on("/get_color", []() {
    String rgbcolor = String(main_color.red, HEX) + String(main_color.green, HEX) + String(main_color.blue, HEX);
    server.send(200, "text/plain", rgbcolor );
    Serial.print("/get_color: ");
    Serial.println(rgbcolor);
  });

  server.on("/status", []() {
    getStatusJSON();
  });

  server.on("/off", []() {
    exit_func = true;
    mode = OFF;
    getArgs();
    getStatusJSON();
  });

  server.on("/all", []() {
    exit_func = true;
    mode = ALL;
    getArgs();
    getStatusJSON();
  });

  /*server.on("/wipe", []() {
    exit_func = true;
    mode = WIPE;
    getArgs();
    getStatusJSON();
  });*/

  server.on("/rainbow", []() {
    exit_func = true;
    mode = RAINBOW;
    getArgs();
    getStatusJSON();
  });

  server.begin();



int trys=0;
   while(!getTimeAndDate() && trys<10) {
     trys++;
   }//end while
   
  File debug = SPIFFS.open("/debug.txt", "a");
  if (!debug) {
      Serial.println("file open failed");
  }

#if defined(debugging) 
  Serial.println("====== Writing to SPIFFS file =========");

  const rst_info * resetInfo = system_get_rst_info();
  debug.print(F("reset reason: "));
  debug.print(RST_REASONS[resetInfo->reason]);
  
  debug.print("  Boot time at: ");
  debug.print(hour());
  debug.print(":");
  debug.print(minute());
  debug.print(":");
  debug.println(second());
  Serial.println(millis());
  debug.close();
#endif

  // Hostname defaults to esp8266-[ChipID]
   ArduinoOTA.setHostname(HOSTNAME);

  // No authentication by default
   ArduinoOTA.setPassword("Felix");

  ArduinoOTA.onStart([]() {
    pixels.setPixelColor(colon[0], pixels.Color(150,0,0)); // Moderately bright green color.
    pixels.setPixelColor(colon[1], pixels.Color(150,0,0)); // Moderately bright green color.
    pixels.show(); // Initialize all pixels to 'off'
  });
  //ArduinoOTA.setHostname((const char *)hostname.c_str());
  ArduinoOTA.begin();
  
}// end setup 

bool any_true(){
  for (int i = 0; i < NBREG; i++){
    if (check_state[i] == true)
    return true;
  }
  return false;
}

void loop() {

if (minute() != last_minute ){
  last_minute = minute();
  

  for (int i = 0; i < NBREG; i++){
if ( ((minute() >= workbreak[i].start_minut) || (hour() > workbreak[i].start_hour))  ){
    Serial.println("true");
    }
   else {
    Serial.println("false");
    }


    
    if ((weekday()- 1) == workbreak[i].week_day){
      if ((hour() >= workbreak[i].start_hour) && ((minute() >= workbreak[i].start_minut) || (hour() > workbreak[i].start_hour)) && (hour() <= workbreak[i].stop_hour) && ((minute() <= workbreak[i].stop_minut) || ((workbreak[i].start_hour != workbreak[i].stop_hour) && (hour() != workbreak[i].stop_hour) ) )){
          Serial.print("TEST WORKS: ");
          Serial.print(i);
          Serial.print(" w: ");
          Serial.print(weekday()-1);
          Serial.print(" h: ");
          Serial.print(hour());
          Serial.print(" m: ");
          Serial.print(minute());
          Serial.print(" w: ");
          Serial.print(workbreak[i].week_day);
          Serial.print(" h: ");
          Serial.print(workbreak[i].start_hour);
          Serial.print(" m: ");
          Serial.print(workbreak[i].start_minut);
          Serial.print(" h: ");
          Serial.print(workbreak[i].stop_hour);
          Serial.print(" m: ");
          Serial.println(workbreak[i].stop_minut);
          check_state[i] = true;
          
        }
        else {
          check_state[i] = false;
          
          }
      }
    
    
    }


    
  if(any_true()){
    main_color.red = 0;
    main_color.green = 255;
    main_color.blue = 0;
  }
  else {
    main_color.red = 0;
    main_color.green = 0;
    main_color.blue = 255;
    }
  
    
}//last minute 



  
  static uint32_t lastMillis = 0;
  uint32_t currentMillis = millis() / 1000;
  static unsigned long lastMillisrainbow = 0;
  unsigned long currentMillisrainbow = millis();

  ArduinoOTA.handle();

     // Update the time via NTP server as often as the time you set at the top
    if(now()-ntpLastUpdate > ntpSyncTime || !time_is_set && now()-ntpLastUpdate > ntpSyncTime_not_set) {
      int trys=0;
      while(!getTimeAndDate() && trys<10){
        trys++;
        Serial.print("trys: ");
        Serial.println(trys);
      }
      if(trys<10){
        Serial.println("ntp server update success");
        
      #if defined(debugging)
      File debug = SPIFFS.open("/debug.txt", "a");
      if (!debug) {
          Serial.println("file open failed");
      }
      Serial.println("====== Writing to SPIFFS file =========");
      // write 10 strings to file
      //for (int i=1; i<=10; i++){
      debug.print("  Time set at: ");
      debug.print(hour());
      debug.print(":");
      debug.print(minute());
      debug.print(":");
      debug.println(second());
      debug.close();
      #endif
      
      }
      else{
        Serial.println("ntp server update failed");
        WiFi.begin();

      #if defined(debugging)
              File debug = SPIFFS.open("/debug.txt", "a");
      if (!debug) {
          Serial.println("file open failed");
      }
      Serial.println("====== Writing to SPIFFS file =========");
      // write 10 strings to file
      //for (int i=1; i<=10; i++){
      debug.println("  Time not set: ");
      debug.close();
      #endif
      
      }
    }

     
  if (currentMillis - lastMillis >= 1){
    lastMillis = currentMillis;
    /*Serial.print(hour());
    Serial.print(':');
    Serial.print(minute());
    Serial.print(':');
    Serial.print(second());
    Serial.println();*/
    hours = hour();
    minutes = minute();
    seconds = second();
    if ( analogEnable == true ){
      analogvalue = analogRead(analogInPin);
      //Serial.print("adc ");
      //Serial.print(analogvalue);
  
      if ( analogvalue < lightvalue ){
        //Serial.print(" if. nighttime_count: ");
        nighttime_count += 1;
        //Serial.println(nighttime_count);
      }
      else {
        //Serial.print(" else. nighttime_count: ");
        nighttime_count = 0;
        //Serial.println(nighttime_count);
      }
    }
    //    DateTime now = rtc.now(); //get the current date-time
    /*
    hours = now.hour();
    minutes = now.minute();
    seconds = now.second();
    Serial.printf("%02d:%02d:%02d ", now.hour(), now.minute(), now.second());
    Serial.println();*/
    led_state_change = true; //set led to be updated in this loop 
    
  }
  
  if (nighttime_count > 10 ){
   //Serial.println("night time");
    Red = 0;
    Green = 0;
    Blue = 1;
  }
  else {
    //Serial.println("day time");
    Red = main_color.red;
    Green = main_color.green;
    Blue = main_color.blue;
  }
    
  server.handleClient();
  webSocket.loop();

  // Simple statemachine that handles the different modes
  if (mode == OFF) {
                                            // change brightness instead 
    main_color.red = 0;
    main_color.green = 0;
    main_color.blue = 0;
    //led_state_change = true; //set led to be updated in this loop 
    
    /*uint16_t i;
    for (i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, 0, 0, 0);
    }
    pixels.show();*/
  }
  if (mode == ALL) {
    /*uint16_t i;
    for (i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, main_color.red, main_color.green, main_color.blue);
    }
    pixels.show();*/
  }
  if (mode == RAINBOW) { // leg lidt mere med denne 
    Serial.print("Rainbow");
    if (currentMillisrainbow - lastMillisrainbow >= rainbow_delay){
      Serial.println("2");
      lastMillisrainbow = currentMillisrainbow;
      rainbow(rainbow_swipe); //value is random from 0 to 255
      led_state_change = true; //set led to be updated in this loop 
      rainbow_swipe++;
      if (rainbow_swipe > 255){
        rainbow_swipe = 0;
      }
  }
    
  }




  j = seconds % 10;
  i = seconds / 10;
  h = minutes % 10;
  g = minutes / 10;
  k = hours % 10;
  l = hours / 10;

  //select_number(j, digit1);
  //select_number(i, digit2);
  if (led_state_change){  //check if leds need updating 
  select_number(l, digit1);
  select_number(k, digit2);
  select_number(g, digit3);
  select_number(h, digit4);

  
  if ( (seconds  & 0x01) != 0) { // toggle a second flag to blink the colon
  pixels.setPixelColor(colon[0], pixels.Color(Red,Green,Blue)); // Moderately bright green color.
  pixels.setPixelColor(colon[1], pixels.Color(Red,Green,Blue)); // Moderately bright green color.
      }
  if( ( seconds  & 0x01) == 0) { // toggle a second flag to blink the colon
    pixels.setPixelColor(colon[0], pixels.Color(0,0,0)); // Moderately bright green color.
    pixels.setPixelColor(colon[1], pixels.Color(0,0,0)); // Moderately bright green color.
  }

  pixels.show();

  led_state_change = false; 
  }

  /* // all leds 
  for (uint16_t i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, main_color.red, main_color.green, main_color.blue);
    }
    pixels.show();*/
    //delay(1);//give time to think
}
