#include "tusb.h"
#include "brahmi_keyboard.h"

//----------------------------------------------------DEBUG_FUNCTIONS----------------------------------------------------------//

inline static void debug_main(const hid_keyboard_report_t *report, const KeyboardLayout *layout, CharacterScript script ){
  int count = 0;
  //--------------------------------------------------DEBUG_START---------------------------------------------------------//
  tud_cdc_write_str("\r\n---------------------\r\n");
  count = sprintf(tempbuf, "current script %04x\r\n", script);
  tud_cdc_write(tempbuf, count);
  count = sprintf(tempbuf, "current layout %s\r\n", layout->id);
  tud_cdc_write(tempbuf, count);
  count = sprintf(tempbuf, "report => mod:%04x keycode:%04x, %04x, %04x, %04x, %04x, %04x\r\n", report->modifier, report->keycode[0], report->keycode[1], report->keycode[2], report->keycode[3], report->keycode[4], report->keycode[5]);
  tud_cdc_write(tempbuf, count);
  tud_cdc_write_flush();
  //---------------------------------------------------DEBUG_END----------------------------------------------------------//
}

inline static void debug_modifierRange(KeyboardReportModifierStatus modifier_status, const KeyboardLayout *layout){
  int count = 0;
  //--------------------------------------------------DEBUG_START---------------------------------------------------------//
  count = sprintf(tempbuf, "\r\nmodifier_status: %04x\r\n", modifier_status);
  tud_cdc_write(tempbuf, count);
  count = sprintf(tempbuf, "intervals: %d\r\n", layout->subLayoutFor[modifier_status]->intervals);
  tud_cdc_write(tempbuf, count);
  for(uint8_t i=1; i<= layout->subLayoutFor[modifier_status]->intervals; i++){
    count = sprintf(tempbuf, "Range %d : %04x %04x\r\n", i, layout->subLayoutFor[modifier_status]->ranges[i-1][0], layout->subLayoutFor[modifier_status]->ranges[i-1][1]);
    tud_cdc_write(tempbuf, count);
  }
  //---------------------------------------------------DEBUG_END----------------------------------------------------------//
}

inline static void debug_passthrough(uint8_t keycount, uint8_t *extracted_report, uint8_t scancode){
  int count = 0;
  //--------------------------------------------------DEBUG_START---------------------------------------------------------//
  count = sprintf(tempbuf, "keycount: %d\r\n", keycount);
  tud_cdc_write(tempbuf, count);
  count = sprintf(tempbuf, "s_report => %04x, %04x, %04x, %04x, %04x, %04x\r\n", extracted_report[0], extracted_report[1], extracted_report[2], extracted_report[3], extracted_report[4], extracted_report[5]);
  tud_cdc_write(tempbuf, count);
  count = sprintf(tempbuf, "scancode not in range: %04x \r\n", scancode);
  tud_cdc_write(tempbuf, count);
  tud_cdc_write_flush();
  //---------------------------------------------------DEBUG_END----------------------------------------------------------//
}

inline static void debug_charcode(uint8_t charcode){
int count = 0;
  //--------------------------------------------------DEBUG_START---------------------------------------------------------//
  count = sprintf(tempbuf, "charcode: %04x\r\n", charcode);
  tud_cdc_write(tempbuf, count);
  tud_cdc_write_flush();
  //---------------------------------------------------DEBUG_END----------------------------------------------------------//
}

inline static void debug_unicode(uint16_t unicode){
  int count = sprintf(tempbuf, "send_unicode: %04x\r\n", unicode);
  tud_cdc_write(tempbuf, count);
  tud_cdc_write_flush();

}

//----------------------------------------------------DEBUG_FUNCTIONS----------------------------------------------------------//


