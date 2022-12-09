//#include <FB_Const.h>
//#include <FB_Error.h>
//#include <FB_Network.h>
//#include <FB_Utils.h>
#include <Firebase.h>
#include <FirebaseESP8266.h>
//#include <FirebaseFS.h>
//#include <MB_File.h>
//#include <MB_NTP.h>

//#include <ESP8266Firebase.h>

//#include <FB_Const.h>
//#include <FB_Error.h>
//#include <FB_Network.h>
//#include <FB_Utils.h>
//#include <Firebase.h>
//#include <FirebaseFS.h>
//#include <Firebase_ESP_Client.h>
//#include <MB_NTP.h>

//Libraries needed
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
//#include <FirebaseArduino.h>

////Credentials for connection to firebase
#define API_KEY "AIzaSyBQtOIYkAQdTB2oiuXlQ6sJ0bRlf5OO4rs"
#define DATABASE_URL "infusion-status-default-rtdb.firebaseio.com/"


//Define Firebase Data Object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

int detected1, detected2 = 0;
float sensorValue=0;
boolean isEmpty;

bool signupOK =false;

//Credentials for connection to Wi-Fi
#define WIFI_SSID "Ivcare"
#define WIFI_PASSWORD "iot2022@"


int led1 = D1;      //For first level detector color code yw
int led2 = D0;      //For second level detector cold code bw
int detector1 = A0; //For first detector ow
int detector2=A0;   //For second detector gw
int buzzer = D2;

void setup() {

   Serial.begin(115200);
   pinMode(detector1, INPUT);
   pinMode(detector2, INPUT);
   pinMode(led1, OUTPUT);
   pinMode(led2, OUTPUT);
   pinMode(buzzer, OUTPUT);  
    
  //Wifi Configuration  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());


  //Firbase Connectivity

  
    /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
//  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

 }

void loop() { 
  //detected1 = analogRead(detector1);// read Laser sensor

  //Case 1
  //Checking for first level detection 
  digitalWrite(led1,HIGH);
  delay(50000);

  Serial.println("Procceding to data collection");
  delay(1000);

  
  for(int i=0;i<20;i++)
  {
   digitalWrite(led1,LOW);
   delay(400);
   digitalWrite(led1,HIGH);
   delay(400);
   Serial.println("IV Infusion is not empty");

  }
  digitalWrite(led1,LOW);

  
  for(int i=0;i<20;i++)
    {
     digitalWrite(led2,LOW);
     delay(600);
     digitalWrite(led2,HIGH);
     delay(600);
    
    detected2 = analogRead(detector2);// read Laser sensor
    Firebase.RTDB.setFloat(&fbdo,"/Room234/Bed10/",detected2);

    if(detected2>580)
    {
        digitalWrite(led2,HIGH);
        Serial.println("IV Infusion will soon be empty");
        tone(buzzer,200,250);
        
        //Sending data to firebase
        if (Firebase.ready() && signupOK ) {
          isEmpty=true;
          Firebase.RTDB.setBool(&fbdo,"/Room234/Bed10/dripStatus",isEmpty);
          }
    }
    if(detected2<580)  
    {
        digitalWrite(led2,LOW);
        Serial.println("IV Infusion is not empty");
        delay(1000);
        isEmpty=false;
        Firebase.RTDB.setBool(&fbdo,"/Room234/Bed10/dripStatus",isEmpty);
    }
    }
  //Serial.println(detected2);
  delay(10000);

}
