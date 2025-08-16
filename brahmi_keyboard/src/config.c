#include <pico/stdlib.h>
#include "tusb.h"
#include "brahmi_keyboard.h"
#include "brahmi_akshara.h"

/*
 English transliteration of characters produced by this layout is given in Harvard-Kyoto latin form
*/
//default layout, swara in numbers, vargiyas in qwertyuiop, avargiya in asdfghjkl and numbers with shift

ModifierKeyboardSubLayout DEFAULT_EMPTYsubLayout = {
  
  .key = {
    [0x00 ... 0x03] = CHARCODE_NONE,
    // 0x04 - 0x28
    [HID_KEY_A] = 0x2f, // ya
    [HID_KEY_S] = 0x30, // ra
    [HID_KEY_D] = 0x32, // la
    [HID_KEY_F] = 0x35, // va
    [HID_KEY_G] = 0x36, // Sa
    [HID_KEY_H] = 0x37, // Sha
    [HID_KEY_J] = 0x38, // sa
    [HID_KEY_K] = 0x39, // ha
    [HID_KEY_L] = 0x33, // La
    [HID_KEY_Z] = 0x0b, // Ru
    [HID_KEY_X] = 0x0c, // LRu 
    [HID_KEY_C] = 0x43, // kAguNita Ru
    [HID_KEY_V] = 0x62, // kAguNita LRu
    [HID_KEY_B] = 0x3d, // dIrgha mAtre
    [HID_KEY_N] = 0x03, // visarga
    [HID_KEY_M] = 0x02, // anuswAra
    [HID_KEY_Q] = CHARCODE_BLOCK_KEY, // double-keyed
    [HID_KEY_W] = CHARCODE_BLOCK_KEY, // double-keyed
    [HID_KEY_E] = CHARCODE_BLOCK_KEY, // double-keyed
    [HID_KEY_R] = CHARCODE_BLOCK_KEY, // double-keyed
    [HID_KEY_T] = CHARCODE_BLOCK_KEY, // double-keyed
    [HID_KEY_Y] = CHARCODE_BLOCK_KEY, // double-keyed
    [HID_KEY_U] = CHARCODE_BLOCK_KEY, // double-keyed
    [HID_KEY_I] = CHARCODE_BLOCK_KEY, // double-keyed
    [HID_KEY_O] = CHARCODE_BLOCK_KEY, // double-keyed
    [HID_KEY_P] = CHARCODE_BLOCK_KEY, // double-keyed
    [HID_KEY_1] = 0x4d, // kAguNita a_
    [HID_KEY_2] = 0x3e, // kAguNita A
    [HID_KEY_3] = 0x3f, // kAguNita i
    [HID_KEY_4] = 0x40, // kAguNita I
    [HID_KEY_5] = 0x41, // kAguNita u
    [HID_KEY_6] = 0x42, // kAguNita U
    [HID_KEY_7] = 0x46, // kAguNita e
    [HID_KEY_8] = 0x47, // kAguNita E
    [HID_KEY_9] = 0x48, // kAguNita ai
    [HID_KEY_0] = 0x4a, // kAguNita o
    // 0x04 - 0x28
    [0x28 ... 0x2c] = CHARCODE_NONE,
    [HID_KEY_MINUS] = 0x4b, // kAguNita O
    [HID_KEY_EQUAL] = 0x4c, // kAguNita au
    [0x2f ... 0xff] = CHARCODE_NONE
  },
  .intervals = 0x02,
  .ranges = {
    [0x00] = {0x04, 0x27},
    [0x01] = {0x2d, 0x2e},
    [0x02 ... 0x0f] = { EMPTY_RANGE, EMPTY_RANGE }
  }
};

ModifierKeyboardSubLayout DEFAULT_CTRLsubLayout = {
  .key = {[0x00 ... 0xff] = CHARCODE_NONE},
  .intervals = 0x00,
  .ranges = {[0x00 ... 0x0f] = { EMPTY_RANGE, EMPTY_RANGE }}
};

ModifierKeyboardSubLayout DEFAULT_SHIFTsubLayout = {

  .key = {
    [0x00 ... 0x1d] = CHARCODE_NONE,
    //0x1e - 0x27
    [HID_KEY_1] = 0x05, // a
    [HID_KEY_2] = 0x06, // A
    [HID_KEY_3] = 0x07, // i
    [HID_KEY_4] = 0x08, // I
    [HID_KEY_5] = 0x09, // u
     [HID_KEY_6] = 0x0a, // U
    [HID_KEY_7] = 0x0e, // e
    [HID_KEY_8] = 0x0f, // E
    [HID_KEY_9] = 0x10, // ai
    [HID_KEY_0] = 0x12, // o
    //0x1e - 0x27
    [0x28 ... 0x2c] = CHARCODE_NONE,
    [HID_KEY_MINUS] = 0x13, // O
    [HID_KEY_EQUAL] = 0x14, // ou
    [0x2f ... 0xff] = CHARCODE_NONE
  },
  .intervals = 0x02,
  .ranges = {
    [0x00] = {0x1e, 0x27},
    [0x01] = {0x2d, 0x2e},
    [0x02 ... 0x07] = { EMPTY_RANGE, EMPTY_RANGE }
  }
};

