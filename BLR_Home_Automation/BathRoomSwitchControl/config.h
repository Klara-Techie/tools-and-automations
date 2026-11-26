#pragma once
#define OTA_PASSWORD NODE_PASSWORD

#define HOME_SSID "Black_Pearl"
#define HOME_PASSWORD "Karthik_Ravi"
#define AP_SSID "Home-Automation"
#define AP_PASSWORD "black-pearl-2306"
#define NODE_PASSWORD "black-pearl-2306"
// Confirm that this address is unused and outside the router's DHCP pool.
#define MASTER_IP IPAddress(192, 168, 31, 200)
// Provisional: replace with the router's actual gateway.
#define ROUTER_GATEWAY IPAddress(192, 168, 31, 1)
#define AP_IP IPAddress(192, 168, 4, 1)

// ESP8266 GPIO5 (NodeMCU D1); use a properly designed relay driver.
#define HEATER_RELAY 5
#define HEATER_ACTIVE_LOW true
