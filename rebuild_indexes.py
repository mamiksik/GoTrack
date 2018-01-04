#!/usr/bin/env python

from os.path import basename, isdir, isfile, join, dirname, abspath
from jinja2 import Template
from jinja2 import Environment, FileSystemLoader

THIS_DIR = dirname(abspath(__file__))
FRAMEWORK_DIR = "/usr/local/esp-idf"

ESP = [
    join(THIS_DIR, "components"),
    join(THIS_DIR, "build", "include"),
    join(THIS_DIR, "components", "arduino", "cores", "esp32"),
    join(FRAMEWORK_DIR, "components", "aws_iot", "include"),
    join(FRAMEWORK_DIR, "components", "aws_iot",
         "aws-iot-device-sdk-embedded-C", "include"),
    join(FRAMEWORK_DIR, "components", "app_trace", "include"),
    join(FRAMEWORK_DIR, "components", "app_update", "include"),
    join(FRAMEWORK_DIR, "components", "xtensa-debug-module", "include"),
    join(FRAMEWORK_DIR, "components", "bootloader_support", "include"),
    join(FRAMEWORK_DIR, "components",
         "bootloader_support", "include_priv"),
    join(FRAMEWORK_DIR, "components", "bt", "include"),
    join(FRAMEWORK_DIR, "components", "bt", "bluedroid", "api", "include"),
    join(FRAMEWORK_DIR, "components", "coap", "port", "include"),
    join(FRAMEWORK_DIR, "components", "coap", "port", "include", "coap"),
    join(FRAMEWORK_DIR, "components", "coap", "libcoap", "include"),
    join(FRAMEWORK_DIR, "components", "coap",
         "libcoap", "include", "coap"),
    join(FRAMEWORK_DIR, "components", "cxx", "include"),
    join(FRAMEWORK_DIR, "components", "driver", "include"),
    join(FRAMEWORK_DIR, "components", "driver", "include", "driver"),
    join(FRAMEWORK_DIR, "components", "esp32", "include"),
    join(FRAMEWORK_DIR, "components", "ethernet", "include"),
    join(FRAMEWORK_DIR, "components", "expat", "include", "expat"),
    join(FRAMEWORK_DIR, "components", "expat", "port", "include"),
    join(FRAMEWORK_DIR, "components", "fatfs", "src"),
    join(FRAMEWORK_DIR, "components", "freertos", "include"),
    join(FRAMEWORK_DIR, "components", "jsmn", "include"),
    join(FRAMEWORK_DIR, "components", "json", "include"),
    join(FRAMEWORK_DIR, "components", "json", "port", "include"),
    join(FRAMEWORK_DIR, "components", "libsodium", "libsodium", "src",
         "libsodium", "include"),
    join(FRAMEWORK_DIR, "components", "libsodium", "libsodium", "src",
         "libsodium", "include", "sodium"),
    join(FRAMEWORK_DIR, "components", "log", "include"),
    join(FRAMEWORK_DIR, "components", "lwip", "include", "lwip"),
    join(FRAMEWORK_DIR, "components", "lwip", "include", "lwip", "port"),
    join(FRAMEWORK_DIR, "components", "lwip", "include", "lwip", "posix"),
    join(FRAMEWORK_DIR, "components", "lwip", "apps", "ping"),
    join(FRAMEWORK_DIR, "components", "mbedtls", "port", "include"),
    join(FRAMEWORK_DIR, "components", "mbedtls", "include"),
    join(FRAMEWORK_DIR, "components", "mdns", "include"),
    join(FRAMEWORK_DIR, "components", "micro-ecc", "micro-ecc"),
    join(FRAMEWORK_DIR, "components", "newlib", "include"),
    join(FRAMEWORK_DIR, "components", "newlib", "platform_include"),
    join(FRAMEWORK_DIR, "components", "nghttp", "include"),
    join(FRAMEWORK_DIR, "components", "nghttp", "port", "include"),
    join(FRAMEWORK_DIR, "components", "nvs_flash", "include"),
    join(FRAMEWORK_DIR, "components", "openssl", "include"),
    join(FRAMEWORK_DIR, "components", "openssl", "include", "internal"),
    join(FRAMEWORK_DIR, "components", "openssl", "include", "platform"),
    join(FRAMEWORK_DIR, "components", "openssl", "include", "openssl"),
    join(FRAMEWORK_DIR, "components", "sdmmc", "include"),
    join(FRAMEWORK_DIR, "components", "spi_flash", "include"),
    join(FRAMEWORK_DIR, "components", "tcpip_adapter", "include"),
    join(FRAMEWORK_DIR, "components", "soc", "esp32", "include"),
    join(FRAMEWORK_DIR, "components", "soc", "include"),
    join(FRAMEWORK_DIR, "components", "heap", "include"),
    join(FRAMEWORK_DIR, "components", "ulp", "include"),
    join(FRAMEWORK_DIR, "components", "vfs", "include"),
    join(FRAMEWORK_DIR, "components", "wear_levelling", "include"),
    join(FRAMEWORK_DIR, "components", "wpa_supplicant", "include"),
    join(FRAMEWORK_DIR, "components", "wpa_supplicant", "port", "include")
]

LIBS = [
    join(THIS_DIR, "main", "libs", "atoms", "include"),
    join(THIS_DIR, "main", "libs", "MPU", "include"),
    join(THIS_DIR, "main", "libs", "Logging", "include"),
    join(THIS_DIR, "main", "libs", "BMP280"),
    join(THIS_DIR, "main", "libs", "SSD1306"),
    join(THIS_DIR, "main", "libs", "SmartLeds", "src"),
    join(THIS_DIR, "main", "libs", "BLE", "src"),
    join(THIS_DIR, "main", "libs", "base64"),
    join(THIS_DIR, "main", "callbacks"),
    join(THIS_DIR, "main", "sinks"),
    join(THIS_DIR, "main", "IO"),
    join(THIS_DIR, "main", "sensors"),
    join(THIS_DIR, "main", "display"),
    join(THIS_DIR, "main"),
]

ARDUINO_LIBS = [
    join(THIS_DIR, "components", "arduino", "libraries", "FS", "src"),
    join(THIS_DIR, "components", "arduino", "libraries", "SD", "src"),
    join(THIS_DIR, "components", "arduino", "libraries", "SPI", "src"),
    join(THIS_DIR, "components", "arduino", "libraries", "Wire", "src"),
]

j2_env = Environment(loader=FileSystemLoader(THIS_DIR),
                     trim_blocks=True)

j2_env.get_template('CMakeListsPrivate.template.txt').stream(
    project_src_dir=join(THIS_DIR, "main"),
    esp_includes=ESP,
    arduino_includes=ARDUINO_LIBS,
    libs_includes=LIBS
).dump("CMakeListsPrivate.txt")

print("\t+--------------------------------------------+")
print("\t|     CMakeListsPrivate.txt - Generated      |")
print("\t+--------------------------------------------+")

# template = Template("CMakeListPrivate.template.txt")
# print(template.render(project_src_dir="./main/", includes=CPPPATH))
