# Arduino IDE home automation

This is a **prototype, not a verified installation**. It has not been tested
on the boards. Do not connect it to mains equipment until the
firmware and hardware have been tested and a qualified electrician has
designed and checked the installation.

## Boards

| Board | Sketch | Selection |
| --- | --- | --- |
| ESP32 master + TV, amplifier and LED relays | `Master/Master.ino` | ESP32 Dev Module |
| ESP32 receiver and knob LEDs | `AmpRemoteControl/AmpRemoteControl.ino` | ESP32 Dev Module |
| ESP32 fan, light and two sockets | `RoomSwitchControl/RoomSwitchControl.ino` | ESP32 Dev Module |
| ESP8266 water heater | `BathRoomSwitchControl/BathRoomSwitchControl.ino` | Appropriate ESP8266 board |

The active installation uses **four boards**. The separate Wi-Fi power switch
was removed as requested. Each board has its own sketch and configuration.
Relay implementations live in `HomeNodeNetwork`; the master and the retained
standalone entertainment sketch use the same entertainment relay/timer engine.
The receiver has its own web interface and controls the FR6311 directly, not by IR.

## Installation and configuration

1. Install Arduino IDE 2, the ESP32 and ESP8266 board packages, and select the
   correct board and port before each upload. The code targets a classic ESP32
   Dev Module and an ESP8266 NodeMCU-class board.
2. In Library Manager install **WebSockets by Markus Sattler** for the master,
   and **ArduinoJson 6.21.5** plus **Adafruit NeoPixel by Adafruit**
   for the receiver.
3. Copy the `libraries/HomeNodeNetwork` folder into your Arduino sketchbook's
   `libraries` directory, then restart Arduino IDE. The master and all three
   nodes need this library. When upgrading, replace the installed library with
   the current copy and re-upload affected boards.
4. In each of `Master`, `AmpRemoteControl`,
   `RoomSwitchControl`, and `BathRoomSwitchControl`, edit the private `config.h`.
   Open each `.ino` from its own folder. Set matching master-hotspot
   `AP_SSID`, `AP_PASSWORD`, `AP_IP` and `NODE_PASSWORD` in the master,
   room and bathroom files. Set the receiver's home Wi-Fi credentials in its
   own `config.h`, as described in [its README](AmpRemoteControl/README.md). Set a
   dashboard username and password in the master. Set an `OTA_PASSWORD` of at
   least eight characters on each board; the current configs use `NODE_PASSWORD`
   for OTA, but separate strong passwords are recommended. Keep private configs
   out of source control.
5. The current configurations use master `192.168.31.200`, gateway
   `192.168.31.1`, and subnet `255.255.255.0`. Confirm the
   gateway and reserve the master address outside the router's DHCP pool; it
   must not be assigned to another device. When moving from Jio to ACT, check
   the new router's actual gateway/subnet and update only the master's
   `MASTER_IP`, `ROUTER_GATEWAY` and `ROUTER_SUBNET` if needed, and keep the
   receiver's `MASTER_IP` equal to the master's address. Ensure the router
   network does not overlap `192.168.4.0/24`, which the master hotspot uses,
   or `192.168.5.0/24`, which the receiver recovery hotspot uses. Reserve
   `192.168.31.201` for the receiver and keep `RECEIVER_IP` synchronized in
   the master and receiver configurations.
   Router client isolation can prevent access to the master or receiver. It
   does not carry room and bathroom node-to-master communication. Those nodes use the
   master's hotspot and register at `AP_IP` (`192.168.4.1`). The receiver joins
   the home router independently and does not register with the master.
   Legacy router macros can remain in node configs; they are unused.
6. Check the pin plan below against the actual board and hardware. Set relay
   polarity in the master and relay nodes' `config.h` to match their drivers; the defaults
   assume active-low inputs. A GPIO value of `-1` disables that channel.
   Serial Monitor runs at 115200 baud.
7. Compile and upload each board. Open
   `http://192.168.31.200/` from a phone on the router Wi-Fi and sign in with
   the master dashboard credentials.

## Wiring and default pinouts

These are firmware defaults, not a verified wiring diagram. GPIO numbers are
not physical pin numbers; ESP8266 D labels below assume NodeMCU 1.0. Check
the exact board, its `config.h`, and driver compatibility before wiring.
Disconnect power before working on hardware. Have a qualified electrician
design and check any mains installation.

