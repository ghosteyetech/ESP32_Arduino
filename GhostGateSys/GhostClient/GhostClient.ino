  
#include <WiFi.h>
#include <HTTPClient.h>
 
const char* ssid = "GhostAP";
const char* password =  "12345678aa";

RTC_DATA_ATTR int bootCount = 0;

void print_wakeup_reason() {
  esp_deep_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_deep_sleep_get_wakeup_cause();

  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println("EXT0 Test");

  switch (wakeup_reason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case 3  : Serial.println("Wakeup caused by timer"); break;
    case 4  : Serial.println("Wakeup caused by touchpad"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); break;
  }
}
 
void setup() {
 
  Serial.begin(115200);
  delay(3000);

  //================<deep_sleep_config>================================
  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  /*
    First we configure the wake up source
    We set our ESP32 to wake up for an external trigger.
    There are two types for ESP32, ext0 and ext1 .
    ext0 uses RTC_IO to wakeup thus requires RTC peripherals
    to be on while ext1 uses RTC Controller so doesnt need
    peripherals to be powered on.
    Note that using internal pullups/pulldowns also requires
    RTC peripherals to be turned on.
  */
  Serial.println(esp_deep_sleep_enable_ext0_wakeup(GPIO_NUM_39, 1)); //1 = High, 0 = Low | GPIO_NUM_39 == VN pin in dev1 module

  //================</deep_sleep_config>================================
  
  WiFi.begin(ssid, password);
  delay(1000);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
 
}
 
void loop() {
 
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
 
    HTTPClient http;
 
    http.begin("http://192.168.4.1/"); //Specify the URL
    int httpCode = http.GET();                                        //Make the request
 
    if (httpCode > 0) { //Check for the returning code
 
        String payload = http.getString();
        Serial.println(httpCode);
        Serial.println(payload);

        //===============================
        //Go to sleep now
        Serial.println("Going to sleep now");
        esp_deep_sleep_start();
        Serial.println("This will never be printed");
        //===============================
      }
 
    else {
      Serial.println("Error on HTTP request");
    }
 
    http.end(); //Free the resources
  }
 
  delay(10000);
 
}
