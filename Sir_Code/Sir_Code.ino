#define BLYNK_TEMPLATE_ID "TMPL3bdpQNZjt"
#define BLYNK_TEMPLATE_NAME "WaterMeter"
#define BLYNK_AUTH_TOKEN "P95h_5bU6qCSRwFUqFi11IgffeeYjn3G"

#ifdef ESP32
 #include <WiFi.h>
 #include <BlynkSimpleEsp32.h>
#else
 #include <ESP8266WiFi.h>
 #include <BlynkSimpleEsp8266.h>
#endif

// #define BLYNK_MAX_SENDBYTES 128
#define BLYNK_PRINT Serial

char auth[] = "P95h_5bU6qCSRwFUqFi11IgffeeYjn3G";
char ssid[] = "project02";
char pass[] = "project02";

#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define BOTtoken "7151261608:AAFfcpbsUJ_U4-mzVYXSqk_EdOdU2Or72R8"
#define CHAT_ID "7084901954"
// #define BOTtoken "7151261608:AAFfcpbsUJ_U4-mzVYXSqk_EdOdU2Or72R8"
// #define CHAT_ID "7084901954"

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

#define soil D5
int soil_sense;
String soil_status=" ";

#define sw_leak D6
int leak_sense;
String leak_status=" ";

// #define LED_BUILTIN 16
#define SENSOR  2

#define sw 23

int i=0; 
long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
boolean ledState = LOW;
float calibrationFactor = 4.5;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
unsigned long flowMilliLitres;
unsigned int totalMilliLitres;
float flowLitres;
float totalLitres;
 
void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}

void setup()
{
  Serial.begin(115200);
  client.setTrustAnchors(&cert);

  pinMode(soil,INPUT);

  pinMode(sw,INPUT_PULLUP);
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SENSOR, INPUT_PULLUP);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  Blynk.config(auth);

  bot.sendMessage(CHAT_ID, "kit ready");

  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.print("READY");
 
  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;
 
  attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);
}
 
void loop()
{
  leak_sense=digitalRead(sw_leak);
  leak_sense=1-leak_sense;

  if(leak_sense==1)
  {
    i = i+1;
    Serial.println("leak detected");
    if(i==1)
    {
      bot.sendMessage(CHAT_ID,"leak detected");
    }
    leak_status="ALERT";
  }
  else
  {
    i=0;
    Serial.println("Normal");
    leak_status="NORMAL";
  }

  soil_sense=digitalRead(soil);
  soil_sense=1-soil_sense;

  if(soil_sense==1)
  {
    soil_status="LOW";
  }
  else
  {
    soil_status="NORMAL";
  }

  currentMillis = millis();
  if (currentMillis - previousMillis > interval) 
  {
    
    pulse1Sec = pulseCount;
    pulseCount = 0;
    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    previousMillis = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    flowLitres = (flowRate / 60);
    totalMilliLitres += flowMilliLitres;
    totalLitres += flowLitres;
    Serial.print("Flow rate: ");
    Serial.print(float(flowRate));  
    Serial.print("L/min");
    Serial.print("\t");    
 
    Serial.print("Output Liquid Quantity: ");
    Serial.print(totalMilliLitres);
    Serial.print("mL / ");
    Serial.print(totalLitres);
    Serial.println("L");
  }
  
  lcd.clear();
  lcd.print("SOIL : ");lcd.print(soil_status);
  lcd.setCursor(0,1);
  lcd.print("LEAK : ");lcd.print(leak_status);
  delay(2000);
  lcd.clear();
  lcd.print("FLOW  : ");lcd.print(float(flowRate));
  lcd.setCursor(0,1);
  lcd.print("VOL : ");lcd.print(totalLitres);
  delay(2000);
}