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

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <stdbool.h>
#include <stdio.h>

/**
 * Opaque pointer to a config parser
 */
struct config_parser;

/**
 * The type of token that has been parsed
 */
enum config_token_type {
			/**
			 * A simple value
			 */
			VALUE,

			/**
			 * The start of a list
			 */
			LIST_START,

			/**
			 * The end of a list
			 */
			LIST_END,

			/**
			 * The start of a map
			 */
			MAP_START,

			/**
			 * A key within a map
			 */
			MAP_KEY,
			
			/**
			 * The end of a map
			 */
			MAP_END
};

/**
 * A symbol table allowing for easy switching on map labels
 */
struct config_symbols;

/**
 * Creates a symbol table
 * sets status to BAD_ALLOC if memory allocation failed
 * \param not_found the value that should be returned if a symbol is not found
 * \return an opaque pointer to the symbol table or NULL on failure
 */
struct config_symbols * create_config_symbols(int not_found);

/**
 * Sets the current scope of the symbol table
 * \param s the symbol table
 * \param scope the scope
 */
void set_config_scope(struct config_symbols * s, int scope);

/**
 * Registers a symbol
 * \param s the symbol table
 * \param label a string constant representation of the label
 * \param value a value (an enum usually)
 * \return 0 on success, -1 on failure
 */
int add_config_symbol(struct config_symbols * s, const char * label, int value);

/**
 * Returns the value associated with the symbol within the current scope
 * \param s the symbol table
 * \param label the label
 * \return the symbol or the not found sentinel value
 */
int lookup_config_symbol(struct config_symbols * s, const char * label);

/**
 * Destroys the symbol table
 * \param s the symbol tables
 */
void destroy_config_symbols(struct config_symbols * s);

/**
 * Creates a config parser
 * \return an opaque pointer to the parser or NULL on failure
 */
struct config_parser * create_config_parser();

/**
 * Start parsing a file
 * \param p the parser
 * \param file a file ready for reading
 * \return 0 on success, -1 on failure
 */
int start_config_parser(struct config_parser * p, FILE * file);

/**
 * Parses the next config token
 * \param p the parser
 * \return 0 on success, -1 on failure
 */
int next_config_token(struct config_parser * p);

/**
 * Returns the type of the current token
 * \param p the parser
 * \return the type
 */
enum config_token_type get_config_token_type(const struct config_parser * p);

/**
 * Returns the string value of the current token or NULL if there is none
 * \param p the parser
 * \return a pointer to a string that will remain valid until the next call to next_config_token
 */
const char * get_config_string(struct config_parser * p);

/**
 * Returns the value stored in the symbol table for the current token
 * \param p the parser
 * \param s the symbol table
 * \return the value associated with the symbol or the not found sentinel token
 */
int get_config_symbol(struct config_parser * p, struct config_symbols * s);

/**
 * Stores the value of the current token in the int pointed to by the pointer
 * \param p the parser
 * \param result a pointer to store the result
 * \return 0 on success, -1 on error
 */
int get_config_int(struct config_parser * p, int * result);

/**
 * Stores the value of the current token in the double pointed to by the pointer
 * \param p the parser
 * \param result a pointer to store the result
 * \return 0 on success, -1 on error
 */
int get_config_double(struct config_parser * p, double * result);

/**
 * Stores the value of the current token in the float pointed to by the pointer
 * \param p the parser
 * \param result a pointer to store the result
 * \return 0 on success, -1 on error
 */
int get_config_float(struct config_parser * p, float * result);

/**
 * Stores the value of the current token in the bool pointed to by the pointer
 * \param p the parser
 * \param result a pointer to store the result
 * \return 0 on success, -1 on error
 */
int get_config_bool(struct config_parser * p, bool * result);

/**
 * Stops the parser, allowing it to be reused
 * \param p the parser
 * \return 0 on success, -1 on failure
 */
void stop_config_parser(struct config_parser * p);

/**
 * Destroys the config parser
 * \param p the parser
 */
void destroy_config_parser(struct config_parser * p);

#endif
