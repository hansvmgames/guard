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

#ifndef ORDERED_MAP_H
#define ORDERED_MAP_H

#include "rb_tree.h"

/**
 * The comparator function for an ordered map
 * Takes twho arguments: two keys cast to a const void * pointer and returns
 * < 0 if first < second
 * > if first > second
 * 0 if first == second
 */
typedef int (*ordered_map_cmp)(const void *, const void *);

/**
 * A destructor for keys or values in an ordered map
 * Takes one argument: the key or value cast to a void *
 */
typedef void (*ordered_map_destroy)(void *);


/**
 * An ordered map
 */
struct ordered_map {

  /**
   * A red black tree
   * Note that this is an implementation detail
   */
  struct rb_tree tree;
};

/**
 * Initializes the ordered map
 * \param map a pointer to the map
 * \param cmp the comparator operator
 * \param destroy_key the destructor for the keys or NULL if none is required
 * \param destroy_map the destructor for the values or NULL if none is required
 */
void init_ordered_map(struct ordered_map * map, ordered_map_cmp cmp, ordered_map_destroy destroy_key, ordered_map_destroy destroy_value);

/**
 * Inserts an entry into the map, replacing an already existing value if needed
 * Maps the following status codes on error:
 * STATUS_CODE_BAD_ALLOC if memory alllocation fails
 * \param map the map
 * \param key the key to insert
 * \param value the value to insert
 * \return 0 on succes, -1 on error
 */
int insert_into_ordered_map(struct ordered_map * map, void * key, void * value);

/**
 * Gets a value from the ordered map associated with the specified key or NULL if it does not exist
 * \param map the map
 * \param key the key associated with the value
 * \return the value or NULL if not found
 */

void * get_from_ordered_map(const struct ordered_map * map, const void * key);

/**
 * Disposes the ordered map
 * \param map the map
 */
void dispose_ordered_map(struct ordered_map * map);

#endif
