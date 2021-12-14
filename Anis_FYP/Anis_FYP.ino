#define BLYNK_TEMPLATE_ID "TMPLRhSvXcTB"
#define BLYNK_DEVICE_NAME "Anis FYP"
#define BLYNK_AUTH_TOKEN "7F6-PHH5O1l0t8ZMPSVqXE7dJXSFbSq0"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Initialize Telegram BOT
#define BOTtoken "2009933213:AAGjSOKIea96DVEjrx8KGlkpl2Q-mOJuUMY"  // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "824154400"
#define rainSensor 35 //D35

#define VPIN_BUTTON_1  V1
#define VPIN_BUTTON_2  V2

// define the GPIO connected with Relays and switches
#define RelayPin1 15 //D15
#define RelayPin2 2 //D2

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "AnisAziz";
char pass[] = "anisaziz";

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int SoilSensor1 = A0;
int Soilsensorvalue1 = 0;
int Soiloutputvalue1 = 0;

int SoilSensor2 = A3;
int Soilsensorvalue2 = 0;
int Soiloutputvalue2 = 0;

int rainSensorVal; 

int toggleState_1 = 0; //Define integer to remember the toggle state for relay 1
int toggleState_2 = 0; //Define integer to remember the toggle state for relay 2


BlynkTimer timer;

BLYNK_CONNECTED()
{
    // Request the latest state from the server
    Blynk.syncVirtual(VPIN_BUTTON_1);
    Blynk.syncVirtual(VPIN_BUTTON_2);
}

// When App button is pushed - switch the state
BLYNK_WRITE(VPIN_BUTTON_1)
{
  toggleState_1 = param.asInt();
  digitalWrite(RelayPin1, toggleState_1);
}

BLYNK_WRITE(VPIN_BUTTON_2)
{
  toggleState_2 = param.asInt();
  digitalWrite(RelayPin2, toggleState_2);
}


void automation1()
{
    if (Soiloutputvalue1>65)
    {
      delay(2000);

      digitalWrite(RelayPin1, HIGH); // turn Off relay 1
      toggleState_1 = 1;
      Serial.println("Pump OFF");
      bot.sendMessage(CHAT_ID, "Pump 1 is Turning Off", "");
      Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);   // Update Button Widget
    }

    else if (Soiloutputvalue1<25)
    {
      delay(2000);

      digitalWrite(RelayPin1, LOW); // turn On relay 1
      toggleState_1 = 0;
      Serial.println("Pump ON");
      bot.sendMessage(CHAT_ID, "Pump 1 is turning ON", "");
      Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);   // Update Button Widget
    }
}

void automation2()
{
    if (Soiloutputvalue2>65)
    {
      digitalWrite(RelayPin2, HIGH); // turn off relay 2
      toggleState_2 = 1;
      Serial.println("Pump OFF");
      bot.sendMessage(CHAT_ID, "Pump 2 is turning OFF", "");
      Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);   // Update Button Widget
    }

    else if (Soiloutputvalue2<25)
    {
      delay(2000);

      digitalWrite(RelayPin2, LOW); // turn on relay 2
      toggleState_2 = 0;
      Serial.println("Pump ON");
      bot.sendMessage(CHAT_ID, "Pump 2 is turning ON", "");
      Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);   // Update Button Widget
    }
}

void setup()
{
  Serial.begin(115200); /* Define baud rate for serial communication */
  //Relay Pin
  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);

  //During Starting all Relays should TURN OFF
  digitalWrite(RelayPin1, toggleState_1);
  digitalWrite(RelayPin2, toggleState_2);

 //Rain Sensor
  pinMode(rainSensor,INPUT);

  Blynk.begin(auth, ssid, pass);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

bot.sendMessage(CHAT_ID, "Bot started up", "");
}

void loop()
{
    //Soil Moisture Sensor Read And Print
    Soilsensorvalue1=analogRead(SoilSensor1);
    Soiloutputvalue1=map(Soilsensorvalue1,4095,1500,0,100); //100,0 pottupaaru
    Soilsensorvalue2=analogRead(SoilSensor2);
    Soiloutputvalue2=map(Soilsensorvalue2,4095,1500,0,100); //100,0 pottupaaru
    //Rain Sensor Read and Print
    rainSensorVal = digitalRead(rainSensor);
    

     if(rainSensorVal==1)
    {
        Serial.println("Soil Moiture Level:");
        Serial.println(Soiloutputvalue1);
        automation1();
    
        Serial.println("Soil Moiture Level:");
        Serial.println(Soiloutputvalue2);
        automation2();
        
    }

    else if(rainSensorVal==0)
    {
        bot.sendMessage(CHAT_ID, "Raining", "");
        bot.sendMessage(CHAT_ID, "Please turn OFF both Pump if It is turning ON", "");
        
    }
    
    
    Blynk.virtualWrite(V4,Soiloutputvalue1);
    Blynk.virtualWrite(V5,Soiloutputvalue2);
    Blynk.virtualWrite(V7,rainSensorVal);
    
    Blynk.run();
}