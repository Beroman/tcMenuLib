/*
    The code in this file uses open source libraries provided by thecoderscorner

    DO NOT EDIT THIS FILE, IT WILL BE GENERATED EVERY TIME YOU USE THE UI DESIGNER
    INSTEAD EITHER PUT CODE IN YOUR SKETCH OR CREATE ANOTHER SOURCE FILE.

    All the variables you may need access to are marked extern in this file for easy
    use elsewhere.
 */

#ifndef MENU_GENERATED_CODE_H
#define MENU_GENERATED_CODE_H

#include <Arduino.h>
#include <tcMenu.h>
#include "tcMenuAdaFruitGfx.h"
#include "EthernetTransport.h"
#include <RemoteConnector.h>
#include <RuntimeMenuItem.h>
#include <ScrollChoiceMenuItem.h>
#include <RemoteMenuItem.h>
#include <EepromAbstractionWire.h>
#include <RemoteAuthentication.h>

// variables we declare that you may need to access
extern const PROGMEM ConnectorLocalInfo applicationInfo;
extern TcMenuRemoteServer remoteServer;
extern Adafruit_ST7735 gfx;
extern AdafruitDrawable gfxDrawable;
extern GraphicsDeviceRenderer renderer;
extern IoAbstractionRef io8574;
extern EthernetServer server;
extern EthernetInitialisation ethernetInitialisation;

// Global Menu Item exports
extern ActionMenuItem menuTakeDisplay;
extern ActionMenuItem menuSaveItem;
extern TextMenuItem menuRomText;
extern ScrollChoiceMenuItem menuRomLocation;
extern ScrollChoiceMenuItem menuRomChoice;
extern BackMenuItem menuBackRomValues;
extern SubMenuItem menuRomValues;
extern EepromAuthenticationInfoMenuItem menuAuthenticator;
extern RemoteMenuItem menuIoTMonitor;
extern IpAddressMenuItem menuIpAddress;
extern BackMenuItem menuBackConnectivity;
extern SubMenuItem menuConnectivity;
extern FloatMenuItem menuVoltA1;
extern FloatMenuItem menuVoltA0;
extern BackMenuItem menuBackStatus;
extern SubMenuItem menuStatus;
extern Rgb32MenuItem menuRGB;
extern BooleanMenuItem menuTempCheck;
extern ActionMenuItem menuHiddenItem;
extern BooleanMenuItem menuSCircuitProtect;
extern BackMenuItem menuBackAdvanced;
extern SubMenuItem menuAdvanced;
extern ActionMenuItem menuSaveAll;
extern BooleanMenuItem menuPwrDelay;
extern BackMenuItem menuBackSettings;
extern SubMenuItem menuSettings;
extern EnumMenuItem menuLimit;
extern AnalogMenuItem menuCurrent;
extern AnalogMenuItem menuVoltage;

// Provide a wrapper to get hold of the root menu item and export setupMenu
inline MenuItem& rootMenuItem() { return menuVoltage; }
void setupMenu();

// Callback functions must always include CALLBACK_FUNCTION after the return type
#define CALLBACK_FUNCTION

int fnRomLocationRtCall(RuntimeMenuItem* item, uint8_t row, RenderFnMode mode, char* buffer, int bufferSize);
void CALLBACK_FUNCTION onCurrentChange(int id);
void CALLBACK_FUNCTION onLimitMode(int id);
void CALLBACK_FUNCTION onRgbChanged(int id);
void CALLBACK_FUNCTION onSaveItem(int id);
void CALLBACK_FUNCTION onSaveRom(int id);
void CALLBACK_FUNCTION onTakeDisplay(int id);
void CALLBACK_FUNCTION onVoltageChange(int id);

#endif // MENU_GENERATED_CODE_H
