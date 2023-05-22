/*
    The code in this file uses open source libraries provided by thecoderscorner

    DO NOT EDIT THIS FILE, IT WILL BE GENERATED EVERY TIME YOU USE THE UI DESIGNER
    INSTEAD EITHER PUT CODE IN YOUR SKETCH OR CREATE ANOTHER SOURCE FILE.

    All the variables you may need access to are marked extern in this file for easy
    use elsewhere.
 */

#include <tcMenu.h>
#include "esp32s3TftEncoder_menu.h"
#include "../ThemeCoolBlueTraditional.h"
#include <Fonts/OpenSansCyrillicLatin12.h>

// Global variable declarations
const  ConnectorLocalInfo applicationInfo = { "ESP32 S3 Tiny", "c035e186-32cc-45e4-ac28-773f57e108ee" };

TFT_eSPI gfx;
TfteSpiDrawable gfxDrawable(&gfx, 64);
GraphicsDeviceRenderer renderer(30, applicationInfo.name, &gfxDrawable);

// Global Menu Item declarations
const BooleanMenuInfo minfoToGo = { "To go", 5, 0xffff, 1, NO_CALLBACK, NAMING_YES_NO };
BooleanMenuItem menuToGo(&minfoToGo, false, nullptr, INFO_LOCATION_PGM);
const char enumStrFoods_0[] = "Pizza";
const char enumStrFoods_1[] = "Pasta";
const char enumStrFoods_2[] = "Salad";
const char* const enumStrFoods[]  = { enumStrFoods_0, enumStrFoods_1, enumStrFoods_2 };
const EnumMenuInfo minfoFoods = { "Foods", 4, 0xffff, 2, NO_CALLBACK, enumStrFoods };
EnumMenuItem menuFoods(&minfoFoods, 0, &menuToGo, INFO_LOCATION_PGM);
const char enumStrBatteryCondition_0[] = "Bad";
const char enumStrBatteryCondition_1[] = "Normal";
const char enumStrBatteryCondition_2[] = "Good";
const char* const enumStrBatteryCondition[]  = { enumStrBatteryCondition_0, enumStrBatteryCondition_1, enumStrBatteryCondition_2 };
const EnumMenuInfo minfoBatteryCondition = { "Condition", 16, 0xffff, 2, NO_CALLBACK, enumStrBatteryCondition };
EnumMenuItem menuBatteryCondition(&minfoBatteryCondition, 0, nullptr, INFO_LOCATION_PGM);
const AnalogMenuInfo minfoBatteryCharge = { "Charge", 15, 0xffff, 100, NO_CALLBACK, 0, 1, "%" };
AnalogMenuItem menuBatteryCharge(&minfoBatteryCharge, 0, &menuBatteryCondition, INFO_LOCATION_PGM);
const SubMenuInfo minfoBattery = { "Battery", 14, 0xffff, 0, NO_CALLBACK };
BackMenuItem menuBackBattery(&minfoBattery, &menuBatteryCharge, INFO_LOCATION_PGM);
SubMenuItem menuBattery(&minfoBattery, &menuBackBattery, &menuFoods, INFO_LOCATION_PGM);
const BooleanMenuInfo minfoMute = { "Mute", 13, 0xffff, 1, NO_CALLBACK, NAMING_TRUE_FALSE };
BooleanMenuItem menuMute(&minfoMute, false, &menuBattery, INFO_LOCATION_PGM);
const AnyMenuInfo minfoSettingsLgeNum1 = { "Lge Num1", 12, 0xffff, 0, NO_CALLBACK };
EditableLargeNumberMenuItem menuSettingsLgeNum1(&minfoSettingsLgeNum1, LargeFixedNumber(6, 4, 0U, 0U, false), true, nullptr, INFO_LOCATION_PGM);
const AnalogMenuInfo minfoSettingsTenths1 = { "Tenths1", 11, 0xffff, 1000, NO_CALLBACK, 0, 10, "" };
AnalogMenuItem menuSettingsTenths1(&minfoSettingsTenths1, 0, &menuSettingsLgeNum1, INFO_LOCATION_PGM);
const AnalogMenuInfo minfoSettingsPercent1 = { "Percent1", 10, 0xffff, 100, NO_CALLBACK, 0, 1, "%" };
AnalogMenuItem menuSettingsPercent1(&minfoSettingsPercent1, 0, &menuSettingsTenths1, INFO_LOCATION_PGM);
const AnyMenuInfo minfoNewSubMenuDate = { "Date", 9, 0xffff, 0, NO_CALLBACK };
DateFormattedMenuItem menuNewSubMenuDate(&minfoNewSubMenuDate, DateStorage(1, 1, 2020), &menuSettingsPercent1, INFO_LOCATION_PGM);
const AnyMenuInfo minfoNewSubMenuText = { "Text", 8, 0xffff, 0, NO_CALLBACK };
TextMenuItem menuNewSubMenuText(&minfoNewSubMenuText, "", 5, &menuNewSubMenuDate, INFO_LOCATION_PGM);
const AnyMenuInfo minfoNewSubMenuColor = { "Color", 7, 0xffff, 0, NO_CALLBACK };
Rgb32MenuItem menuNewSubMenuColor(&minfoNewSubMenuColor, RgbColor32(0, 0, 0), false, &menuNewSubMenuText, INFO_LOCATION_PGM);
const SubMenuInfo minfoSettings = { "Settings", 6, 0xffff, 0, NO_CALLBACK };
BackMenuItem menuBackSettings(&minfoSettings, &menuNewSubMenuColor, INFO_LOCATION_PGM);
SubMenuItem menuSettings(&minfoSettings, &menuBackSettings, &menuMute, INFO_LOCATION_PGM);

void setupMenu() {
    // First we set up eeprom and authentication (if needed).
    setSizeBasedEEPROMStorageEnabled(false);
    // Now add any readonly, non-remote and visible flags.
    menuBatteryCondition.setReadOnly(true);
    menuBatteryCharge.setReadOnly(true);

    // Code generated by plugins.
    gfx.begin();
    gfx.setRotation(1);
    renderer.setUpdatesPerSecond(15);
    switches.init(internalDigitalIo(), SWITCHES_NO_POLLING, true);
    menuMgr.initForEncoder(&renderer, &menuSettings, 3, 4, 5);
    renderer.setTitleMode(BaseGraphicalRenderer::TITLE_FIRST_ROW);
    renderer.setUseSliderForAnalog(true);
    renderer.enableTcUnicode();
    installCoolBlueTraditionalTheme(renderer, MenuFontDef(&OpenSansCyrillicLatin12, 0), MenuFontDef(&OpenSansCyrillicLatin12, 0), true);
}

