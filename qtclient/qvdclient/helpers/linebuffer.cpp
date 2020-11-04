#include "linebuffer.h"

LineBuffer::LineBuffer()
{

}

void LineBuffer::add(const QByteArray &data)
{
    m_data.append(data);

    int start_pos = 0;
    int newline_pos = 0;
    int byte_count = m_data.length();


    while( start_pos < byte_count && newline_pos < byte_count ) {

        for(newline_pos = start_pos; newline_pos < byte_count; newline_pos++) {
            if (m_data.at(newline_pos) == '\n' || m_data.at(newline_pos) == '\r') {
                QString line = QString::fromUtf8(m_data.mid(start_pos, newline_pos - start_pos) );
                m_lines.append(line);

                while( (newline_pos < byte_count) && (m_data.at(newline_pos) == '\n' || m_data.at(newline_pos) == '\r') )   {
                    newline_pos++;
                }

                start_pos = newline_pos;
                break;
            }
        }

    }

    m_data.remove(0, start_pos);


    /*
    while ( (pos = getNewlinePos()) >= 0 ) {
        m_lines.append(m_text_data.left(pos));
        m_text_data.remove(0, pos);

        while( m_text_data.length() > 0 && (m_text_data.front() == "\n" || m_text_data.front() == "\r")) {
            m_text_data.remove(0, 1);
        }
    }
    */
}

QString LineBuffer::getLine()
{
    if ( !m_lines.isEmpty() ) {
        return m_lines.takeFirst();
    } else {
        return QString("");
    }
}

