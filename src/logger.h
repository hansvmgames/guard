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

/**
 * The public API of the logging subsystem
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

/**
 * The log levels
 */
enum log_level {
		/**
		 * For debug logging
		 */
		LOG_LEVEL_DEBUG,

		/**
		 * For printing information to the user
		 */
		LOG_LEVEL_INFO,

		/**
		 * For printing warnings, i.e. recoverable but unusual situations
		 */
		LOG_LEVEL_WARNING,

		/**
		 * For errors
		 */
		LOG_LEVEL_ERROR,
};

/**
 * Initializes the logger
 * \param min_level all messages with lower priority get discarded
 * \return 0 on success, -1 otherwise
 */
int init_logger(enum log_level min_level);

/**
 * Adds an output file to the logger
 * This function may only be called after initialization of but before starting the log system
 * \param file a pointer to the file to print from
 * \return 0 on success, -1 otherwise
 */
int add_logger_output(FILE * file);

/**
 * Starts the logger
 * \return 0 on success, -1 on error
 */
int start_logger();

/**
 * Creates and adds a log message
 * Users should use the utility macro's instead for better performance
 * \level the log level
 * \file the file where the message originates from
 * \line the line where the lessage originates from
 * \format the format string
 * \... optional parameters for formatting						
 * \return 0 on success, -1 on failure
 */
int add_log_message(enum log_level level, const char * file, int line, const char * format, ...);

/**
 * Returns the minimum log level, all messages with lower priority are discarded
 */
enum log_level get_min_log_level();

/**
 * Stops the logger, blocking until all log messages have been written
 * \return 0 on success, -1 on error
 */
int stop_logger();

/**
 * Disposes the logger
 */
void dispose_logger();

/**
 * Utility macro for log messages
 */
#define LOG_MSG(level, ...) if(get_min_log_level() <= (level)) add_log_message((level), __FILE__, __LINE__, __VA_ARGS__)

/**
 * Utility macro for debug messages
 */
#define LOG_DEBUG(...) LOG_MSG(LOG_LEVEL_DEBUG, __VA_ARGS__)

/**
 * Utility macro for info messages
 */
#define LOG_INFO(...) LOG_MSG(LOG_LEVEL_INFO, __VA_ARGS__)

/**
 * Utility macro for warning messages
 */
#define LOG_WARNING(...) LOG_MSG(LOG_LEVEL_WARNING, __VA_ARGS__)

/**
 * Utility macro for error messages
 */
#define LOG_ERROR(...) LOG_MSG(LOG_LEVEL_ERROR, __VA_ARGS__)

#endif
