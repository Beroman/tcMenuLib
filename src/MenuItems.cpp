/*
 * Copyright (c) 2018 https://www.thecoderscorner.com (Nutricherry LTD).
 * This product is licensed under an Apache license, see the LICENSE file in the top-level directory.
 */

#include "tcMenu.h"
#include "MenuItems.h"
#include "RuntimeMenuItem.h"

MenuItem::MenuItem(MenuType menuType, const AnyMenuInfo* menuInfo, MenuItem* next) {
	this->flags = 0;
	this->menuType = menuType;
	if(menuInfo != NULL) this->info = menuInfo;
	this->next = next;
    this->setChanged(true); // items always start out needing redrawing.
    this->setVisible(true); // always start out visible.
}

bool MenuItem::isSendRemoteNeeded(uint8_t remoteNo) {
    return bitRead(flags, (remoteNo + (int)MENUITEM_REMOTE_SEND0));
}

void MenuItem::setSendRemoteNeeded(uint8_t remoteNo, bool needed) {
	bitWrite(flags, (remoteNo + (int)MENUITEM_REMOTE_SEND0), (needed && !isLocalOnly()));
}

void MenuItem::setEditing(bool active) {
	bool isEditOnEntry = isEditing();
	bitWrite(flags, MENUITEM_EDITING, active); setChanged(true);
	if (isMenuRuntimeMultiEdit(this) && !active && isEditOnEntry) {
		EditableMultiPartMenuItem<void*>* item = reinterpret_cast<EditableMultiPartMenuItem<void*>*>(this);
		item->stopMultiEdit();
	}
}

void MenuItem::setSendRemoteNeededAll() {
    // make sure local only fields are never marked for sending.
    if(isLocalOnly()) clearSendRemoteNeededAll();

	flags = flags | MENUITEM_ALL_REMOTES;
}

void MenuItem::clearSendRemoteNeededAll() {
	flags = flags & (~MENUITEM_ALL_REMOTES);
}

void MenuItem::triggerCallback() {
	if (isMenuRuntime(this)) {
		return asRuntimeItem(this)->runCallback();
	}
	else {
		MenuCallbackFn fn = get_info_callback(&info->callback);
		if (fn != NULL) fn(getId());
	}
}

uint8_t MenuItem::copyNameToBuffer(char* buf, int offset, int size) {
	if (isMenuRuntime(this)) {
		asRuntimeItem(this)->copyRuntimeName(buf + offset, size - offset);
		return strlen(buf + offset) + offset;
	}
	else {
		const char* name = info->name;
		uint8_t ret = safeProgCpy(buf + offset, name, size - offset);
		return ret + offset;
	}
}

uint16_t MenuItem::getId()
{
	if (isMenuRuntime(this)) {
		return asRuntimeItem(this)->getRuntimeId();
	}
	
	return get_info_uint(&info->id);
}

uint16_t MenuItem::getMaximumValue()
{
	if (isMenuRuntime(this)) {
		return asRuntimeItem(this)->getNumberOfParts();
	}
	else return get_info_uint(&info->maxValue);
}

uint16_t MenuItem::getEepromPosition()
{
	if (isMenuRuntime(this)) return asRuntimeItem(this)->getRuntimeEeprom();
	
	return get_info_uint(&info->eepromAddr);
}

// on avr boards we store all info structures in progmem, so we need this code to
// pull the enum structures out of progmem. Otherwise we just read it out normally

#ifdef __AVR__

void EnumMenuItem::copyEnumStrToBuffer(char* buffer, int size, int idx) {
    char** itemPtr = ((char**)pgm_read_ptr_near(&((EnumMenuInfo*)info)->menuItems) + idx);
    char* itemLoc = (char *)pgm_read_ptr_near(itemPtr);
    strncpy_P(buffer, itemLoc, size);
	buffer[size - 1] = 0;
}

