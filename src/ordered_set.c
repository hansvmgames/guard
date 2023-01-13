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

#include "ordered_set.h"

#include <assert.h>
#include <stdlib.h>

void init_ordered_set(struct ordered_set * set, ordered_set_cmp cmp, ordered_set_destroy destroy) {
  assert(set != NULL);
  assert(cmp != NULL);

  init_rb_tree(&set->tree, cmp, destroy, NULL);
}

int insert_into_ordered_set(struct ordered_set * set, void * entry) {
  assert(set != NULL);
  assert(entry != NULL);

  return insert_into_rb_tree(&set->tree, entry, entry);
}

bool ordered_set_contains(const struct ordered_set * set, const void * entry) {
  assert(set != NULL);
  assert(entry != NULL);
  
  return find_in_rb_tree(&set->tree, entry) != NULL;
}

void dispose_ordered_set(struct ordered_set * set) {
  assert(set != NULL);

  dispose_rb_tree(&set->tree);
}
