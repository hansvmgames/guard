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

#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <stdio.h>
#include <stdlib.h>

/**
 * Opaque serializer handle
 */
struct serializer;

/**
 * Opaque serializer builder handle
 */
struct serializer_builder;

/**
 * An initialization function for a serializer
 */
typedef int (*init_serializer)(struct serializer_builder *);

/**
 * All supported serializer field type
 */
enum serializer_field_type {
			    /**
			     * A string
			     */
			    SERIALIZER_FIELD_TYPE_STRING,

			    /**
			     * An integer
			     */
			    SERIALIZER_FIELD_TYPE_INT,

			    /**
			     * An object to be serialized by another serializer
			     */
			    SERIALIZER_FIELD_TYPE_OBJECT
};

/**
  * A setter function pointer
  */
typedef void (*serializer_setter)(void *, void *);

/**
  * A getter function pointer
  */
typedef void (*serializer_getter)(const void *, void *);

/**
 * Attributes for a serializer field
 */
struct serializer_field_attr {

  /**
   * the type of the field
   */
  enum serializer_field_type type;

  /**
   * Maximim length (for strings)
   */
  size_t max_len;

  /**
   * A string constant specifying the serializer to use (for objects)
   */
  const char * serializer_name;

  /**
   * The setter
   */
  serializer_setter setter;

  /**
   * The getter
   */
  serializer_getter getter;
};

/**
 * Initializes the serialization subsystem
 */
void init_serializers();

/**
 * Creates a serializer if it does not exist
 * This function is thread safe and may block
 * \param id the unique ID of this serializer
 * \param constructor a function that initializes the serializer
 */
struct serializer * get_or_create_serializer(const char * id, init_serializer constructor);

/**
 * Adds a field to the serializer
 * \param s the serializer builder
 * \param name the name of the field
 * \param attr a pointer to the attributes descriptor from which to copy the field attributes. These can be reused after the call and non applicable attributes are ignored
 */
int add_serializer_field(struct serializer_builder * s, const char * name, const struct serializer_field_attr * attr);

/**
 * Serializes an object
 * \param s the serializer
 * \param file the file from which to read the object
 * \param object a pointer to the object to serialize
 * \return 0 on success, -1 on failure
 */
int serialize(const struct serializer * s, FILE * file, const void * object);

/**
 * Deserializes an object
 * \param s the serializer
 * \param file the file to which to write the object
 * \param object a pointer to the object to deserialize
 * \return 0 on success, -1 on failure
 */
int deserialize(const struct serializer * s, FILE * file, void * object);

/**
 * Disposes the serialization subsystem, destroying all serializer
 */
void dispose_serializers();

#endif
