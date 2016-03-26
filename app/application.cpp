#include <user_config.h>
#include <SmingCore/SmingCore.h>

// Default Xiaoyi network password and prefix
#define WIFI_PWD "1234567890"
#define NETWORK_PREFIX "YDXJ_"

// Default network_message_daemon port and camera IP
#define SERVER_IP "192.168.42.1"
#define SERVER_PORT 7878

// Static IP
#define CLIENT_IP IPAddress(192, 168, 42, 250)

// GPIO config
#define BTN_PIN 0
#define LED_PIN 2

void listNetworks(bool succeeded, BssList list);

enum MessageType {
	MSG_CONFIG_SET = 2,
	MSG_CONFIG_GET = 3,
	MSG_STORAGE_USAGE = 5,
	MSG_STATUS = 7,
	MSG_BATTERY = 13,
	MSG_AUTHENTICATE = 257,
	MSG_PREVIEW_STOP = 258,
	MSG_PREVIEW_START = 259,
	MSG_RECORD_START = 513,
	MSG_RECORD_STOP = 514,
	MSG_CAPTURE = 769,
};

class AmbaClient : TcpClient {
public:
	AmbaClient(bool autoDestruct = false) : TcpClient(autoDestruct)
	{
	}

	enum ClientState {
		S_AUTH,
		S_READY,
		S_WAITING,
	} currentState = S_AUTH;

	int token = 0;

	err_t onReceive(pbuf* buf)
	{
		int start = 0;
		int eol;

		while ((eol = NetUtils::pbufFindStr(buf, "}", start)) != -1)
		{
			StaticJsonBuffer<256> jsonBuffer;
			String msg = NetUtils::pbufStrCopy(buf, start, eol + 1 - start);
			Serial.print("<<- ");
			Serial.println(msg);

			JsonObject& root = jsonBuffer.parseObject(msg);
			if (!root.success())
			{
				Serial.println("JSON parse failed!");
			}
			else
			{
				onMessage(root["msg_id"], root);
			}
			start = eol + 1;
		}

		return TcpClient::onReceive(buf);
	}

	void onMessage(int msg_id, JsonObject& root)
	{
		String type;

		Serial.print("msg_id found: ");
		Serial.println(msg_id);

		switch(msg_id) {
			case MSG_AUTHENTICATE:
			token = root["param"];
			Serial.print(" -> Token updated: ");
			Serial.println(token);

			currentState = S_READY;
			digitalWrite(LED_PIN, HIGH);
			break;

			case MSG_STATUS:
			type = String((const char*) root["type"]);

			Serial.print(" -> Event occured: ");
			Serial.println(type);

			onStatus(type, root);
			break;

			default:
			Serial.println("unknown msg_id");
			break;
		}
	}

	void onStatus(String type, JsonObject& root)
	{
		if(type == "photo_taken")
		{
			digitalWrite(LED_PIN, HIGH);
		}
	}

	void authenticate()
	{
		Serial.println("Authenticating...");
		sendMessage(MSG_AUTHENTICATE);
	}

	void sendMessage(int msg_id)
	{
		StaticJsonBuffer<256> jsonBuffer;

		// Default to empty object
		sendMessage(msg_id, jsonBuffer.createObject());
	}

	void sendMessage(int msg_id, JsonObject& json)
	{
		char buf[256];

		json["msg_id"] = msg_id;
		json["token"] = token;

		Serial.print("->> ");
		json.printTo(Serial);
		Serial.println();

		json.printTo(buf, sizeof buf);
		sendString(buf);
	}

	using TcpClient::connect;
};

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
