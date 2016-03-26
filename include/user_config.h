#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

	// UART config
	#define SERIAL_BAUD_RATE 115200

	// ESP SDK config
	#define LWIP_OPEN_SRC
	#define USE_US_TIMER

	// Default types
	#define __CORRECT_ISO_CPP_STDLIB_H_PROTO
	#include <limits.h>
	#include <stdint.h>

	// Override c_types.h include and remove buggy espconn
	#define _C_TYPES_H_
	#define _NO_ESPCON_

	// Updated, compatible version of c_types.h
	// Just removed types declared in <stdint.h>
	#include <espinc/c_types_compatible.h>

	// System API declarations
	#include <esp_systemapi.h>

	// C++ Support
	#include <esp_cplusplus.h>
	// Extended string conversion for compatibility
	#include <stringconversion.h>
	// Network base API
	#include <espinc/lwip_includes.h>

	// Beta boards
	#define BOARD_ESP01

	// GPIO config
	#define BTN_PIN 0
	#define LED_PIN 2

	// Default Xiaoyi network password and prefix
	#define WIFI_PWD "1234567890"
	#define NETWORK_PREFIX "YDXJ_"

	// Default network_message_daemon port and camera IP
	#define SERVER_IP "192.168.42.1"
	#define SERVER_PORT 7878

	// Static IP
	#define CLIENT_IP IPAddress(192, 168, 42, 250)

#ifdef __cplusplus
}
#endif

#endif
