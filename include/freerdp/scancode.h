/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 * RDP protocol "scancodes"
 *
 * Copyright 2009-2012 Marc-Andre Moreau <marcandre.moreau@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FREERDP_LOCALE_KEYBOARD_RDP_SCANCODE_H
#define FREERDP_LOCALE_KEYBOARD_RDP_SCANCODE_H

/* @msdn{cc240584} says:
 * "... (a scancode is an 8-bit value specifying a key location on the keyboard).
 * The server accepts a scancode value and translates it into the correct character depending on the language locale and keyboard layout used in the session."
 * The 8-bit value is later called "keyCode"
 * The extended flag is for all practical an important 9th bit with a strange encoding - not just a modifier.
 */

typedef UINT32 RDP_SCANCODE;	/* Our own representation of a RDP protocol scancode */
#define RDP_SCANCODE_CODE(_rdp_scancode) ((BYTE)(_rdp_scancode & 0xFF))
#define RDP_SCANCODE_EXTENDED(_rdp_scancode) (((_rdp_scancode) & 0x100) ? TRUE : FALSE)
#define MAKE_RDP_SCANCODE(_code, _extended) (((_code) & 0xFF) | ((_extended) ? 0x100 : 0))

/* Defines for known RDP_SCANCODE protocol values.
 * Mostly the same as the PKBDLLHOOKSTRUCT scanCode, "A hardware scan code for the key", @msdn{ms644967}.
 * Based @msdn{ms894073} US, @msdn{ms894072} UK, @msdn{ms892472} */

#define RDP_SCANCODE_UNKNOWN       MAKE_RDP_SCANCODE(0x00, FALSE)

