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

// Classic ESP32 GPIO numbers. Switches are isolated contacts to GND.
#define WALL_FAN 25
#define WALL_LIGHT 26
#define WALL_SOCKET1 27
#define WALL_SOCKET2 33
#define WALL_FAN_SWITCH 18
#define WALL_LIGHT_SWITCH 19
#define WALL_SOCKET1_SWITCH 21
#define WALL_SOCKET2_SWITCH 22
#define WALL_ACTIVE_LOW true
