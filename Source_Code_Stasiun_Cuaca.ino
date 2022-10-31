 #include <TFT_eSPI.h>
#include <WiFi.h>
#include <ThingSpeak.h>

#define TX        17
#define RX        15
#define weather    Serial1

TFT_eSPI tft = TFT_eSPI();

String message, temp;
float direction, speed1, speed5, temperature;
float rainfall1, rainfall24, humidity, pressure;

//Hotspot
const char *ssid = "XLGO-DDBA";
const char *pass = "60749431";

const char *server = "api.thingspeak.com";

WiFiClient client;

//pengaturan thingspeak
unsigned long channel = 1295837; //channel ID number
const char *myWriteAPIKey = "X6GLS586360DXX86";

unsigned long m;
int x, y;

void setup()
{
  Serial.begin(115200);
  weather.begin(9600, SERIAL_8N1, RX, TX);
  tft.init();
  tft.invertDisplay(true);
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  delay(10);

  Serial.println("Connecting to: ");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected\n");
  ThingSpeak.begin(client); //initialize ThingSpeak
}

void loop()
{
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.println("  Weather Station   ");
  tft.println();

  message = "Direction = " + String(direction, 0) + "deg";
  while (message.length() < 19) message += " ";
  tft.println(message);
  message = "Speed = " + String(speed1) + "m/s";
  while (message.length() < 19) message += " ";
  tft.println(message);
  message = "Rainfall = " + String(rainfall1) + "mm";
  while (message.length() < 19) message += " ";
  tft.println(message);
  message = "Pressure = " + String(pressure, 1) + "hPa";
  while (message.length() < 19) message += " ";
  tft.println(message);
  message = "Temp = " + String(temperature) + "degC";
  while (message.length() < 19) message += " ";
  tft.println(message);
  message = "Humidity = " + String(humidity, 0) + "%";
  while (message.length() < 19) message += " ";
  tft.println(message);

  if (weather.available())
  {
    delay(10);
    message = "";
    while (weather.available()) message += (char)weather.read();

    if (message.indexOf("c") > -1)
    {
      temp = message.substring(message.indexOf("c") + 1); //resolution 45 degree
      direction = temp.toInt();
    }
    if (message.indexOf("s") > -1)
    {
      temp = message.substring(message.indexOf("s") + 1); //resolution 0.1 miles per hour
      speed1 = 0.1 * 0.44704 * temp.toInt(); //convert miles per hour to meter per second
    }
    if (message.indexOf("g") > -1)
    {
      temp = message.substring(message.indexOf("g") + 1); //resolution 0.1 miles per hour
      speed5 = 0.1 * 0.44704 * temp.toInt(); //convert miles per hour to meter per second
    }
    if (message.indexOf("t") > -1)
    {
      temp = message.substring(message.indexOf("t") + 1); //resolution 1 Fahrenheit
      temperature = (temp.toInt() - 32) * 5.0 / 9.0; //convert Fahrenheit to Celcius
    }
    if (message.indexOf("r") > -1)
    {
      temp = message.substring(message.indexOf("r") + 1); //resolution 0.01 inches
      rainfall1 = 0.01 * 25.4 * temp.toInt(); //convert inches to mm
    }
    if (message.indexOf("p") > -1)
    {
      temp = message.substring(message.indexOf("p") + 1); //resolution 0.01 inches
      rainfall24 = 0.01 * 25.4 * temp.toInt(); //convert inches to mm
    }
    if (message.indexOf("h") > -1)
    {
      temp = message.substring(message.indexOf("h") + 1); //resolution 1%
      humidity = temp.toInt();
      if (humidity == 0) humidity = 100;
    }
    if (message.indexOf("b") > -1)
    {
      temp = message.substring(message.indexOf("b") + 1);  //resolution 0.1hPa
      pressure = 0.1 * temp.toInt();
    }


    Serial.print("Wind Direction: ");
    Serial.print(direction, 0);
    Serial.println("deg");
    Serial.print("Average Wind Speed (1 Minute): ");
    Serial.print(speed1);
    Serial.println("m/s");
    Serial.print("Max Wind Speed (5 Minutes): ");
    Serial.print(speed5);
    Serial.println("m/s");
    Serial.print("Rain Fall (1 Hour): ");
    Serial.print(rainfall1);
    Serial.println("mm");
    Serial.print("Rain Fall (24 Hours): ");
    Serial.print(rainfall24);
    Serial.println("mm");
    Serial.print("Pressure: ");
    Serial.print(pressure, 1);
    Serial.println("hPa");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println("degC");
    Serial.print("Humidity: ");
    Serial.print(humidity, 0);
    Serial.println("%");
  }


  if (millis() - m >= 1000 || millis() - m < 0) {
    x++;
    m = millis();
  }

  if (x >= 300) {
    y++;
  }

  if (y >= 1) {
    ThingSpeak.setField(1, direction);
    ThingSpeak.setField(2, speed1);
    ThingSpeak.setField(3, speed5);
    ThingSpeak.setField(4, rainfall1);
    ThingSpeak.setField(5, rainfall24);
    ThingSpeak.setField(6, pressure);
    ThingSpeak.setField(7, temperature);
    ThingSpeak.setField(8, humidity);
    ThingSpeak.writeFields(channel, myWriteAPIKey);
  }

  if (y >= 3) {
    y = 0;
    x = 0;
  }

  yield();

  //delay(1000);
}