#define RDP_SCANCODE_ESCAPE        MAKE_RDP_SCANCODE(0x01, FALSE) /* VK_ESCAPE */
#define RDP_SCANCODE_KEY_1         MAKE_RDP_SCANCODE(0x02, FALSE) /* VK_KEY_1 */
#define RDP_SCANCODE_KEY_2         MAKE_RDP_SCANCODE(0x03, FALSE) /* VK_KEY_2 */
#define RDP_SCANCODE_KEY_3         MAKE_RDP_SCANCODE(0x04, FALSE) /* VK_KEY_3 */
#define RDP_SCANCODE_KEY_4         MAKE_RDP_SCANCODE(0x05, FALSE) /* VK_KEY_4 */
#define RDP_SCANCODE_KEY_5         MAKE_RDP_SCANCODE(0x06, FALSE) /* VK_KEY_5 */
#define RDP_SCANCODE_KEY_6         MAKE_RDP_SCANCODE(0x07, FALSE) /* VK_KEY_6 */
#define RDP_SCANCODE_KEY_7         MAKE_RDP_SCANCODE(0x08, FALSE) /* VK_KEY_7 */
#define RDP_SCANCODE_KEY_8         MAKE_RDP_SCANCODE(0x09, FALSE) /* VK_KEY_8 */
#define RDP_SCANCODE_KEY_9         MAKE_RDP_SCANCODE(0x0A, FALSE) /* VK_KEY_9 */
#define RDP_SCANCODE_KEY_0         MAKE_RDP_SCANCODE(0x0B, FALSE) /* VK_KEY_0 */
#define RDP_SCANCODE_OEM_MINUS     MAKE_RDP_SCANCODE(0x0C, FALSE) /* VK_OEM_MINUS */
#define RDP_SCANCODE_OEM_PLUS      MAKE_RDP_SCANCODE(0x0D, FALSE) /* VK_OEM_PLUS */
#define RDP_SCANCODE_BACKSPACE     MAKE_RDP_SCANCODE(0x0E, FALSE) /* VK_BACK Backspace */
#define RDP_SCANCODE_TAB           MAKE_RDP_SCANCODE(0x0F, FALSE) /* VK_TAB */
#define RDP_SCANCODE_KEY_Q         MAKE_RDP_SCANCODE(0x10, FALSE) /* VK_KEY_Q */
#define RDP_SCANCODE_KEY_W         MAKE_RDP_SCANCODE(0x11, FALSE) /* VK_KEY_W */
#define RDP_SCANCODE_KEY_E         MAKE_RDP_SCANCODE(0x12, FALSE) /* VK_KEY_E */
#define RDP_SCANCODE_KEY_R         MAKE_RDP_SCANCODE(0x13, FALSE) /* VK_KEY_R */
#define RDP_SCANCODE_KEY_T         MAKE_RDP_SCANCODE(0x14, FALSE) /* VK_KEY_T */
#define RDP_SCANCODE_KEY_Y         MAKE_RDP_SCANCODE(0x15, FALSE) /* VK_KEY_Y */
#define RDP_SCANCODE_KEY_U         MAKE_RDP_SCANCODE(0x16, FALSE) /* VK_KEY_U */
#define RDP_SCANCODE_KEY_I         MAKE_RDP_SCANCODE(0x17, FALSE) /* VK_KEY_I */
#define RDP_SCANCODE_KEY_O         MAKE_RDP_SCANCODE(0x18, FALSE) /* VK_KEY_O */
#define RDP_SCANCODE_KEY_P         MAKE_RDP_SCANCODE(0x19, FALSE) /* VK_KEY_P */
#define RDP_SCANCODE_OEM_4         MAKE_RDP_SCANCODE(0x1A, FALSE) /* VK_OEM_4 '[' on US */
#define RDP_SCANCODE_OEM_6         MAKE_RDP_SCANCODE(0x1B, FALSE) /* VK_OEM_6 ']' on US */
#define RDP_SCANCODE_RETURN        MAKE_RDP_SCANCODE(0x1C, FALSE) /* VK_RETURN Normal Enter */
#define RDP_SCANCODE_LCONTROL      MAKE_RDP_SCANCODE(0x1D, FALSE) /* VK_LCONTROL */
#define RDP_SCANCODE_KEY_A         MAKE_RDP_SCANCODE(0x1E, FALSE) /* VK_KEY_A */
#define RDP_SCANCODE_KEY_S         MAKE_RDP_SCANCODE(0x1F, FALSE) /* VK_KEY_S */
#define RDP_SCANCODE_KEY_D         MAKE_RDP_SCANCODE(0x20, FALSE) /* VK_KEY_D */
#define RDP_SCANCODE_KEY_F         MAKE_RDP_SCANCODE(0x21, FALSE) /* VK_KEY_F */
#define RDP_SCANCODE_KEY_G         MAKE_RDP_SCANCODE(0x22, FALSE) /* VK_KEY_G */
#define RDP_SCANCODE_KEY_H         MAKE_RDP_SCANCODE(0x23, FALSE) /* VK_KEY_H */
#define RDP_SCANCODE_KEY_J         MAKE_RDP_SCANCODE(0x24, FALSE) /* VK_KEY_J */
#define RDP_SCANCODE_KEY_K         MAKE_RDP_SCANCODE(0x25, FALSE) /* VK_KEY_K */
#define RDP_SCANCODE_KEY_L         MAKE_RDP_SCANCODE(0x26, FALSE) /* VK_KEY_L */
#define RDP_SCANCODE_OEM_1         MAKE_RDP_SCANCODE(0x27, FALSE) /* VK_OEM_1 ';' on US */
#define RDP_SCANCODE_OEM_7         MAKE_RDP_SCANCODE(0x28, FALSE) /* VK_OEM_7 "'" on US */
#define RDP_SCANCODE_OEM_3         MAKE_RDP_SCANCODE(0x29, FALSE) /* VK_OEM_3 Top left, '`' on US, JP DBE_SBCSCHAR */
#define RDP_SCANCODE_LSHIFT        MAKE_RDP_SCANCODE(0x2A, FALSE) /* VK_LSHIFT */
#define RDP_SCANCODE_OEM_5         MAKE_RDP_SCANCODE(0x2B, FALSE) /* VK_OEM_5 Next to Enter, '\' on US */
#define RDP_SCANCODE_KEY_Z         MAKE_RDP_SCANCODE(0x2C, FALSE) /* VK_KEY_Z */
#define RDP_SCANCODE_KEY_X         MAKE_RDP_SCANCODE(0x2D, FALSE) /* VK_KEY_X */
#define RDP_SCANCODE_KEY_C         MAKE_RDP_SCANCODE(0x2E, FALSE) /* VK_KEY_C */
#define RDP_SCANCODE_KEY_V         MAKE_RDP_SCANCODE(0x2F, FALSE) /* VK_KEY_V */
#define RDP_SCANCODE_KEY_B         MAKE_RDP_SCANCODE(0x30, FALSE) /* VK_KEY_B */
#define RDP_SCANCODE_KEY_N         MAKE_RDP_SCANCODE(0x31, FALSE) /* VK_KEY_N */
#define RDP_SCANCODE_KEY_M         MAKE_RDP_SCANCODE(0x32, FALSE) /* VK_KEY_M */
#define RDP_SCANCODE_OEM_COMMA     MAKE_RDP_SCANCODE(0x33, FALSE) /* VK_OEM_COMMA */
#define RDP_SCANCODE_OEM_PERIOD    MAKE_RDP_SCANCODE(0x34, FALSE) /* VK_OEM_PERIOD */
#define RDP_SCANCODE_OEM_2         MAKE_RDP_SCANCODE(0x35, FALSE) /* VK_OEM_2 '/' on US */
#define RDP_SCANCODE_RSHIFT        MAKE_RDP_SCANCODE(0x36, FALSE) /* VK_RSHIFT */
#define RDP_SCANCODE_MULTIPLY      MAKE_RDP_SCANCODE(0x37, FALSE) /* VK_MULTIPLY Numerical */
#define RDP_SCANCODE_LMENU         MAKE_RDP_SCANCODE(0x38, FALSE) /* VK_LMENU Left 'Alt' key */
#define RDP_SCANCODE_SPACE         MAKE_RDP_SCANCODE(0x39, FALSE) /* VK_SPACE */
#define RDP_SCANCODE_CAPSLOCK      MAKE_RDP_SCANCODE(0x3A, FALSE) /* VK_CAPITAL 'Caps Lock', JP DBE_ALPHANUMERIC */
#define RDP_SCANCODE_F1            MAKE_RDP_SCANCODE(0x3B, FALSE) /* VK_F1 */
#define RDP_SCANCODE_F2            MAKE_RDP_SCANCODE(0x3C, FALSE) /* VK_F2 */
#define RDP_SCANCODE_F3            MAKE_RDP_SCANCODE(0x3D, FALSE) /* VK_F3 */
#define RDP_SCANCODE_F4            MAKE_RDP_SCANCODE(0x3E, FALSE) /* VK_F4 */
#define RDP_SCANCODE_F5            MAKE_RDP_SCANCODE(0x3F, FALSE) /* VK_F5 */
#define RDP_SCANCODE_F6            MAKE_RDP_SCANCODE(0x40, FALSE) /* VK_F6 */
#define RDP_SCANCODE_F7            MAKE_RDP_SCANCODE(0x41, FALSE) /* VK_F7 */
#define RDP_SCANCODE_F8            MAKE_RDP_SCANCODE(0x42, FALSE) /* VK_F8 */
#define RDP_SCANCODE_F9            MAKE_RDP_SCANCODE(0x43, FALSE) /* VK_F9 */
#define RDP_SCANCODE_F10           MAKE_RDP_SCANCODE(0x44, FALSE) /* VK_F10 */
#define RDP_SCANCODE_NUMLOCK       MAKE_RDP_SCANCODE(0x45, FALSE) /* VK_NUMLOCK */ /* Note: when this seems to appear in PKBDLLHOOKSTRUCT it means Pause which must be sent as Ctrl + NumLock */
#define RDP_SCANCODE_SCROLLLOCK    MAKE_RDP_SCANCODE(0x46, FALSE) /* VK_SCROLL 'Scroll Lock', JP OEM_SCROLL */
#define RDP_SCANCODE_NUMPAD7       MAKE_RDP_SCANCODE(0x47, FALSE) /* VK_NUMPAD7 */
#define RDP_SCANCODE_NUMPAD8       MAKE_RDP_SCANCODE(0x48, FALSE) /* VK_NUMPAD8 */
#define RDP_SCANCODE_NUMPAD9       MAKE_RDP_SCANCODE(0x49, FALSE) /* VK_NUMPAD9 */
#define RDP_SCANCODE_SUBTRACT      MAKE_RDP_SCANCODE(0x4A, FALSE) /* VK_SUBTRACT */
#define RDP_SCANCODE_NUMPAD4       MAKE_RDP_SCANCODE(0x4B, FALSE) /* VK_NUMPAD4 */
#define RDP_SCANCODE_NUMPAD5       MAKE_RDP_SCANCODE(0x4C, FALSE) /* VK_NUMPAD5 */
#define RDP_SCANCODE_NUMPAD6       MAKE_RDP_SCANCODE(0x4D, FALSE) /* VK_NUMPAD6 */
#define RDP_SCANCODE_ADD           MAKE_RDP_SCANCODE(0x4E, FALSE) /* VK_ADD */
#define RDP_SCANCODE_NUMPAD1       MAKE_RDP_SCANCODE(0x4F, FALSE) /* VK_NUMPAD1 */
#define RDP_SCANCODE_NUMPAD2       MAKE_RDP_SCANCODE(0x50, FALSE) /* VK_NUMPAD2 */
#define RDP_SCANCODE_NUMPAD3       MAKE_RDP_SCANCODE(0x51, FALSE) /* VK_NUMPAD3 */
#define RDP_SCANCODE_NUMPAD0       MAKE_RDP_SCANCODE(0x52, FALSE) /* VK_NUMPAD0 */
#define RDP_SCANCODE_DECIMAL       MAKE_RDP_SCANCODE(0x53, FALSE) /* VK_DECIMAL Numerical, '.' on US */
#define RDP_SCANCODE_SYSREQ        MAKE_RDP_SCANCODE(0x54, FALSE) /* Sys Req */
#define RDP_SCANCODE_OEM_102       MAKE_RDP_SCANCODE(0x56, FALSE) /* VK_OEM_102 Lower left '\' on US */
#define RDP_SCANCODE_F11           MAKE_RDP_SCANCODE(0x57, FALSE) /* VK_F11 */
#define RDP_SCANCODE_F12           MAKE_RDP_SCANCODE(0x58, FALSE) /* VK_F12 */
#define RDP_SCANCODE_SLEEP         MAKE_RDP_SCANCODE(0x5F, FALSE) /* VK_SLEEP OEM_8 on FR (undocumented?) */
#define RDP_SCANCODE_ZOOM          MAKE_RDP_SCANCODE(0x62, FALSE) /* VK_ZOOM (undocumented?) */
#define RDP_SCANCODE_HELP          MAKE_RDP_SCANCODE(0x63, FALSE) /* VK_HELP (undocumented?) */

