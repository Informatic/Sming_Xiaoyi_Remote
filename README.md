Sming\_Xiaoyi\_Remote
=====================

Simple remote for Xiaomi Yi / Xiaoyi based on Sming Framework on ESP8266.
Falling edge on `GPIO0` sends "capture" message, and `GPIO2` goes low when
connecting or capturing photos. GPIO mappings and other settings are
configurable in `user_config.h`. `AmbaClient` is a simple ambarella RPC
client class, if you wanted to incorporate it in some other project.

`GPIO2` is blue LED on ESP12E module, and `GPIO0` is "Flash" button on NodeMCU
boards. (and most probably some other boards, since that's pretty generic)

License
-------
MIT
