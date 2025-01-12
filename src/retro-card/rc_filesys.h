#pragma once
#include <Arduino.h>

/**
 * Initialize the SD and file system
 * @returns true if the file system was initialized successfully, false otherwise
 */
extern bool rc_filesys_init();

/**
 * Open a directory from path
 * @param dirpath Path to the directory
 * @returns true if the directory was successfully opened, false otherwise (e.g. path does not exist or is not a directory)
 */
extern bool rc_open_dir(const char *dirpath);

/**
 * Get the number of files in the current directory
 * @returns Number of files in the current directory
 */
extern unsigned int rc_get_num_files();

extern bool rc_get_file_names(char **file_names, unsigned int num_files);

/**
 * Open a file from the current directory
 * @param index Index of the file
 * @returns true if the file was successfully opened, false otherwise
 */
extern bool rc_open_file(const char *filepath);

/**
 * Get the size of the current file
 * @returns Size of the file
 */
extern size_t rc_get_file_size();

/**
 * Read from the current file
 * @param buffer Buffer to read into
 * @param size Number of bytes to read
 * @returns Number of bytes read
 */
extern unsigned int rc_read_file(void *buffer, unsigned int size);

/**
 * Close the current file
 * @returns true if the file was successfully closed, false otherwise
 */
extern bool rc_close_file();

/**
 * Close the current directory
 * @returns true if the directory was successfully closed, false otherwise
 */
extern bool rc_close_dir();