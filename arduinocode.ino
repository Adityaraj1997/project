//#include<String.h>

String ssid = "adi";
String password = "aditya123";
const  String APIKEY =  "eda34fbebdb5d20439e28112820945916d0152905449eeff1a76e8085acafd3e";   // Replace with your CARRIOTS apikey
const String DEVICE =  "sensor@aditya.aditya"; // "IOT@Ujala25991.Ujala25991";// //"";
String strdistance = "";
String data;
String server = "82.223.244.60";
long temp = 0.00;
long temp1=32.00;
long tempis = 0.00;
int chpin = 5;



//mq4 variable

int gas_sensor4 = A0; //Sensor pin
float R04 = 4.74; //Sensor Resistance in fresh air from previous code
float b4=1.133;
float m4=-0.318;
 float sensor_volt4; //Define variable for sensor voltage
  float RS_gas4; //Define variable for sensor resistance
  float ratio4; //Define variable for ratio
double ppm4;

//mq5 variable

float R05=1.08; 
 float sensor_volt5;
 float RS_gas5; // Get value of RS in a GAS
 float ratio5; // Get ratio RS_GAS/RS_air


 //mq7 variables
  float R07=.20;
    float sensor_volt7;
    float RS_gas7; // Get value of RS in a GAS
    float ratio7; // Get ratio RS_GAS/RS_air


 
void setup() 
{
pinMode(10 ,OUTPUT);
Serial.begin(115200);
Serial1.begin(115200); 
digitalWrite(chpin , HIGH);
reset();
Serial1.println("AT+CWMODE=3");
if (Serial1.find("OK") ) Serial.println("ModE 3 activated");
connectWifi(); 
}
void reset() 
{
 Serial1.println("AT+RST");
 delay(1000);  
 if (Serial1.find("OK") ) 
 {
 Serial.println("Module Reset");
 }
 if (Serial1.available()) 
 {
  Serial.write(Serial1.read());
 }
 }
// connect to wifi network
void connectWifi() 
{
String cmd = "AT+CWJAP=\"" + ssid + "\",\"" + password + "\"";
Serial.println(cmd);
Serial1.println(cmd);
delay(2000);
if (Serial1.find("OK")) 
{
 Serial.println("Connected!");
}
else 
{
connectWifi();
Serial.println("Cannot connect to wifi");
}
}
void loop() // READ DATA
{ 

   //mq4 programing
  float sensorValue4 = analogRead(gas_sensor4); //Read analog values of sensor
  sensor_volt4 = sensorValue4 * (5.0 / 1023.0); //Convert analog values to voltage
  RS_gas4 = ((5.0 * 10.0) / sensor_volt4) - 10.0; //Get value of RS in a gas
  ratio4 = RS_gas4 / R04;   // Get ratio RS_gas/RS_air

  double ppm_log4 = (log10(ratio4) - b4) / m4; //Get ppm value in linear scale according to the the ratio value
   ppm4 = pow(10, ppm_log4); //Convert ppm value to log scale
  Serial.print("PPM methane = ");
  Serial.println(ppm4);


 // mq5 programing

int sensorValue5 = analogRead(A1);
 sensor_volt5=(float)sensorValue5/1024*5.0;
 RS_gas5 = (5.0-sensor_volt5)/sensor_volt5; // omit *RL
 /*-Replace the name "R0" with the value of R0 in the demo of First Test -*/
 ratio5 = RS_gas5/R05; // ratio = RS/R0
 /*-----------------------------------------------------------------------*/
 
 Serial.print("mq5 resistance Rs5/R05 = ");
 Serial.println(ratio5);
 Serial.print("\n");  



//mq7 programing


 int sensorValue7 = analogRead(A3);
    sensor_volt7=(float)sensorValue7/1024*5.0;
    RS_gas7 = (5.0-sensor_volt7)/sensor_volt7; // omit *RL

          /*-Replace the name "R0" with the value of R0 in the demo of First Test -*/
    ratio7 = RS_gas7/R07;  // ratio = RS/R0
          /*-----------------------------------------------------------------------*/
\
    Serial.print("Rs/R0 = ");
    Serial.println(ratio7);

    Serial.print("\n\n");

if(ppm4>567394 || ratio5>2.5 || ratio7>20)
digitalWrite(10, HIGH);
else
digitalWrite(10, LOW);


  
httppost();
Serial.println("hello");

delay(6000);
}
void httppost() 
{

Serial1.println("AT+CIPSTART=\"TCP\",\"" + server + "\",80"); //start a TCP connection.
delay(500);
if ( Serial1.find("OK")) 
{
Serial.println("TCP connection ready");
}
else 
{
Serial.println("TCP connection not ready");
}
String getStr = "{\"protocol\":\"v2\",\"device\":\""+DEVICE+"\",\"at\":\"now\",\"data\":{\"methane in ppm\":\""+ppm4+"\"""}}";
//String getStr = "{\"protocol\":\"v2\",\"device\":\""+DEVICE+"\",\"at\":\"now\",\"data\":{ \"temperature\":\""+String(temp1)+"\"""}}";
String postRequest = "POST /streams HTTP/1.1\n""Host: " + server + "\n" "Accept:application/json\n""User-Agent:Arduino-Carriots\n""Content-Type:application/json\n""carriots.apikey:" + APIKEY + "\n""Content-length:" + getStr.length() + "\n""Connection: close\n\n";
postRequest += getStr;
String sendCmd = "AT+CIPSEND=";  //determine the number of caracters to be sent.
Serial1.print(sendCmd);
Serial1.println(postRequest.length() );
delay(500);
if (Serial1.find(">")) 
{
Serial.println("Sending.."); 
Serial1.print(postRequest);
if ( Serial1.find("SEND OK")) 
{
Serial.println("Packet sent");
while (Serial1.available()) 
{
 String tmpRSerial1 = Serial1.readString();
 Serial.println(tmpRSerial1);
}                              // close the connection
Serial1.println("AT+CIPCLOSE");
}
}
}


