#include "rc_filesys.h"

#include "SdFat.h"
#include "../defines.h"

SdFat SD;
File file;
File dir;

bool rc_filesys_init() {
    return SD.begin(SD_CS, SPI_FULL_SPEED);
}

bool rc_open_dir(const char *dirpath) {
    dir = SD.open(dirpath);
    if (!dir || !dir.isDirectory()) {
        return false;
    }

    dir.rewind();

    return true;
}

unsigned int rc_get_num_files() {
    unsigned int num_files = 0;
    while (file.openNext(&dir, O_RDONLY)) {
        num_files++;
        file.close();
    }

    dir.rewindDirectory();
    return num_files;
}

bool rc_get_file_names(char **file_names, unsigned int num_files) {
    for (unsigned int i = 0; i < num_files; i++) {
        file.openNext(&dir, O_READ);
        file_names[i] = (char *)malloc(256);
        file.getName(file_names[i], 255);
        file.close();
    }
    dir.rewindDirectory();

    return true;
}

bool rc_open_file(const char *filepath) {
    file = SD.open(filepath);
    if (!file) {
        return false;
    }

    file.rewind();

    return true;
}

size_t rc_get_file_size() {
    return file.size();
}

unsigned int rc_read_file(void *buffer, unsigned int size) {
    return file.read(buffer, size);
}

bool rc_close_file() {
    file.close();
    return true;
}

bool rc_close_dir() {
    dir.close();
    return true;
}