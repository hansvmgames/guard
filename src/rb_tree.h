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

#ifndef RB_TREE_H
#define RB_TREE_H

#include <stdbool.h>

/**
 * A red black node
 */
struct rb_node {

  /**
   * The key
   */
  void * key;

  /**
   * The value
   */
  void * value;
  
  /**
   * Whether this node is red
   */
  bool red;
  
  /**
   * A link to the parent node
   */
  struct rb_node * parent;

  /**
   * A link to the left child node
   */
  struct rb_node * left;

  /**
   * A link to the right child node
   */
  struct rb_node * right;
};

/**
 * The comparison function to be used for this red black tree
 */
typedef int (*rb_tree_cmp)(const void *, const void *);

/**
 * A function to be called when disposing a key or value
 */
typedef void (*rb_tree_destroy)(void *);

/**
 * A red black tree
 */
struct rb_tree {

  /**
   * The root of the tree
   */
  struct rb_node * root;

  /**
   * Implementation dependent state to be passed to the functions
   */
  void * state;
  
  /**
   * The comparison function
   */
  rb_tree_cmp cmp;

  /**
   * Destroy the key
   */
  rb_tree_destroy destroy_key;

  /**
   * Destroys the value
   */
  rb_tree_destroy destroy_value;
  
};

/**
 * Initializes the red black tree
 * \param tree the treee
 * \param state implementation dependent state to be passed to the functions or NULL if not applicable
 * \param cmp  the comparison function
 * \param destroy_key the function to be called when disposing of a key or NULL if no action should be taken
 * \param destroy_value the function to be called when disposing of a value or NULL if no action should be taken
 */
void init_rb_tree(struct rb_tree * tree, rb_tree_cmp cmp, rb_tree_destroy destroy_key, rb_tree_destroy destroy_value);

/**
 * Inserts a value into the red black tree
 * If memory allocation fails, status is set to STATUS_CODE_BAD_ALLOC
 * If the key already exists, status is set to STATUS_CODE_ALREADY_EXISTS
 * \param tree the tree
 * \param key the key,
 * \param value the value (must not be NULL)
 * \return 0 on success, -1 on failure
 */
int insert_into_rb_tree(struct rb_tree * tree, void * key, void * value);

/**
 * Inserts a value into the red black tree, replacing an existing value if necessary
 * If memory allocation fails, status is set to STATUS_CODE_BAD_ALLOC
 * \param tree the tree
 * \param key the key,
 * \param value the value (must not be NULL)
 * \return 0 on success, -1 on failure
 */
int insert_or_replace_into_rb_tree(struct rb_tree * tree, void * key, void * value);

/**
 * Removes a node from the red black tree
 * \param tree the tree
 * \param key the key
 * \return 0 if the key was found and removed, -1 if it wasn't found
 */
int remove_from_rb_tree(struct rb_tree * tree, const void * key);

/**
 * Attempts to find a value in the red black tree
 */
void * find_in_rb_tree(const struct rb_tree * tree, const void * key);

/**
 * Walks the tree in order and calls the supplied function on each key/value pair until the
 * tree ends or the functions returns a non zero value
 * \param tree the tree
 * \param fn the function
 * \return 0 if the tree was walked completely, or the non-zero return value of the last call to the supplied function
 */
int walk_rb_tree(const struct rb_tree * tree, int (*fn)(const void *, const void *));

/**
 * Disposes the red black tree
 * \param tree the tree
 */
void dispose_rb_tree(struct rb_tree * tree);

#endif
