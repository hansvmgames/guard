/*
 *
 * This file is part of guard.
 *
 * guard is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation, 
 * either version 3 of the License, or (at your option) any later version.
 * 
 * guard is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with guard. 
 * If not, see <https://www.gnu.org/licenses/>. 
 * 
 */

#include "configuration.h"
#include "settings.h"

#include <assert.h>

/**
 * Settings scope for config parsing
 */
enum settings_scope {
		     SETTINGS_SCOPE_ROOT,
		     SETTINGS_SCOPE_VIDEO,
};

enum settings_symbol {
		      SETTINGS_SYMBOL_UNKNOWN,
		      SETTINGS_SYMBOL_VIDEO,
		      SETTINGS_SYMBOL_SCREEN_WIDTH,
		      SETTINGS_SYMBOL_SCREEN_HEIGHT
};

int load_settings(struct settings * settings) {

  struct config_symbols * symbols = create_config_symbols(SETTINGS_SYMBOL_UNKNOWN);
  set_config_scope(symbols, SETTINGS_SCOPE_ROOT);

  if(add_config_symbol(symbols, "video", SETTINGS_SYMBOL_VIDEO) != 0) {
    return -1;
  }
  set_config_scope(symbols, SETTINGS_SCOPE_VIDEO);
  if(add_config_symbol(symbols, "screen_width", SETTINGS_SYMBOL_SCREEN_WIDTH) != 0) {
    return -1;
  }
  if(add_config_symbol(symbols, "screen_height", SETTINGS_SYMBOL_SCREEN_HEIGHT) != 0) {
    return -1;
  };
  assert(lookup_config_symbol(symbols, "screen_width") == SETTINGS_SYMBOL_SCREEN_WIDTH); 
  return 0;
}

void dispose_settings(struct settings * settings) {}

