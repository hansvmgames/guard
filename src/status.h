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

#ifndef STATUS_H
#define STATUS_H

/**
 * Status codes for errors
 */
enum status_code {
		  /**
		   * Everything is OK
		   */
		  STATUS_CODE_OK
};

/**
 * Clears the status flag for this thread
 */
void clear_status();

/**
 * Sets the status flag for this thread
 * \param status the status code
 */
void set_status(enum status_code status);

/**
 * Gets the status flag for this thread
 * \return the status code
 */
enum status_code get_status();

/**
 * Returns a string constant describing the specified status code
 * \param status the status code
 * \return a string constant
 */
const char * get_status_label(enum status_code status);

#endif