int EnumMenuItem::getLengthOfEnumStr(int idx) {
    char** itemPtr = ((char**)pgm_read_ptr_near(&((EnumMenuInfo*)info)->menuItems) + idx);
    char* itemLoc = (char *)pgm_read_ptr_near(itemPtr);
    return strlen_P(itemLoc);
}

#else 

void EnumMenuItem::copyEnumStrToBuffer(char* buffer, int size, int idx) {
    EnumMenuInfo* enumInfo = (EnumMenuInfo*)info;
    const char * const* choices = enumInfo->menuItems;
    const char * choice = choices[idx];
    strncpy(buffer, choice, size);
	buffer[size - 1] = 0;
}

int EnumMenuItem::getLengthOfEnumStr(int idx) {
    EnumMenuInfo* enumInfo = (EnumMenuInfo*)info;
    const char * const* choices = enumInfo->menuItems;
    const char * choice = choices[idx];
    return strlen(choice);
}

#endif

bool isSame(float d1, float d2) {
	float result = abs(d1 - d2);
	return result < 0.0000001;
}

void AnalogMenuItem::copyValue(char * buffer, uint8_t bufferSize) {
	uint8_t dpNeeded = getDecimalPlacesForDivisor();
	WholeAndFraction wf = getWholeAndFraction();

	buffer[0]=0;
	if(wf.negative) appendChar(buffer, '-', bufferSize);
	fastltoa(buffer, wf.whole, 5, NOT_PADDED, bufferSize);

	if (dpNeeded != 0) {
		appendChar(buffer, '.', sizeof bufferSize);
		fastltoa(buffer, wf.fraction, dpNeeded, '0', bufferSize);
	}
	uint8_t numLen = strlen(buffer);
	if(numLen < bufferSize) copyUnitToBuffer(buffer + numLen, bufferSize - numLen);
}

float AnalogMenuItem::getAsFloatingPointValue() {
	WholeAndFraction wf = getWholeAndFraction();
	serdebugF4("getasF ", wf.whole, wf.fraction, wf.negative);
	float fract = (float(wf.fraction) / float(getActualDecimalDivisor()));
	float whole = (wf.negative) ? -float(wf.whole) : float(wf.whole);
	serdebugF3("whole, fract ", whole, fract);
	return (wf.negative) ? (whole - fract) : (whole + fract);

}

uint16_t AnalogMenuItem::getActualDecimalDivisor() {
	uint16_t divisor = getDivisor();
	if (divisor < 2) return 1;
	return (divisor > 1000) ? 10000 : (divisor > 100) ? 1000 : (divisor > 10) ? 100 : 10;
}

WholeAndFraction AnalogMenuItem::getWholeAndFraction() {
	WholeAndFraction wf;
	int calcVal = int16_t(getCurrentValue()) + getOffset();
	int divisor = getDivisor();

    wf.negative = (calcVal < 0);

	if (divisor < 2) {
		wf.whole = calcVal;
		wf.fraction = 0;
	}
	else {
		wf.whole = abs(calcVal) / int16_t(divisor);
		int fractMax = getActualDecimalDivisor();
		wf.fraction = abs((calcVal % divisor)) * (fractMax / divisor);
	}
	return wf;
}

void AnalogMenuItem::setFromWholeAndFraction(WholeAndFraction wf) {
	int fractMax = getActualDecimalDivisor();
	uint16_t divisor = getDivisor();
	int correctedFraction = wf.fraction / (fractMax / divisor);
	auto val = (wf.whole * getDivisor());
	if(wf.negative) {
        val = -val;
        val = val - correctedFraction;
	}
	else {
	    val = val + correctedFraction;
	}
	setCurrentValue(val - getOffset());

	serdebugF4("setWF ", wf.whole, wf.fraction, getCurrentValue());
}

void AnalogMenuItem::setFromFloatingPointValue(float value) {
	WholeAndFraction wf;
	if(value < 0.0F) {
	    wf.negative = true;
	    value = abs(value);
	}
	wf.whole = value;
	wf.fraction = (value - float(wf.whole)) * getActualDecimalDivisor();

	setFromWholeAndFraction(wf);
}

