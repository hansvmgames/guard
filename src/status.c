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

#include "status.h"

#include <pthread.h>

__thread enum status_code cur_status = STATUS_CODE_OK;

static const char * status_labels[] = {
  "ok"
};

void clear_status() {
  cur_status = STATUS_CODE_OK;
}

void set_status(enum status_code status) {
  cur_status = status;
}

enum status_code get_status() {
  return cur_status;
}

const char * get_status_label(enum status_code status) {
  return status_labels[(size_t)status];
}
