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

#include "logger.h"
#include "window.h"

#include <SDL2/SDL.h>

int init_window() {
  LOG_DEBUG("initializing SDL");
  if(SDL_Init(SDL_INIT_VIDEO) != 0) {
    LOG_ERROR("could not initialize SDL: '%s'", SDL_GetError());
    return -1;
  }
  return 0;
}

void dispose_window() {
  LOG_DEBUG("shutting down SDL");
  SDL_Quit();
}
