# Arduino IDE home automation

This is a **prototype, not a verified installation**. It has not been tested
on the boards. Do not connect it to mains equipment until the
firmware and hardware have been tested and a qualified electrician has
designed and checked the installation.

## Boards

| Board | Sketch | Selection |
| --- | --- | --- |
| ESP32 master + TV, amplifier and LED relays | `Master/Master.ino` | ESP32 Dev Module |
| ESP8266 amplifier IR and knob LEDs | `AmpRemoteControl/AmpRemoteControl.ino` | Appropriate ESP8266 board |
| ESP32 fan, light and two sockets | `RoomSwitchControl/RoomSwitchControl.ino` | ESP32 Dev Module |
| ESP8266 water heater | `BathRoomSwitchControl/BathRoomSwitchControl.ino` | Appropriate ESP8266 board |

The active installation uses **four boards**. The separate Wi-Fi power switch
was removed as requested. Each board has its own sketch and configuration.
Relay implementations live in `HomeNodeNetwork`; the master and the retained
standalone entertainment sketch use the same entertainment relay/timer engine.
The IR codes come from the original Blynk sketch.

## Migrating entertainment relays to the master

The master now drives the TV, amplifier and LED-strip relay inputs directly:
**GPIO25, GPIO26 and GPIO27**, respectively, with active-low polarity.
These settings are in the master's private `config.h` as `LIVING_TV`,
`LIVING_AMP`, `LIVING_LED` and `LIVING_ACTIVE_LOW`. A value of `-1` disables
an individual relay; an invalid or duplicate GPIO configuration disables the
entire local relay group and logs an error.

1. Switch off and isolate the installation before changing wiring. Move only
   the properly isolated, low-voltage relay control connections to the master;
   verify driver compatibility, grounding, supply and startup OFF behavior.
   Never connect mains or relay coils directly to ESP32 GPIOs.
2. Replace the installed `HomeNodeNetwork` library with the current copy, and
   compile/upload **Master**. The room, bathroom and IR boards keep their
   existing protocols and do not require a reflash solely for this merge.
   The always-on hotspot migration below does require uploading all nodes.
3. Disconnect the old entertainment controller. Its
   [standalone sketch](EntertainmentControl/EntertainmentControl.ino) and
   configuration are retained for reference, not as a fifth active board.
   The merged master rejects old `living` registrations; another controller
   cannot overwrite its local entertainment state.
4. Bench-test individual/group commands, all timer cancellation combinations,
   Wi-Fi/hotspot transitions, master reset and OTA with safe low-voltage
   indicators before reconnecting real loads.

**Changed reset/OTA behavior:** the TV, amplifier and LED relays now turn OFF
when the master initializes or accepts an OTA update, and their timers are
cleared. They remain OFF after an OTA transfer fails. Master power loss also
loses these timers; use relay hardware that defaults OFF. The wall-board
socket timer and bathroom heater cutoff still run on their own boards.
Wi-Fi loss alone does not stop local entertainment timers. They are
cooperative software timers, so blocking network work can briefly delay
expiry handling; they are not electrical safety devices.

## Dashboard theme backup

The previous neon theme, including the captain name and pirate flag, is saved
unchanged in [themes/NeonBedroom/dashboard.h](themes/NeonBedroom/dashboard.h).
To restore it, copy that file over `Master/dashboard.h`, then upload only the
master sketch. That historical backup does not include the new knob-lighting
controls. The active theme uses original pirate-inspired styling with
local fonts and CSS/SVG artwork; it needs no external images or Internet access.
The full ship and moon form a compact, centered emblem with smaller Black Pearl
lettering and no promotional tagline. Opaque shaded sails replace the wireframe
look, and the water runs into the wordmark. Neither mobile nor desktop layouts
crop the ship or moon.
The dashboard is mobile-first: compact two-column appliance cards, full-width
volume rows and a smaller static ship emblem. The charcoal/silver theme uses
local outline SVG appliance symbols instead of emoji. ON icons stay light on
a dark background, with a matching status badge and a tinted card; All on and
START use muted dark fills rather than bright highlights. Controls retain
44-pixel tap targets and 16-pixel mobile form text. Hover lifts, transitions,
press-flash timers and vibration are removed; live status, countdowns and knob
auto-save remain. Decorative LED firmware patterns are unchanged.
The Entertainment power tile places equally sized All on/off, minutes and
START/CANCEL ALL controls in a single row beneath its heading.
Volume-command progress/results appear beside the corresponding channel name
(for example, `Master | Volume up · sent`), with requests still serialized.
Clicking another volume control clears the previous inline message; only the
latest command updates its channel's feedback, even if an older request finishes later.
An active individual timer replaces START with CANCEL in the same position;
its remaining time appears inside the Min field instead of a separate timer
heading. Expiry or cancellation restores the minutes input and START. The group row similarly shows CANCEL
ALL while any TV/amplifier/LED timer is active, preserving its existing
cancel-all behavior. Socket timers remain independent. Offline timer actions
are disabled.

