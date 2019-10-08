#include "qvdbackend.h"

QVDBackend::QVDBackend(QObject *parent) : QObject(parent)
{

}

QVDBackend::~QVDBackend()
{

}

QVDConnectionParameters QVDBackend::parameters() const
{
    return m_parameters;
}

void QVDBackend::setParameters(const QVDConnectionParameters &parameters)
{
    m_parameters = parameters;
}

int64_t QVDBackend::bytesRead()
{
    return 0;
}

int64_t QVDBackend::bytesWritten()
{
    return 0;
}
