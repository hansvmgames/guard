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

#include "ordered_map.h"

#include <assert.h>
#include <stdlib.h>

void init_ordered_map(struct ordered_map * map, ordered_map_cmp cmp, ordered_map_destroy destroy_key, ordered_map_destroy destroy_value) {
  assert(map != NULL);
  assert(cmp != NULL);

  init_rb_tree(&map->tree, cmp, destroy_key, destroy_value);
}

/**
 * Inserts an entry into the map
 * Maps the following status codes on error:
 * STATUS_CODE_BAD_ALLOC if memory alllocation fails
 * \param map the map
 * \param key the key to insert
 * \param value the value to insert
 * \return 0 on succes, -1 on error
 */
int insert_into_ordered_map(struct ordered_map * map, void * key, void * value) {
  assert(map != NULL);

  return insert_or_replace_into_rb_tree(&map->tree, key, value);
}

/**
 * Gets a value from the ordered map associated with the specified key or NULL if it does not exist
 * \param map the map
 * \param key the key associated with the value
 * \return the value or NULL if not found
 */

void * get_from_ordered_map(const struct ordered_map * map, const void * key) {
  assert(map != NULL);
  return find_in_rb_tree(&map->tree, key);
}

/**
 * Disposes the ordered map
 * \param map the map
 */
void dispose_ordered_map(struct ordered_map * map) {
  assert(map != NULL);

  dispose_rb_tree(&map->tree);
}
