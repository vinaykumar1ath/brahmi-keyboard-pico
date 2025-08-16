#include <pico/stdlib.h>
#include "tusb.h"
#include "brahmi_keyboard.h"
#include "brahmi_akshara.h"

/* ===== Layout: Layout1 ===== */
ModifierKeyboardSubLayout Layout1_ALTsubLayout = {
  .key = {
    [HID_KEY_0] = SANKYA_0,
    [HID_KEY_1] = SANKYA_1,
    [HID_KEY_2] = SANKYA_2,
    [HID_KEY_3] = SANKYA_3,
    [HID_KEY_4] = SANKYA_4,
    [HID_KEY_5] = SANKYA_5,
    [HID_KEY_6] = SANKYA_6,
    [HID_KEY_7] = SANKYA_7,
    [HID_KEY_8] = SANKYA_8,
    [HID_KEY_9] = SANKYA_9,
    [HID_KEY_MINUS] = CHARCODE_DANDA,
    [HID_KEY_EQUAL] = CHARCODE_DDANDA,
  },
  .intervals = 0x02,
  .ranges = {
    [0x00] = {HID_KEY_1, HID_KEY_0},
    [0x01] = {HID_KEY_MINUS, HID_KEY_EQUAL},
    [0x02 ... 0x0f] = { EMPTY_RANGE, EMPTY_RANGE }
  }
};

ModifierKeyboardSubLayout Layout1_CTRLsubLayout = {
  .key = {
  },
  .intervals = 0x00,
  .ranges = {
    [0x00 ... 0x0f] = { EMPTY_RANGE, EMPTY_RANGE }
  }
};

ModifierKeyboardSubLayout Layout1_SHIFTsubLayout = {
  .key = {
    [HID_KEY_1] = SVARA_A,
    [HID_KEY_2] = SVARA_AA,
    [HID_KEY_3] = SVARA_I,
    [HID_KEY_4] = SVARA_II,
    [HID_KEY_5] = SVARA_U,
    [HID_KEY_6] = SVARA_UU,
    [HID_KEY_7] = SVARA_E,
    [HID_KEY_8] = SVARA_EE,
    [HID_KEY_9] = SVARA_AI,
    [HID_KEY_0] = SVARA_O,
    [HID_KEY_MINUS] = SVARA_OO,
    [HID_KEY_EQUAL] = SVARA_AU,
  },
  .intervals = 0x02,
  .ranges = {
    [0x00] = {HID_KEY_A, HID_KEY_0},
    [0x01] = {HID_KEY_MINUS, HID_KEY_EQUAL},
    [0x02 ... 0x0f] = { EMPTY_RANGE, EMPTY_RANGE }
  }
};

ModifierKeyboardSubLayout Layout1_EMPTYsubLayout = {
  .key = {
    [HID_KEY_A] = VYANJANA_YA,
    [HID_KEY_S] = VYANJANA_RA,
    [HID_KEY_D] = VYANJANA_LA,
    [HID_KEY_F] = VYANJANA_VA,
    [HID_KEY_G] = VYANJANA_SHA,
    [HID_KEY_H] = VYANJANA_SHHA,
    [HID_KEY_J] = VYANJANA_SA,
    [HID_KEY_K] = VYANJANA_HA,
    [HID_KEY_L] = VYANJANA_LLA,
    [HID_KEY_Z] = SVARA_RU,
    [HID_KEY_X] = SVARA_LRU,
    [HID_KEY_C] = KAGUNITA_RU,
    [HID_KEY_V] = KAGUNITA_LRU,
    [HID_KEY_B] = SANNE_DIRGHA,
    [HID_KEY_N] = YOGAVAHA_AM,
    [HID_KEY_M] = YOGAVAHA_AH,
    [HID_KEY_Q] = CHARCODE_BLOCK_KEY,
    [HID_KEY_W] = CHARCODE_BLOCK_KEY,
    [HID_KEY_E] = CHARCODE_BLOCK_KEY,
    [HID_KEY_R] = CHARCODE_BLOCK_KEY,
    [HID_KEY_T] = CHARCODE_BLOCK_KEY,
    [HID_KEY_Y] = CHARCODE_BLOCK_KEY,
    [HID_KEY_U] = CHARCODE_BLOCK_KEY,
    [HID_KEY_I] = CHARCODE_BLOCK_KEY,
    [HID_KEY_O] = CHARCODE_BLOCK_KEY,
    [HID_KEY_P] = CHARCODE_BLOCK_KEY,
    [HID_KEY_1] = KAGUNITA_A,
    [HID_KEY_2] = KAGUNITA_AA,
    [HID_KEY_3] = KAGUNITA_I,
    [HID_KEY_4] = KAGUNITA_II,
    [HID_KEY_5] = KAGUNITA_U,
    [HID_KEY_6] = KAGUNITA_UU,
    [HID_KEY_7] = KAGUNITA_E,
    [HID_KEY_8] = KAGUNITA_EE,
    [HID_KEY_9] = KAGUNITA_AI,
    [HID_KEY_0] = KAGUNITA_O,
    [HID_KEY_MINUS] = KAGUNITA_OO,
    [HID_KEY_EQUAL] = KAGUNITA_AU,
  },
  .intervals = 0x02,
  .ranges = {
    [0x00] = {HID_KEY_A, HID_KEY_0},
    [0x01] = {HID_KEY_MINUS, HID_KEY_EQUAL},
    [0x02 ... 0x0f] = { EMPTY_RANGE, EMPTY_RANGE }
  }
};

KeyboardLayout Layout1 = {
  .id = "Layout1",
  .subLayoutFor = {
    [MOD_NONE] = &Layout1_EMPTYsubLayout,
    [MOD_CTRL] = &Layout1_CTRLsubLayout,
    [MOD_SHIFT] = &Layout1_SHIFTsubLayout,
    [MOD_ALT] = &Layout1_ALTsubLayout,
    [MOD_META] = &Layout1_EMPTYsubLayout
  },
  .VARGIYAdoubleKey = {
    {HID_KEY_Q, HID_KEY_W, HID_KEY_E, HID_KEY_R, HID_KEY_T},
    {HID_KEY_Y, HID_KEY_U, HID_KEY_I, HID_KEY_O, HID_KEY_P},
  },
  .supportedScripts = {
    [0] = KANNADA,
    [1] = TELUGU,
    [2] = DEVANAGARI,
    [3 ... 31] = NO_SCRIPT,
  }
};

KeyboardLayout* defined_layout[6] = {
  &Layout1,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};