#define RDP_SCANCODE_F13           MAKE_RDP_SCANCODE(0x64, FALSE) /* VK_F13 */ /* JP agree, should 0x7d according to ms894073 */
#define RDP_SCANCODE_F14           MAKE_RDP_SCANCODE(0x65, FALSE) /* VK_F14 */
#define RDP_SCANCODE_F15           MAKE_RDP_SCANCODE(0x66, FALSE) /* VK_F15 */
#define RDP_SCANCODE_F16           MAKE_RDP_SCANCODE(0x67, FALSE) /* VK_F16 */
#define RDP_SCANCODE_F17           MAKE_RDP_SCANCODE(0x68, FALSE) /* VK_F17 */
#define RDP_SCANCODE_F18           MAKE_RDP_SCANCODE(0x69, FALSE) /* VK_F18 */
#define RDP_SCANCODE_F19           MAKE_RDP_SCANCODE(0x6A, FALSE) /* VK_F19 */
#define RDP_SCANCODE_F20           MAKE_RDP_SCANCODE(0x6B, FALSE) /* VK_F20 */
#define RDP_SCANCODE_F21           MAKE_RDP_SCANCODE(0x6C, FALSE) /* VK_F21 */
#define RDP_SCANCODE_F22           MAKE_RDP_SCANCODE(0x6D, FALSE) /* VK_F22 */
#define RDP_SCANCODE_F23           MAKE_RDP_SCANCODE(0x6E, FALSE) /* VK_F23 */ /* JP agree */
#define RDP_SCANCODE_F24           MAKE_RDP_SCANCODE(0x6F, FALSE) /* VK_F24 */ /* 0x87 according to ms894073 */

