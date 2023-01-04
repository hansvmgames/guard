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

#include <stdlib.h>

/**
 * Main function
 * \param arg_count the number of arguments
 * \param args the arguments
 * \return EXIT_SUCESS if the program closes normally, EXIT_FAILURE otherwise
 */
int main(int arg_count, const char * args[]) {

  if(init_logger(LOG_LEVEL_INFO) != 0) {
    fputs("logger failed to initialize\n", stderr);
    return EXIT_FAILURE;
  }
  if(add_logger_output(stdout) != 0) {
    fputs("logger output could not be set\n", stderr);
    return EXIT_FAILURE;
  }
  if(start_logger() != 0) {
    fputs("logger could not be started\n", stderr);
    return EXIT_FAILURE;
  }

  int result = init_window();
  
  if(result == 0) {
    dispose_window();
  }
  
  stop_logger();
  dispose_logger();
  
  if(result == 0) {
    return EXIT_SUCCESS;
  } else {
    return EXIT_FAILURE;
  }
}
