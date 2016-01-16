#ifndef ZIPEXTRACTOR_H
#define ZIPEXTRACTOR_H

#include <QString>

class ZipExtractor
{
public:
    ZipExtractor();

    static bool unpackArchive(const QString &archiveFilename, const QString &targetDir);
};

#endif // ZIPEXTRACTOR_H
