/*
* Team Id: 9
* Author List: Nishanth Rumandla, Mahindar, Nikhil Sri Ram, Dheeraj Kalmekolanu  
* Filename: smartIrrigation.ino
* Theme: Cargo Sorting – eYRC Specific
* Functions: setup() , loop(), wifiSetup(), pinSetup(), calc_params(), makeDecision()
* Global Variables: ssid[], password[], keyIndex, EchoPin, numPoints, KNNparam, threshold, FREQ, server, trainArr_temp[], trainArr_moist[], 
*                   trainArr_output[], w1, w2, decision, x, curr_water_level, temperature, soil_moisture, values[];
*/
#ifndef __CC3200R1M1RGC__
// Do not include SPI for CC3200 LaunchPad
#include <SPI.h>
#endif
#include <WiFi.h>

// your network
char ssid[] = "myhotspot";
// your network password
char password[] = "qwerty0987";
// your network key Index number (needed only for WEP)
int keyIndex = 0;

//Definitions
#define EchoPin 29
#define numPoints 45
#define KNNparam 9
#define threshold 1200
#define FREQ 100

WiFiServer server(80);   

/*
* Function Name: wifiSetup
* Input: None
* Output: None
* Logic: sets up the wifi connection and obtains an IP address
* Example Call: wifiSetup();
*/
void wifiSetup() {
  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to Network named: ");
  // print the network name (SSID);
  Serial.println(ssid); 
  int status;
  // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
  status =  WiFi.begin(ssid, password);
  //Serial.println(status); 
  while ( WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    delay(300);
  }
  Serial.println("\nYou're connected to the network");
  Serial.println("Waiting for an ip address");

  while (WiFi.localIP() == INADDR_NONE) {
    // print dots while we wait for an ip addresss
    Serial.print(".");
    delay(300);
  }

  Serial.println("\nIP Address obtained");
  printWifiStatus();
  Serial.println("Starting webserver on port 80");
  server.begin();                           // start the web server on port 80
  Serial.println("Webserver started!");  
}

/*
* Function Name: pinSetup
* Input: None
* Output: None
* Logic: sets pinmodes
* Example Call: pinSetup();
*/
void pinSetup() {
  pinMode(RED_LED, OUTPUT);      // set the LED pin mode
  pinMode(GREEN_LED,OUTPUT);
  pinMode(EchoPin, INPUT);
  pinMode(25, INPUT);
  pinMode(26, OUTPUT);
}

/*
* Function Name: setup
* Input: None
* Output: None
* Logic: calls wifiSetup(), pinSetup()
* Example Call: Called automatically 
*/
void setup() {
  Serial.begin(9600); // initialize serial communication
  wifiSetup();
  pinSetup();
}

float trainArr_temp[numPoints]={299.95,300.45,300.35,298.55,300.45,296.95,301.05,297.95,301.05,301.45,308.75,311.15,310.95,310.95,310.95,317.15,316.15,317.05,317.15,318.35,318.15,319.15,316.05,317.15,318.35,318.95,319.45,317.35,318.55,317.05,308.75,311.15,310.95,310.95,310.95,309.95,311.05,310.95,310.35,311.45,298.75,299.15,301.05,299.95,297.65};
float trainArr_moist[numPoints]={2432.00,2411.00,2391.00,2353.00,2351.00,4066.00,4058.00,4059.00,4059.00,4058.00,3481.00,3501.00,3401.00,3413.00,3427.00,3510.00,3515.00,3515.00,3517.00,3518.00,2510.00,2485.00,2525.00,2417.00,2378.00,2432.00,2411.00,2367.00,2393.00,2351.00,2485.00,2391.00,2403.00,2413.00,2427.00,4066.00,4048.00,4067.00,4059.00,4069.00,3461.00,3512.00,3491.00,3463.00,3487.00};
int trainArr_output[numPoints]={0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0};
float w1=1,w2=1/10000.0;
int decision;
int x = FREQ;
int curr_water_level;
float temperature;
float soil_moisture;
float values[10];


