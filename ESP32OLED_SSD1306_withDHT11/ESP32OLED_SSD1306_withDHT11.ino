// Include the correct display library
// For a connection via I2C using Wire include
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
// or #include "SH1106Wire.h", legacy include: `#include "SH1106.h"`
// For a connection via I2C using brzo_i2c (must be installed) include
// #include <brzo_i2c.h> // Only needed for Arduino 1.6.5 and earlier
// #include "SSD1306Brzo.h"
// #include "SH1106Brzo.h"
// For a connection via SPI include
// #include <SPI.h> // Only needed for Arduino 1.6.5 and earlier
// #include "SSD1306Spi.h"
// #include "SH1106SPi.h"

//For Humidity DHT11 sensor
#include "DHTesp.h"

DHTesp dht;

// Include custom images
#include "images.h"

// Initialize the OLED display using SPI
// D5 -> CLK
// D7 -> MOSI (DOUT)
// D0 -> RES
// D2 -> DC
// D8 -> CS
// SSD1306Spi        display(D0, D2, D8);
// or
// SH1106Spi         display(D0, D2);

// Initialize the OLED display using brzo_i2c
// D3 -> SDA
// D5 -> SCL
// SSD1306Brzo display(0x3c, D3, D5);
// or
// SH1106Brzo  display(0x3c, D3, D5);

// Initialize the OLED display using Wire library
SSD1306Wire  display(0x3c, 21, 22);//For ESP 32 (SDA-21, SCL-22)
// SH1106 display(0x3c, D3, D5);


#define DEMO_DURATION 3000
typedef void (*Demo)(void);

int demoMode = 0;
int counter = 1;

#define InMoisturePin 27
int moistureRead;

#define InModeButtonPin 26
#define InMenuButtonPin 25
#define InSelectButtonPin 33
#define InHourButtonPin 32
#define InMinuteButtonPin 35
#define InDurationButtonPin 34

volatile int interruptCounter = 0;
int numberOfInterrupts = 0;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR handleInterrupt() {

  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) 
  {
  portENTER_CRITICAL_ISR(&mux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&mux);
  }
  last_interrupt_time = interrupt_time;
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();


  // Initialising the UI will init the display too.
  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

  dht.setup(13); // Connect DHT sensor to GPIO 13

  //Input Buttons
  //pinMode(InModeButtonPin, INPUT);
  pinMode(InMenuButtonPin, INPUT);
  pinMode(InSelectButtonPin, INPUT);
  pinMode(InHourButtonPin, INPUT);
  pinMode(InMinuteButtonPin, INPUT);
  pinMode(InDurationButtonPin, INPUT);

  pinMode(InMenuButtonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(InMenuButtonPin), handleInterrupt, FALLING);
  
}

void drawFontFaceDemo() {
    // Font Demo1
    // create more fonts at http://oleddisplay.squix.ch/
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "Hello Dilu");
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 10, "Hello Dilu");
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 26, "Hello Dilu");
}

void drawTextFlowDemo() {
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawStringMaxWidth(0, 0, 128,
      "Lorem ipsum\n dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore." );
}

void drawTextAlignmentDemo() {
    // Text alignment demo
  display.setFont(ArialMT_Plain_10);

  // The coordinates define the left starting point of the text
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 10, "Left aligned (0,10)");

  // The coordinates define the center of the text
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 22, "Center aligned (64,22)");

  // The coordinates define the right end of the text
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, 33, "Right aligned (128,33)");
}

void drawRectDemo() {
      // Draw a pixel at given position
    for (int i = 0; i < 10; i++) {
      display.setPixel(i, i);
      display.setPixel(10 - i, i);
    }
    display.drawRect(12, 12, 20, 20);

    // Fill the rectangle
    display.fillRect(14, 14, 17, 17);

    // Draw a line horizontally
    display.drawHorizontalLine(0, 40, 20);

    // Draw a line horizontally
    display.drawVerticalLine(40, 0, 20);
}

void drawCircleDemo() {
  for (int i=1; i < 8; i++) {
    display.setColor(WHITE);
    display.drawCircle(32, 32, i*3);
    if (i % 2 == 0) {
      display.setColor(BLACK);
    }
    display.fillCircle(96, 32, 32 - i* 3);
  }
}

