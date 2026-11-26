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

#define LIVING_TV 25
#define LIVING_AMP 26
#define LIVING_LED 27
#define LIVING_ACTIVE_LOW true
