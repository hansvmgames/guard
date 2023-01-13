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
#include "rb_tree.h"
#include "settings.h"
#include "window.h"

#include <stdlib.h>

int cmp_test(const void * a, const void * b) {
  return *((const int *)a) - *((const int *)b);
}

int print_test(const void * key, const void * value) {
  LOG_DEBUG("(%d, %s)", *((const int *)key), (const char *)value);
  return 0;
}

void test() {

  struct rb_tree tree;
  init_rb_tree(&tree, cmp_test, NULL, NULL);
  
  int keys[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const char * values[] =  {
			    "zero",
			    "one",
			    "two",
			    "three",
			    "four",
			    "five",
			    "six",
			    "seven",
			    "eight",
			    "nine"
  };
  for(size_t i = 0; i < 10; ++i) {
    if(insert_into_rb_tree(&tree, &keys[i], (void *)values[i]) != 0) {
      LOG_ERROR("could not insert (%d, %s)", keys[i], values[i]);
    }
  }
  walk_rb_tree(&tree, print_test);
  
  dispose_rb_tree(&tree);
}

/**
 * Main function
 * \param arg_count the number of arguments
 * \param args the arguments
 * \return EXIT_SUCESS if the program closes normally, EXIT_FAILURE otherwise
 */
int main(int arg_count, const char * args[]) {

  if(init_logger(LOG_LEVEL_DEBUG) != 0) {
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

  test();
  
  struct settings settings;

  //load_settings(&settings);

  int result = 0;
  /*
    int result = init_window();
    
  if(result == 0) {
  dispose_window();
  }
  */
  
  stop_logger();
  dispose_logger();
  
  if(result == 0) {
    return EXIT_SUCCESS;
  } else {
    return EXIT_FAILURE;
  }
}
