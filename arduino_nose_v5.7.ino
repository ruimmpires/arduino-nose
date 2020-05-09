#include <WiFi.h>
#include <PubSubClient.h>
#include <time.h>


// Update these with values suitable for your network.
const char* ssid = "ESCRITORIO";
const char* password = "g0mesp1res";
const char* mqtt_server = "192.168.1.201";
#define mqtt_port 1883
#define MQTT_USER ""
#define MQTT_PASSWORD ""
#define MQTT_PUBLISH "home/nose"
#define MQTT_RECEIVER "home/nose/rx"

char* hour_and_date;
//int total= 0;
bool clean_air=false;


//Inputs and outputs
int MQ2 = 36; //Gas fuel sensor pin
int MQ3 = 39; //Alcohol sensor pin
int MQ4 = 34; //Methane sensor pin
int MQ5 = 35; //LPG sensor pin
int MQ6 = 32; //GLP sensor pin
//As ADC2 is unusable because WiFi is used these are not used
//int MQ7 = 33; //CO sensor pin
int MQ8 = 33; //Industrial sensor pin
//int MQ9 = 26; //xx sensor pin
//int MQ135 = 27; //harmful sensor pin
//int WiFiPin = 12; // WiFi on pin
//int MQTTPin = 14; // MQTT on pin

//Arrays to store sensor values.

int sensorVal[] = {0, 0, 0, 0, 0, 0};
int sensortotal[] = {0, 0, 0, 0, 0, 0};
int sensorValmin[] = {4095, 4095, 4095, 4095, 4095, 4095};

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    //Connect to NTP and acquire time
    configTime(1 * 3600, 1, "pool.ntp.org", "time.nist.gov");
    Serial.println("\nWaiting for time");
    while (!time(nullptr)) {
      Serial.print(".");
      delay(500);
    }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) {
      Serial.println("connected");
      //Once connected, publish an announcement...
      time_t now = time(nullptr);
      hour_and_date = ctime(&now);
      publishSerialData(hour_and_date);
      publishSerialData("Nose is online");
      // ... and resubscribe
      client.subscribe(MQTT_RECEIVER);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte *payload, unsigned int length) {
    //Serial.println("-------new message from broker-----");
    //Serial.print("channel:");
    //Serial.println(topic);
    //Serial.print("data:");  
    //Serial.write(payload, length);
    //Serial.println();
    //Serial.print("payload length"); 
    //Serial.println(length); 
    payload[length]=0;
    //received = atoi((char*)(payload));    
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(500);// Set time out for 
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
}

void publishSerialData(char *serialData){
  if (!client.connected()) {
    reconnect();
  }
  client.publish(MQTT_PUBLISH, serialData);
}


void readsensorsraw(){
    //String dataString;
    
    sensorVal[0] = analogRead(MQ2);
    sensorVal[1] = analogRead(MQ3);
    sensorVal[2] = analogRead(MQ4);
    sensorVal[3] = analogRead(MQ5);
    sensorVal[4] = analogRead(MQ6);
    sensorVal[5] = analogRead(MQ8);

    //dataString = String(sensorVal[0])+","+sensorVal[1]+","+sensorVal[2]+","+sensorVal[3]+","+sensorVal[4]+","+sensorVal[5];
    //Serial.println(dataString);
    //char copy[40];
    //dataString.toCharArray(copy, 40);
    //publishSerialData(copy);
  }
  
void readsensors(int numReadings){
  int sensor[6][numReadings];
  
  //initialize total and sensor data
  for (int i=0;i<6;i++){
    sensortotal[i]=0;
    for (int n=0;n<numReadings;n++){
      sensor[i][n]=0;
    }
  }

  for (int n=0;n<numReadings;n++){

    for (int i=0;i<6;i++){
      sensortotal[i]=sensortotal[i]-sensor[i][n];
    }
    
    sensor[0][n] = analogRead(MQ2);
    sensor[1][n] = analogRead(MQ3);
    sensor[2][n] = analogRead(MQ4);
    sensor[3][n] = analogRead(MQ5);
    sensor[4][n] = analogRead(MQ6);
    sensor[5][n] = analogRead(MQ8);
      
    for (int i=0;i<6;i++){
       sensortotal[i] = sensortotal[i]+sensor[i][n];
    }

  }

  //calculate average
  for (int i=0;i<6;i++){
     sensorVal[i] = sensortotal[i]/numReadings;
  }   
     
     //debug
     /*Serial.println("readsensors");
        for (int i=0;i<6;i++){ 
          Serial.print(sensorVal[i]);Serial.print(",");
        }
      Serial.println();
      delay(1000);*/
}


void weightedsensors(){
   //int index = 0;
   int sensorweight[] = {0, 0, 0, 0, 0, 0};
   String dataString;
      
   // reset total
   int total=0;
   
   // calculate the weighted changed value
   for (int i=0;i<6;i++){
     sensorValmin[i] = min(sensorVal[i],sensorValmin[i]);
     sensorweight[i]= (sensorVal[i]-sensorValmin[i]);
     total=total+sensorweight[i];
  }
  //index=index+1;
  //debug
     /* Serial.println("sensorValmin");
        for (int i=0;i<6;i++){ 
          Serial.print(sensorValmin[i]);Serial.print(",");
        }
      Serial.println();
      delay(1000);
  */

   //transform the data into a string and send it
   //dataString = String(index+","+sensorweight[0])+","+sensorweight[1]+","+sensorweight[2]+","+sensorweight[3]+","+sensorweight[4]+","+sensorweight[5];
   //dataString = String(index)+","+sensorweight[0]+","+sensorweight[1]+","+sensorweight[2]+","+sensorweight[3]+","+sensorweight[4]+","+sensorweight[5];
   dataString = String(sensorweight[0])+","+sensorweight[1]+","+sensorweight[2]+","+sensorweight[3]+","+sensorweight[4]+","+sensorweight[5];
   Serial.println(dataString);
   //delay(1000);

   //SEND DATA TO MQTT if there is a potential gas detected
   if (total > 200) {
     char copy[40];
     dataString.toCharArray(copy, 40);
     publishSerialData(copy);
     clean_air=false;
   }
   else {
     clean_air=true;
   }

}

void save_to_SD_card(){
   // open the file. note that only one file can be open at a time,
   // so you have to close this one before opening another.
   //File dataFile = SD.open("datalog.csv", FILE_WRITE);

   // if the file is available, write to it:
   //if (dataFile) {
   //  dataFile.println(dataString);
   //  dataFile.close();
     // print to the serial port too:
     //Serial.println(dataString);
  //}
  // if the file isn't open, pop up an error:
  //else {
  //  Serial.println("error opening datalog.csv");
  //}

}



void loop() {
   client.loop();
   readsensorsraw();
   //readsensors(100); //smoothing
   weightedsensors();
      

   delay(100);        // delay in between reads for stability
 }
