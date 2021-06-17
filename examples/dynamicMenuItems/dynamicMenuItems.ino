/**
 * This example shows how to create scroll choices, lists, and dynamically add items to both dialogs and existing menus
 * at runtime. Although it's set up to run on SAMD/MKR there is no reason it could not be easily reconfigured for any
 * other board.
 */

#include "dynamicMenuItems_menu.h"
#include <IoAbstractionWire.h>

// Part of the helper method for dealing with dialogs.
typedef void (*DialogInitialiser)(MenuBasedDialog*);

// we declare a reference to an io expander as our encoder is on I2C
IoAbstractionRef io8574 = ioFrom8574(0x20, 0); // on addr 0x20 and interrupt pin 0

// Pizza toppings that we forward referenced in the scroll choice item, when in RAM or EEPROM then they are declared in
// a flat array based on the item size.
//                            0123456789 0123456789 0123456789 0123456789 0123456789 0123456789
const char* pizzaToppings = {"Peperoni\0 Onions\0   Olives\0   Sweetcorn\0Mushrooms\0Peppers\0  "};

//
// We now create two menu items that we manually add to the oven menu during initialisation. We set the sub menu child
// to be the first of the two menu items, which are provided in a linked list.
//
BooleanMenuInfo minfoOvenFull = { "Start Oven", nextRandomId(), 0xffff, 1, NO_CALLBACK, NAMING_YES_NO};
BooleanMenuItem menuOvenFull(&minfoOvenFull, false, nullptr, false);

AnyMenuInfo minfoOvenPower = { "Start Oven", nextRandomId(), 0xffff, 255, NO_CALLBACK};
ActionMenuItem menuOvenPower(&minfoOvenPower, &menuOvenFull, false);

AnalogMenuInfo minfoOvenTempInfo = { "Oven Temp", nextRandomId(), 0xffff, 255, NO_CALLBACK, 0, 1, "C" };
AnalogMenuItem menuOvenTempItem(&minfoOvenTempInfo, 0, &menuOvenPower, false);

const char* listElements[] = {
        "Beans",
        "Tomatoes",
        "Rice",
        "Pasta",
        "Coke",
        "Oranges",
        "Apples",
};
const int numListItems = 7;

void prepareOvenMenuAtRuntime() {
    // as we declared these two items as not in progmem (last menu item parameter is false), we can adjust these items at runtime.
    minfoOvenPower.maxValue = 300;

    // here we add an entire child menu after the back option on the oven menu
    menuMgr.addMenuAfter(&menuBackOven, &menuOvenTempItem);
}

void setup() {
    Serial.begin(115200);
    Wire.begin();
    setupMenu();
    prepareOvenMenuAtRuntime();
    menuList.setNumberOfRows(numListItems);
}

void loop() {
    taskManager.runLoop();
}

void serialPrintMenuItem(MenuItem* item) {
    char sz[32];
    copyMenuItemNameAndValue(item, sz, sizeof sz);
    Serial.println(sz);
}

void CALLBACK_FUNCTION onStartCooking(int id) {
    Serial.println("We are making pizza!");
    serialPrintMenuItem(&menuPizzaMakerOvenTemp);
    serialPrintMenuItem(&menuPizzaMakerTopping1);
    serialPrintMenuItem(&menuPizzaMakerTopping2);
    serialPrintMenuItem(&menuPizzaMakerTopping3);
}

//
// a small helper that checks if we can use the dialog, and then calls the provided function if we can.
//
void withDialogIfAvailable(DialogInitialiser dlgFn) {
    BaseDialog* dlg = renderer.getDialog();
    if(dlg && !dlg->isInUse() && dlg->isMenuItemBased()) {
        dlgFn((MenuBasedDialog*)dlg);
    }
}

//
// here we create an extra menu item that allows us to have three lines of text in the info dialog, made up of the
// title, the buffer line, and also this line, then the buttons. It uses the showRam function which shows from
// a non constant / PGM string
//
RENDERING_CALLBACK_NAME_INVOKE(fnExtraDialogLine, textItemRenderFn, "Detail: ", -1, NO_CALLBACK)
TextMenuItem secondItem(fnExtraDialogLine, nextRandomId(), 12, nullptr);

void CALLBACK_FUNCTION onDialogInfo(int id) {
    withDialogIfAvailable([](MenuBasedDialog* dlg) {
        // we set it to have only one button, named close.
        dlg->setButtons(BTNTYPE_NONE, BTNTYPE_CLOSE);

        // and now we add an extra item to the dialog
        secondItem.setTextValue("More text!");
        dlg->insertMenuItem(&secondItem);

        // and we tell the rendering framework to draw it as a textual item, without it's name, left justified.
        GridPosition gridPlacement(GridPosition::DRAW_TEXTUAL_ITEM, GridPosition::JUSTIFY_LEFT_VALUE_ONLY, 2);
        renderer.getDisplayPropertiesFactory().addGridPosition(&secondItem, gridPlacement);

        // and lastly we now show the menu and set the first buffer line text too.
        dlg->showRam("Non Pgm Header", true);
        dlg->copyIntoBuffer("line 2");
    });
}

//
// Shows a question dialog that gets the title from const/program memory. Notice that in this case we have a completion handler
// that accepts the final value, so that we could do something with the result.
//
const char pgmQuestionHeader[] PROGMEM = {"Question.."};
taskid_t questionTask = TASKMGR_INVALIDID;
void CALLBACK_FUNCTION onDialogQuestion(int id) {
    withDialogIfAvailable([] (MenuBasedDialog* dlg) {
        dlg->setButtons(BTNTYPE_ACCEPT, BTNTYPE_CANCEL);
        dlg->show(pgmQuestionHeader, true, [](ButtonType btn, void* data) {
            // this is the completion task that runs when the dialog is dismissed.
            Serial.print("Question result was "); Serial.println(btn);
            taskManager.cancelTask(questionTask);
        });

        dlg->copyIntoBuffer("...");
        questionTask = taskManager.scheduleFixedRate(250, [] {
            char sz[10];
            ltoaClrBuff(sz, rand() % 100, 4, '0', sizeof sz);
            renderer.getDialog()->copyIntoBuffer(sz);
        });
    });
}

void CALLBACK_FUNCTION onDialogController(int id) {
    withDialogIfAvailable([](MenuBasedDialog* dlg) {

    });
}

//
// This is called back each time a list item needs to draw data. This is a list that is based on an array of string values.
//
int CALLBACK_FUNCTION fnListRtCall(RuntimeMenuItem* item, uint8_t row, RenderFnMode mode, char* buffer, int bufferSize) {
   switch(mode) {
    case RENDERFN_INVOKE:
        Serial.print("Selected "); Serial.println(row);
        return true;
    case RENDERFN_NAME:
        if(row > 253) {
            // 254 and 255 are reserved for title and back, this is what is rendered in the back item text and also
            // as the list name / title on the parent menu.
            strncpy(buffer, "Food List", bufferSize);
        }
        else {
            // when row is between 0..253 then the name is the item name.
            ltoaClrBuff(buffer, row, 2, NOT_PADDED, bufferSize); // otherwise it's an active row
        }
        return true;
    case RENDERFN_VALUE:
        if(row < numListItems) {
            // if the value is in range we copy the value from our array
            strncpy(buffer, listElements[row], bufferSize);
        }
        else {
            // otherwise just blank it.
            buffer[0] = 0;
        }
        return true;
    case RENDERFN_EEPROM_POS: return 0xffff; // lists are generally not saved to EEPROM
    default: return false;
    }
}
