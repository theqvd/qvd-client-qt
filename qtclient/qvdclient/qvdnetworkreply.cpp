#include "qvdnetworkreply.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTcpSocket>
#include <QDebug>
#include <QObject>
#include <QIODevice>



QVDNetworkReply::QVDNetworkReply(QObject *parent, const QNetworkRequest &req, const QNetworkAccessManager::Operation op, QIODevice *data, QTcpSocket &socket)
	: QNetworkReply(parent), m_socket(socket), m_buffer_pos(0) {


	connect(&m_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));



	QByteArray buf;

	switch(op) {
	case QNetworkAccessManager::Operation::HeadOperation: buf.append("HEAD "); break;
	case QNetworkAccessManager::Operation::GetOperation: buf.append("GET "); break;
	case QNetworkAccessManager::Operation::PutOperation: buf.append("PUT "); break;
	case QNetworkAccessManager::Operation::PostOperation: buf.append("POST "); break;
	case QNetworkAccessManager::Operation::DeleteOperation: buf.append("DELETE "); break;
	case QNetworkAccessManager::Operation::CustomOperation: break;
	case QNetworkAccessManager::Operation::UnknownOperation: break;
	}

	buf.append( req.url().path().toUtf8() );

	if ( req.url().query().length() > 0 ) {
		buf.append("?");
		buf.append( req.url().query().toUtf8() );
	}

	buf.append(" HTTP/1.1\n");

	for( auto hdr  : req.rawHeaderList() ) {
		buf.append(hdr);
		buf.append(": ");
		buf.append(req.rawHeader(hdr));
		buf.append("\n");
	}

	buf.append(("\n"));

	qInfo() << "REQUEST: " << buf;
	m_read_state = ReadState::HTTPResponse;
	socket.write(buf);
}

void QVDNetworkReply::readyRead() {
	QByteArray data;


	qInfo() << "readyRead()";
	while(m_socket.bytesAvailable() > 0 && m_read_state != ReadState::SwitchedProtocol ) {

		qInfo() << "Bytes available: " << m_socket.bytesAvailable();

		switch(m_read_state) {
			case ReadState::Idle:
				data = m_socket.readAll();
				qCritical() << "Received data " << data << " from socket when not expecting any!";
				break;
			case ReadState::HTTPResponse: {

				//./src/network/access/qnetworkreplyhttpimpl.cpp parseHttpOptionHeader ?

				if (!m_socket.canReadLine())
					return;

				data = m_socket.readLine();
				qInfo() << "Received HTTP response: " << data;

				if ( !data.startsWith("HTTP/1.1 ")) {
					qCritical() << "Not HTTP 1.1!";
					m_read_state = ReadState::Error;
					return;
				}

				QList<QByteArray> parts = data.split(' ');
				for(auto part : parts ) {
					qInfo() << "PART: " << part;
				}

				bool ok = false;
				int code = parts[1].toInt(&ok);

				this->setAttribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute, code);
				this->setAttribute(QNetworkRequest::Attribute::HttpReasonPhraseAttribute, parts[2]);
				//setErrorString(parts[2]);

				m_read_state = ReadState::Headers;

				}
				break;
			case ReadState::Headers:
				if (!m_socket.canReadLine())
					return;

				data = m_socket.readLine();
				if ( data == "\r\n") {
					qInfo() << "End of HTTP headers";

					int status_code = attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toInt();
					if ( status_code == 102 ) {
						// HTTP processing
						m_read_state = ReadState::HTTPResponse;
						emit processing();
					} else if ( status_code == 101 ) {
						// HTTP switching protocols
						m_read_state = ReadState::SwitchedProtocol;

						// The socket isn't ours anymore
                        disconnect(&m_socket, nullptr, this, nullptr);
						emit finished();
					} else {
						m_read_state = ReadState::Body;
					}
				} else {
					qInfo() << "Received HTTP header: " << data;
					QByteArray key;
					QByteArray value;


					for(int i=0;i<data.length();i++) {
						if ( data.at(i) == ':' ) {
							key = data.mid(0, i);
							value = data.mid(i+1);

							qInfo() << "Key = " << key << "; value = " << value;

							break;
						}
					}

					if ( key.length() ) {
						setRawHeader(key, value);
					} else {
						qCritical() << "Parse error";
						m_read_state = ReadState::Error;
					}


//					QList<QByteArray> parts = data.split(':');
//	/				data.
//					if (parts.count() != 2) {
//						qCritical() << "Parse error";
//						m_read_state = ReadState::Error;
//						return;
//					}
//
	//				setRawHeader(parts[0], parts[1]);
				}

				break;
			case ReadState::Body:
				data = m_socket.readAll();
				m_buffer.append(data);
				qInfo() << "Received HTTP body data: " << data;

				if ( m_buffer.length() == header(QNetworkRequest::ContentLengthHeader).toInt() ) {
					qInfo() << "HTTP download complete";
					m_read_state = ReadState::Idle;
					setOpenMode(OpenModeFlag::ReadOnly);
					emit finished();
				}


				break;
			case ReadState::SwitchedProtocol:
				// Nothing, the socket isn't ours anymore.
				break;
			case ReadState::Error:
				qFatal("Received data from socket during error state!");
                                //break;
		}
	}
}



void QVDNetworkReply::abort() {
	m_read_state = ReadState::Idle;
}

void QVDNetworkReply::close() {
	m_read_state = ReadState::Idle;
}

qint64 QVDNetworkReply::bytesAvailable() const {
	return m_buffer.length() - m_buffer_pos;
}

qint64 QVDNetworkReply::readData(char *data, qint64 maxlen) {
	Q_ASSERT( m_buffer.length() >= m_buffer_pos );

	qint64 bytes_read = qMin(m_buffer.length() - m_buffer_pos, maxlen);


	memcpy(data, m_buffer.data() + m_buffer_pos, bytes_read);
	m_buffer_pos += bytes_read;

	// Free the buffer once we're done reading
	if ( m_buffer_pos == m_buffer.length() ) {
		m_buffer.clear();
		m_buffer_pos = 0;
	}

	return bytes_read;
}

bool QVDNetworkReply::isSequential () const {
	// Non-sequential requires ability to seek
	return true;
}
