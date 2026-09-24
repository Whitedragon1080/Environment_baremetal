/*cat four_sensor_led_test.py
import time
import RPi.GPIO as GPIO
from smbus import SMBus
from rpi_ws281x import PixelStrip, Color


# =========================================================
# WS2812
# =========================================================

LED_COUNT = 64
LED_PIN = 18
LED_BRIGHTNESS = 80

strip = PixelStrip(
    LED_COUNT,
    LED_PIN,
    800000,
    10,
    False,
    LED_BRIGHTNESS,
    0
)

strip.begin()


# =========================================================
# ADS1115
# =========================================================

ADS_ADDR = 0x48

REG_CONVERSION = 0x00
REG_CONFIG = 0x01

bus = SMBus(1)

# =========================================================
# CD74HC4067
# =========================================================

S0 = 17
S1 = 27
S2 = 22
S3 = 23

# EN der vier MUX
# EN = LOW  -> MUX aktiv
# EN = HIGH -> MUX aus

EN1 = 24
EN2 = 25
EN3 = 5
EN4 = 6

MUX_EN = (EN1, EN2, EN3, EN4)

GPIO.setmode(GPIO.BCM)

for pin in (S0, S1, S2, S3):
    GPIO.setup(pin, GPIO.OUT)

for pin in MUX_EN:
    GPIO.setup(pin, GPIO.OUT)

# Beim Start alle MUX ausschalten
for pin in MUX_EN:
    GPIO.output(pin, GPIO.HIGH)


# =========================================================
# Einstellungen
# =========================================================
SENSOR_COUNT = 64
THRESHOLD = 0.10


# =========================================================
# LEDs
# =========================================================

def all_off():
    for i in range(LED_COUNT):
        strip.setPixelColor(i, Color(0, 0, 0))
    strip.show()


def set_led(index, on):
    if on:
        strip.setPixelColor(index, Color(0, 255, 0))
    else:
        strip.setPixelColor(index, Color(0, 0, 0))

    strip.show()


# =========================================================
# MUX Kanal auswählen
# =========================================================

def select_channel(channel):

    GPIO.output(S0, (channel >> 0) & 1)
    GPIO.output(S1, (channel >> 1) & 1)
    GPIO.output(S2, (channel >> 2) & 1)
    GPIO.output(S3, (channel >> 3) & 1)

    time.sleep(0.01)


# =========================================================
# MUX auswählen
# =========================================================

def select_mux(mux):

    # Alle MUX ausschalten
    for pin in MUX_EN:
        GPIO.output(pin, GPIO.HIGH)

    # Nur den gewünschten MUX einschalten
    GPIO.output(MUX_EN[mux], GPIO.LOW)

    time.sleep(0.001)

# =========================================================
# ADS1115 lesen
# =========================================================

def read_adc(adc_channel):

    # ADS1115 Single-Ended:
    # A0 = 0xC383
    # A1 = 0xD383
    # A2 = 0xE383
    # A3 = 0xF383

    config = 0xC383 + (adc_channel << 12)

    bus.write_i2c_block_data(
        ADS_ADDR,
        REG_CONFIG,
        [
            (config >> 8) & 0xFF,
            config & 0xFF
        ]
    )

    deadline = time.monotonic()+0.1

    while True:

        data = bus.read_i2c_block_data(
            ADS_ADDR,
            REG_CONFIG,
            2
        )

        current_config = (data[0] << 8) | data[1]

        if current_config & 0x8000:
            break

        if time.monotonic() > deadline:
            raise TimeoutError("ADS1115 Timeout")

        time.sleep(0.001)

    data = bus.read_i2c_block_data(
        ADS_ADDR,
        REG_CONVERSION,
        2
    )

    raw = (data[0] << 8) | data[1]

    if raw & 0x8000:
        raw -= 65536

    return raw * 0.000125

# =========================================================
# Sensor lesen
# =========================================================

def read_sensor(sensor):

    # Welcher 4067?
    mux = sensor // 16

    # Welcher Kanal innerhalb des 4067?
    channel = sensor % 16

    # MUX einschalten
    select_mux(mux)

    # Kanal auswählen
    select_channel(channel)

    # ADS1115-Eingang entspricht dem MUX
    adc_channel = mux

    # Erste Messung nach Umschaltung verwerfen
    read_adc(adc_channel)

    return read_adc(adc_channel)

# =========================================================
# Start
# =========================================================

all_off()

print()
print("======================================")
print("      64 SENSOR TEST")
print("======================================")
print()

print("MUX 1 -> Sensor 1-16  -> LED 1-16")
print("MUX 2 -> Sensor 17-32 -> LED 17-32")
print("MUX 3 -> Sensor 33-48 -> LED 33-48")
print("MUX 4 -> Sensor 49-64 -> LED 49-64")
print()

print("Alle Magnete entfernen.")
print("Grundwerte werden gemessen...")
print()


# =========================================================
# Grundwerte
# =========================================================

baseline = []

for sensor in range(SENSOR_COUNT):

    values = []

    for _ in range(10):
        values.append(read_sensor(sensor))
        time.sleep(0.02)

    value = sum(values) / len(values)

    baseline.append(value)

    print(
        f"Sensor {sensor + 1}: "
        f"Grundwert = {value:.3f} V"
    )


print()
print(f"Schwelle = +/- {THRESHOLD:.3f} V")
print()
print("Test läuft.")
print("Strg+C zum Beenden.")
print()


# =========================================================
# Hauptprogramm
# =========================================================

try:

    while True:

        for sensor in range(SENSOR_COUNT):

            voltage = read_sensor(sensor)

            difference = voltage - baseline[sensor]

            detected = abs(difference) >= THRESHOLD

            set_led(sensor, detected)

            print(
                f"Sensor {sensor + 1}: "
                f"{voltage:.3f} V | "
                f"Diff {difference:+.3f} V | "
                f"{'MAGNET' if detected else 'leer'}"
            )

        print()

        time.sleep(0.05)


except KeyboardInterrupt:

    print()
    print("Beendet.")


finally:

    all_off()

    # Alle MUX ausschalten
    for pin in MUX_EN:
        GPIO.output(pin, GPIO.HIGH)

    GPIO.cleanup()
    bus.close()
    */