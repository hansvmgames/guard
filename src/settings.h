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

#ifndef SETTINGS_H
#define SETTINGS_H

/**
 * Video settings
 */
struct video_settings {

  /**
   * The screen width in pixels
   */
  int screen_width;

  /**
   * The screen height in pixels
   */
  int screen_height;
    
};

/**
 * Application settings
 */
struct settings {

  /**
   * The video settings
   */
  struct video_settings video;
};

/**
 * Loads the settings
 * \param settings a pointer to a settings buffer
 * \return 0 on success, -1 on error
 */
int load_settings(struct settings * settings);

/**
 * Disposes the settings
 * \param settings the settings
 */
void dispose_settings(struct settings * settings);

#endif