/*
* Function Name: calc_params
* Input: None
* Output: Reads the 3 parameters temperature, soil_moisture, curr_water_level and prints them
* Logic: To calculate curr_water_level, we take 10 readings from the sensor, sorts them and 
* compares the 5th smallest reading with threshold
* Example Call: calc_params();
*/
void calc_params() {
  temperature = (float) (4260.5 - 2475*analogRead(TEMPSENSOR)/4096)/10                               ;
  Serial.print(temperature);
  Serial.write("K ");
  soil_moisture = analogRead(EchoPin);
  Serial.print(soil_moisture);
  Serial.write(" ");  
  curr_water_level=1;
  for(int a=0; a<10;a++){
    digitalWrite(26, HIGH);
    float level = analogRead(25);
    Serial.println(level);
    values[a]=level;
    delay(100);
  }
  //two for loops sort the 10 reading values
  for(int j=0;j<10;j++){
    for(int k=1; k<10-j;k++){
      if(values[k-1]>values[k]){
        float p=values[k-1];
        values[k-1]=values[k];
        values[k]=p;
      }
    }
  }
  //if 5th smallest reading is less than threshold, curr_water_level is set to 0
  if(values[4]<=threshold){
    curr_water_level=0;
  }   
}
/*
* Function Name: makeDecision
* Input: None
* Output: changes the global variable decision
* Logic: Take the nearest 9 neighbours out of the 45 training samples and poll their votes
* Decision is what more than half of them give
* Example Call: makeDecision();
*/
void makeDecision() {
  float distances[numPoints];      
  for(int j=0;j<numPoints; j++){
    float dist=w1*(trainArr_temp[j]-temperature)*(trainArr_temp[j]-temperature)+w2*(trainArr_moist[j]-soil_moisture)*(trainArr_moist[j]-soil_moisture);
    Serial.write("Temp-diff "); Serial.print(w1*(trainArr_temp[j]-temperature)*(trainArr_temp[j]-temperature));
    Serial.write("Moist-diff "); Serial.println(w2*(trainArr_moist[j]-soil_moisture)*(trainArr_moist[j]-soil_moisture));
    distances[j]=dist;
  }
  //sort the training points by distance to the point in question
  for(int j=0;j<numPoints;j++){
    for(int k=1; k<numPoints-j;k++){
      if(distances[k-1]>distances[k]){
        //swapping code
        float p=trainArr_temp[k-1];
        trainArr_temp[k-1]=trainArr_temp[k];
        trainArr_temp[k]=p;
        p=trainArr_moist[k-1];
        trainArr_moist[k-1]=trainArr_moist[k];
        trainArr_moist[k]=p;
        p=distances[k-1];
        distances[k-1]=distances[k];
        distances[k]=p;
        int p1=trainArr_output[k-1];
        trainArr_output[k-1]=trainArr_output[k];
        trainArr_output[k]=p1;        
      }
    }
  }
  int votecount = 0; 
  decision=0;
  for(int j=0; j<KNNparam ; j++){
    votecount += trainArr_output[j];
  }
  if (votecount > KNNparam/2){
    decision=1;
  }
}
/*
* Function Name: loop
* Input: None
* Output: LEDs are changed after every FREQ iterations based on curr_water_level and decision
* In every iteration, we check if a client is present and serve them with a HTML page
* Logic: None
* Example Call: Called automatically
*
*/
void loop() {  
  if(x==FREQ){
    calc_params(); 
    x=0;
    Serial.print(curr_water_level);
    Serial.println("");
    
    if(curr_water_level == 0){
      //only if current water level is empty, we have to make decision whether to release water
      makeDecision();
      if(decision==0){
        //decision = 1 means there is water in the trough so GREEN
        digitalWrite(RED_LED,LOW);
        digitalWrite(GREEN_LED,HIGH);
      }
      else{
        //decision = 1 means there is no water in the trough so RED
        digitalWrite(GREEN_LED,LOW);
        digitalWrite(RED_LED,HIGH);
      }
      Serial.print("\ndecision: ");
      Serial.println(decision);
    }
    else{
      //curr_water_level == 1 means there is water in the trough so GREEN
      digitalWrite(RED_LED,LOW);
      digitalWrite(GREEN_LED,HIGH);
    }
  }
  
  int i = 0;
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    char buffer[150] = {
      0    };                 // make a buffer to hold incoming data
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (strlen(buffer) == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.println("<html><head><title>Current sensor values</title></head><body align=center>");
            client.print("<p><font color=\"red\"> Temperature(K) :</font>");
            client.print(temperature);
            client.print("</p>");
            client.print("<br><p><font color=\"brown\"> Soil moisture:</font>");
            client.print(soil_moisture);
            client.print("</p>");
            if(decision==0 || curr_water_level==1){
              client.print("<br><p><font color=\"green\"> Don't worry your plant is in safe hands</font> </p>");
            }
            else{
              client.print("<br><p><font color=\"red\"> watch out!!! plant needs water</font></p>");
            }
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear the buffer:
            memset(buffer, 0, 150);
            i = 0;
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          buffer[i++] = c;      // add it to the end of the currentLine
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
  delay(100);
  x++;
}

/*
* Function Name: printWifiStatus
* Input: None
* Output: Prints wifi status
* Logic: None
* Example Call: printWifiStatus();
*
*/
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

