# Šindler-ročníkový projekt
Cílem mého projektu bylo využití produktů od M5Stack v každodenním životě. V tomto případě jsem pracoval s [M5Stack ATOM Matrix](https://rpishop.cz/m5stack/2766-m5stack-atom-matrix-esp32-development-kit.html) a [M5Stack Core 2](https://rpishop.cz/m5stack/3066-m5stack-core2-esp32-iot-vyvojova-sada.html) a využil oba tyto produkty ke sbírání dat ohledně teploty, tlaku atd..

Obě části projektu byly programovány v [Arduino IDE](https://www.arduino.cc/en/software).

## M5Stack ATOM Matrix
ATOM matrix jsem použil pro měření pokojové teploty a tlaku pomocí senzoru [BMP 280](https://www.laskakit.cz/arduino-senzor-barometrickeho-tlaku-a-teploty-bmp280/). Výsledky měření se poté zobrazovaly na webové stránce a také v aplikaci blynk pomocí grafů.

### Blynk setup

>Blynk stáhneme buď na Google Play nebo App Store

<img src="./images/blynk1.jpg" width="250" height="540"></img> 

>Po stažení a registraci klikneme na tlačítko pro vytvoření nového projektu

<img src="./images/blynk13.jpg" width="250" height="540"></img>

>Projekt si pojmenujeme

<img src="./images/blynk2.jpg" width="250" height="540"></img> 

>Vybereme zařízení, v tomto případě to bude **ESP32 Dev Board**

<img src="./images/blynk3.jpg" width="250" height="540"></img>

>Jako typ připojení zvolíme WiFi a vytvoříme projekt

<img src="./images/blynk4.jpg" width="250" height="540"></img>

>V novém projektu klikneme na tlačíkto por přidání widgetů

<img src="./images/blynk6.jpg" width="250" height="540"></img>

>Pro zobrazení hodnot pomocí grafu zvolíme SuperChat

<img src="./images/blynk7.jpg" width="250" height="540"></img>

>Po zvolení widgetu klineme na tlačítko pro úpravy 

<img src="./images/blynk8.jpg" width="250" height="540"></img>

>Do grafu přídáme proměnné, které cheme aby se zobrazovaly

<img src="./images/blynk9.jpg" width="250" height="540"></img>

>Proměnné si pojmenujeme a klikneme na tlačítko pro jejich úpravu

<img src="./images/blynk10.jpg" width="250" height="540"></img>

>Klikneme na tlačítko pro přidělelní pinu

<img src="./images/blynk11.jpg" width="250" height="540"></img>

>Vybereme virtuální pin 4, u druhé proměnné vybereme pin 5, tímto krokem je Blynk setup hotov

<img src="./images/blynk12.jpg" width="250" height="540"></img>

### Arduino setup

>V Arduino IDE klikeneme na Soubor>Vlastnosti

<img src="./images/img1.JPG"></img>