void drawProgressBarDemo() {
  int progress = (counter / 5) % 100;
  // draw the progress bar
  display.drawProgressBar(0, 32, 120, 10, progress);

  // draw the percentage as String
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 15, String(progress) + "%");
}

void drawImageDemo() {
    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
    // on how to create xbm files
    display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
}

//Demo demos[] = {getDHTData, drawFontFaceDemo, drawTextFlowDemo, drawTextAlignmentDemo, drawRectDemo, drawCircleDemo, drawProgressBarDemo, drawImageDemo};
Demo demos[] = {getDHTData, dayView, dayView, dayView, dayView, dayView, dayView, dayView};
int demoLength = (sizeof(demos) / sizeof(Demo));

void loop() {
   if(interruptCounter>0){
 
      portENTER_CRITICAL(&mux);
      interruptCounter--;
      portEXIT_CRITICAL(&mux);
 
      numberOfInterrupts++;
      Serial.print("An interrupt has occurred. Total: ");
      Serial.println(numberOfInterrupts);

      demoMode = (demoMode + 1)  % demoLength;
  }
  
  // clear the display
  display.clear();

  //Get Humidity sesor data
  //getDHTData();


  // draw the current demo method
  demos[demoMode]();

  display.display();
  delay(10);
}


byte pgmIndexNo =0;

void dayView(){
  String day = "Monday";
  switch (demoMode) {
    case 1:
      day = "Monday";
      break;
    case 2:
      day = "Tuesday";
      break;
    case 3:
      day = "Wednesday";
      break;
    case 4:
      day = "Thursday";
      break;
    case 5:
      day = "Friday";
      break;
    case 6:
      day = "Saturday";
      break;
    case 7:
      day = "Sunday";
      break;
  }

  
  // create more fonts at http://oleddisplay.squix.ch/
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  display.setFont(ArialMT_Plain_10);
  display.drawString(10, 0, day);

  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 7, "  Pgm HH MM Duration");

  byte selectIndecator=0, hourVal = 13, minuteVal= 45, duration = 10, numPrograms = 3;

//  #define InModeButtonPin 26
//#define InMenuButtonPin 25
//#define InSelectButtonPin 33
//#define InHourButtonPin 32
//#define InMinuteButtonPin 35
//#define InDurationButtonPin 34
  int buttonState = digitalRead(InSelectButtonPin);
  if(buttonState == 0){
    pgmIndexNo = (pgmIndexNo + 1) % numPrograms;
    delay(200);
  }

  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 17, "> ");
  
  display.drawString(10, 17, String(pgmIndexNo)+" "+String(hourVal)+" "+String(minuteVal)+" "+String(duration));
  display.drawString(10, 33, String(pgmIndexNo)+" "+String(hourVal)+" "+String(minuteVal)+" "+String(duration));
  display.drawString(10, 49, String(pgmIndexNo)+" "+String(hourVal)+" "+String(minuteVal)+" "+String(duration));
}

void getDHTData() {

  delay(dht.getMinimumSamplingPeriod());

  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();

  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(humidity, 1);
  Serial.print("\t\t");
  Serial.print(temperature, 1);
  Serial.print("\t\t");
  Serial.print(dht.toFahrenheit(temperature), 1);
  Serial.print("\t\t");
  Serial.print(dht.computeHeatIndex(temperature, humidity, false), 1);
  Serial.print("\t\t");
  Serial.println(dht.computeHeatIndex(dht.toFahrenheit(temperature), humidity, true), 1);  

    // Font OLED
  // create more fonts at http://oleddisplay.squix.ch/
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Current env:");
  
  if(!isnan(humidity)){
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 10, "Humidity: "+String(humidity) + "%");
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 26, "Temperature: "+ String(temperature));
  }else{
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 10, "Humidity: ...");
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 26, "Temperature: ...");
  }

  
  moistureRead = analogRead( InMoisturePin );
  Serial.println(moistureRead);
  moistureRead = map(moistureRead, 0, 1023, 0, 255);
  
    
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 42, "Moisture: "+ String(moistureRead));

  
}