## Installation and configuration

1. Install Arduino IDE 2, the ESP32 and ESP8266 board packages, and select the
   correct board and port before each upload. The code targets a classic ESP32
   Dev Module and an ESP8266 NodeMCU-class board.
2. In Library Manager install **WebSockets by Markus Sattler** for the master,
   and **IRremoteESP8266 by David Conran** plus **Adafruit NeoPixel by Adafruit**
   for the IR/knob-lighting node.
3. Copy the `libraries/HomeNodeNetwork` folder into your Arduino sketchbook's
   `libraries` directory, then restart Arduino IDE. The master and all three
   nodes need this library. When upgrading, replace the installed library with
   the current copy and re-upload affected boards.
4. In each of `Master`, `AmpRemoteControl`,
   `RoomSwitchControl`, and `BathRoomSwitchControl`, edit the private `config.h`.
   Open each `.ino` from its own folder. Set matching master-hotspot
   `AP_SSID`, `AP_PASSWORD`, `AP_IP` and `NODE_PASSWORD` in all four active files.
   Only the master uses the router's `HOME_SSID` and `HOME_PASSWORD`. Set a
   dashboard username and password in the master. Set an `OTA_PASSWORD` of at
   least eight characters on each board; the current configs use `NODE_PASSWORD`
   for OTA, but separate strong passwords are recommended. Keep private configs
   out of source control.
5. The current configurations use master `192.168.31.200`, gateway
   `192.168.31.1`, and subnet `255.255.255.0`. Confirm the
   gateway and reserve the master address outside the router's DHCP pool; it
   must not be assigned to another device. When moving from Jio to ACT, check
   the new router's actual gateway/subnet and update only the master's
   `MASTER_IP`, `ROUTER_GATEWAY` and `ROUTER_SUBNET` if needed. Ensure the router
   network does not overlap `192.168.4.0/24`, which the master hotspot uses.
   Router client isolation can affect phone-to-master access, but no longer
   carries or controls node-to-master communication. Nodes use the master's
   hotspot DHCP server and always register at `AP_IP` (`192.168.4.1`).
   Legacy router macros can remain in node configs; they are unused.
6. Check the pin plan below against the actual board and hardware. Set relay
   polarity in the master and relay nodes' `config.h` to match their drivers; the defaults
   assume active-low inputs. A GPIO value of `-1` disables that channel.
   Serial Monitor runs at 115200 baud.
7. Compile and upload each board. Open
   `http://192.168.31.200/` from a phone on the router Wi-Fi and sign in with
   the master dashboard credentials.

## Default pin plan

These are GPIO numbers, not ESP8266 D labels. The ESP32 assignments assume a
classic ESP32 Dev Module with these GPIOs exposed; verify the exact board's
pinout. The ESP8266 D labels assume NodeMCU 1.0.

| Board | Function | GPIO | NodeMCU label |
| --- | --- | --- | --- |
| Master ESP32 | TV / amplifier / LED relays | 25 / 26 / 27 | — |
| Room ESP32 | Fan / light / Socket / Socket 2 relays | 25 / 26 / 27 / 33 | — |
| Room ESP32 | Fan / light / Socket / Socket 2 switches | 18 / 19 / 21 / 22 | — |
| Bathroom ESP8266 | Heater relay | 5 | D1 |
| IR ESP8266 | IR LED driver | 4 | D2 |
| IR ESP8266 | WS2812B knob chain data | 5 | D1 |

