/*
  AsyncHttpClient.cpp - Library for sending simple data via HTTP GET/POST.
  Created by Jonathan Strauss, July 27, 2020.
*/

#include "Arduino.h"
#include "ESPAsyncTCP.h"
#include "AsyncHttpClient.h"

/*

    String type = POST or GET (most used for POST)
    String fullUrl = full URL e.g. http://server.com:8080/push.php or http://server.com or http://server.com:8081
    String dataMode = only for POST - Content Type --> e.g. application/json , application/x-www-form-urlencoded
    String data = data to submit --> depends on Type (GET/POST) e.g. (application/x-www-form-urlencoded) api-key=1asnd12i3kas&firstname=Jonathan&lastname=Strauss or (application/json) {"api-key": "1asnd12i3kas", "firstname": "Jonathan", "lastname": "Strauss"}

*/
void AsyncHttpClient::init(String type, String fullUrl, String dataMode, String data)
{
    _type = type;
    _fullUrl = fullUrl;
    _dataMode = dataMode;
    _data = data;
    getHostname(fullUrl);
}

/*

    String type = POST or GET (most used for GET)
    String fullUrl = full URL e.g. http://server.com:8080/push.php?api-key=1asnd12i3kas&firstname=Jonathan&lastname=Strauss  or http://server.com or http://server.com:8081
*/

void AsyncHttpClient::init(String type, String fullUrl)
{
    _type = type;
    _fullUrl = fullUrl;
    getHostname(fullUrl);
}

void AsyncHttpClient::setType(String type){
    _type = type;
}
void AsyncHttpClient::setFullURL(String fullUrl){
    _fullUrl = fullUrl;
    getHostname(fullUrl);
}
void AsyncHttpClient::setDataMode(String dataMode){
    _dataMode = dataMode;
}
void AsyncHttpClient::setData(String data){
    _data = data;
}

/*

    SUBTRACT Hostname and Port from URL

*/
void AsyncHttpClient::getHostname(String url)
{
    int index = url.indexOf(':');
    if (index < 0)
    {
        _hostname = "";
        return;
    }
    String protocol = url.substring(0, index);
    _port = 80;
    if (index == 5)
    {
        _port = 443;
    }
    
    url.remove(0, (index + 3));
    index = url.indexOf('/');
    String hostPart;
    if (index > 0)
    {
        hostPart = url.substring(0, index);
    }else{
        hostPart = url;
    }

    url.remove(0, index);

    index = hostPart.indexOf(':');
    String host;
    if (index >= 0)
    {
        _hostname = hostPart.substring(0, index);
        host = _hostname;
        hostPart.remove(0, (index + 1)); // remove hostname + :
        _port = hostPart.toInt();        // get port
    }
    else
    {
        _hostname = hostPart;
    }
}

void AsyncHttpClient::send()
{
    Serial.println("Type: " + _type + " URL: " + _fullUrl + " DataMode: " + _dataMode + " Data: " + _data);

    if (_hostname.length() <= 0) {
		Serial.println("Hostname is not defined");
		return;
	}

    if (_port <= 0) {
		Serial.println("Port is not defined");
		return;
	}

    if (_type.length() <= 0) {
		Serial.println("Type is not defined (Only GET or POST)");
		return;
	}

    if (_fullUrl.length() <= 0) {
		Serial.println("URL is not defined");
		return;
	}

    if (_dataMode.length() <= 0) {
		Serial.println("DataMode is not defined");
		return;
	}

    if (_data.length() <= 0) {
		Serial.println("Data is not defined");
		return;
	}

    if (aClient) //client already exists
        return;

    aClient = new AsyncClient();
    if (!aClient) //could not allocate client
        return;

    aClient->onError([&](void *arg, AsyncClient *client, int error) {
        Serial.println("Connect Error");
        aClient = NULL;
        delete client;
    },NULL);

    aClient->onConnect([&](void *arg, AsyncClient *client) {
        Serial.println("Connected");
        aClient->onError(NULL, NULL);

        client->onDisconnect([&](void *arg, AsyncClient *c) {
            Serial.println("Disconnected");
            aClient = NULL;
            delete c;
        },NULL);

        client->onData([&](void *arg, AsyncClient *c, void *data, size_t len) {
            Serial.print("\r\nResponse: ");
            Serial.println(len);
            uint8_t *d = (uint8_t *)data;
            for (size_t i = 0; i < len; i++)
                Serial.write(d[i]);
        },NULL);

        //send the request
        String PostHeader;
        if (_type == "POST")
        {
            PostHeader = "POST " + _fullUrl + " HTTP/1.1\r\nHost: " + _hostname + "\r\nUser-Agent: Arduino/1.0\r\nConnection: close\r\nContent-Type: " + _dataMode + ";\r\nContent-Length: " + String(_data.length()) + "\r\n\r\n" + _data + "\r\n";
        }
        else if (_type == "GET")
        {
            PostHeader = "GET " + _fullUrl + " HTTP/1.1\r\nHost: " + _hostname + "\r\n\r\n";
        }
        client->write(PostHeader.c_str());
    },NULL);

    if (!aClient->connect(_hostname.c_str(), _port))
    {
        Serial.println("Connect Fail");
        AsyncClient *client = aClient;
        aClient = NULL;
        delete client;
    }
}