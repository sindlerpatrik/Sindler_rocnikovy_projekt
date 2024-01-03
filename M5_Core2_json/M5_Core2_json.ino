//import potřebných knihoven
#include <WiFi.h>
#include <M5Core2.h>
#include <ArduinoJson.h>
#include "fonts.h"
#include "time.h"

//definování proměnných
#define timeout 20000         //timeout 20s 

const char ssid[] = "SSID";   //jméno sítě
const char pass[] = "password";      //heslo sítě
const char server[] = "api.openweathermap.org";//server pro sbírání dat
const char apiKey[] = "88338ec27e1c8ca89107de0bc0c4e561";//api klíč
const char ntpServer[] = "pool.ntp.org";
const char *icons[] = {"/ICONS/clear.jpg", "/ICONS/clouds.jpg", "/ICONS/rain.jpg",
                 "/ICONS/thunderstorm.jpg", "/ICONS/snow.jpg", "/ICONS/mist.jpg"//denní ikonky
                };
const char *iconsNight[] = {"/ICONSNIGHT/clear_night.jpg", "/ICONSNIGHT/clouds_night.jpg",
                      "/ICONSNIGHT/thunderstorm_night.jpg", "/ICONSNIGHT/snow_night.jpg",
                      "/ICONSNIGHT/mist_night.jpg" //noční ikonky
                     };
const String location = "city, country";//lokace, např.: prague,CZ, warsaw,PL, bratislava,SK atd.
const int gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;
byte reset = 0;
const byte posX = 125;                //pozice ikonek
const byte posY = 80;
char timeBuff[10];            //buffery pro datum a čas
char dateBuff[20];
char timeHour[4];

WiFiClient client;//pomocná proměnná client

//******************************************************************

void doConnect() {
  M5.begin();//zahájení komunikace s Core2
  M5.Lcd.setTextDatum(MC_DATUM);//vycentrování textu na střed
  M5.Lcd.setTextSize(2);        //nastavení velikosti textu
  M5.Lcd.setFreeFont(FF33);     //nastavení fontu
  M5.Lcd.setTextColor(BLACK);   //nastavení barvy textu
  M5.Lcd.clear(WHITE);          //nastavení pozadí

  WiFi.mode(WIFI_STA);    //nastavení Core2 jako stanice
  WiFi.begin(ssid, pass); //zahájení WiFi

  while (WiFi.status() != WL_CONNECTED && millis() < timeout) {
    M5.Lcd.drawString("Connecting to WiFi...", 160, 120, 2);  //pokud nebude Core2 připojen k WiFi (160 = osa X, 120 = osa Y, 2 = font)
    //a zároveň millis(čas od spuštění Core2) bude menší než timeout,
    //bude se vypisovat zpráva o připojování k WiFi
  }

  M5.Lcd.clear(WHITE);

  if (WiFi.status() != WL_CONNECTED) {                  //pokud se Core2 nepřipojí,
    M5.Lcd.drawString("Unable to connect", 160, 120, 2);//vypíše se zpráva
    delay(3000);
    M5.Lcd.clear(WHITE);
    M5.Lcd.drawString("Restarting...", 160, 120, 2);    //a poté se Core2 restartuje
    delay(3000);
    ESP.restart();
  }
  else {                                          //pokud se Core2 připojí,
    M5.Lcd.drawString("Connected!", 160, 120, 2); //vypíše se zpráva a program pokračuje dále
    delay(3000);
    M5.Lcd.clear(WHITE);
  }
}

//******************************************************************

void ntpTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
  }
  char timeMinute[3], year[5], month[5], day[4];
  
  strftime(timeHour,4, "%H:", &timeinfo);
  strftime(timeMinute,3, "%M", &timeinfo);
  strftime(year,5, "%Y", &timeinfo);
  strftime(month,5, "%b/", &timeinfo);
  strftime(day,4, "%d/", &timeinfo);
  
  M5.Lcd.setTextDatum(TR_DATUM);
  M5.Lcd.drawString(year, 315, 0, 2);
  M5.Lcd.drawString(month, 250, 0, 2);
  M5.Lcd.drawString(day, 190, 0, 2);
  
  M5.Lcd.setTextDatum(TL_DATUM);
  M5.Lcd.drawString(timeHour, 5, 0, 2);
  M5.Lcd.drawString(timeMinute, 45, 0, 2);
}

void connection() {
  M5.Lcd.setTextDatum(BL_DATUM);             //text vlevo dole
  M5.Lcd.drawString("WiFi", 15, 240, 2);     //zobrazení textu WiFi

  if (WiFi.status() == WL_CONNECTED) {       //pokud je Core2 připojen k WiFi,
    M5.Lcd.fillRect(0, 230, 10, 10, GREEN);  //indikátor vedle textu je zelený
  }
  else {                                     //pokud není připojen,
    M5.Lcd.fillRect(0, 230, 10, 10, RED);    //indikátor je červený
  }
}

//******************************************************************

