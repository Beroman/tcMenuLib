/*
 * Copyright (c) 2018 https://www.thecoderscorner.com (Nutricherry LTD).
 * This product is licensed under an Apache license, see the LICENSE file in the top-level directory.
 */

#ifndef __MENU_MANAGER_H
#define __MENU_MANAGER_H

#include <LiquidCrystalIO.h>
#include <IoAbstraction.h>
#include <tcMenu.h>
#include "MenuItems.h"
#include "BaseRenderers.h"
#include "EepromAbstraction.h"

class MenuManager {
private:
	MenuItem* rootMenu;
	MenuRenderer* renderer;
public:
	/**
	 * Handle the various types of input
	 */
	void initForEncoder(MenuRenderer* renderer, MenuItem* root, uint8_t encoderPinA, uint8_t encoderPinB, uint8_t encoderButton);
	void initForUpDownOk(MenuRenderer* renderer, MenuItem* root, uint8_t upPin, uint8_t downPin, uint8_t okPin);
	void valueChanged(int value);
	void onMenuSelect(bool held);
	void setItemsInCurrentMenu(int size, int offs = 0) { switches.changeEncoderPrecision(size, offs); }
	void changePrecisionForType(MenuItem* item);

	/**
	 * Used during initialisation to load the previously stored state. Only if the magic key matches at location 0.
	 */
	void load(EepromAbstraction& eeprom, uint16_t magicKey = 0xfade);

	/**
	 * Call to save all item values into eeprom. The magic key is saved at location 0 if not already set. This is a
	 * lazy save that reads the eeprom values first, and only saves to eeprom when there are changes.
	 */
	void save(EepromAbstraction& eeprom, uint16_t magicKey = 0xfade);

	/**
	 * Find the menu item that is currently active.
	 */
	MenuItem* findCurrentActive();

	/**
	 * Get the root of all menus, the first menu item basically
	 */
	MenuItem* getRoot() { return rootMenu; }
};

extern MenuManager menuMgr;

#endif // defined header file
