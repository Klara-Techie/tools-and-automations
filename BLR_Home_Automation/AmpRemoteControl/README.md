# ESP32 / FR6311 receiver controller

This is an **initial, incomplete implementation**. It is not a production-ready
receiver and has not been tested on the hardware. Whole-receiver factory reset
and a channel identification sequence are not implemented.
Do not connect an amplifier or speakers until the serial waveform and the full
audio path have been verified with suitable equipment.

## Arduino IDE

1. Install the Espressif ESP32 board package, ArduinoJson 6.21.5 and Adafruit
   NeoPixel, and install the home automation `HomeNodeNetwork` library.
2. Set `HOME_SSID` and `HOME_PASSWORD` in `config.h` to the home router
   credentials, matching the master. Open `AmpRemoteControl.ino` from this
   folder, select the actual ESP32 board and port, then compile and upload.
3. Check the [FR6311 wiring](#fr6311-control-wiring) and verify the complete
   analog circuit and supply against the datasheet before applying power.
4. The receiver connects to home Wi-Fi automatically. On the same network,
   open `http://192.168.31.201/`. For recovery, open Serial Monitor at 115200
   baud, connect to the `AV-Receiver` access point using its randomly generated
   password printed there, and open `http://192.168.5.1/`.

The receiver first uses DHCP to discover the gateway, subnet and DNS, then
reconnects using the requested fixed address `192.168.31.201`. Reserve that
address for this receiver or exclude it from the router's DHCP pool; automatic
IP conflict detection is not provided. If the discovered subnet does not
contain that address, the home connection cannot be configured. The receiver
access point stays available for setup and recovery; station connections can
briefly interrupt it when the radio changes channel.

The master dashboard links to this receiver using its own `RECEIVER_IP` setting;
keep that setting identical to this sketch's `RECEIVER_IP`. The link requires
the browser and receiver to be reachable over the home router network. The
master's hotspot does not route to the receiver; use home Wi-Fi or connect to
the receiver's recovery hotspot directly. The receiver is an independent web
server, not a proxied master API, and does not register with the master.
The master enables its AVR link only when amplifier power is on and the
receiver's `/api/health` endpoint responds over home Wi-Fi. This is a web
service check, not confirmation of the audio hardware. Navigation uses direct
addresses to avoid an extra redirect; the master button reflects the last
reported status and may briefly remain clickable after a disconnect.
The receiver's **Master dashboard** link uses `MASTER_IP` from its `config.h`;
keep it synchronized with the master configuration. It also requires a route
to the master on the home network.
Moving the firmware to a different ESP32 does not transfer NVS settings or
presets. Export audio settings from the previous receiver before moving, then
restore the backup on the new device; presets must be configured again.
Home Wi-Fi credentials come from the uploaded `config.h`. Backups restore muted.

The receiver page also controls the two decorative WS2812B knob rings. They
start off and their settings are held in RAM only; see the home automation
README for pinout, patterns, supply and wiring. Configure an OTA password in
`config.h`; OTA turns off the rings and requests mute before an update.

Home Wi-Fi credentials are compiled into the firmware from `config.h`; keep
them private and do not commit them. Previously saved home Wi-Fi credentials
are ignored but remain in ESP32 NVS. Flash contents and NVS are not encrypted
unless the device is separately configured with flash encryption. The interface
uses HTTP without transport encryption or individual user accounts; devices
with network access can control the receiver.

### Connection troubleshooting

Open the correct ESP32 serial port at 115200 baud, then press reset to see
startup messages and the receiver access-point password. Do not share that
password when sharing logs. After startup, the serial monitor reports the
access-point address and connected client count, home Wi-Fi status and, when
connected, the home IP. Status is printed when it changes, not periodically.
The web server reports rejected Host headers; open the printed numeric IP
address rather than a hostname.

On the master serial monitor, receiver probe failures report whether connecting,
sending, or receiving the health response failed. If the receiver page opens
directly but AVR control stays offline, check the master's home Wi-Fi
connection, amplifier relay state and these probe messages. Open
`http://192.168.31.201/api/health` on home Wi-Fi; the response should be
`AV-Receiver`. A browser reaching the receiver does not guarantee that the
master can reach it.

Receiver serial logs report accepted audio target changes, including volume
levels and corresponding attenuation, tone levels and dB, knob-light settings,
and preset actions. Failed action requests also log their HTTP status. These are
requested settings, not confirmation that the FR6311 or LEDs responded.

If there is no serial output, check the board and port selection, USB data
cable and power, and any board-specific USB CDC settings. Verify this receiver
sketch was uploaded, then reset the board with the monitor open. Early storage,
audio initialization or access-point errors stop startup and are printed to
serial. The receiver AP is `AV-Receiver` at `http://192.168.5.1/`; connect to
it directly for recovery. On home Wi-Fi, use the home IP reported in the log.
Home credentials must be configured in the receiver's `config.h`; the master
node's settings do not automatically configure the receiver.

The interface uses a 0-60 volume scale across the chip's -99 to 0 dB
attenuation range, rounded to the nearest supported hardware setting. Level 0
is the lowest level, not mute. Each UI step is approximately 1.65 dB; hardware
ramping still uses 1 dB steps. Bass and treble use a 0-14 scale where 7 is
flat, each step corresponds to 2 dB, and the endpoints are -14 and +14 dB.
Fresh settings default to volume 25, bass 7, treble 7, and 0 dB input and
channel output gain. Existing saved settings are not reset to these defaults.

L/R and C/SW/SL/SR are routed from the decoder's six analog outputs through
their respective volume controls. At startup, older saved routing is changed to
the 5.1 multichannel input and REC is disabled. Other routing options are not
available in this configuration. Backup restores also retain 5.1 routing and
leave REC disabled. The receiver starts muted and respects saved mute and volume
settings. Verify the wiring and all six outputs on hardware.

HDMI, coaxial and optical source switching has not been implemented or shown
in the interface. Their digital audio requires suitable external
decoding/conversion to six analog channels; the FR6311 cannot decode those
signals.

The sketch initializes all four formats muted, then ramps up if the saved
state was unmuted. It remembers settings in ESP32 NVS and attempts to save each
accepted change immediately; save errors are displayed and retried. A power
loss during a failed or interrupted flash write can still lose the latest
change. If last-volume restore is disabled, the configured startup volume is
used instead of the previous master level. The saved mute state is restored in
either case. Maximum volume limits attenuation only, not the separate input
and output gains. Changes from multiple browsers are checked against a state
revision. Backups contain audio settings, not presets or Wi-Fi credentials.

New installations start with no presets. To create one, adjust the speaker
levels and L/R tone, open Listening presets, enter a name, and select Create.
The preset saves current channel trims, gains, mutes and L/R tone. Selecting a
saved preset applies it after confirmation; close the section manually.
The displayed selection is local to the current browser page and resets on
reload, while audio settings and presets remain stored on the receiver.
Reapply, update, rename or delete a selected preset. Existing
saved presets, including older starter presets, are retained and can be
deleted individually. Applying one keeps the current master volume and input,
but channel gains and tone can still change loudness. Output-gain increases are preceded by extra attenuation where
possible, followed by a ramp; attenuation cannot exceed the chip's 99 dB
limit. Up to 16 presets can be stored. Presets are software snapshots, not
FR6311 DSP effects.

Volume changes ramp in 1 dB steps. The interval is configurable from 5 to
1000 ms per step (default 20 ms) in the advanced volume settings, and is
saved with the configuration. Existing saved settings and backups without an
interval use the default. A new target replaces the previous target; mute and
a tighter volume limit take effect immediately. Startup and unmuting begin at
-99 dB and ramp to the requested level. Channel displays show the last
requested target and mute status, not a measured audio level. Master and
channel +/- buttons use the configured volume increment. Tone and gain
buttons use the FR6311's 2 dB steps; ramp-interval buttons adjust by 5 ms.
FL/FR and SL/SR are combined by default. If a pair has different levels it
shows Mixed; adjusting the pair sets both to the same level. Use Individual
view for separate channel adjustments. Pair mute mutes both if either is
unmuted, and unmutes both if both were muted. Master mute highlights all
channel mute buttons without changing their individual mute settings.
The ramp runs cooperatively in the main loop, so network or storage activity can delay
steps; it is not a hardware safety interlock. Use an independent amplifier
mute or disconnect the amplifier for safety-critical situations.

## FR6311 control wiring

These are the firmware's intended connections, **not a hardware-verified
pinout**. Verify the exact IC, package orientation and pin numbers against its
datasheet and the board schematic before wiring. GPIO numbers are ESP32 signal
names, not physical header pin numbers.

| ESP32 | FR6311 | Purpose |
| --- | --- | --- |
| GPIO16 | DATA, pin 13 | Serial data from ESP32 |
| GPIO17 | CLOCK, pin 14 | Serial clock from ESP32 |
| GND | DGND, pin 12 | Common signal ground |

With power disconnected, check that the board's digital ground can safely share
the ESP32 ground. Power the ESP32 and FR6311 from appropriate, separately
regulated supplies; do not connect the FR6311 supply to an ESP32 GPIO or its
3.3 V rail. The intended FR6311 supply is 9 V, but its supply pin, voltage,
decoupling and analog connections must be verified for the actual circuit.
Check the FR6311 DATA/CLOCK input voltage thresholds against ESP32 3.3 V
outputs; use a suitable level shifter if required. Do not assume direct
connection is compatible, and avoid driving the IC while it is unpowered.
Verify that GPIO16 and GPIO17 are available on the specific ESP32 module.

## Audio-path test idea

An ESP32-generated test tone and a measured FL output could help check relative
volume and mute behavior. This firmware does not yet generate or measure a
test tone. Start with a low-level external signal generator and oscilloscope or
audio analyzer, with the power amplifier and speakers disconnected. Confirm
the multichannel FL input path, then compare FL output amplitude at several
volume settings and mute. Check the other channels separately; test L/R tone
controls at multiple frequencies.

Do **not** connect an FR6311 audio output directly to an ESP32 ADC pin. The
output may have DC bias or voltage swings outside the ESP32's permitted input
range. A future ESP32 loopback requires a properly designed, verified
AC-coupled, biased, attenuated and protected input, plus a suitable conditioned
test-tone output. ADC calibration, clipping and noise limit measurement
accuracy. On a classic ESP32, use an available ADC1 input for measurements
while Wi-Fi is active; confirm capabilities and pin availability for the
actual module. Such a test cannot replace checking serial waveforms, all
channels and the complete audio circuit.

## Hardware limitations and validation

The FR6311 has no serial acknowledgement or readback. The interface reports
the requested/transmitted settings, not confirmed IC state or measured audio.
Powering or resetting the FR6311 independently of the ESP32 requires restarting
the ESP32 so all four formats are resent. Verify actual rising-edge sampling,
D0-first transmission, latch timing, power-up behavior and all outputs with a
logic analyzer and a suitable test setup before use. The datasheet's format
table assigns ADC attenuation to D6/D7, while its ADC setting table labels
D4/D5; nonzero ADC attenuation is therefore disabled pending verification.
IN4 uses the specific IN4/REC1-off encoding from the datasheet and cannot be
combined with REC enabled; verify that interaction on the actual board.

Tone controls affect L/R only. The chip does not provide a test-tone generator,
meters, DSP modes, decoding, crossover, delays, or room correction. No such
features are represented as available here.

## Host tests

`tests/FR6311CommandsTest.cpp` is a standalone host C++ test for command
encodings and validation. `tests/AudioControlTest.cpp` tests ramp timing,
retargeting, mute and limit priority using a fake transport and clock. For
example, with a C++ compiler installed:

```
g++ -std=c++11 -I. tests\FR6311CommandsTest.cpp FR6311Commands.cpp -o fr6311-test.exe
.\fr6311-test.exe
g++ -std=c++11 -Itests\fakes -I. tests\AudioControlTest.cpp AudioControl.cpp FR6311Commands.cpp -o audio-control-test.exe
.\audio-control-test.exe
```

These tests do not validate electrical timing, analog behavior, or board wiring.
After uploading, verify preset updates and power-cycle persistence on the real
device. Also verify home-network connection, the fixed address, loss and
recovery of home Wi-Fi, and access through the receiver access point. Use a
safe audio test setup because neither hardware path has been validated yet.