void getWeather() {
  Serial.println("\nStarting connection to server...");//na sériový monitor se vypíše zpráva
  if (client.connect(server, 80)) {                    //pokud se Core2 připojí na server a port číslo 80,
    Serial.println("connected to server");             //vypíše se zpráva
    client.print("GET /data/2.5/forecast?q=" + location + "&appid=" + apiKey + "&cnt=3");//a vytvoří se http request
    client.println("&units=metric");
    client.println("Host: api.openweathermap.org");
    client.println("Connection: close");
    client.println();
  }
  else {                                              //pokud se Core2 nepřipojí,
    Serial.println("Unable to connect");              //na sériový monitor se vypíše zpráva
    connection();
    reset++;                                          //reset se zvýší o jedna
    if (reset == 10) {                                //pokud reset = 10;
      M5.Lcd.clear(WHITE);
      M5.Lcd.setTextDatum(MC_DATUM);
      M5.Lcd.drawString("Restarting...", 160, 120, 2);//Core2 se restartuje
      delay(3000);
      ESP.restart();
    }
    delay(5000);
  }

  String line = "";                                  //vytvoření prázdné string proměnné

  if (client.connected()) {                          //pokud je Core2 připojen k serveru,
    line = client.readStringUntil('\n');             //do prázdné proměnné se zapíší data z api klíče

    StaticJsonDocument<5000> doc;                    //vytvoření json bufferu pro manipulaci s daty

    DeserializationError error = deserializeJson(doc, line);//vytvoření proměnné error, která se aktivuje, když line = ""

    if (error) {                                            //pokud nastane error,
      Serial.println("Deserialization failed");             //na sériový monitor se vypíše zpráva
      Serial.println(error.f_str());                        //a důvod erroru
      return;
    }

    //vytažení dat z json bufferu, která budou vypsána na displeji
    String temp = doc["list"][0]["main"]["temp"];
    String humidity = doc["list"][0]["main"]["humidity"];
    String wind = doc["list"][0]["wind"]["speed"];
    String city = doc["city"]["name"];
    String condition = doc["list"][0]["weather"][0]["main"];
    String description = doc["list"][0]["weather"][0]["description"];

    wind = wind.toDouble() * 3.6; //převod rychlosti větru z m/s na km/h

    M5.Lcd.clear(WHITE);

    ntpTime();

    int Hours = atoi(timeHour);

    if (condition == "Clear") {                           //zobrazení ikonek podle condition
      if (Hours >= 21 || Hours <= 4) {    //pokud je hodnota z hodin větší než 21 nebo menší než 4,
        M5.Lcd.drawJpgFile(SD, iconsNight[0], posX, posY);//z SD karty se načte noční ikonka odpovídající condition
      }
      else {
        M5.Lcd.drawJpgFile(SD, icons[0], posX, posY);     //v jiném případě se načte denní ikonka
      }
    }
    else if (condition == "Clouds") {
      if (Hours >= 21 || Hours <= 4) {
        M5.Lcd.drawJpgFile(SD, iconsNight[1], posX, posY);
      }
      else {
        M5.Lcd.drawJpgFile(SD, icons[1], posX, posY);
      }
    }
    else if (condition == "Thunderstorm") {
      if (Hours >= 21 || Hours <= 4) {
        M5.Lcd.drawJpgFile(SD, iconsNight[2], posX, posY);
      }
      else {
        M5.Lcd.drawJpgFile(SD, icons[3], posX, posY);
      }
    }
    else if (condition == "Snow") {
      if (Hours >= 21 || Hours <= 4) {
        M5.Lcd.drawJpgFile(SD, iconsNight[3], posX, posY);
      }
      else {
        M5.Lcd.drawJpgFile(SD, icons[4], posX, posY);
      }
    }
    else if (condition == "Mist") {
      if (Hours >= 21 || Hours <= 4) {
        M5.Lcd.drawJpgFile(SD, iconsNight[4], posX, posY);
      }
      else {
        M5.Lcd.drawJpgFile(SD, icons[5], posX, posY);
      }
    }
    else if (condition == "Rain") {
      M5.Lcd.drawJpgFile(SD, icons[2], posX, posY);
    }

    M5.Lcd.setTextDatum(MC_DATUM);                          //text uprostřed
    M5.Lcd.drawString(city, 160, 50, 1);                    //zobrazení názvu města
    M5.Lcd.drawString(description, 160, 190, 2);            //zobrazení popisu aktuálního počasí
    M5.Lcd.drawString("Wind: " + wind + "km/h", 160, 80, 2);//zobrazení rychlosti větru v km/h

    M5.Lcd.setTextDatum(MR_DATUM);                          //text vpravo uprostřed
    if (temp.toInt() < 0) {
      M5.Lcd.setTextColor(RED);
      M5.Lcd.drawString(temp + "C", 315, 120, 2);
      }
    
    else if (temp.toInt() > 0) {
      M5.Lcd.setTextColor(BLUE);
      M5.Lcd.drawString(temp + "C", 315, 120, 2);
      }

    else{
      M5.Lcd.drawString(temp + "C", 315, 120, 2);
      }

    M5.Lcd.setTextColor(BLACK);

    M5.Lcd.setTextDatum(ML_DATUM);                          //text vlevo uprostřed
    M5.Lcd.drawString("Hum: " + humidity + "%", 5, 120, 2); //zobrazení vlhkosti v %

    M5.Lcd.drawLine(5, 30, 315, 30, BLACK);                 //zobrazení rozdělovací čáry nahoře
    M5.Lcd.drawLine(5, 205, 315, 205, BLACK);               //zobrazení rozdělovací čáry dole

    connection();                                           //přivolání předešlých funkcí
    delay(10000);                                           //celý displej se aktualizuje každých 20 sekund
  }
}

//******************************************************************

void setup() {
  //přivolání funkcí pro setup
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  doConnect();
}

//******************************************************************

void loop() {
  //přivolání funkcí pro loop
  getWeather();
  M5.update();
}
