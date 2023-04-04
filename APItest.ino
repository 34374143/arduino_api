#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#define DHT_SENSOR_PIN  23 // ESP32 pin GIOP23 connected to DHT11 sensor
#define DHT_SENSOR_TYPE DHT11
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);


const char* ssid = "ZenFone 8";
const char* password = "Wen901204Ni";

//Your Domain name with URL path or IP address with path
const char* serverName = "http://192.168.186.142:5000/api/add";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  dht_sensor.begin(); // initialize the DHT sensor
  lcd.init();
  lcd.backlight();
  
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop() {
  float humidity  = dht_sensor.readHumidity();    // read humidity
  float temperature = dht_sensor.readTemperature(); // read temperature
  Serial.print("溫度:");
  Serial.print(temperature);
  Serial.println("度C ");
  Serial.print("濕度:");
  Serial.print(humidity);
  Serial.println("%\t");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);  //設定游標位置 (字,行)
  lcd.print("Humidity   ");  //Relative Humidity 相對濕度簡寫
  lcd.print((int)humidity);
  lcd.print("%");
  lcd.setCursor(0, 1);  //設定游標位置 (字,行)
  lcd.print("Temp:   ");  
  lcd.print((int)temperature);
  lcd.print((char)223); //用特殊字元顯示符號的"度"
  lcd.print("C");
  delay(1500);
  
  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
    
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);
      
      // If you need Node-RED/server authentication, insert user and password below
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
      
      // Specify content-type header
      //http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      //String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&value1=24.25&value2=49.54&value3=1005.14";           
      // Send HTTP POST request
      //int httpResponseCode = http.POST(httpRequestData);
      
      // If you need an HTTP request with a content type: application/json, use the following:
      http.addHeader("Content-Type", "application/json");
      char location[] = "Living Room";

      char jsonString[256];
      sprintf(jsonString, "{\"token\":\"123456\",\"humidity\":\"%f\",\"location\":\"%s\",\"temperature\":\"%f\"}", humidity, location, temperature);

      int httpResponseCode = http.POST(jsonString);

//      int httpResponseCode = http.POST(jsonStr);
//            int httpResponseCode = http.POST("{\"token\":\"123456\",\"humidity\":\"64\",\"location\":\"Living Room\",\"temperature\":\"24\"}");

      // If you need an HTTP request with a content type: text/plain
      //http.addHeader("Content-Type", "text/plain");
      //int httpResponseCode = http.POST("Hello, World!");
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}