Room switches are isolated contacts between the input GPIO and GND; firmware
enables the ESP32's internal pull-ups. The selected pins avoid ESP8266 boot
strapping pins and the ESP32's input-only, boot-strapping, serial and flash
pins. The pin plan is not a substitute for checking the exact board or testing
reset behavior.

## Always-on master hotspot

The master starts its password-protected `Home-Automation` hotspot immediately
and keeps it running alongside home Wi-Fi (`WIFI_AP_STA`). All three nodes
connect **only** to this hotspot, not to the home router. The hotspot allows
eight clients, leaving room for the three nodes and phones or an OTA computer.
No router changes, port forwarding, bridging or Internet connection are needed
for node commands. The master forwards authenticated commands using each
node's hotspot address.

* Phone on home Wi-Fi: open `http://192.168.31.200/` (or the configured master IP).
* Phone on `Home-Automation`: open `http://192.168.4.1/`, with or without home Wi-Fi.
  If the phone warns that this network has no Internet, choose to stay connected.
* Router loss/restoration does not stop the hotspot or clear registered nodes.
  The master retries the router every 30 seconds; nodes retry only the master
  hotspot after a disconnect or a 15-second connection timeout.
* ESP32 AP and station share one radio/channel. Joining a router or a router
  channel change can briefly interrupt hotspot clients; nodes reconnect
  automatically. There is no guarantee of interruption-free Wi-Fi.
* Status updates are pushed to open dashboards; a periodic refresh is also used.
  The existing API `ap` flag means hotspot-only operation (router disconnected),
  not whether the always-on hotspot exists.

### Migrating from router-connected nodes

1. Replace the installed `HomeNodeNetwork` library with the current copy.
2. Upload **Master**, then **AmpRemoteControl**, **RoomSwitchControl** and
   **BathRoomSwitchControl**. Updating only the master leaves old nodes trying
   the router first. Use USB for this migration if network uploads are unreliable.
3. Expect the master to log `Always-on master hotspot address: 192.168.4.1`.
   Each node should log `Connected to master hotspot Home-Automation`, obtain
   a `192.168.4.x` address, and report
   `Registered with master: http://192.168.4.1/internal/register`.
4. With safe low-voltage indicators, verify commands and state updates from
   both phone networks. Disconnect/restore the router and verify node recovery,
   commands and timers through the hotspot; also restart a node and confirm
   it rejoins without router Wi-Fi. Test OTA from the hotspot separately.

Uploading/restarting the master turns its three local relays OFF and clears
their timers, as described above. No GPIO or private credential changes are
required for this network migration.

### Node registration troubleshooting

The node's startup log identifies the master-hotspot SSID, local IP, gateway,
signal strength and the master address it will use. A registration failure
also prints the full destination URL and HTTP-client error text.

* **`-1 (connection failed)`** means the TCP connection could not be established;
  it is not a password rejection. Confirm that the master is powered and its
  hotspot log reports `192.168.4.1`, then join `Home-Automation` on a phone and
  open `http://192.168.4.1/`. Check matching hotspot settings, range, power and
  available hotspot client slots. Router isolation is not in this node path.
* **HTTP 401/403** means a server answered but authentication or the request
  header was rejected. Check matching node credentials and firmware/library
  versions. **HTTP 400** indicates rejected registration fields.
* **`-11 (read Timeout)`** means registration did not receive complete response
  headers in time, not that Wi-Fi disconnected. Previously both master-to-node
  commands and node-to-master registration waited synchronously, so simultaneous
  requests could block each other until a timeout. Nodes now poll registration
  response bytes from their loop, allowing incoming commands, LEDs, timers and
  OTA to keep running while the master is busy. TCP connection/request writes
  can still briefly block; the response deadline is five seconds.
* A PC VPN may route `192.168.31.x` away from Wi-Fi even while the PC is joined
  to the router. A browser/PC timeout alone does not prove the ESP32 is down;
  compare with a phone on that Wi-Fi with mobile data disabled.

