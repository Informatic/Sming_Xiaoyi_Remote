#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include "AmbaClient.h"

// You can find some configuration variables in user_config.h

void listNetworks(bool succeeded, BssList list);

AmbaClient rpc;


// Will be called when WiFi station was connected to AP
void connectOk()
{
	debugf("I'm CONNECTED");
	Serial.println(WifiStation.getIP().toString());
	rpc.connect(SERVER_IP, SERVER_PORT);
	rpc.authenticate();
}

// Will be called when WiFi station timeout was reached
void connectFail()
{
	debugf("I'm NOT CONNECTED!");
	WifiStation.waitConnection(connectOk, 30, connectFail); // Repeat and check again
	WifiStation.startScan(listNetworks); // In Sming we can start network scan from init method without additional code
}

// Will be called when WiFi station network scan was completed
void listNetworks(bool succeeded, BssList list)
{
	if (!succeeded)
	{
		Serial.println("Failed to scan networks");
		return;
	}

	Serial.print("Found ");
	Serial.print(list.count());
	Serial.println(" networks.");

	for (int i = 0; i < list.count(); i++)
	{
		Serial.print(list[i].ssid);
		if (list[i].ssid.startsWith(NETWORK_PREFIX)) {
			Serial.print(" -> XIAOYI");
			WifiStation.config(list[i].ssid, WIFI_PWD);
		}
		Serial.println();
	}
}

void init()
{
	Serial.begin(SERIAL_BAUD_RATE);
	Serial.systemDebugOutput(false); // Allow debug print to serial

	// Soft access point
	WifiAccessPoint.enable(false);

	// Station - WiFi client
	WifiStation.enable(true);

	// Set static IP for quicker connection
	WifiStation.setIP(CLIENT_IP);

	WifiStation.waitConnection(connectOk, 10, connectFail);

	// Configure GPIO
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, LOW);

	pinMode(BTN_PIN, INPUT);
	attachInterrupt(BTN_PIN, *[]() {
		if(rpc.currentState == AmbaClient::S_READY) {
			rpc.sendMessage(MSG_CAPTURE);
			Serial.println("Capturing...");
			digitalWrite(2, LOW);
		}
	}, FALLING);
}
