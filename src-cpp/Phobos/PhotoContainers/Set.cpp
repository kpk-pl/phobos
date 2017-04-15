#include "PhotoContainers/Set.h"
#include "Config.h"

#ifdef _WIN32
#include <windows.h>
namespace {
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

        return filetimeToUnix(creation);
    }
} // unnamed namespace
#elif __linux__
#include <sys/stat.h>
#include <sys/types.h>
namespace {
    unsigned detailModTime(std::string const& fileName)
    {
        struct stat attrib;
        if (-1 == stat(fileName.c_str(), &attrib))
            return 0;
        return attrib.st_mtime;
    }
} // unnamed namespace
#else
#error "OS not supported"
#endif

namespace phobos { namespace pcontainer {

SeriesPtr const& Set::front() const
{
    assert(!photoSeries.empty());
    return photoSeries.front();
}

namespace {
    unsigned lastModTime(std::string const& fileName)
    {
        return detailModTime(fileName);
    }

    std::vector<std::vector<std::string> > divideToSeries(QStringList const& photos)
    {
        std::vector<std::pair<std::string, unsigned> > fileWithCreation;
        fileWithCreation.reserve(photos.size());

        for (auto const& photo : photos)
        {
            std::string const stdStr = photo.toStdString();
            fileWithCreation.emplace_back(stdStr, lastModTime(stdStr));
        }

        std::stable_sort(fileWithCreation.begin(), fileWithCreation.end(),
                         [](auto const& fc1, auto const& fc2) {
                             return fc1.second < fc2.second;
                         });

        std::vector<std::vector<std::string> > result;
        std::vector<std::string> current;
        unsigned last_time = 0;
        unsigned const timeThreshold = config::get()->get_qualified_as<unsigned>("photoSet.seriesTimeThreshold").value_or(2);

        for (auto const& fm : fileWithCreation)
        {
            if (!current.empty() && (fm.second - last_time > timeThreshold))
            {
                result.emplace_back(std::move(current));
                current.clear();
            }
            current.push_back(fm.first);
            last_time = fm.second;
        }

        if (!current.empty())
            result.emplace_back(std::move(current));

        return result;
    }
} // unnamed namespace

void Set::addPhotos(QStringList const& fileNames)
{
    std::vector<std::vector<std::string> > const photosInSeries = divideToSeries(fileNames);
    for (auto const& series : photosInSeries)
    {
        SeriesPtr seriesPtr = std::make_shared<Series>();
        seriesPtr->addPhotoItems(series);
        photoSeries.push_back(seriesPtr);
        emit newSeries(seriesPtr);
    }
}

SeriesPtr const& Set::findSeries(QUuid const& seriesUuid,
                                 int offset) const
{
    for (std::size_t i = 0; i<photoSeries.size(); ++i)
        if (photoSeries[i]->uuid() == seriesUuid)
            return photoSeries[(int(i)+offset) % photoSeries.size()];

    assert(false);
    return photoSeries.front(); // dummy
}

}} // namespace phobos::pcontainer
