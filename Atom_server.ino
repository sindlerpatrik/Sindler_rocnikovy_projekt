//import potřebných knihoven
#include <M5Atom.h>
#include <WiFi.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

//definování proměnných
#define bmpAddress 0x76   //i2c adresa senzoru, pokud neznáme lze zjistit přes https://navody.dratek.cz/navody-k-produktum/i2c-skener.html
#define timeout 10000     //timeout 10s
#define led 12            //prostřední LED 

char ssid[] = "Sindlerovi";                       //jméno sítě
char pass[] = "patrik123";                        //heslo sítě
char token[] = "AOC9ZLKrJsmab8lZwU3GlChJhTB0QvYF";//token z Blynk aplikace
int reset = 0;

Adafruit_BMP280 bmp;                              //pomocná proměnná pro senzor
WebServer server (80);                            //server na portu 80

//******************************************************************

void doConnect() {
  M5.begin(true, true, true);                     //zahájení komunikace přes Atom
  Wire.begin(21, 22);                             //zahajení komunikace se senzorem přes piny 21 a 22

  WiFi.mode(WIFI_STA);                            //nastavení Atomu jako stanice
  WiFi.begin(ssid, pass);                         //zahájení WiFi

  while (WiFi.status() != WL_CONNECTED && millis() < timeout) {  //pokud Atom nebude připojen k WiFi a millis (čas od spuštení Atomu) bude menší než timeout,
    Serial.print("Connecting to: ");                             //bude se vypisovat zpráva o připojování k síti
    Serial.println(ssid);
    M5.dis.drawpix(led, 0xFFA500);                                //a prostřední LED bude blikat oranžově
    delay(500);
    M5.dis.drawpix(led, 0x000000);
    delay(500);
  }

  if (WiFi.status() != WL_CONNECTED) {                           //pokud se Atom nepřipojí k síti,
    Serial.println("Unable to connect to WiFi");                 //vypíše se zpráva
    Serial.println("Restarting...");                             //a Atom se restartuje
    delay(3000);
    ESP.restart();
  }
  else {                                                         //pokud se Atom připojí k síti,
    Serial.println("Connected");                                 //vypíše se zpráva o úspěšném připojení
    Serial.print("IP: ");                                        //a IP adresa Atomu
    Serial.println(WiFi.localIP());
  }
}

//******************************************************************

void connection() {
  if (WiFi.status() != WL_CONNECTED) {                           //pokud Atom není připojen,
    Serial.println("Server is down");                            //vypisuje se zpráva, že server neběží
    sensor();                                                    //přivolání funkce pro kontrolu připojení senzoru
    M5.dis.drawpix(led, 0xFF0000);                               //a prostřední LED svítí červeně
    reset++;                                                     //reset se zvýší o 1
    if (reset == 10) {                                           //pokud reset = 10,
      Serial.println("Restarting...");                           //Atom se restartuje
      delay(3000);
      ESP.restart();
    }
  }
  else {                                                         //pokud je Atom připojen,
    Serial.println("Server is up");                              //vypisuje se zpráva, že server běží
    sensor();                                                    //přivolání funkce pro kontrolu připojení senzoru
    M5.dis.drawpix(led, 0x00FF00);                               //a prostřední LED svítí zeleně                               
  }
}

//******************************************************************

void sensor() {
  if (!bmp.begin(bmpAddress)) {                                  //pokud na předem definované adrese nebyl nalezen senzor,
    Serial.println("Couldn't find BMP sensor. Check wires!");    //vypíše se zpráva, že není připojen
  }
  else {                                                         //pokud na adrese senzor nalezen byl,
    Serial.println("Sensor connected");                          //vypíše se zpráva o jeho úspěšném připojení
  }
}

//******************************************************************

void getPressTemp() {
  bmp.readTemperature();                                         //funkce pro získání teploty a tlaku
  bmp.readPressure();
}

//******************************************************************

void beginServer() {
  server.on("/", handleRoot);                                    //zahájení serveru s html stránkou
  server.begin();
  connection();                                                  //přivolání předešlé funkce
}

//******************************************************************

void handleRoot() {
  server.send (200, "text/html", getPage());                     //na server se status codem 200 se odešle html stránka
}

//******************************************************************

void serverHandle() {
  server.handleClient();                                        //udržováí serveru stále v provozu
  connection();                                                 //přivolání předešlé funkce
}

//******************************************************************

void blynk() {
  Blynk.run();                                                  //blynk získání teploty a tlaku do mobilní aplikace
  Blynk.virtualWrite(V4, bmp.readTemperature());
  Blynk.virtualWrite(V5, bmp.readPressure());
}

//******************************************************************

void setup() {
  //přivolání funkcí pro setup
  doConnect();
  beginServer();
  Blynk.begin(token, ssid, pass);                              //zahájení komunikace pře blynk
}

//******************************************************************

void loop() {
  //přivolání funkcí pro loop
  serverHandle();
  getPressTemp();
  blynk();
  delay(5000);                                                //loop se po 5 skundách opkauje
}

//******************************************************************