#define RDP_SCANCODE_HIRAGANA      MAKE_RDP_SCANCODE(0x70, FALSE) /* JP DBE_HIRAGANA */
#define RDP_SCANCODE_HANJA_KANJI   MAKE_RDP_SCANCODE(0x71, FALSE) /* VK_HANJA / VK_KANJI (undocumented?) */
#define RDP_SCANCODE_KANA_HANGUL   MAKE_RDP_SCANCODE(0x72, FALSE) /* VK_KANA / VK_HANGUL (undocumented?) */
#define RDP_SCANCODE_ABNT_C1       MAKE_RDP_SCANCODE(0x73, FALSE) /* VK_ABNT_C1 JP OEM_102 */
#define RDP_SCANCODE_F24_JP        MAKE_RDP_SCANCODE(0x76, FALSE) /* JP F24 */
#define RDP_SCANCODE_CONVERT_JP    MAKE_RDP_SCANCODE(0x79, FALSE) /* JP CONVERT */
#define RDP_SCANCODE_NONCONVERT_JP MAKE_RDP_SCANCODE(0x7B, FALSE) /* JP NONCONVERT */
#define RDP_SCANCODE_TAB_JP        MAKE_RDP_SCANCODE(0x7C, FALSE) /* JP TAB */
#define RDP_SCANCODE_BACKSLASH_JP  MAKE_RDP_SCANCODE(0x7D, FALSE) /* JP OEM_5 ('\') */
#define RDP_SCANCODE_ABNT_C2       MAKE_RDP_SCANCODE(0x7E, FALSE) /* VK_ABNT_C2, JP */
#define RDP_SCANCODE_ABNT_C2       MAKE_RDP_SCANCODE(0x7E, FALSE) /* JP OEM_PA2 */

