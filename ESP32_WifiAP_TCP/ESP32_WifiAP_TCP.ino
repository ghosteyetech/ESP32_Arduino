
// Include files.
#include <WiFi.h>

// Remove this line once you've entered WiFi SSID and password below.
//#error "WiFi SSID and password required!"

// Your network SSID (network name).
// TODO: Enter the name of your wifi network here.
//char ssid[] = "GhostAP";

// Your network password.
// TODO: Enter the password of your wifi network here.
//char pass[] = "12345678aa";

const char* ssid     = "GhostAP";
const char* passphrase = "12345678aa";

// Your network key Index number (needed only for WEP).
int keyIndex = 0;

// Server status flag.
int status = WL_IDLE_STATUS;

// Create WiFi server listening on the given port.
WiFiServer server(80);

void setup()
{
  bool ret;
  
  Serial.begin(115200);
 
  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Take Online ... ");
  Serial.println(ssid);

  /*
   * Set up an access point
   * @param ssid          Pointer to the SSID (max 63 char).
   * @param passphrase    (for WPA2 min 8 char, for open use NULL)
   * @param channel       WiFi channel number, 1 - 13.
   * @param ssid_hidden   Network cloaking (0 = broadcast SSID, 1 = hide SSID)
   */
    ret = WiFi.softAP(ssid, passphrase, 2, 0);

    Serial.println("");
    Serial.println("WiFi AP online ...");
    
    server.begin();

    // Print WiFi status.
    printWifiStatus();
    
}

void loop()
{
  

  // Listen for incoming client requests.
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  Serial.println("Client connected");

  String request = readRequest(&client);
  executeRequest(&client, &request);

  // Close the connection.
  //client.stop();

  Serial.println("Client disonnected");
}

// Read the request line. The string from the JavaScript client ends with a newline.
String readRequest(WiFiClient* client)
{
  String request = "";

  // Loop while the client is connected.
  while (client->connected())
  {
    // Read available bytes.
    while (client->available())
    {
      // Read a byte.
      char c = client->read();

      // Print the value (for debugging).
      Serial.write(c);

      // Exit loop if end of line.
      if ('\n' == c)
      {
        return request;
      }

      // Add byte to request line.
      request += c;
    }
  }
  return request;
}

void executeRequest(WiFiClient* client, String* request)
{
  char command = readCommand(request);
  int n = readParam(request);
  if ('O' == command)
  {
    pinMode(n, OUTPUT);
  }
  else if ('I' == command)
  {
    pinMode(n, INPUT);
  }
  else if ('L' == command)
  {
    digitalWrite(n, LOW);
  }
  else if ('H' == command)
  {
    digitalWrite(n, HIGH);
  }
  else if ('R' == command)
  {
    sendResponse(client, String(digitalRead(n)));
  }
  else if ('A' == command)
  {
    sendResponse(client, String(analogRead(n)));
  }
}

// Read the command from the request string.
char readCommand(String* request)
{
  String commandString = request->substring(0, 1);
  return commandString.charAt(0);
}

// Read the parameter from the request string.
int readParam(String* request)
{
  // This handles a hex digit 0 to F (0 to 15).
  char buffer[2];
  buffer[0] = request->charAt(1);
  buffer[1] = 0;
  return (int) strtol(buffer, NULL, 16);
}

void sendResponse(WiFiClient* client, String response)
{
  // Send response to client.
  client->println(response);

  // Debug print.
  Serial.println("sendResponse:");
  Serial.println(response);
}

void printWifiStatus()
{
  Serial.println("WiFi status");

  // Print network name.
  Serial.print("  SSID: ");
  Serial.println(WiFi.SSID());

  // Print WiFi shield IP address.
  IPAddress ip = WiFi.localIP();
  Serial.print("  IP Address: ");
  Serial.println(ip);

  // Print the signal strength.
  long rssi = WiFi.RSSI();
  Serial.print("  Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
