/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 * Keyboard Localization
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

#include <stdio.h>
#include <string.h>

#include <freerdp/types.h>
#include <freerdp/utils/memory.h>
#include <freerdp/locale/keyboard.h>
#include <freerdp/locale/locale.h>

#include "keyboard_keymap.h"
#include "liblocale.h"

#ifdef WITH_X11
#include "keyboard_x11.h"

#ifdef WITH_XKBFILE
#include "keyboard_xkbfile.h"
#endif

#ifdef WITH_SUN
#include "keyboard_sun.h"
#endif

#endif

uint32 RDP_SCANCODE_TO_X11_KEYCODE[256][2];
RDP_SCANCODE X11_KEYCODE_TO_RDP_SCANCODE[256];

extern const RDP_SCANCODE VIRTUAL_KEY_CODE_TO_DEFAULT_RDP_SCANCODE_TABLE[256];

uint32 freerdp_detect_keyboard(uint32 keyboardLayoutID)
{
	if (keyboardLayoutID != 0)
		DEBUG_KBD("keyboard layout configuration: %X", keyboardLayoutID);

	if (keyboardLayoutID == 0)
	{
		keyboardLayoutID = freerdp_detect_keyboard_layout_from_system_locale();
		DEBUG_KBD("detect_keyboard_layout_from_locale: %X", keyboardLayoutID);
	}

	if (keyboardLayoutID == 0)
	{
		keyboardLayoutID = 0x0409;
		DEBUG_KBD("using default keyboard layout: %X", keyboardLayoutID);
	}

	return keyboardLayoutID;
}

uint32 freerdp_keyboard_init(uint32 keyboardLayoutId)
{
#ifdef WITH_X11

#ifdef WITH_XKBFILE
	keyboardLayoutId = freerdp_keyboard_init_xkbfile(keyboardLayoutId);
	if (keyboardLayoutId == 0)
		keyboardLayoutId = freerdp_keyboard_init_x11(keyboardLayoutId);
#else
	keyboardLayoutId = freerdp_keyboard_init_x11(keyboardLayoutId);
#endif

#endif
	return keyboardLayoutId;
}

uint32 freerdp_keyboard_get_rdp_scancode_from_x11_keycode(uint32 keycode, boolean* extended)
{
	DEBUG_KBD("x11 keycode: %02X -> rdp code: %02X%s", keycode,
		X11_KEYCODE_TO_RDP_SCANCODE[keycode].code,
		X11_KEYCODE_TO_RDP_SCANCODE[keycode].extended ? " extended" : "");

	*extended = X11_KEYCODE_TO_RDP_SCANCODE[keycode].extended;

	return X11_KEYCODE_TO_RDP_SCANCODE[keycode].code;
}

uint32 freerdp_keyboard_get_x11_keycode_from_rdp_scancode(uint32 scancode, boolean extended)
{
	if (extended)
		return RDP_SCANCODE_TO_X11_KEYCODE[scancode][1];
	else
		return RDP_SCANCODE_TO_X11_KEYCODE[scancode][0];
}

uint32 freerdp_keyboard_get_rdp_scancode_from_virtual_key_code(uint32 vkcode, boolean* extended)
{
	*extended = VIRTUAL_KEY_CODE_TO_DEFAULT_RDP_SCANCODE_TABLE[vkcode].extended;
	return VIRTUAL_KEY_CODE_TO_DEFAULT_RDP_SCANCODE_TABLE[vkcode].code;
}