#define RDP_SCANCODE_RETURN_KP     MAKE_RDP_SCANCODE(0x1C, TRUE)  /* not RDP_SCANCODE_RETURN Numerical Enter */
#define RDP_SCANCODE_RCONTROL      MAKE_RDP_SCANCODE(0x1D, TRUE)  /* VK_RCONTROL */
#define RDP_SCANCODE_DIVIDE        MAKE_RDP_SCANCODE(0x35, TRUE)  /* VK_DIVIDE Numerical */
#define RDP_SCANCODE_PRINTSCREEN   MAKE_RDP_SCANCODE(0x37, TRUE)  /* VK_EXECUTE/VK_PRINT/VK_SNAPSHOT Print Screen */
#define RDP_SCANCODE_RMENU         MAKE_RDP_SCANCODE(0x38, TRUE)  /* VK_RMENU Right 'Alt' / 'Alt Gr' */
#define RDP_SCANCODE_PAUSE         MAKE_RDP_SCANCODE(0x46, TRUE)  /* VK_PAUSE Pause / Break (Slightly special handling) */
#define RDP_SCANCODE_HOME          MAKE_RDP_SCANCODE(0x47, TRUE)  /* VK_HOME */
#define RDP_SCANCODE_UP            MAKE_RDP_SCANCODE(0x48, TRUE)  /* VK_UP */
#define RDP_SCANCODE_PRIOR         MAKE_RDP_SCANCODE(0x49, TRUE)  /* VK_PRIOR 'Page Up' */
#define RDP_SCANCODE_LEFT          MAKE_RDP_SCANCODE(0x4B, TRUE)  /* VK_LEFT */
#define RDP_SCANCODE_RIGHT         MAKE_RDP_SCANCODE(0x4D, TRUE)  /* VK_RIGHT */
#define RDP_SCANCODE_END           MAKE_RDP_SCANCODE(0x4F, TRUE)  /* VK_END */
#define RDP_SCANCODE_DOWN          MAKE_RDP_SCANCODE(0x50, TRUE)  /* VK_DOWN */
#define RDP_SCANCODE_NEXT          MAKE_RDP_SCANCODE(0x51, TRUE)  /* VK_NEXT 'Page Down' */
#define RDP_SCANCODE_INSERT        MAKE_RDP_SCANCODE(0x52, TRUE)  /* VK_INSERT */
#define RDP_SCANCODE_DELETE        MAKE_RDP_SCANCODE(0x53, TRUE)  /* VK_DELETE */
#define RDP_SCANCODE_NULL          MAKE_RDP_SCANCODE(0x54, TRUE)  /* <00> */
#define RDP_SCANCODE_HELP2         MAKE_RDP_SCANCODE(0x56, TRUE)  /* Help - documented, different from VK_HELP */
#define RDP_SCANCODE_LWIN          MAKE_RDP_SCANCODE(0x5B, TRUE)  /* VK_LWIN */
#define RDP_SCANCODE_RWIN          MAKE_RDP_SCANCODE(0x5C, TRUE)  /* VK_RWIN */
#define RDP_SCANCODE_APPS          MAKE_RDP_SCANCODE(0x5D, TRUE)  /* VK_APPS Application */
#define RDP_SCANCODE_POWER_JP      MAKE_RDP_SCANCODE(0x5E, TRUE)  /* JP POWER */
#define RDP_SCANCODE_SLEEP_JP      MAKE_RDP_SCANCODE(0x5F, TRUE)  /* JP SLEEP */

/* _not_ valid scancode, but this is what a windows PKBDLLHOOKSTRUCT for NumLock contains */
#define RDP_SCANCODE_NUMLOCK_EXTENDED  MAKE_RDP_SCANCODE(0x45, TRUE)  /* should be RDP_SCANCODE_NUMLOCK */
#define RDP_SCANCODE_RSHIFT_EXTENDED   MAKE_RDP_SCANCODE(0x36, TRUE)  /* should be RDP_SCANCODE_RSHIFT */

#endif /* FREERDP_LOCALE_KEYBOARD_RDP_SCANCODE_H */