### Master ESP32: entertainment relays

| Function | GPIO | Configuration | Low-voltage connection |
| --- | --- | --- | --- |
| TV | 25 | `LIVING_TV` | Relay driver input |
| Amplifier | 26 | `LIVING_AMP` | Relay driver input |
| LED strip | 27 | `LIVING_LED` | Relay driver input |

The defaults use active-low relay inputs (`LIVING_ACTIVE_LOW`). Use a relay
driver compatible with 3.3 V logic and an independent hardware OFF state;
never connect a relay coil or mains directly to a GPIO.

### Room ESP32: relays and wall switches

| Function | Relay GPIO | Switch GPIO | Configuration |
| --- | --- | --- | --- |
| Fan | 25 | 18 | `WALL_FAN`, `WALL_FAN_SWITCH` |
| Light | 26 | 19 | `WALL_LIGHT`, `WALL_LIGHT_SWITCH` |
| Socket | 27 | 21 | `WALL_SOCKET1`, `WALL_SOCKET1_SWITCH` |
| Socket 2 | 33 | 22 | `WALL_SOCKET2`, `WALL_SOCKET2_SWITCH` |

Relay GPIOs control compatible relay driver inputs, active-low by default
(`WALL_ACTIVE_LOW`). Each switch GPIO uses the internal pull-up and an
isolated, maintained contact to the board's GND. These inputs are low-voltage
only; never connect mains to them.

### Bathroom ESP8266: heater relay

| Function | GPIO | NodeMCU | Configuration | Low-voltage connection |
| --- | --- | --- | --- | --- |
| Water heater | 5 | D1 | `HEATER_RELAY` | Relay driver input |

The default relay polarity is active-low (`HEATER_ACTIVE_LOW`). The heater
requires independent thermostat and thermal protection; its software timer
is not a safety device.

### Receiver ESP32: FR6311 and knob lighting

| Function | GPIO | Configuration | Low-voltage connection |
| --- | --- | --- | --- |
| FR6311 data | 16 | `FR6311Transport.cpp` | FR6311 pin 13 |
| FR6311 clock | 17 | `FR6311Transport.cpp` | FR6311 pin 14 |
| Knob LED data | 18 | `KNOB_LED_PIN` | Logic buffer, then first WS2812B ring DIN |

