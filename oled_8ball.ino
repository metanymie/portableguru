#include <SPI.h> //Serial Peripheral Interface
#include <Wire.h> //to communicate with I2C / TWI devices
#include <Adafruit_GFX.h> //graphics lib
#include <Adafruit_SSD1306.h> //oled screen lib
#include <ArduinoHttpClient.h> // for accessing the json server
#include <WiFi101.h> //to connect to wifi
#include <ArduinoJson.h> //json library

#include <Fonts/TomThumb.h> //smaller font for longer advice

Adafruit_SSD1306 display = Adafruit_SSD1306();                //This is all OLED Wing code

// M0 to oled screen button pins
#define BUTTON_A 9
#define BUTTON_B 6
#define BUTTON_C 5
#define LED      13

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

int advice;                           //int to pull from our list of useful/less advice
bool push;                            //bool for 1 button push
long timer = 0;                          //timer to clear screen after being inactive for 1min
long newTimer = 0;
long startupT = 0;

static char ssid[] = "ocadu-embedded";      //SSID of the wireless network at school
static char pass[] = "internetofthings";
//static char ssid[] = "SmartRG-ffb6";      //SSID of the wireless network at Roxanne's place
//static char pass[] = "3d369c7eb6";

char serverAddress[] = "rawgit.com";  // server address
int port = 80;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);
int status = WL_IDLE_STATUS;
int statusCode = 0;
String response;

String sentence[] = {
  "question",
  "Action",
  "person",
  "connector",
  "object",
  "punctuation"
};

String json;

void setup() {
  Serial.begin(9600);
  //  while (!Serial) {
  //    ;
  //  }

  WiFi.setPins(8, 7, 4, 2); //This is specific to the feather M0

  /***************************** OLED STUFF *****************************/

  //display.setFont(&TomThumb);

  Serial.println("OLED FeatherWing test");              // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);            // initialize with the I2C addr 0x3C (for the 128x32)
  // init done
  Serial.println("OLED begun");                         //More OLED wing specific code

  display.display();

  // Clear the buffer.
  display.clearDisplay();
  display.display();

  Serial.println("IO test");

  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);

  startupT = millis();

  //connection to wifi

  while (status != WL_CONNECTED && millis() - startupT <= 10000) {
    Serial.print("Attempting to connect to Network named: ");

    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
    delay(1000);
  }

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  //get the json from webserver

  json = GetJson();

  //init random seed

  randomSeed(analogRead(0));


  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Ask me for advice");
  display.setCursor(0, 20);                          //Set the question (Subject to change, possibly personalize?)
}


void loop() {


  if (! digitalRead(BUTTON_A)) ask();               //If any of the buttons are pushed, ask for advice

  if (! digitalRead(BUTTON_B)) ask();

  if (! digitalRead(BUTTON_C)) ask();

  //delay(10);
  yield();
  display.display();

  if (digitalRead(BUTTON_A) == LOW) {                 // boolean functions to limit all the buttons to push = 1 value
    delay(100);
    push = !push;
    digitalWrite(BUTTON_A, push);
  }
  if (digitalRead(BUTTON_B) == LOW) {
    delay(100);
    push = !push;
    digitalWrite(BUTTON_B, push);
  }
  if (digitalRead(BUTTON_C) == LOW) {
    delay(100);
    push = !push;
    digitalWrite(BUTTON_C, push);
  }

  if (! digitalRead(BUTTON_A));
  (! digitalRead(BUTTON_A));
  (! digitalRead(BUTTON_A));

  if ((millis() - newTimer >= 10000) && (millis() - newTimer <= 10100)) {
    display.clearDisplay();
    display.display();
    display.setCursor(0, 0);                       //Reset the cursor position
    display.print("Ask me for advice");           //Reprint the question (Should always be shown)
    display.setCursor(0, 20);                     //Set the cursor position for the answer
  }

}


void ask() {

  timer = millis();

  display.clearDisplay();                         //If there has been previous advice,
  display.display();
  display.setCursor(0, 0);                       //Set the cursor position for the answer

  if (status = WL_CONNECTED) {
    display.print(randomAdvice());
  } 
  else
  { advice = round(random(0, 6));                                              //randomise what advice is called

    if (advice == 0) {
      display.print("Hell Yeah!");                                                 //advice should be limited to 21char
    }
    if (advice == 1) {
      display.print("That's a bad idea....");
    }
    if (advice == 2) {
      display.print("What why?");
    }
    if (advice == 3) {
      display.print("Go for a beer instead");                                                 //advice should be limited to 21char
    }
    if (advice == 4) {
      display.print("Go back to bed");
    }
    if (advice == 5) {
      display.print("What would Batman do?");
    }
  }

  if (millis() - timer >= 5000) {
    display.clearDisplay();
    display.display();
    display.setCursor(0, 0);                     //Reset the cursor position
    display.print("Ask me for advice");         //Reprint the question (Should always be shown)
    display.setCursor(0, 20);                   //Set the cursor position for the answer

  }
  newTimer = millis();

}

String RandomInArray(JsonArray& ar) {
  String ret = ar[random(0, ar.size())];
  return ret;
}


String randomAdvice() {
  DynamicJsonBuffer  jsonBuffer(200);
  JsonObject& root = jsonBuffer.parseObject(json);
  String answer = "";

  for (int i = 0; i < sizeof(sentence) / sizeof(sentence[0]); i++) {
    JsonArray& actionArray = root[sentence[i]];
    String test = root[sentence[i]];
    Serial.println(test);
    answer += RandomInArray(actionArray) + " ";

  }
  return answer;
}


String GetJson() {
  Serial.println("making GET request");
  client.get("/metanymie/portableguru/master/advice_syntax.json");

  // read the status code and body of the response
  statusCode = client.responseStatusCode();
  response = client.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  return response;
}
