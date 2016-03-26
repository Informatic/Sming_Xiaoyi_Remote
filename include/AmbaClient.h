#include <SmingCore/SmingCore.h>

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
	AmbaClient(bool autoDestruct = false) : TcpClient(autoDestruct) { }

	enum ClientState {
		S_AUTH,
		S_READY,
		S_WAITING,
	} currentState = S_AUTH;

	int token = 0;

	using TcpClient::connect;
    err_t onReceive(pbuf* buf);
    void onMessage(int msg_id, JsonObject& root);
    void onStatus(String type, JsonObject& root);
    void authenticate();
    void sendMessage(int msg_id);
    void sendMessage(int msg_id, JsonObject& json);
};