The two ten-LED rings form one daisy chain. See
[Low-voltage wiring and power](#low-voltage-wiring-and-power) for their
separate supply, shared ground, level shifting and power precautions.
See the receiver README for FR6311 power, grounding and validation precautions.

All relay drivers must match their configured polarity and have an independent
hardware OFF state during startup and reset. `-1` disables a relay channel;
invalid or conflicting configurations can disable outputs and are logged.
The selected pins avoid ESP8266 boot-strapping pins and the ESP32's input-only,
boot-strapping, serial and flash pins, but the exact board and reset behavior
still need verification.

## Always-on master hotspot

The master starts its password-protected `Home-Automation` hotspot immediately
and keeps it running alongside home Wi-Fi (`WIFI_AP_STA`). The room and bathroom nodes
connect **only** to this hotspot, not to the home router. The hotspot allows
eight clients, leaving room for those nodes and phones or an OTA computer.
No router changes, port forwarding, bridging or Internet connection are needed
for room and bathroom commands. The master forwards authenticated commands
using those nodes' hotspot addresses.

* Phone on home Wi-Fi: open `http://192.168.31.200/` (or the configured master IP).
  Choose **AVR control** in the Amplifier tile to open `http://192.168.31.201/`.
  Use **Master dashboard** on the receiver page to return.
  AVR control stays disabled until the amplifier relay is on and the master
  receives a successful health response from the receiver over home Wi-Fi.
  The master checks approximately every two seconds and requires two
  consecutive failed checks before marking a previously online receiver
  offline. Turning the amplifier relay off or losing the master's home Wi-Fi
  disables the link without waiting for failed probes. Network failure or a
  receiver restart can delay the status change. Navigation links use direct
  addresses to avoid an extra redirect. The button reflects the last reported
  status, so a recently disconnected receiver may briefly remain clickable.
  The check confirms the web service is reachable, not that the audio hardware
  is functioning.
* Phone on `Home-Automation`: open `http://192.168.4.1/`, with or without home Wi-Fi.
  If the phone warns that this network has no Internet, choose to stay connected.
  The receiver link requires a route to the home router network and normally
  will not work from the master hotspot; switch the phone to home Wi-Fi.
  For receiver setup or recovery, connect to its `AV-Receiver` hotspot instead.
* Router loss/restoration does not stop the hotspot or clear registered nodes.
  The master retries the router every 30 seconds; nodes retry only the master
  hotspot after a disconnect or a 15-second connection timeout.
* ESP32 AP and station share one radio/channel. Joining a router or a router
  channel change can briefly interrupt hotspot clients; nodes reconnect
  automatically. There is no guarantee of interruption-free Wi-Fi.
* Status updates are pushed to open dashboards; a periodic refresh is also used.
  The existing API `ap` flag means hotspot-only operation (router disconnected),
  not whether the always-on hotspot exists.

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
changing the relay pins. The master does not need re-uploading for the
registration response-polling fix.

The receiver logs LED initialization errors. The interface shows accepted
settings, not physical LED output.

## iPhone Shortcuts and Siri

The master API can be used from an iPhone on the same Wi-Fi without changing
the firmware. For example, create a shortcut named **Turn on the TV**:

1. Connect the iPhone to the home Wi-Fi and open the Shortcuts app.
2. Add a **Text** action containing your dashboard username and password in
   the format `username:password`. Use your own credentials, not those
   literal words.
3. Add **Base64 Encode**, using the text from the previous action as input.
4. Add **Get Contents of URL** and configure:
   * URL: `http://192.168.31.200/api/relay` (or your configured master IP).
   * Method: **POST**.
   * Headers: `X-Requested-With` = `XMLHttpRequest` and `Authorization` =
     `Basic ` followed by the **Base64 Encoded** variable from step 3.
   * Request Body: **Form**, with `id` = `0` and `on` = `1`.
5. Run the shortcut once to test it, then say **“Siri, Turn on the TV.”**
   Duplicate it and change `on` to `0` to turn the TV off.

Use the following IDs with `/api/relay`:

| Device | `id` |
| --- | --- |
| TV | 0 |
| Amplifier | 1 |
| LED strip | 2 |
| Fan | 3 |
| Light | 4 |
| Socket | 5 |
| Socket 2 | 6 |
| Water heater | 7 |

For group control, use `POST /api/all` with the same headers and a form field
`on` set to `1` or `0`. This controls **only the TV, amplifier and LED strip**.
The authenticated `GET /api/state` endpoint returns the current reported
states. A `401` response indicates authentication failed; `403` indicates the
required request header is missing; `409` can indicate a device is offline or
not configured.

HTTP and Base64 do not encrypt credentials. Use only a trusted local network,
do not expose the API to the internet, and do not share shortcuts containing
your password. Test first with a safe load; a reported state is not electrical
feedback.

## Arduino IDE OTA updates

1. Install the current `HomeNodeNetwork` library as described above, then
   upload the OTA-enabled sketch **once by USB to each board**. Ensure each
   board connects to the network. The ESP32 partition scheme must have an OTA
   app slot large enough for the new firmware; the ESP8266 also needs enough
   flash space for its running and new images.
2. Connect the computer to `Home-Automation` for room and bathroom updates.
   Connect to home Wi-Fi or the receiver hotspot for receiver updates. The master can
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
The receiver clears its knob LEDs and requests audio mute when an OTA transfer
starts. This is not a hardware safety interlock; disconnect the amplifier for
safety-critical work.

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
and re-upload both the master and wall board; the receiver and heater nodes are
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

The receiver ESP32 controls **one chain of 20 WS2812B RGB LEDs**:
10 around the encoder, followed by 10 around the manual master-volume knob.
Both rings share power, colour, brightness and pattern. Most patterns repeat
the same animation on each ring; Dual breathing and Colour segments use
different phases and colours. This is decorative lighting only: neither knob is
connected to the ESP32 or sensed, and the LEDs do not indicate actual volume.
Knob lighting is independent of the entertainment relays and their timers.

The **Knob lighting** panel is on the receiver page, below Tone shaping.
Use Ring power to switch both physical rings ON/OFF.
Power and pattern changes apply automatically; colour and brightness changes are
debounced by 300 ms to avoid flooding the ESP32 while dragging. Only one
lighting request runs at a time; edits made during a request are combined
into the latest settings and sent next. Periodic status refreshes do not
overwrite pending edits. Errors are shown explicitly; change a setting to
retry after checking the connection. Failed requests are not retried endlessly.

Patterns are **Solid, Breathing, Rainbow, Chase, Comet, Colour cycle, Aurora,
Ocean waves, Fire glow, Candlelight, Starlight twinkle, Pendulum, Heartbeat,
Lighthouse, Nebula, Dual breathing and Colour segments**.
Colour cycle slowly changes the whole ring's hue; Aurora blends cool colours;
Ocean waves gives a gentle blue swell. Fire glow and Candlelight provide warm
flickering ambience; Starlight twinkle softly varies individual LEDs; Pendulum
sweeps a highlight back and forth. Heartbeat produces a soft double pulse;
Lighthouse rotates two opposing beams; Nebula shimmers in violet and blue.
Dual breathing uses offset blue and violet pulses on the two rings; Colour
segments rotates six coloured segments, offset between the rings.
Rainbow, Colour cycle, Aurora, Ocean waves, Fire glow, Candlelight, Nebula,
Dual breathing and Colour segments choose their own colours, so the colour
picker is disabled in those modes.
Other effects use the selected colour.

The receiver's `config.h` contains:

```cpp
#define KNOB_LED_PIN 18
#define KNOB_LEDS_PER_RING 10
#define KNOB_LED_MAX_BRIGHTNESS 96
```

The brightness slider is 0-100% **of the configured ceiling**, currently
96 out of 255 (about 38% of unrestricted output). Zero brightness is dark
even when Ring power is enabled. The physical LED count must match
`KNOB_LEDS_PER_RING`. Both rings must have equal counts.
Lighting accepts classic ESP32 GPIO 18, 19, 21, 22, 23, 25, 26, 27, 32 or 33.
Invalid LED GPIO settings disable lighting and log an error. Verify the selected
pin against the exact ESP32 module and wiring.

Settings live in RAM. At firmware startup LEDs are cleared and defaults are
OFF, sea-green, 40% brightness and Solid. They keep the last applied effect
through a Wi-Fi outage, but a node reboot resets settings. Frames are
scheduled without animation delays; network and audio activity can briefly
pause animation.

### Low-voltage wiring and power

Use an external, regulated **5 V supply with suitable current headroom**.
Twenty conventional WS2812B LEDs can require about **1.2 A at unrestricted
full white**, before controller consumption; a quality 5 V / 2 A supply is a
reasonable starting point for this LED count. Do not rely on the software
brightness ceiling as electrical overcurrent protection.

* With power disconnected, connect GPIO18 through a suitable **3.3 V to
  5 V logic buffer**, such as a 5 V-powered 74AHCT125, to the first ring's
  **DIN**, with a 330-470 ohm series resistor near that DIN. Wire the buffer's
  enable/direction and decoupling as specified by its manufacturer. Do not
  assume a generic bidirectional I2C level-shifter is suitable.
* Connect the last LED's **DOUT** on ring 1 to **DIN** on ring 2. Follow the
  strip's data arrows. Feed both rings' 5 V and GND from the external supply;
  connect its GND to ESP32 GND and buffer GND.
* Fit a correctly polarized bulk capacitor, typically 500-1000 uF rated at
  least 6.3 V, across 5 V/GND near the first ring. Keep data wiring short and
  use suitably rated wiring and supply protection.
* Never power LEDs from a GPIO or the ESP32's 3.3 V pin, and never connect
  5 V to an ESP32 GPIO. If the board is USB-powered, share **GND only** with
  the LED supply; do not join its 5 V rail to USB power without verifying a
  board-specific, backfeed-safe power arrangement.
* Keep LED hardware insulated from amplifier circuitry and moving knobs.
  Do not tap amplifier internals or connect to its potentiometer/encoder
  terminals for this decorative feature.

Replace the installed `HomeNodeNetwork` library with the current copy and
install Adafruit NeoPixel. After uploading the master and receiver sketches,
bench-test the LED chain on a protected low-voltage supply: check all patterns,
matching rings, colour/brightness limits, reset, Wi-Fi recovery and OTA
off behaviour before fitting it.

