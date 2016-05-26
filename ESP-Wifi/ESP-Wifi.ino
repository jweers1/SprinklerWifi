#include <ESP8266WiFi.h>

// wifi setup data
const char* ssid = "ssid";
const char* password = "password";

const char* AP_Name = "ESP8266-Sprinkler";
const char* ap_passwd="a good password";

WiFiServer server(80);

// the pin used for the sprinkler
int millis_per_sec = 1000;
int sprinklerPin = 2; // GPIO2
int timer;
long curr_millis;
long dest_millis;
int timer_remaining = 0;
int new_request=0;

void setup() {
  // start serial debugging
  Serial.begin(115200);
  delay(10);

  // set the output pins
  pinMode(sprinklerPin, OUTPUT);
  digitalWrite(sprinklerPin, HIGH);
  timer_remaining=0;
  
  // start Wifi -- choose one
  setupWifiAP();
  //setupWifi();
 
  // Start the web server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

 
void loop() {

  int value = HIGH;

  // check and turn them off if a timer expires...
  curr_millis=millis();
  if ((curr_millis>dest_millis) && (timer>0)){
     digitalWrite(sprinklerPin,HIGH);
     timer=0;
     value=HIGH;
     Serial.println("turning sprinklers off at:");
     Serial.print(curr_millis);
     Serial.print(" > ");
     Serial.println(dest_millis);
     
  }
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
   
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
   
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
   
  // Match the request
 
  if (request.indexOf("/SPRINKLER?timer=5") != -1) {
    timer=5;
    new_request=1;
  }

 if (request.indexOf("/SPRINKLER?timer=10") != -1) {
    timer=10;
    new_request=1;
  }
 if (request.indexOf("/SPRINKLER?timer=15") != -1) {
    timer=15;
    new_request=1;
  }
 if (request.indexOf("/SPRINKLER?timer=20") != -1) {
    timer=20;
    new_request=1;
  }
 if (request.indexOf("/SPRINKLER?timer=0") != -1) {
    timer=0;
    new_request=1;
  }

 if (new_request ==1){
    curr_millis=millis();
    dest_millis=timer*60*millis_per_sec + curr_millis;     

    if (timer>0){
      digitalWrite(sprinklerPin, LOW);
      value=LOW;
    } else {
      digitalWrite(sprinklerPin,HIGH);
      value=HIGH;
    }

 }
     
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
   
  client.print("Sprinkler is currently ");
   
  if(value == HIGH) {
    client.print("On");  
  } else {
    client.print("Off");
  }

  client.print("<br><br>Time set was: ");
  client.print(timer);
  client.println("<br><br>");

  if (value == HIGH){
  client.print("Time Left is: ");
  client.println((dest_millis-millis())/1000);
  client.println("<br><br>");
  }
  
  client.println("<form action=\"SPRINKLER\" method=\"get\">");
  client.println(" <label>Set Sprinkler Duration</label><br><br>");
  //client.println("  <input type=\"text\" name=\"timer\" list=\"numbers\"/><br>");
  client.println("  <datalist id=\"numbers\">");
  client.println("    <select name=\"timer\">");
  client.println("       <option value=\"0\">off");
  client.println("       <option value=\"5\">5");
  client.println("       <option value=\"10\">10");
  client.println("       <option value=\"15\">15");
  client.println("       <option value=\"20\">20");
  client.println("    </select>");
  client.println("  </datalist><br><br>");
  client.println("  <input type=\"submit\" value=\"Make Changes\"><br><br>");
  
  client.print("   <a href=\"http://"); 
  client.print(WiFi.softAPIP()); 
  client.println("\">Refresh</a>");

  
  client.println("</form>");
  client.println("<br>");
  client.println("<br>");
  
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
 
}
 
void setupWifiAP()
{
//  WiFi.mode(WIFI_AP);

  WiFi.softAP(AP_Name,ap_passwd);
}

void setupWifi(){
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
    
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
   
}
