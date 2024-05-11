// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "exceptions.h"

const char* InvalidNArgumentsException::what() const noexcept {
    return "Invalid number of arguments provided!";
}
int InvalidNArgumentsException::error_code() {
    return 1;
}

const char* OpenConfigFileException::what () const noexcept {
    return "An error occurred while opening config file!";
}
int OpenConfigFileException::error_code() {
    return 3;
}

const char* OpenWriteFileException::what () const noexcept {
    return "An error occurred while opening write file!";
}
int OpenWriteFileException::error_code() {
    return 4;
}

const char* ReadConfigFileException::what () const noexcept {
    return "An error occurred while reading config file";
}
int ReadConfigFileException::error_code() {
    return 5;
}

const char* WriteWriteFileException::what() const noexcept {
    return "An error occurred while writing to write file";
}
int WriteWriteFileException::error_code() {
    return 6;
}

const char* IOException::what() const noexcept {
    return "An I/O error occurred!";
}
int IOException::error_code() {
    return 9;
}
///////////////////////////////////////////////////////////
const char* ConversionException::what() const noexcept {
    return "An error occurred during conversion!";
}
int ConversionException::error_code() {
    return 7;
}

const char* MemoryException::what() const noexcept {
    return "An error occurred during memory allocation!";
}
int MemoryException::error_code() {
    return 8;
}