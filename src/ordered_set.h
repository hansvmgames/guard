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

#ifndef ORDERED_SET_H
#define ORDERED_SET_H

#include "rb_tree.h"

/**
 * The comparator function for an ordered set
 * Takes twho arguments: two entries cast to a const void * pointer and returns
 * < 0 if first < second
 * > if first > second
 * 0 if first == second
 */
typedef int (*ordered_set_cmp)(const void *, const void *);

/**
 * A destructor for entries in the ordered set
 * Takes one argument: the entry cast to a void *
 */
typedef void (*ordered_set_destroy)(void *);

/**
 * An ordered set
 */
struct ordered_set {
  
  /**
   * A red black tree
   * Note that this is an implementation detail
   */
  struct rb_tree tree;
};

/**
 * Initializes the ordered set
 * \param set a pointer to the set
 * \param cmp the comparator operator
 * \param destroy the destructor for the entry or NULL if none is required
 */
void init_ordered_set(struct ordered_set * set, ordered_set_cmp cmp, ordered_set_destroy destroy);

/**
 * Inserts an entry into the set
 * Sets the following status codes on error:
 * STATUS_CODE_BAD_ALLOC if memory alllocation fails
 * STATUS_CODE_ALREADY_EXISTS if a duplicate node was entered
 * \param set the set
 * \param entry the entry to insert
 * \return 0 on succes, -1 on error
 */
int insert_into_ordered_set(struct ordered_set * set, void * entry);

/**
 * Checks whether the supplied entry is already present in the ordered set
 * \param set the set
 * \param entry the entry
 */
bool ordered_set_contains(const struct ordered_set * set, const void * entry);

/**
 * Disposes the ordered set
 * \param set the set
 */
void dispose_ordered_set(struct ordered_set * set);

#endif
