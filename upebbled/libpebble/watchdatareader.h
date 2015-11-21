#ifndef WATCHDATAREADER_H
#define WATCHDATAREADER_H

#include <QByteArray>
#include <QtEndian>
#include <QString>

class WatchDataReader {
public:
    WatchDataReader(const QByteArray &data):
        m_data(data)
    {
    }

    template <typename T>
    T read() {
        if (checkBad(sizeof(T))) return 0;
        const uchar *u = p();
        m_offset += sizeof(T);
        return qFromBigEndian<T>(u);
    }

    inline bool checkBad(int n = 0)
    {
        if (m_offset + n > m_data.size()) {
            m_bad = true;
        }
        return m_bad;
    }
    inline const uchar * p()
    {
        return reinterpret_cast<const uchar *>(&m_data.constData()[m_offset]);
    }
    inline void skip(int n)
    {
        m_offset += n;
        checkBad();
    }
    QString readFixedString(int n)
    {
        if (checkBad(n)) return QString();
        const char *u = &m_data.constData()[m_offset];
        m_offset += n;
        return QString::fromUtf8(u, strnlen(u, n));
    }
    QByteArray peek(int n) {
        return m_data.left(m_offset + n).right(n);
    }

private:
    QByteArray m_data;
    int m_offset = 0;
    bool m_bad = false;
};

#endif // WATCHDATAREADER_H
