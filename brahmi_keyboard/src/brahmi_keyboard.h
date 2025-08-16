/*
  charcode - language independent character code assigned to similar characters in brahmi based scripts
  scancode - unique 8-bit value associated with a key in keyboard
  unicode - unique value assigned to every character in every unique scripts
*/

#include "tusb.h"

#ifndef BRAHMI_KEYBOARD_H_
#define BRAHMI_KEYBOARD_H_

//---------------------------------------------------------------------------
// brahmi_keyboard APIs
//---------------------------------------------------------------------------

// The primary function / API that exposes the brahmi_keyboard functionality
void processReportForBrahmiKeyboard(const hid_keyboard_report_t *report);

// function initialising gpio pins for configuring layout and language
void BrahmiKeyboardGPIOinit(void);


//---------------------------------------------------------------------------
// brahmi based scripts
//---------------------------------------------------------------------------

// "script" or CharacterSript is defined by the unicode of first character of it's unicode block
typedef enum {
  NO_SCRIPT = 0x0000,
  KANNADA = 0x0c80,
  TELUGU = 0x0c00,
  DEVANAGARI = 0x0900,
  MALAYALAM = 0x0d00,
  TAMIL = 0x0b80,
  BANGLA = 0x0980
} CharacterScript;


//---------------------------------------------------------------------------
// Layout Configuration Structures
//---------------------------------------------------------------------------
/*
  converts from keyboard hid scancodes to layout character code (charcode)
  layout manipulation is divided into several parts with and without modifiers, keypresses with different modifier combinations are given separate spaces
  layout is divided into sublayouts which are reusable, each sublayout differs for each modifier state
  Scripts compatible with the layout are included in supported scripts
  double key space is not standardised
  [0x00, 0x7f] is valid character code range , [0x80, 0xff] is used for any workarounds and manipulations
*/

typedef struct {
  uint8_t key[0x100];
  uint8_t intervals;
  uint8_t ranges[0x10][0x02];
} ModifierKeyboardSubLayout;

typedef struct {
  char *id;
  ModifierKeyboardSubLayout* subLayoutFor[0x05];
  uint8_t VARGIYAdoubleKey[0x02][0x05];
  CharacterScript supportedScripts[0x20];
} KeyboardLayout;


//---------------------------------------------------------------------------
// constants used in Layouts
//---------------------------------------------------------------------------

// delay between successive keys
#define KEY_DELAY 0x10

// Defining Modifier key states, If any modifier key is not found or more than one modifier key is found then it is defined as NONE
typedef enum {
  MOD_NONE,
  MOD_CTRL,
  MOD_SHIFT,
  MOD_ALT,
  MOD_META
} KeyboardReportModifierStatus;

/*
  only 7-bit number[128 numbers] is enough to represent all character codes
  but since the variable is declared with uint8_t 8-bit number, we have space for another 128 numbers
  these can be used for any other purposes
  character key limit, maximum possible value of character code that produces valid character
  use 0xff to denote blocking character instead of 0x00, 0x00 may be a valid character
  not in range can be initialised with 0x00, empty character if in range initialise with 0xff
  0xe4 for devanagari danda, 0xe5 for devanagari double danda
  empty range indicator
*/
enum {
  CHARCODE_LIMIT = 0x80,
  CHARCODE_NONE = 0xff,
  CHARCODE_BLOCK_KEY = 0xff,
  CHARCODE_DANDA = 0xe4,
  CHARCODE_DDANDA = 0xe5,
  EMPTY_RANGE = 0xff
};

// empty scancode and invalid scancode
enum {
  SCANCODE_EMPTY = 0x00,
  SCANCODE_INVALID = 0Xff
};


//---------------------------------------------------------------------------------------------
// functions for fetching selected language and layout from GPIO
//---------------------------------------------------------------------------------------------

/*
  current layout function return pointer to selected layout
  current script function return value of selected script
  current selected mode for input in windows or linux
*/
KeyboardLayout *currentKeyboardLayout(void);
CharacterScript currentCharacterScript(void);
uint8_t currentInputMode(void);


//---------------------------------------------------------------------------
// Data used by brahmi_keyboard defined in config.c
//---------------------------------------------------------------------------

// temporary buffer for displaying debug information
extern char tempbuf[0x100];

// keyboard layout array defined in layout.c
extern KeyboardLayout* defined_layout[6];

// numpad number keys for easy access
extern const uint8_t NUMKEYwindows[10];
extern const uint8_t NUMKEYlinux[16];

// used for fetching doubleKey character code value for given layout through scancodes
extern const uint8_t VARGIYAdoubleKeyPositionMap[0x05][0x05];


//---------------------------------------------------------------------------
// utility functions used by brahmi_keyboard defined in utils.c
//---------------------------------------------------------------------------

// checking if pressed key is in brahmi_keyboard character producing range
bool key_in_range (uint8_t scancode, ModifierKeyboardSubLayout* subLayout);

// send unicode function
void send_unicode(uint16_t unicode);

// send HID report
void send_hid_report(const hid_keyboard_report_t *report, uint8_t keycount, uint8_t keycount_in_range);

// since double doubleKey indices are not directly scancode values like other scancode to charcode conversions
uint8_t getVARGIYAdoubleKey (KeyboardLayout const *layout, uint8_t scancode1, uint8_t scancode2);

#endif