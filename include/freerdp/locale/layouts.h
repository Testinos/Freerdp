/**
 * FreeRDP: A Remote Desktop Protocol Client
 * XKB-based Keyboard Mapping to Microsoft Keyboard System
 *
 * Copyright 2009 Marc-Andre Moreau <marcandre.moreau@gmail.com>
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

/* Keyboard layout IDs used in the RDP protocol */

#ifndef __LAYOUT_IDS_H
#define __LAYOUT_IDS_H

#include <freerdp/api.h>
#include <freerdp/types.h>
#include <freerdp/locale/keyboard.h>

/* Keyboard layout IDs */

#define KBD_ARABIC_101				0x00000401
#define KBD_BULGARIAN				0x00000402
#define KBD_CHINESE_TRADITIONAL_US		0x00000404
#define KBD_CZECH				0x00000405
#define KBD_DANISH				0x00000406
#define KBD_GERMAN				0x00000407
#define KBD_GREEK				0x00000408
#define KBD_US					0x00000409
#define KBD_SPANISH				0x0000040A
#define KBD_FINNISH				0x0000040B
#define KBD_FRENCH				0x0000040C
#define KBD_HEBREW				0x0000040D
#define KBD_HUNGARIAN				0x0000040E
#define KBD_ICELANDIC				0x0000040F
#define KBD_ITALIAN				0x00000410
#define KBD_JAPANESE				0x00000411
#define KBD_KOREAN				0x00000412
#define KBD_DUTCH				0x00000413
#define KBD_NORWEGIAN				0x00000414
#define KBD_POLISH_PROGRAMMERS			0x00000415
#define KBD_PORTUGUESE_BRAZILIAN_ABNT		0x00000416
#define KBD_ROMANIAN				0x00000418
#define KBD_RUSSIAN				0x00000419
#define KBD_CROATIAN				0x0000041A
#define KBD_SLOVAK				0x0000041B
#define KBD_ALBANIAN				0x0000041C
#define KBD_SWEDISH				0x0000041D
#define KBD_THAI_KEDMANEE			0x0000041E
#define KBD_TURKISH_Q				0x0000041F
#define KBD_URDU				0x00000420
#define KBD_UKRAINIAN				0x00000422
#define KBD_BELARUSIAN				0x00000423
#define KBD_SLOVENIAN				0x00000424
#define KBD_ESTONIAN				0x00000425
#define KBD_LATVIAN				0x00000426
#define KBD_LITHUANIAN_IBM			0x00000427
#define KBD_FARSI				0x00000429
#define KBD_VIETNAMESE				0x0000042A
#define KBD_ARMENIAN_EASTERN			0x0000042B
#define KBD_AZERI_LATIN				0x0000042C
#define KBD_FYRO_MACEDONIAN			0x0000042F
#define KBD_GEORGIAN				0x00000437
#define KBD_FAEROESE				0x00000438
#define KBD_DEVANAGARI_INSCRIPT			0x00000439
#define KBD_MALTESE_47_KEY			0x0000043A
#define KBD_NORWEGIAN_WITH_SAMI			0x0000043B
#define KBD_KAZAKH				0x0000043F
#define KBD_KYRGYZ_CYRILLIC			0x00000440
#define KBD_TATAR				0x00000444
#define KBD_BENGALI				0x00000445
#define KBD_PUNJABI				0x00000446
#define KBD_GUJARATI				0x00000447
#define KBD_TAMIL				0x00000449
#define KBD_TELUGU				0x0000044A
#define KBD_KANNADA				0x0000044B
#define KBD_MALAYALAM				0x0000044C
#define KBD_MARATHI				0x0000044E
#define KBD_MONGOLIAN_CYRILLIC			0x00000450
#define KBD_UNITED_KINGDOM_EXTENDED		0x00000452
#define KBD_SYRIAC				0x0000045A
#define KBD_NEPALI				0x00000461
#define KBD_PASHTO				0x00000463
#define KBD_DIVEHI_PHONETIC			0x00000465
#define KBD_LUXEMBOURGISH			0x0000046E
#define KBD_MAORI				0x00000481
#define KBD_CHINESE_SIMPLIFIED_US		0x00000804
#define KBD_SWISS_GERMAN			0x00000807
#define KBD_UNITED_KINGDOM			0x00000809
#define KBD_LATIN_AMERICAN			0x0000080A
#define KBD_BELGIAN_FRENCH			0x0000080C
#define KBD_BELGIAN_PERIOD			0x00000813
#define KBD_PORTUGUESE				0x00000816
#define KBD_SERBIAN_LATIN			0x0000081A
#define KBD_AZERI_CYRILLIC			0x0000082C
#define KBD_SWEDISH_WITH_SAMI			0x0000083B
#define KBD_UZBEK_CYRILLIC			0x00000843
#define KBD_INUKTITUT_LATIN			0x0000085D
#define KBD_CANADIAN_FRENCH_LEGACY		0x00000C0C
#define KBD_SERBIAN_CYRILLIC			0x00000C1A
#define KBD_CANADIAN_FRENCH			0x00001009
#define KBD_SWISS_FRENCH			0x0000100C
#define KBD_BOSNIAN				0x0000141A
#define KBD_IRISH				0x00001809
#define KBD_BOSNIAN_CYRILLIC			0x0000201A

