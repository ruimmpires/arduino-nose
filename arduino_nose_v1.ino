/*

  Parts required:
  - one LM35 temperature sensor
  - 1 k ohm resistors
  - gas sensors by tamagushi

  created May 2019
  by Rui Pires

  based on
  http://www.arduino.cc/starterKit


A0; //LM35 temperature
A1; //VHeater
A2; //TGS2600 domestic pollutants
A3; //TGS2281 Humidity
A4; //TGS2281 Alcohol
*/


float Gas1RsMax=0;
float Gas2RsMax=0;
float Gas3RsMax=0;

/*Create an array to store sensor values. I'm using floats. Floats use 4 bytes to represent numbers in exponential notation. Use int if you are representing whole numbers from -32,768 to 32,767.
  For more info on the appropriate data type for your sensor values, check out the language reference on data type: http://arduino.cc/en/Reference/HomePage
  Customize the array's size to be equal to your number of sensors.
*/
float sensorVal[] = {0, 0, 0, 0, 0};

void setup() {
  // set the Digital pins as outputs
  // the for() loop saves some extra coding
  for (int pinNumber = 2; pinNumber < 8; pinNumber++) {
    pinMode(pinNumber, OUTPUT);
    //digitalWrite(pinNumber, HIGH);
    //delay(500);
    digitalWrite(pinNumber, LOW);
  }
  //debug
  delay(500);
  
  // open a serial connection to display values
  Serial.begin(9600); // initialize Serial communication
  while (!Serial);    // wait for serial port to connect.
  Serial.println("temperature,Gas1RS/Rmax,Gas2RS/Rmax,Gas3RS/Rmax");

   
}


void loop() {

  
  
  //workaround until I find a way to get more power into arduino
  float Vc = 5;
  
  //read_analog();
  sensorVal[0] = analogRead(A0);
  sensorVal[1] = analogRead(A1);
  sensorVal[2] = analogRead(A2);
  sensorVal[3] = analogRead(A3);
  sensorVal[4] = analogRead(A4);

  int temperature = round((sensorVal[0] / 1024) * Vc * 100);
  float Vhvoltage = (sensorVal[1] / 1024) * Vc;
  float Gas1voltage = (sensorVal[2] / 1024) * Vc;
  float Gas2voltage = (sensorVal[3] / 1024) * Vc;
  float Gas3voltage = (sensorVal[4] / 1024) * Vc;

  float Gas1Rs = Vc*10000/Gas1voltage-10000;
  float Gas2Rs = Vc*10000/Gas2voltage-10000;
  float Gas3Rs = Vc*10000/Gas3voltage-10000;
  
  Gas1RsMax = max(Gas1RsMax,Gas1Rs);
  Gas2RsMax = max(Gas2RsMax,Gas2Rs);
  Gas3RsMax = max(Gas3RsMax,Gas3Rs);

  int Gas1pct = (100*Gas1Rs/Gas1RsMax);
  int Gas2pct = (100*Gas2Rs/Gas2RsMax);
  int Gas3pct = (100*Gas3Rs/Gas3RsMax);

  //debug
  /*Serial.print(Gas1RsMax/1000);  Serial.print(","); Serial.print(Gas2RsMax/1000);  Serial.print(","); Serial.print(Gas3RsMax/1000); Serial.print(","); 
  Serial.print(Gas1Rs/1000);  Serial.print(","); Serial.print(Gas2Rs/1000);  Serial.print(","); Serial.print(Gas3Rs/1000); Serial.print(","); 
  Serial.print(Gas1voltage);  Serial.print(","); Serial.print(Gas2voltage);  Serial.print(","); Serial.print(Gas3voltage); Serial.print(","); 
  */

  Serial.print(temperature);  Serial.print(",");  Serial.print(Gas1pct);  Serial.print(",");  Serial.print(Gas2pct);  Serial.print(",");  Serial.println(Gas3pct);
  
  //Relay control
  // if the temperature sensor is above 30
  if (temperature > 30) digitalWrite(2, LOW);
    else digitalWrite(2, HIGH);
    
  // if the gas1 sensor is lower than 70%
  if (Gas1pct < 70)  digitalWrite(3, LOW);
    else digitalWrite(3, HIGH);
  
  // if the gas2 sensor is lower than 80%
  if (Gas2pct < 70) digitalWrite(4, LOW);
    else digitalWrite(4, HIGH);

  // if the gas3 sensor is lower than 70%
  if (Gas3pct < 70) digitalWrite(5, LOW);
    else digitalWrite(5, HIGH);
  
    
  delay(100);
}
