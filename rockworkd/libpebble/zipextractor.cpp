#include "zipextractor.h"

#include <QFileInfo>
#include <QDebug>
#include <QDir>

#include <quazip/quazipfile.h>
#include <quazip/quazip.h>

ZipExtractor::ZipExtractor()
{

}

bool ZipExtractor::unpackArchive(const QString &archiveFilename, const QString &targetDir)
{
    QuaZip zipFile(archiveFilename);
    if (!zipFile.open(QuaZip::mdUnzip)) {
        qWarning() << "Failed to open zip file" << zipFile.getZipName();
        return false;
    }

    foreach (const QuaZipFileInfo &fi, zipFile.getFileInfoList()) {
        QuaZipFile f(archiveFilename, fi.name);
        if (!f.open(QFile::ReadOnly)) {
            qWarning() << "could not extract file" << fi.name;
            return false;
        }
        qDebug() << "Inflating:" << fi.name;
        QFileInfo dirInfo(targetDir + "/" + fi.name);
        if (!dirInfo.absoluteDir().exists() && !dirInfo.absoluteDir().mkpath(dirInfo.absolutePath())) {
            qWarning() << "Error creating target dir" << dirInfo.absoluteDir();
            return false;
        }
        QFile of(targetDir + "/" + fi.name);
        if (!of.open(QFile::WriteOnly | QFile::Truncate)) {
            qWarning() << "Could not open output file for writing" << fi.name;
            f.close();
            return false;
        }
        of.write(f.readAll());
        f.close();
        of.close();
    }
    return true;
}
