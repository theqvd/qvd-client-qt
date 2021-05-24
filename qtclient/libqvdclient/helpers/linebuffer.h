#ifndef LINEBUFFER_H
#define LINEBUFFER_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QList>

#include "qvdclient_global.h"

/**
 * @brief Reassembles output read from a process into lines
 *
 * This class takes an incoming stream of text from a process or a similar source,
 * and extracts whole lines from it. Partial lines are left in the buffer, waiting
 * for more data to arrive.
 *
 * The intention is to deal with parsing application output, where applications may
 * be writing a single line in multiple write calls.
 *
 * The class works both for Unix, Windows and Mac style line endings.
 *
 * @warning Due to the indiscriminate detection of line endings, any contiguous sequence
 * of \r and \n is counted as a single line ending, so empty lines will be removed.
 *
 */
class LIBQVDCLIENT_EXPORT LineBuffer
{
public:
    LineBuffer();

    void add(const QByteArray &data);

    /**
     * @brief hasLine
     * @return Whether there is a line to be read with getLine()
     */
    bool hasLine() const { return !m_lines.empty(); }

    /**
     * @brief count
     * @return Number of lines that can be read
     */
    int count() const { return m_lines.count(); }


    /**
     * @brief hasPartialLine
     * @return Whether there is an internally buffered part of a line
     */
    bool hasPartialLine() const { return !m_data.isEmpty();  }

    /**
     * @brief getLine
     * @return Returns a line from the buffer. Returns an empty string if there aren't any
     */
    QString getLine();

    /**
     * @brief getPartialLine
     * @return  Returns a partial line from the buffer, or an empty string
     */
    QString getPartialLine() const { return QString(m_data); }
private:

    //QString m_text_data;
    QByteArray m_data;
    QList<QString> m_lines;
};

#endif // LINEBUFFER_H