uint8_t AnalogMenuItem::getDecimalPlacesForDivisor() {
	uint16_t divisor = getDivisor();
	if (divisor < 2) return 0;

	return (divisor > 1000) ? 4 : (divisor > 100) ? 3 : (divisor > 10) ? 2 : 1;
}

void FloatMenuItem::setFloatValue(float newVal, bool silent) {
	if(isSame(newVal, currValue)) return;
	
	this->currValue = newVal;
	setSendRemoteNeededAll();
	setChanged(true);
	if(!silent) triggerCallback();
}

void ValueMenuItem::setCurrentValue(uint16_t val, bool silent) {
	if (val == currentValue || val > getMaximumValue()) {
		return;
	}
	
	setChanged(true);
	setSendRemoteNeededAll();
	currentValue = val;
    if(!silent)	triggerCallback();
}

const char ON_STR[] PGM_TCM   = "ON";
const char OFF_STR[] PGM_TCM  = "OFF";
const char YES_STR[] PGM_TCM  = "YES";
const char NO_STR[] PGM_TCM   = " NO";
const char TRUE_STR[] PGM_TCM = " TRUE";
const char FALSE_STR[] PGM_TCM= "FALSE";

void copyMenuItemNameAndValue(MenuItem* item, char* buffer, size_t bufferSize, char additionalSep) {
    item->copyNameToBuffer(buffer, bufferSize);
    if(additionalSep != 0) appendChar(buffer, additionalSep, bufferSize);
    appendChar(buffer, ' ', bufferSize);

    int pos = strlen(buffer);
    copyMenuItemValue(item, buffer + pos, bufferSize - pos);
}

void copyMenuItemValue(MenuItem* item, char* buffer, size_t bufferSize) {
    buffer[0] = 0;
    if(item->getMenuType() == MENUTYPE_ENUM_VALUE) {
        auto* enItem = reinterpret_cast<EnumMenuItem*>(item);
        enItem->copyEnumStrToBuffer(buffer, bufferSize, enItem->getCurrentValue());
    }
    else if(item->getMenuType() == MENUTYPE_BOOLEAN_VALUE) {
        auto* boolItem = reinterpret_cast<BooleanMenuItem*>(item);
        BooleanNaming naming = boolItem->getBooleanNaming();
        const char* val;
        switch(naming) {
            case NAMING_ON_OFF:
                val = boolItem->getBoolean() ? ON_STR : OFF_STR;
                break;
            case NAMING_YES_NO:
                val = boolItem->getBoolean() ? YES_STR : NO_STR;
                break;
            default:
                val = boolItem->getBoolean() ? TRUE_STR : FALSE_STR;
                break;
        }
        safeProgCpy(buffer, val, bufferSize);
    }
    else if(item->getMenuType() == MENUTYPE_FLOAT_VALUE) {
        auto* flItem = reinterpret_cast<FloatMenuItem*>(item);
        fastftoa(buffer, flItem->getFloatValue(), flItem->getDecimalPlaces(), bufferSize);
    }
    else if(item->getMenuType() == MENUTYPE_INT_VALUE) {
        auto* anItem = reinterpret_cast<AnalogMenuItem*>(item);
        anItem->copyValue(buffer, bufferSize);
    }
    else if(item->getMenuType() == MENUTYPE_ACTION_VALUE || item->getMenuType() == MENUTYPE_SUB_VALUE) {
        appendChar(buffer, '>', bufferSize);
        appendChar(buffer, '>', bufferSize);
    }
    else if(item->getMenuType() == MENUTYPE_BACK_VALUE) {
        buffer[0]=0;
        if(item->isActive()) {
            strncpy(buffer, "[..]", bufferSize);
        }
    }
    else if(item->getMenuType() == MENUTYPE_TITLE_ITEM) {
        buffer[0] = 0;
    }
    else if(isMenuRuntime(item)) {
        auto* rtItem = reinterpret_cast<RuntimeMenuItem*>(item);
        rtItem->copyValue(buffer, bufferSize);
    }
}