Nodes wait for the previous Wi-Fi association to disconnect before starting
the next attempt. Idle heartbeats and failed-request retries use a four-second
interval on the same hotspot. Room relay changes request a status report on the
next loop instead of waiting for that interval. Only one registration is in
flight at a time; changes during a pending request are coalesced into a fresh
report after it completes.
Neither transport failures nor HTTP rejections switch a node to router Wi-Fi.
After replacing the shared library, re-upload affected node sketches to get
these diagnostics and connection-transition changes. This does not require
changing the relay or IR pins. The master does not need re-uploading for the
registration response-polling fix.

The amp logs each accepted knob-lighting request with ON/OFF, pattern, color and
brightness, and logs rejected requests explicitly. Startup also logs the LED
GPIO/count if initialization succeeds. These confirm firmware settings, not
physical LED output; settings can change while lighting is OFF.

## Arduino IDE OTA updates

1. Install the current `HomeNodeNetwork` library as described above, then
   upload the OTA-enabled sketch **once by USB to each board**. Ensure each
   board connects to the network. The ESP32 partition scheme must have an OTA
   app slot large enough for the new firmware; the ESP8266 also needs enough
   flash space for its running and new images.
2. Connect the computer to `Home-Automation` for node updates. The master can
   also be updated from home Wi-Fi, but node OTA/mDNS is not forwarded between
   the two networks. In Arduino
   IDE select the correct board type and sketch, then select its network port
   under **Tools > Port**:

   | Board | Network port name |
   | --- | --- |
   | Master | `home-master` |
   | Room | `home-room` |
   | Bathroom | `home-bathroom` |
   | Amplifier remote | `home-amp-remote` |

3. Upload and enter that board's `OTA_PASSWORD` when prompted. The board
   restarts on success. Updating a password requires the currently installed
   password for that upload; the new password is used for later uploads.
   This is an IDE network upload, not a firmware-upload page on the dashboard.

The IDE discovers network ports using mDNS; firewall settings or network client
isolation may prevent discovery or uploads. The OTA host and board must be
able to communicate in both directions. Wait for boards to reconnect after
restarting the master or a node. The master's OTA service stays anchored to
its hotspot address rather than restarting on home-Wi-Fi transitions, and
listens on both interfaces. Interrupted updates, unsuitable flash
layouts, or firmware without OTA support can require USB recovery.

On an accepted update, a relay board turns all its outputs off and clears its
timers before transferring the firmware; they remain off if that transfer
fails after starting. A successful reboot also clears timers. The master does
the same for its three local entertainment relays. Its update does not turn
off remote node outputs or their independent timers. Do not rely
on this software behavior for electrical safety: switch off and isolate
hazardous loads before updating or testing.
The IR board also clears its knob LEDs when an OTA transfer starts; they stay
off if the transfer fails, and start off again after a reboot.