ModifierKeyboardSubLayout DEFAULT_ALTsubLayout = {

  .key = {
    [0x00 ... 0x1d] = CHARCODE_NONE,
    //0x1e - 0x27
    [HID_KEY_0] = 0x66, // 0
    [HID_KEY_1] = 0x67, // 1
    [HID_KEY_2] = 0x68, // 2
    [HID_KEY_3] = 0x69, // 3
    [HID_KEY_4] = 0x6a, // 4
    [HID_KEY_5] = 0x6b, // 5
    [HID_KEY_6] = 0x6c, // 6
    [HID_KEY_7] = 0x6d, // 7
    [HID_KEY_8] = 0x6e, // 8
    [HID_KEY_9] = 0x6f, // 9
    //0x1e - 0x27
    [0x28 ... 0x2c] = CHARCODE_NONE,
    [HID_KEY_MINUS] = CHARCODE_DANDA, // danda
    [HID_KEY_EQUAL] = CHARCODE_DDANDA, // double danda
    [0x2f ... 0xff] = CHARCODE_NONE
  },
  .intervals = 0x02,
  .ranges = {
    [0x00] = {0x1e, 0x27},
    [0x01] = {0x2d, 0x2e},
    [0x02 ... 0x07] = { EMPTY_RANGE, EMPTY_RANGE }
  }
};

KeyboardLayout DEFAULTlayout = {

  .id = "DEFAULTlayout",

  .subLayoutFor = {
    [MOD_NONE] = &DEFAULT_EMPTYsubLayout,
    [MOD_CTRL] = &DEFAULT_CTRLsubLayout,
    [MOD_SHIFT] = &DEFAULT_SHIFTsubLayout,
    [MOD_ALT] = &DEFAULT_ALTsubLayout,
    [MOD_META] = &DEFAULT_EMPTYsubLayout
  },

  .VARGIYAdoubleKey = {
    {HID_KEY_Q, HID_KEY_W, HID_KEY_E, HID_KEY_R, HID_KEY_T},
    {HID_KEY_Y, HID_KEY_U, HID_KEY_I, HID_KEY_O, HID_KEY_P}
  },

  .supportedScripts = {
    [0x00] = KANNADA,
    [0x01] = TELUGU,
    [0x02] = DEVANAGARI,
    [0x03 ... 0X1f] = NO_SCRIPT
  }
};



//-----------------------------------config data-------------------------------------

const uint8_t VARGIYAdoubleKeyPositionMap[0x05][0x05] = {
  {VYANJANA_KA, VYANJANA_KHA, VYANJANA_GA, VYANJANA_GHA, VYANJANA_GNYA}, // ka kha ga gha Ga
  {VYANJANA_CA, VYANJANA_CHA, VYANJANA_JA, VYANJANA_JHA, VYANJANA_JNYA}, // ca cha ja jha Ja
  {VYANJANA_TTA, VYANJANA_TTHA, VYANJANA_DDA, VYANJANA_DDHA, VYANJANA_NNA}, // Ta Tha Da Dha Na
  {VYANJANA_TA, VYANJANA_THA, VYANJANA_DA, VYANJANA_DHA, VYANJANA_NA}, // ta tha da dha na
  {VYANJANA_PA, VYANJANA_PHA, VYANJANA_BA, VYANJANA_BHA, VYANJANA_MA}  // pa pha ba bha ma
  //return 0xff through function when double key combo its not found
};

uint8_t const NUMKEYwindows[10] = {HID_KEY_KEYPAD_0, HID_KEY_KEYPAD_1, HID_KEY_KEYPAD_2, HID_KEY_KEYPAD_3, HID_KEY_KEYPAD_4, HID_KEY_KEYPAD_5, HID_KEY_KEYPAD_6, HID_KEY_KEYPAD_7, HID_KEY_KEYPAD_8, HID_KEY_KEYPAD_9 };

uint8_t const NUMKEYlinux[16] = { HID_KEY_0, HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4, HID_KEY_5, HID_KEY_6, HID_KEY_7, HID_KEY_8, HID_KEY_9, HID_KEY_A, HID_KEY_B, HID_KEY_C, HID_KEY_D, HID_KEY_E, HID_KEY_F};

uint16_t defined_script[] = {KANNADA, TELUGU, DEVANAGARI, MALAYALAM, TAMIL, BANGLA};



//-----------------------------------GPIO functions-------------------------------------

void BrahmiKeyboardGPIOinit(){
  // gpio 10 to 22, 10 - 15 layout, 16 - 21 language
  for(uint8_t i=10; i<22; i++) { gpio_init(i); gpio_set_dir(i, GPIO_IN); }
  gpio_init(28); gpio_set_dir(28, GPIO_IN);
  return;
}

uint8_t currentInputMode(){
  gpio_pull_up(28);
  return gpio_get(28);
}

//to be extended with gpio
KeyboardLayout* currentKeyboardLayout(void){
  uint8_t gpio_layout_offset = 10;
  uint8_t active_pin_count = 0;
  uint8_t active_pin = 0;
  for(uint8_t i=0;i<6; i++){ gpio_pull_up(gpio_layout_offset +i); }
  for(uint8_t i=0;i<6; i++){
    if(gpio_get(gpio_layout_offset +i) == 0){
      active_pin_count++;
      active_pin = i;
    }
  }
  if(active_pin_count != 1) return &DEFAULTlayout;
  return ((defined_layout[active_pin]!=NULL) ? defined_layout[active_pin] : &DEFAULTlayout);
}
CharacterScript currentCharacterScript(void){
  uint8_t gpio_script_offset = 16;
  uint8_t active_pin_count = 0;
  uint8_t active_pin = 0;
  for(uint8_t i=0;i<6; i++){ gpio_pull_up(gpio_script_offset +i); }
  for(uint8_t i=0;i<6; i++){
    if(gpio_get(gpio_script_offset +i) == 0){
      active_pin_count++;
      active_pin = i;
    }
  }
  if(active_pin_count != 1) return KANNADA;
  return defined_script[active_pin];
}

