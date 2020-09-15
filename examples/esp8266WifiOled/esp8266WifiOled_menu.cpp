/*
    The code in this file uses open source libraries provided by thecoderscorner

    DO NOT EDIT THIS FILE, IT WILL BE GENERATED EVERY TIME YOU USE THE UI DESIGNER
    INSTEAD EITHER PUT CODE IN YOUR SKETCH OR CREATE ANOTHER SOURCE FILE.

    All the variables you may need access to are marked extern in this file for easy
    use elsewhere.
*/

#include <Arduino.h>
#include <tcMenu.h>
#include "esp8266WifiOled_menu.h"

// Global variable declarations

const PROGMEM ConnectorLocalInfo applicationInfo = { "ESP8266 Greenhouse", "01b9cb76-c108-4be3-a133-6159f8f1c9c1" };
U8g2GfxMenuConfig gfxConfig;
U8g2MenuRenderer renderer;
ArduinoAnalogDevice analogDevice;
WiFiServer server(3333);

// Global Menu Item declarations

RENDERING_CALLBACK_NAME_INVOKE(fnIpAddressRtCall, ipAddressRenderFn, "IpAddress", -1, NO_CALLBACK)
IpAddressMenuItem menuIpAddress(fnIpAddressRtCall, 10, NULL);
RENDERING_CALLBACK_NAME_INVOKE(fnPwdRtCall, textItemRenderFn, "Pwd", 23, NO_CALLBACK)
TextMenuItem menuPwd(fnPwdRtCall, 12, 15, &menuIpAddress);
RENDERING_CALLBACK_NAME_INVOKE(fnSSIDRtCall, textItemRenderFn, "SSID", 8, NO_CALLBACK)
TextMenuItem menuSSID(fnSSIDRtCall, 11, 15, &menuPwd);
const SubMenuInfo PROGMEM minfoConnectivity = { "Connectivity", 9, 0xFFFF, 0, NO_CALLBACK };
RENDERING_CALLBACK_NAME_INVOKE(fnConnectivityRtCall, backSubItemRenderFn, "Connectivity", -1, NO_CALLBACK)
BackMenuItem menuBackConnectivity(fnConnectivityRtCall, &menuSSID);
SubMenuItem menuConnectivity(&minfoConnectivity, &menuBackConnectivity, NULL);
const BooleanMenuInfo PROGMEM minfoSecretEntry = { "Secret Entry", 13, 0xFFFF, 1, NO_CALLBACK, NAMING_TRUE_FALSE };
BooleanMenuItem menuSecretEntry(&minfoSecretEntry, false, NULL);
const AnyMenuInfo PROGMEM minfoSaveAll = { "Save All", 8, 0xFFFF, 0, onSaveAll };
ActionMenuItem menuSaveAll(&minfoSaveAll, &menuSecretEntry);
const char enumStrWinOpening_0[] PROGMEM  = "NARROW";
const char enumStrWinOpening_1[] PROGMEM  = "WIDE";
const char* const enumStrWinOpening[] PROGMEM  = { enumStrWinOpening_0, enumStrWinOpening_1 };
const EnumMenuInfo PROGMEM minfoWinOpening = { "Win Opening", 7, 6, 1, onWindowOpening, enumStrWinOpening };
EnumMenuItem menuWinOpening(&minfoWinOpening, 0, &menuSaveAll);
const char enumStrHeaterPower_0[] PROGMEM  = "LOW";
const char enumStrHeaterPower_1[] PROGMEM  = "MEDIUM";
const char enumStrHeaterPower_2[] PROGMEM  = "HIGH";
const char* const enumStrHeaterPower[] PROGMEM  = { enumStrHeaterPower_0, enumStrHeaterPower_1, enumStrHeaterPower_2 };
const EnumMenuInfo PROGMEM minfoHeaterPower = { "Heater Power", 6, 4, 2, onHeaterPower, enumStrHeaterPower };
EnumMenuItem menuHeaterPower(&minfoHeaterPower, 0, &menuWinOpening);
const SubMenuInfo PROGMEM minfoSetup = { "Setup", 5, 0xFFFF, 0, NO_CALLBACK };
RENDERING_CALLBACK_NAME_INVOKE(fnSetupRtCall, backSubItemRenderFn, "Setup", -1, NO_CALLBACK)
BackMenuItem menuBackSetup(fnSetupRtCall, &menuHeaterPower);
SubMenuItem menuSetup(&minfoSetup, &menuBackSetup, &menuConnectivity);
const BooleanMenuInfo PROGMEM minfoElectricHeater = { "Electric Heater", 4, 3, 1, onElectricHeater, NAMING_ON_OFF };
BooleanMenuItem menuElectricHeater(&minfoElectricHeater, false, &menuSetup);
const BooleanMenuInfo PROGMEM minfoWindowOpen = { "Window Open", 3, 2, 1, onWindowOpen, NAMING_YES_NO };
BooleanMenuItem menuWindowOpen(&minfoWindowOpen, false, &menuElectricHeater);
const AnalogMenuInfo PROGMEM minfoCucumberTemp = { "Cucumber Temp", 2, 0xFFFF, 255, NO_CALLBACK, -20, 4, "C" };
AnalogMenuItem menuCucumberTemp(&minfoCucumberTemp, 0, &menuWindowOpen);
const AnalogMenuInfo PROGMEM minfoTomatoTemp = { "Tomato Temp", 1, 0xFFFF, 255, NO_CALLBACK, -20, 4, "C" };
AnalogMenuItem menuTomatoTemp(&minfoTomatoTemp, 0, &menuCucumberTemp);


// Set up code

void setupMenu() {
    menuTomatoTemp.setReadOnly(true);
    menuCucumberTemp.setReadOnly(true);
    menuSecretEntry.setVisible(false);
    menuSSID.setLocalOnly(true);
    menuPwd.setLocalOnly(true);
    menuIpAddress.setReadOnly(true);

    prepareBasicU8x8Config(gfxConfig);
    renderer.setGraphicsDevice(&gfx, &gfxConfig);
    switches.initialise(ioUsingArduino(), true);
    switches.addSwitch(13, NULL);
    switches.onRelease(13, [](uint8_t /*key*/, bool held) {
            menuMgr.onMenuSelect(held);
        });
    setupAnalogJoystickEncoder(&analogDevice, A0, [](int val) {
            menuMgr.valueChanged(val);
        });
    menuMgr.initWithoutInput(&renderer, &menuTomatoTemp);
    remoteServer.begin(&server, &applicationInfo);
}