The entertainment section provides individual OFF timers for the TV,
amplifier, LED strip and Socket (the wall board's Socket 1). The shared power
button and all-OFF timer control only the TV, amplifier and LED strip: the
socket remains independent. When all three are on the button turns them off;
otherwise it turns them all on.
Tap an appliance's icon to toggle it; the icon highlights when its relay is on.

Enter a whole number of minutes from 1 to 10080. Entertainment timers now run
locally on the master; the socket timer runs on the wall board. An individual
timer can start only while its appliance is ON; the group timer requires at
least one of the TV, amplifier or LED strip to be ON. The dashboard disables
minutes/START while the corresponding devices are OFF, and the master and
wall-board timer endpoints reject OFF-device starts with HTTP 409. Cancellation
remains available for an existing timer even if its devices are OFF.
When upgrading this behavior, replace the installed HomeNodeNetwork library
and re-upload both the master and wall board; the IR and heater nodes are
unchanged. Timers continue
without Wi-Fi, but are cleared if their controlling board restarts. The socket
timer also continues while the master is offline. Setting a device, including changing the
socket's physical switch, cancels its individual timer. All-on/all-off
cancels the all-OFF timer and the TV, amplifier and LED timers; starting or cancelling the all-OFF
timer also clears those three individual timers, as in the original
controller. These group operations do not affect the socket or its timer.
Cancelling an individual timer does not cancel the all-OFF timer. Timer
displays can lag the node by a few seconds. These timers do not control the
heater.

## Amplifier knob lighting

The existing IR ESP8266 controls **one chain of 20 WS2812B RGB LEDs**:
10 around the encoder, followed by 10 around the manual master-volume knob.
Both rings share power, colour, brightness and pattern; the same animation is
repeated on each ring. This is decorative lighting only: neither knob is
connected to the ESP8266 or sensed, and the LEDs do not indicate actual volume.
Knob lighting is independent of the entertainment relays and their timers.

The **Knob lighting** panel is last, below Bedroom controls. Tap the single
on-screen knob to switch both physical rings ON/OFF. The control also supports
keyboard focus and Space. The knob, Pattern dropdown and Colour picker share
one compact row, with the full-width Brightness slider below.
Power and pattern changes apply automatically; colour and brightness changes are
debounced by 300 ms to avoid flooding the ESP8266 while dragging. Only one
lighting request runs at a time; edits made during a request are combined
into the latest settings and sent next. Periodic status refreshes do not
overwrite pending edits. Errors are shown explicitly; change a setting to
retry after checking the connection. Failed requests are not retried endlessly.

Patterns are **Solid, Breathing, Rainbow, Chase, Comet, Colour cycle, Aurora,
Ocean waves, Fire glow, Candlelight, Starlight twinkle and Pendulum**.
Colour cycle slowly changes the whole ring's hue; Aurora blends cool colours;
Ocean waves gives a gentle blue swell. Fire glow and Candlelight provide warm
flickering ambience; Starlight twinkle softly varies individual LEDs; Pendulum
sweeps a highlight back and forth. Rainbow, Colour cycle, Aurora, Ocean waves,
Fire glow and Candlelight choose their own colours, so the colour picker is
disabled in those modes. Other effects use the selected colour. The single
dashboard ring is a colour illustration, not live animation or electrical
feedback.

The IR board's private `config.h` now contains:

```cpp
#define KNOB_LED_PIN 5
#define KNOB_LEDS_PER_RING 10
#define KNOB_LED_MAX_BRIGHTNESS 96
```

The brightness slider is 0-100% **of the configured ceiling**, currently
96 out of 255 (about 38% of unrestricted output). Zero brightness is dark
even when Ring power is enabled. The physical LED count must match
`KNOB_LEDS_PER_RING`; changing this count also requires updating the dashboard's
two ten-dot illustrations and count label. Both rings must have equal counts.
Lighting accepts GPIO 4, 5, 12, 13 or 14, excluding the IR driver pin.
Invalid/conflicting LED GPIO settings disable lighting and log an error; IR
can still operate independently. The default LED pin is D1 on the **IR board**,
not the bathroom board.

Settings live in RAM. At firmware startup LEDs are cleared and defaults are
OFF, sea-green, 40% brightness and Solid. They keep the last applied effect
through a Wi-Fi/master outage, but a node reboot resets settings. Frames are
scheduled without animation delays; existing HTTP activity and IR transmission
can briefly pause animation. LED writes and IR sends run sequentially.

### Low-voltage wiring and power

Use an external, regulated **5 V supply with suitable current headroom**.
Twenty conventional WS2812B LEDs can require about **1.2 A at unrestricted
full white**, before controller consumption; a quality 5 V / 2 A supply is a
reasonable starting point for this LED count. Do not rely on the software
brightness ceiling as electrical overcurrent protection.

* With power disconnected, connect GPIO5/D1 through a suitable **3.3 V to
  5 V logic buffer**, such as a 5 V-powered 74AHCT125, to the first ring's
  **DIN**, with a 330-470 ohm series resistor near that DIN. Wire the buffer's
  enable/direction and decoupling as specified by its manufacturer. Do not
  assume a generic bidirectional I2C level-shifter is suitable.
* Connect the last LED's **DOUT** on ring 1 to **DIN** on ring 2. Follow the
  strip's data arrows. Feed both rings' 5 V and GND from the external supply;
  connect its GND to ESP8266 GND and buffer GND.
* Fit a correctly polarized bulk capacitor, typically 500-1000 uF rated at
  least 6.3 V, across 5 V/GND near the first ring. Keep data wiring short and
  use suitably rated wiring and supply protection.
* Never power LEDs from a GPIO or the ESP8266's 3.3 V pin, and never connect
  5 V to an ESP8266 GPIO. If the board is USB-powered, share **GND only** with
  the LED supply; do not join its 5 V rail to USB power without verifying a
  board-specific, backfeed-safe power arrangement.
* Keep LED hardware insulated from amplifier circuitry and moving knobs.
  Do not tap amplifier internals or connect to its potentiometer/encoder
  terminals for this decorative feature.

For this addition, replace the installed `HomeNodeNetwork` library with the
current copy, install Adafruit NeoPixel, and upload only **Master** and
**AmpRemoteControl** (USB or existing OTA). The relay boards are unchanged.
An older IR sketch still supports the remote but leaves knob controls
unavailable until it is updated. Compile in Arduino IDE, then bench-test the
LED chain on a protected low-voltage supply: check all twelve patterns, matching
rings, colour/brightness limits, IR operation, reset, Wi-Fi recovery and OTA
off behaviour before fitting it.

## Hardware and safety

* Have a qualified electrician select and install properly rated, isolated
  switches, relays/contactors, fusing, protective devices, enclosures and
  independent heater thermostat and thermal cutoff. The 60-minute heater
  timer is only software: resets, crashes, stuck relays or wiring faults can
  defeat it. It is not a safety device. The timer starts on an OFF-to-ON
  transition; repeated ON commands do not extend it.
* Outputs start logically OFF after firmware initialization. MCU boot and reset
  pin levels can still briefly energize relay inputs; use a hardware design
  that keeps loads OFF throughout power-up, reset and failure. Do not rely on
  the firmware to make an unsafe relay circuit safe.
* The wall inputs are **low-voltage, isolated, maintained switch contacts**,
  using internal pull-ups. They must never be connected directly to mains.
  Each physical switch transition toggles its relay, including after a web
  command; its physical position is not an absolute ON/OFF indication. At
  startup the initial switch positions are sampled without toggling outputs.
  After the 50 ms debounce, a room relay change queues an immediate status
  report. The master pushes the change to connected dashboards over WebSocket.
  Update the installed HomeNodeNetwork library and re-upload RoomSwitchControl
  for this reporting change; the master and other nodes need no re-upload.
* GPIO choices must suit the exact board. On ESP8266, GPIO 6–11 are normally
  flash pins, GPIO 1/3 are reserved for serial, and GPIO 0, 2 and 15 affect
  boot mode. On classic ESP32, GPIO 34–39 are input-only and lack internal
  pull-ups. Do not select boot, flash or serial pins without checking the
  board and attached circuits.
* Use an appropriate IR LED driver and supply for the selected LED; do not
  assume an ESP8266 GPIO can directly drive the desired IR LED current. Make
  sure the driver stays off while the GPIO is floating during reset.
* Use relay drivers compatible with 3.3 V GPIO logic, with an independent
  hardware OFF state during startup and reset. Verify that the assumed
  active-low polarity matches the actual relay board.
* A dashboard state is the last commanded relay state, **not** electrical
  feedback; IR reception is not confirmed. Physical switch updates no longer
  wait for the heartbeat, but Wi-Fi latency, a pending registration response,
  or a disconnected dashboard can still delay them. A timed-out command may still have reached
  the node, so check the state before retrying.

## Network security and limitations

The dashboard and nodes use HTTP Basic authentication **without TLS**. Anyone
able to observe local network traffic may capture credentials and commands.
Arduino OTA is password-protected but its firmware transfer is not encrypted or
signed; ESP8266 OTA uses weak MD5-based authentication. The current configs
reuse the node password for OTA, so change them to strong, distinct passwords.
Use only a trusted, isolated network; do not expose these ports to the Internet,
and do not treat this as a secure production control system. The WebSocket on
port 81 sends refresh notifications only; dashboard data is fetched through
the authenticated HTTP endpoint. The original standalone sketch remains
unchanged. The heater's separate 60-minute timeout is independent of the
entertainment timers.

Before any real load is attached, bench-test every relay output with a safe
low-voltage indicator, test switch changes and reset behavior, verify every IR
command, test disconnect/reconnect in both directions, and verify the heater
timeout and independent protection. Replace any credentials already exposed
in older sketches before deploying.
