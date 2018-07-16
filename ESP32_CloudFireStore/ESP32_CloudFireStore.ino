  
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

 
const char* ssid = "TP-LINK_85E310";
const char* password =  "mUkUnAqaz.12345.wsx";

String FIREBASE_DATABSE_URL = "https://firestore.googleapis.com";
String FIREBASE_APIKEY = "AIzaSyBZPJpezs1V-yfyKbo8T7-0uOS60zAT44E";

void setup() {
 
  Serial.begin(115200);
  delay(4000);   //Delay needed before calling the WiFi.begin
 
  WiFi.begin(ssid, password); 
 
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());
 
}
 
void loop() {
 
 if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
 
   HTTPClient http;   

   String url = FIREBASE_DATABSE_URL + "/v1beta1/projects/cloud-firestore-test-62d13/databases/(default)/documents/items?&key=" + FIREBASE_APIKEY;
   http.begin(url);  //Specify destination for HTTP request
   http.addHeader("Content-Type", "application/json");             //Specify content-type header
   

   StaticJsonBuffer<300> JSONbuffer;   //Declaring static JSON buffer
    JsonObject& JSONencoder = JSONbuffer.createObject(); 
 
    JSONencoder["fields"] = JSONbuffer.createObject();// Declare main object as field (must)
    JSONencoder["fields"]["aa"] = JSONbuffer.createObject();// define a field want to save(must)
    JSONencoder["fields"]["aa"]["stringValue"] = "dcdcdc";// define above field data type(must)
 
    char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);
    
    int httpResponseCode = http.POST(JSONmessageBuffer);   //Send the actual POST request

    Serial.println("Sending data Res Code: "+String(httpResponseCode));
      
   if(httpResponseCode>0){

    Serial.println("====> Code: "+String(httpResponseCode));
    //String response = http.getString();                       //Get the response to the request
 
    //Serial.println(httpResponseCode);   //Print return code
    //Serial.println(response);           //Print request answer
 
   }else{
 
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
 
   }
 
   http.end();  //Free resources
 
 }else{
 
    Serial.println("Error in WiFi connection");   
 
 }
 
  delay(1000);  //Send a request every 1 seconds
 
}

