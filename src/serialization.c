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
#include "ordered_map.h"
#include "serialization.h"
#include "status.h"

#include <pthread.h>

#define SERIALIZER_NAME_MAX_LEN 64


struct serializer_field {

  /**
   * A name unique within the serializer
   */
  char name[SERIALIZER_NAME_MAX_LEN];

  /**
   * The type of this field
   */
  enum serializer_field_type type;

    /**
   * Maximim length (for strings)
   */
  size_t max_len;

  /**
   * A string constant specifying the serializer to use (for objects)
   */
  const char serializer_name[SERIALIZER_NAME_MAX_LEN];

  /**
   * The setter
   */
  serializer_setter setter;

  /**
   * The getter
   */
  serializer_getter getter;
  
};

struct serializer {

  /**
   * The serializer's unique name
   */
  char name[SERIALIZER_NAME_MAX_LEN];

  /**
   * Fields
   */
  struct serializer_field * fields;

  /**
   * The number of fields
   */
  size_t field_len;

  /**
   * Fields by name
   */
  struct ordered_map fields_by_name;
  
  /**
   * A pointer to the next serializer
   */
  struct serializer * next;
  
};

/**
 * A builder for a field
 */
struct serializer_field_builder {

  struct serializer_field field;
  
  /**
   * A pointer to the next serializer field within this serializer
   */
  struct serializer_field_builder * next;
};

struct serializer_builder {

  struct serializer serializer;
  
  /**
   * The first serializer field
   */
  struct serializer_field_builder * head;

  /**
   * The last serializer field
   */
  struct serializer_field_builder * tail;
};

/**
 * The list of serializers
 */
static struct serializer * serializers;

/**
 * The serializers by name
 */
static ordered_map serializers_by_name;

/**
 * A mutex to protect the serializer list and lookup tree
 */
static pthread_mutex_t serializer_mutex = PTHREAD_MUTEX_INITIALIZER;

void init_serializers() {
  serializers = NULL;
  init_ordered_map(&serializers_by_name);
}

struct serializer * get_or_create_serializer(const char * id, init_serializer constructor);

int add_serializer_field(struct serializer_builder * s, const char * name, const struct serializer_field_attr * attr);

int serialize(const struct serializer * s, FILE * file, const void * object);

int deserialize(const struct serializer * s, FILE * file, void * object);

void dispose_serializers();
