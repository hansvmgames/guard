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

#include "configuration.h"
#include "rb_tree.h"
#include "status.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/**
 * The symbol table implemented as a red black tree
 */
struct config_symbols {

  /**
   * A red black tree to hold the symbol table
   */
  struct rb_tree tree;

  /**
   * The current scope
   */
  int scope;

  /**
   * The sentinel value to return if no value found for a given label
   */
  int not_found;

};

/**
 * A key in the symbol table
 */
struct config_symbol_key {

  /**
   * The scope
   */
  int scope;

  /**
   * The name
   */
  const char * name;
};

/**
 * A comparator for symbol keys
 * \param a a pointer to the first symbol key as a void *
 * \param a pointer to the second symbol key as a void *
 * \return the comparison value
 */
static int cmp_config_symbol_key(const void * a, const void * b) {
  struct config_symbol_key * first = (struct config_symbol_key *) a;
  struct config_symbol_key * second = (struct config_symbol_key *) b;

  int result = first->scope - second->scope;
  if(result == 0) {
    result = strcmp(first->name, second->name);
  }
  return result;
}

/**
 * Destroys either a key or value
 * \param k the key or value
 */
static void destroy_config_symbol(void * k) {
  free(k);
}

/*
 * The public API of the symbol table
 */

struct config_symbols * create_config_symbols(int not_found) {
  struct config_symbols * s = (struct config_symbols *) malloc(sizeof(struct config_symbols));
  if(s == NULL) {
    set_status(STATUS_CODE_BAD_ALLOC);
    return NULL;
  }
  s->scope = 0;
  s->not_found = not_found;
  init_rb_tree(&s->tree, cmp_config_symbol_key, destroy_config_symbol, destroy_config_symbol);
}

void set_config_scope(struct config_symbols * s, int scope) {
  s->scope = scope;
}

int add_config_symbol(struct config_symbols * s, const char * label, int value_) {
  assert(s != NULL);
  assert(label != NULL);
  
  struct config_symbol_key * key = (struct config_symbol_key *) malloc(sizeof(struct config_symbol_key));
  if(key == NULL) {
    set_status(STATUS_CODE_BAD_ALLOC);
    return -1;
  }
  key->scope = s->scope;
  key->name = label;
  
  int * value = (int *) malloc(sizeof(int));
  if(value == NULL) {
    set_status(STATUS_CODE_BAD_ALLOC);
    free(key);
    return -1;
  }
  *value = value_;
  
  if(insert_into_rb_tree(&s->tree, key, value) != 0) {
    set_status(STATUS_CODE_BAD_ARG);
    free(key);
    free(value);
    return -1;
  }
  return 0;
}

int lookup_config_symbol(struct config_symbols * s, const char * label) {
  struct config_symbol_key key = {s->scope, label};
  int * value = find_in_rb_tree(&s->tree, &key);
  if(value == NULL) {
    return s->not_found;
  } else {
    return *value;
  }
}

void destroy_config_symbols(struct config_symbols * s) {
  assert(s != NULL);

  dispose_rb_tree(&s->tree);
  free(s);
}
