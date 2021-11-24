/*
 * std_utils.h
 *
 *  Created on: Nov 24, 2021
 *      Author: Alberto
 */

#ifndef STD_UTILS_H_
#define STD_UTILS_H_

/// Scan a new line input from the user
/// @param prompt a prompt message to display
/// @param buffer the buffer in which text is stored
/// @param size the maximum size of the buffer
///
/// The return value is 0 on success and -1 on failure.
int scan_line(char *prompt, char *buffer, int size);


#endif /* STD_UTILS_H_ */