void send_unicode_with_charcode(CharacterScript script, uint8_t charcode){
  if(charcode >= CHARCODE_LIMIT){
    switch(charcode){
      case CHARCODE_DANDA: send_unicode(0x0964); return;
      case CHARCODE_DDANDA: send_unicode(0x0965); return;
      default: return;
    }
  }
        //heart of the logic, a single important line
  uint16_t unicode = (uint16_t)script + (uint16_t)charcode;
  debug_unicode(unicode);
  send_unicode(unicode);
  return;
}

/*
  assumptions based on expected behavior
  ASSMP1: report keycode space is assumed to be ended when zero(null) values start appearing
  ASSMP2: keycodes are assumed to not start repeating again in report when ended once
  zero "intervals" means no keys in that modifier space is defined
*/
bool passthrough(const hid_keyboard_report_t *report, const KeyboardLayout *layout, uint8_t *charcode, uint8_t *keycount_return, uint8_t* keycount_in_range){
  
  KeyboardReportModifierStatus modifier_status = MOD_NONE;
  uint8_t extracted_report[6] = {0};
  uint8_t keycount = 0;

  switch(report->modifier){
    case KEYBOARD_MODIFIER_LEFTCTRL:
    case KEYBOARD_MODIFIER_RIGHTCTRL: modifier_status = MOD_CTRL; break;
    case KEYBOARD_MODIFIER_LEFTSHIFT:
    case KEYBOARD_MODIFIER_RIGHTSHIFT: modifier_status = MOD_SHIFT; break;
    case KEYBOARD_MODIFIER_LEFTALT:
    case KEYBOARD_MODIFIER_RIGHTALT: modifier_status = MOD_ALT; break;
    case KEYBOARD_MODIFIER_LEFTGUI:
    case KEYBOARD_MODIFIER_RIGHTGUI: modifier_status = MOD_META; break;
    default: modifier_status = MOD_NONE;
  }
  debug_modifierRange(modifier_status, layout);

  uint8_t scancode_not_in_range = 0x00;
  uint8_t prev_keycode = SCANCODE_EMPTY;
  uint8_t count_scancode_in_range = 0;
  for(uint8_t i=0; i<6 && report->keycode[i]!= SCANCODE_EMPTY /*ASSMP1*/ ; i++){
    
    if(report->keycode[i] != prev_keycode /*ASSMP2*/) {
      extracted_report[keycount++] = report->keycode[i];
      if(! key_in_range(report->keycode[i], layout->subLayoutFor[modifier_status]))
        scancode_not_in_range = report->keycode[i];
      else
        count_scancode_in_range++;
    }
    
    prev_keycode = report->keycode[i];
  }

  debug_passthrough(keycount, extracted_report, scancode_not_in_range);
  *keycount_return = keycount;
  *keycount_in_range = count_scancode_in_range;

  /* if a key is not in range when keycount not equal to zero
  for a key to be valid it has to have either 1 key with any modifier and 2 keys with MOD_NONE */
  if(scancode_not_in_range != 0x00 || (scancode_not_in_range == 0x00 && keycount == 0)) return true;
  if(keycount >= 3 || (modifier_status != MOD_NONE && keycount==2) ) return true;  

  if(keycount == 1) *charcode = layout->subLayoutFor[modifier_status]->key[extracted_report[0]];
  else *charcode = getVARGIYAdoubleKey(layout, extracted_report[0], extracted_report[1]);
  debug_charcode(*charcode);
  return false;
}

//main control flow skeleton
void processReportForBrahmiKeyboard(const hid_keyboard_report_t *report){

  const KeyboardLayout *layout = currentKeyboardLayout();
  CharacterScript script = currentCharacterScript();
  uint8_t charcode = CHARCODE_BLOCK_KEY;
  uint8_t keycount = 0;
  uint8_t keycount_in_range = 0;

  debug_main(report, layout, script);

  if(passthrough(report, layout, &charcode, &keycount, &keycount_in_range)){
    send_hid_report(report, keycount, keycount_in_range);
    return;
  }

  send_unicode_with_charcode(script, charcode);
  return;
}