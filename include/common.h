//
// Created by kaba4ok on 14.03.24.
//

#ifndef LAB7_WORDS
#define LAB7_WORDS

#include <cstdint>
#include "exceptions.h"
#include <iostream>
#include <filesystem>
#include <unordered_set>
#include "oneapi/tbb/concurrent_hash_map.h"
#include "oneapi/tbb/concurrent_queue.h"

struct StringHashCompare {
    size_t hash( const std::string& x ) const {
        size_t h = 0;
        for( const char* s = x.c_str(); *s; ++s )
            h = (h*17)^*s;
        return h;
    }
    //! True if strings are equal
    bool equal( const std::string& x, const std::string& y ) const {
        return x==y;
    }
};

using ConcurrentDictionary = tbb::concurrent_hash_map<std::string, size_t, StringHashCompare>;

template<typename T>
using ConcurrentQueue = tbb::concurrent_bounded_queue<T>;

enum FileType
{
    RegularFile,
    Archive,
    None
};

struct Configuration
{
    std::filesystem::path archiveRoot;
    std::filesystem::path outputAlphabetPath;
    std::filesystem::path outputQuantityPath;
    std::unordered_set<std::string> allowedFileExtensions;
    std::unordered_set<std::string> allowedArchiveExtensions;
    size_t maxFileSize;
    size_t indexingThreads;
    size_t mergingThreads;
    size_t maxNamesQueueSize;
    size_t maxRawQueueSize;
    size_t maxDictQueueSize;

#ifndef NDEBUG
    void PrintMe()
    {
        std::cout << "Root: " << archiveRoot << std::endl;
        std::cout << "Alphabet path: " << outputAlphabetPath << std::endl;
        std::cout << "Quantity path: " << outputQuantityPath << std::endl;
        std::cout << "Max file size: " << maxFileSize << std::endl;
        std::cout << "Max file names: " << maxNamesQueueSize << std::endl;
        std::cout << "Max raw files: " << maxRawQueueSize << std::endl;
        std::cout << "Max dictionaries: " << maxDictQueueSize << std::endl;
        std::cout << "Index threads: " <<  indexingThreads << std::endl;
        std::cout << "Merging threads: " <<  mergingThreads << std::endl;

        std::cout << "File extensions: " << std::endl;
        for(const auto& ext : allowedFileExtensions)
        {
            std::cout << ext << std::endl;
        }
        std::cout << "Archive extensions: " << std::endl;
        for(const auto& ext : allowedArchiveExtensions)
        {
            std::cout << ext << std::endl;
        }
    }
#endif
};

Configuration ParseConfigFile(const std::filesystem::path& configPath);

#endif //LAB7_WORDS