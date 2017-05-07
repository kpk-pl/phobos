#include "Utils/FileAttributes.h"

namespace phobos { namespace utils {

#ifdef _WIN32
#include <windows.h>
namespace detail {
    unsigned filetimeToUnix(FILETIME const& filetime)
    {
        // Microseconds between 1601-01-01 00:00:00 UTC and 1970-01-01 00:00:00 UTC
        static const uint64_t EPOCH_DIFFERENCE_MICROS = 11644473600000000ull;

        uint64_t total_us = (((uint64_t)filetime.dwHighDateTime << 32) | (uint64_t)filetime.dwLowDateTime) / 10;
        total_us -= EPOCH_DIFFERENCE_MICROS;

        unsigned const sec = (time_t)(total_us / 1000000);
        //unsigned const usec = (useconds_t)(total_us % 1000000);

        return sec;
    }

    unsigned detailModTime(std::string const& fileName)
    {
        std::wstring wStr(fileName.begin(), fileName.end());
        HANDLE hFile = CreateFile(wStr.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            // TODO: maybe throw?
            return 0;
        }

        FILETIME creation, lastAccess, lastWrite;
        BOOL result = GetFileTime(hFile, &creation, &lastAccess, &lastWrite);
        if (!result)
        {
            // TODO: maybe throw error?
            return 0;
        }

        CloseHandle(hFile);

        return filetimeToUnix(lastWrite);
    }
} // namespace detail
#elif __linux__
#include <sys/stat.h>
#include <sys/types.h>
namespace detail {
    unsigned detailModTime(std::string const& fileName)
    {
        struct stat attrib;
        if (-1 == stat(fileName.c_str(), &attrib))
            return 0;
        return attrib.st_mtime;
    }
} // namespace detail
#else
#error "OS not supported"
#endif

unsigned lastModificationTime(std::string const& fileName)
{
    return detail::detailModTime(fileName);
}

}} // namespace phobos::utils