//html stránka s css úpravou
String getPage() {
  String page = "<html lang=\"en\">\n";
  page += "<head>\n";
  page += "    <meta charset=\"UTF-8\">\n";
  page += "    <meta http-equiv='refresh'\"X-UA-Compatible\" content='30'\"IE=edge\">\n";
  page += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
  page += "    <link rel=\"preconnect\" href=\"https://fonts.googleapis.com\">\n";
  page += "    <link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin>\n";
  page += "    <link href=\"https://fonts.googleapis.com/css2?family=Rubik:wght@400;700&display=swap\" rel=\"stylesheet\">\n";
  page += "    <link rel=\"stylesheet\" href=\"https://use.fontawesome.com/releases/v5.7.2/css/all.css\" integrity=\"sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr\" crossorigin=\"anonymous\">\n";
  page += "    <link rel=\"stylesheet\" href=\"style.css\">\n";
  page += "    <title>METEOROLOGICKÁ STANICE</title>\n";
  page += "    <style>html{\n";
  page += "        box-sizing: border-box;\n";
  page += "    }\n";
  page += "    \n";
  page += "    *, *::after, *::before {\n";
  page += "        box-sizing: inherit;\n";
  page += "    }\n";
  page += "    \n";
  page += "    body::-webkit-scrollbar{\n";
  page += "        display: none;\n";
  page += "    }\n";
  page += "    \n";
  page += "    body{\n";
  page += "        font-family: 'Rubik', sans-serif;\n";
  page += "        text-rendering: optimizeLegibility;\n";
  page += "        margin: 0;\n";
  page += "    }\n";
  page += "    \n";
  page += "    header {\n";
  page += "        font-size: 40px;\n";
  page += "        display:flex;\n";
  page += "        justify-content: center;\n";
  page += "    }\n";
  page += "    \n";
  page += "    .teplota{\n";
  page += "        display: flex;\n";
  page += "        justify-content: center;\n";
  page += "        margin:0;\n";
  page += "        font-size: 30px;\n";
  page += "        background-color: red;\n";
  page += "        border-radius: 5px 5px 0 0;\n";
  page += "        position: relative;\n";
  page += "    }\n";
  page += "\n";
  page += "    p{\n";
  page += "        margin:0;\n";
  page += "    }\n";
  page += "    \n";
  page += "\n";
  page += "    \n";
  page += "    h2{\n";
  page += "        font-weight: 400;\n";
  page += "    }\n";
  page += "    \n";
  page += "    .tlak{\n";
  page += "        display:flex;\n";
  page += "        margin:0;\n";
  page += "        font-size: 30px;\n";
  page += "        justify-content: center;\n";
  page += "        position:relative;\n";
  page += "    }\n";
  page += "    \n";
  page += "    \n";
  page += "    .stanice{\n";
  page += "        background-color: rgb(98, 243, 134);\n";
  page += "        margin: 50px 200px 0 200px;\n";
  page += "        border-radius: 5px;\n";
  page += "        box-shadow: rgba(0, 0, 0, 0.35) 0px 50px 50px;\n";
  page += "    }\n";
  page += "    .teplota i{\n";
  page += "        color: black;\n";
  page += "        position: absolute;\n";
  page += "        bottom:34%;\n";
  page += "        left:35%;\n";
  page += "    }\n";
  page += "\n";
  page += "    .tlak i {\n";
  page += "        color: black;\n";
  page += "        position: absolute;\n";
  page += "        bottom:33%;\n";
  page += "        left:34%;\n";
  page += "    }\n";
  page += "    @media only screen and (max-width: 600px)\n";
  page += "    {\n";
  page += "        header{\n";
  page += "            display: flex;\n";
  page += "            justify-content: center;\n";
  page += "            font-size: 20px;\n";
  page += "        }\n";
  page += "\n";
  page += "        .tlak, .teplota{\n";
  page += "            font-size: 15px;\n";
  page += "        }\n";
  page += "\n";
  page += "        .stanice{\n";
  page += "            background-color: rgb(98, 243, 134);\n";
  page += "            margin: 50px 20px;\n";
  page += "            border-radius: 5px;\n";
  page += "            box-shadow: rgba(0, 0, 0, 0.35) 0px 50px 50px;\n";
  page += "        }\n";
  page += "    }\n";
  page += "    .teplota i{\n";
  page += "        color: black;\n";
  page += "        position: absolute;\n";
  page += "        bottom:45%;\n";
  page += "        left:15%;\n";
  page += "    }\n";
  page += "\n";
  page += "    .tlak i {\n";
  page += "        color: black;\n";
  page += "        position: absolute;\n";
  page += "        bottom:45%;\n";
  page += "        left:15%;\n";
  page += "     }\n";
  page += "    </style>\n";
  page += "</head>\n";
  page += "<body>\n";
  page += "    <header>\n";
  page += "        <p>Meteorologická stanice BMP 280</p>\n";
  page += "    </header>\n";
  page += "<div class=\"stanice\">\n";
  page += "    <div class=\"teplota\">\n";
  page += "        <h2>\n";
  page += "            <i class=\"fas fa-thermometer-half\"></i>\n";
  page += "            <p>Teplota(°C)</p>\n";
  page += bmp.readTemperature();
  page += "        </h2>\n";
  page += "    </div>\n";
  page += "\n";
  page += "    <div class=\"tlak\">\n";
  page += "        <h2>\n";
  page += "            <i class=\"fas fa-cloud-download-alt\"></i>\n";
  page += "            <p>Tlak(Pa)</p>\n";
  page += bmp.readPressure();
  page += "        </h2>\n";
  page += "    </div>\n";
  page += "</div>\n";
  page += "</body>\n";
  page += "</html>";
  return page;
}