/* Keyboard layout variant IDs */

#define KBD_ARABIC_102					0x00010401
#define KBD_BULGARIAN_LATIN				0x00010402
#define KBD_CZECH_QWERTY				0x00010405
#define KBD_GERMAN_IBM					0x00010407
#define KBD_GREEK_220					0x00010408
#define KBD_UNITED_STATES_DVORAK			0x00010409
#define KBD_SPANISH_VARIATION				0x0001040A
#define KBD_HUNGARIAN_101_KEY				0x0001040E
#define KBD_ITALIAN_142					0x00010410
#define KBD_POLISH_214					0x00010415
#define KBD_PORTUGUESE_BRAZILIAN_ABNT2			0x00010416
#define KBD_RUSSIAN_TYPEWRITER				0x00010419
#define KBD_SLOVAK_QWERTY				0x0001041B
#define KBD_THAI_PATTACHOTE				0x0001041E
#define KBD_TURKISH_F					0x0001041F
#define KBD_LATVIAN_QWERTY				0x00010426
#define KBD_LITHUANIAN					0x00010427
#define KBD_ARMENIAN_WESTERN				0x0001042B
#define KBD_HINDI_TRADITIONAL				0x00010439
#define KBD_MALTESE_48_KEY				0x0001043A
#define KBD_SAMI_EXTENDED_NORWAY			0x0001043B
#define KBD_BENGALI_INSCRIPT				0x00010445
#define KBD_SYRIAC_PHONETIC				0x0001045A
#define KBD_DIVEHI_TYPEWRITER				0x00010465
#define KBD_BELGIAN_COMMA				0x0001080C
#define KBD_FINNISH_WITH_SAMI				0x0001083B
#define KBD_CANADIAN_MULTILINGUAL_STANDARD		0x00011009
#define KBD_GAELIC					0x00011809
#define KBD_ARABIC_102_AZERTY				0x00020401
#define KBD_CZECH_PROGRAMMERS				0x00020405
#define KBD_GREEK_319					0x00020408
#define KBD_UNITED_STATES_INTERNATIONAL			0x00020409
#define KBD_THAI_KEDMANEE_NON_SHIFTLOCK			0x0002041E
#define KBD_SAMI_EXTENDED_FINLAND_SWEDEN		0x0002083B
#define KBD_GREEK_220_LATIN				0x00030408
#define KBD_UNITED_STATES_DVORAK_FOR_LEFT_HAND		0x00030409
#define KBD_THAI_PATTACHOTE_NON_SHIFTLOCK		0x0003041E
#define KBD_GREEK_319_LATIN				0x00040408
#define KBD_UNITED_STATES_DVORAK_FOR_RIGHT_HAND		0x00040409
#define KBD_GREEK_LATIN					0x00050408
#define KBD_US_ENGLISH_TABLE_FOR_IBM_ARABIC_238_L	0x00050409
#define KBD_GREEK_POLYTONIC				0x00060408
#define KBD_GERMAN_NEO					0xB0000407

/* Global Input Method Editor (IME) IDs */

#define KBD_CHINESE_TRADITIONAL_PHONETIC			0xE0010404
#define KBD_JAPANESE_INPUT_SYSTEM_MS_IME2002			0xE0010411
#define KBD_KOREAN_INPUT_SYSTEM_IME_2000			0xE0010412
#define KBD_CHINESE_SIMPLIFIED_QUANPIN				0xE0010804
#define KBD_CHINESE_TRADITIONAL_CHANGJIE			0xE0020404
#define KBD_CHINESE_SIMPLIFIED_SHUANGPIN			0xE0020804
#define KBD_CHINESE_TRADITIONAL_QUICK				0xE0030404
#define KBD_CHINESE_SIMPLIFIED_ZHENGMA				0xE0030804
#define KBD_CHINESE_TRADITIONAL_BIG5_CODE			0xE0040404
#define KBD_CHINESE_TRADITIONAL_ARRAY				0xE0050404
#define KBD_CHINESE_SIMPLIFIED_NEIMA				0xE0050804
#define KBD_CHINESE_TRADITIONAL_DAYI				0xE0060404
#define KBD_CHINESE_TRADITIONAL_UNICODE				0xE0070404
#define KBD_CHINESE_TRADITIONAL_NEW_PHONETIC			0xE0080404
#define KBD_CHINESE_TRADITIONAL_NEW_CHANGJIE			0xE0090404
#define KBD_CHINESE_TRADITIONAL_MICROSOFT_PINYIN_IME_3		0xE00E0804
#define KBD_CHINESE_TRADITIONAL_ALPHANUMERIC			0xE00F0404

FREERDP_API rdpKeyboardLayout* get_keyboard_layouts(uint32 types);
FREERDP_API const char* get_layout_name(uint32 keyboardLayoutID);

#endif
