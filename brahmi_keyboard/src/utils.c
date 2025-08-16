#include "tusb.h"
#include "usb_descriptors.h"
#include "brahmi_keyboard.h"


//created for indicating empty report || temporary buffer for debug output
const hid_keyboard_report_t ZERO_REPORT = { 0, 0, { 0}};
char tempbuf[0x100] = {0};

//rows of varga defined in order
inline uint8_t VARGArowKey(uint8_t scancode, KeyboardLayout const *layout){
  for(uint8_t i=0; i<0x05; i++) if(layout->VARGIYAdoubleKey[0][i]==scancode) return i;
  return 0x05;
}
//columns of PRANA and ANUNASIKA defined in order
inline uint8_t PRANAcolumnKey(uint8_t scancode, KeyboardLayout const *layout){
  for(uint8_t i=0; i<0x05; i++) if(layout->VARGIYAdoubleKey[1][i]==scancode) return i;
  return 0x05;
}
//function retrieve double-keys of vargiya function in whatever order keys are pressed
uint8_t getVARGIYAdoubleKey (KeyboardLayout const *layout, uint8_t scancode1, uint8_t scancode2){
  
  const uint8_t VARGAscancode1 = VARGArowKey(scancode1, layout);
  const uint8_t PRANAscancode1 = PRANAcolumnKey(scancode1, layout);
  if(VARGAscancode1 != 0x05){
    const uint8_t PRANAscancode2 = PRANAcolumnKey(scancode2, layout);
    if(PRANAscancode2 != 0x05) return VARGIYAdoubleKeyPositionMap[VARGAscancode1][PRANAscancode2]; else return CHARCODE_BLOCK_KEY;
  } else if(PRANAscancode1 != 0x05){
    const uint8_t VARGAscancode2 = VARGArowKey(scancode2, layout);
    if(VARGAscancode2 != 0x05) return VARGIYAdoubleKeyPositionMap[VARGAscancode2][PRANAscancode1]; else return CHARCODE_BLOCK_KEY;
  }
  return CHARCODE_BLOCK_KEY;
}

//declared as static because these are wrappers of other static functions
static inline void send_hid_report_internal(uint8_t modifier, uint8_t const *keycode){
  tud_hid_keyboard_report(REPORT_ID_KEYBOARD, modifier, (uint8_t*)keycode);
  return;
}

void send_hid_report(const hid_keyboard_report_t *report, uint8_t keycount, uint8_t keycount_in_range){
  //block empty ALT keys, to not let them interfere with the unicode producing ALT keypress simulations
  if( report->modifier & (KEYBOARD_MODIFIER_LEFTALT | KEYBOARD_MODIFIER_RIGHTALT) && keycount == 0 ) return;
  if(report->modifier == MOD_NONE && keycount>2 ) return;
  if(keycount_in_range > 0) return;
  send_hid_report_internal(report->modifier, report->keycode);
  return;
}

static inline void send_unicode_windows(uint16_t unicode){
  static uint8_t keys[16] = {0};
  static uint8_t len = 0;
  static uint8_t keycode[6] = {0};
  static int8_t i=0;
  
  for(i=0; i<len; i++){
    keys[i] = 0;
  }
  len =0;
  while(unicode > 0){
    keys[len++] = unicode % 10;
    unicode = unicode / 10;
  }
  
  //release all currently pressed or held keys
  //can also be sent by sending ZERO modifier and zero_keycode
  send_hid_report_internal(ZERO_REPORT.modifier, ZERO_REPORT.keycode);
  sleep_ms(KEY_DELAY);
  //press LEFTALT
  send_hid_report_internal(KEYBOARD_MODIFIER_LEFTALT, ZERO_REPORT.keycode);
  sleep_ms(KEY_DELAY);
  for (i=len-1;i>=0;i--){
    keycode[0] = NUMKEYwindows[keys[i]];
    
    //press a NUMKEY while holding LEFTALT
    send_hid_report_internal(KEYBOARD_MODIFIER_LEFTALT, keycode);
    sleep_ms(KEY_DELAY);
    //release that NUMKEY while holding LEFTALT
    send_hid_report_internal(KEYBOARD_MODIFIER_LEFTALT, ZERO_REPORT.keycode);
    sleep_ms(KEY_DELAY);
  }
  //release the LEFTALT, release all currently pressed or held keys
  send_hid_report_internal(ZERO_REPORT.modifier, ZERO_REPORT.keycode);
  sleep_ms(KEY_DELAY);
  
  return;
}

static inline void send_unicode_linux(uint16_t unicode){
  static uint8_t keys[16] = {0};
  static uint8_t len = 0;
  static uint8_t keycode[6] = {0};
  static int8_t i=0;
  
  for(i=0; i<len; i++){
    keys[i] = 0;
  }
  len =0;
  while(unicode > 0){
    keys[len++] = unicode % 16;
    unicode = unicode / 16;
  }
  
  //release all currently pressed or held keys
  //can also be sent by sending ZERO modifier and zero_keycode
  send_hid_report_internal(ZERO_REPORT.modifier, ZERO_REPORT.keycode);
  sleep_ms(KEY_DELAY);
  //press CTRL+SHIFT+U
  keycode[0] = HID_KEY_U;
  send_hid_report_internal(KEYBOARD_MODIFIER_LEFTCTRL|KEYBOARD_MODIFIER_LEFTSHIFT, keycode);
  //release CTRL+SHIFT+U
  send_hid_report_internal(ZERO_REPORT.modifier, ZERO_REPORT.keycode);
  sleep_ms(KEY_DELAY);
  for (i=len-1;i>=0;i--){
    keycode[0] = NUMKEYlinux[keys[i]];
    
    //press a UNICODE digit while holding absolutely nothing
    send_hid_report_internal(ZERO_REPORT.modifier, keycode);
    sleep_ms(KEY_DELAY);
    //release that UNICODE digit while holding absolutely nothing
    send_hid_report_internal(ZERO_REPORT.modifier, ZERO_REPORT.keycode);
    sleep_ms(KEY_DELAY);
  }
  //press space
  keycode[0] = HID_KEY_SPACE;
  send_hid_report_internal(ZERO_REPORT.modifier, keycode);
  //release the CTRL+SHIFT, release all currently pressed or held keys
  send_hid_report_internal(ZERO_REPORT.modifier, ZERO_REPORT.keycode);
  sleep_ms(KEY_DELAY);
  
  return;
}

void send_unicode(uint16_t unicode){
  if(currentInputMode() == 1)
    send_unicode_windows(unicode);
  else
    send_unicode_linux(unicode);
  return;
}

inline bool key_in_range (uint8_t scancode, ModifierKeyboardSubLayout* subLayout){
  for (uint8_t i=0; i< subLayout->intervals; i++){
    if(scancode >= subLayout->ranges[i][0] && scancode <= subLayout->ranges[i][1]) return true;
  }
  return false;
}