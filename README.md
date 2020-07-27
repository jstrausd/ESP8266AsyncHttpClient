AsyncHttpClient
===============
A asynchronous HTTP client for the ESP8266 that uses [ESPAsyncTCP](https://github.com/me-no-dev/ESPAsyncTCP).

IDEA
----
Sending simple and asynchronous GET or POST requests with data to a server.
Currently it is not supported to receive a callback! 
The data is only sent to the server and will be there processed.

TODO
----
* Receive callbacks for processing

HOW TO USE
----------

1. Download Git-repo and put Library folder into libraries folder of Arduino
2. Include Library in Arduino Project

```
#include <AsyncHttpClient.h>
```

3. Define AsyncHttpClient Variable

```
AsyncHttpClient aClient;
```

4. Use Methods anywhere in your project

EXAMPLES
--------

**Homebridge example**  
POST request with JSON data
```
String stateValue = (state == "off") ? "false" : "true";
String PostData = "{\"characteristic\":\"On\",\"value\":" + stateValue + ",\"password\":\"password123\"}";
aClient.init("POST","http://10.0.0.30:8081/lamp1", "application/json", PostData);
aClient.send();
```

**Other examples**  
Simple GET request to google.at
```
aClient.init("GET","http://google.at");
aClient.send();
```

Simple GET request with GET Data passed via URL
```
aClient.init("GET","http://server.com/push.php?firstname=Jonathan&lastname=Strauss");
aClient.send();
```

*Set all arguments one by one*

For GET and POST:
```
aClient.setType("GET");
aClient.setFullURL("http://server.com:8080/");
```

Only for POST:
```
aClient.setDataMode("application/x-www-form-urlencoded");
aClient.setData("value=123&number=123");
```