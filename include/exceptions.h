//
// Created by kaba4ok on 14.03.24.
//

#ifndef TEMPLATE_EXCEPTIONS_H
#define TEMPLATE_EXCEPTIONS_H

#include <exception>

class InvalidNArgumentsException : public std::exception {
public:
    [[nodiscard]] const char* what() const noexcept override;
    static int error_code();
};

class IOException : public std::exception {
public:
    [[nodiscard]] const char* what() const noexcept override;
    static int error_code();
};

class MemoryException : public std::exception {
public:
    [[nodiscard]] const char* what() const noexcept override;
    static int error_code();
};

class WriteWriteFileException: public std::exception {
public:
    [[nodiscard]] const char* what() const noexcept override;
    static int error_code();
};

class InvalidMethodIdException : public std::exception {
public:
    [[nodiscard]] const char* what() const noexcept override;
    static int error_code();
};

class ConversionException : public std::exception {
public:
    [[nodiscard]] const char* what() const noexcept override;
    static int error_code();
};

class OpenWriteFileException : public std::exception {
public:
    [[nodiscard]] const char* what() const noexcept override;
    static int error_code();
};

class ReadConfigFileException : public std::exception {
public:
    [[nodiscard]] const char* what() const noexcept override;
    static int error_code();
};

class OpenConfigFileException : public std::exception {
public:
    [[nodiscard]] const char* what() const noexcept override;
    static int error_code();
};

#endif //TEMPLATE_EXCEPTIONS_H
