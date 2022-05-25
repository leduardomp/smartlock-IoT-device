#include <EEPROM.h>

#include <ESP8266WiFi.h>

#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <PubSubClient.h>
#include <ArduinoJson.h>

//GENERALES
const byte JUMPER_PIN = D5;
const char *NUM_SERIE = "";       //Numero de Serie del dispositivo
const char *HOST_NAME = "";       //Nombre del Dispositivo
const char *IP_SERVER_MQTT = "";  //IP del servidor MQTT

const byte PUERTA = D8;
const int PRENDER = HIGH;
const int APAGAR = LOW;

StaticJsonDocument<200> json;

WiFiClient espClient;
ESP8266WebServer serverConfig(80);

struct DatosConfig
{
	int modoConfig;
	char ssid[50];
	char password[50];
}datosConfig;


//MQTT
const char *CANAL_SUBSCRIPCION = "";      //Canal del Servicio MQTT
const char *MQTT_USER = "";               //Usuario servicio MQTT
const char *MQTT_PASS = "";               //Password servicio MQTT

WiFiClient clientWifiMQTT;
PubSubClient clientMQTT(clientWifiMQTT);

//OTA
ESP8266WebServer httpServerOTA(8080);
ESP8266HTTPUpdateServer httpUpdater;
WiFiClient clientOTA;

const char *URL_UPDATE = "";      //Direccion donde se sube el nuevo Firmware
const char *usernameWEB = "";     //Usuario para el servicio http
const char *passwordWEB = "";     //Password para el servicio htpp

void (*resetFunc)(void) = 0;

void setup()
{
	Serial.begin(115200);
	Serial.println("::::::  Inicializando Sensor  ::::::");
	EEPROM.begin(128);

	pinMode(JUMPER_PIN, INPUT_PULLUP);

	if (digitalRead(JUMPER_PIN) == LOW)
	{
		Serial.print("Reset ");
		int contador = 0;
		bool resetParameter = false;

		while (digitalRead(JUMPER_PIN) == LOW && contador < 2000)
		{

			Serial.print(".");

			contador++;
			if (contador == 2000)
			{
				Serial.println("");
				Serial.println("Enviar a resetParameters");
				resetParameter = true;
			}
		}

		Serial.println("");

		if (resetParameter)
		{
			resetParameters();
			Serial.println("Reset susefull!!");
		}

		Serial.println("Un segundo! Reseteando");
		resetFunc();
	}
	else
	{
		// Jumper is not inserted

		EEPROM.get(0, datosConfig);

		Serial.print("valor modoConfig:");
		Serial.println(datosConfig.modoConfig);

		if (datosConfig.modoConfig != 1)
		{
      blinkLed(3);
			serverStationMode();
		}
		else
		{
			pinMode(PUERTA, OUTPUT);
			pinMode(LED_BUILTIN, OUTPUT);

			Serial.print("SSID eeprom:");
			Serial.println(datosConfig.ssid);
			Serial.print("password eeprom:");
			Serial.println(datosConfig.password);

			wifiConfig(datosConfig.ssid, datosConfig.password);
			startOTA(HOST_NAME);
			startHTTP(HOST_NAME);
			startMQTT();

			Serial.println("::::::  Inicializacion Terminada  ::::::");
			blinkLed(5);
		}
	}
}

void loop()
{

	if (datosConfig.modoConfig != 1)
	{
    blinkLed(3);
		serverConfig.handleClient();
	}
	else
	{
		//OTA
		ArduinoOTA.handle();

		//WEB
		httpServerOTA.handleClient();
		MDNS.update();

		//MQTT
		if (!clientMQTT.connected())
		{
			reconnect();
		}
		clientMQTT.loop();
	}
}
