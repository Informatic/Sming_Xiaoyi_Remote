#include "user_config.h"
#include "AmbaClient.h"

err_t AmbaClient::onReceive(pbuf* buf)
{
    int start = 0;
    int eol;

    // FIXME this is plain wrong, however it's pretty hard to inject } into
    // JSON there anyway
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

void AmbaClient::onMessage(int msg_id, JsonObject& root)
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

        // FIXME use proper callbacks for that
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

void AmbaClient::onStatus(String type, JsonObject& root)
{
    if(type == "photo_taken")
    {
        digitalWrite(LED_PIN, HIGH);
    }
}

void AmbaClient::authenticate()
{
    Serial.println("Authenticating...");
    sendMessage(MSG_AUTHENTICATE);
}

void AmbaClient::sendMessage(int msg_id)
{
    StaticJsonBuffer<256> jsonBuffer;

    // Default to empty object
    sendMessage(msg_id, jsonBuffer.createObject());
}

void AmbaClient::sendMessage(int msg_id, JsonObject& json)
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
