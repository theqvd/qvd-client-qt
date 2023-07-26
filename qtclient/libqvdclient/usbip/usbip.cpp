#include "usbip.h"
#include "helpers/pathtools.h"

#include <QDebug>
#include <QProcess>
#include <windows.h>
#include <processthreadsapi.h>
#include <shellapi.h>
#include <synchapi.h>
#include <WinUser.h>
#include <errhandlingapi.h>

UsbIp::UsbIp()
{
    m_usbip_binary = PathTools::findBin("usbipd");
    if ( m_usbip_binary.isEmpty() ) {
       qCritical() << "Failed to find usbip binary";
    }

}

UsbIp::~UsbIp()
{

}

bool UsbIp::unbindDevice(const USBDevice &dev) {
    return unbindDeviceByBusID(dev.busnum());
}

bool UsbIp::unbindDeviceByHardwareID(QString id)
{
    return runElevated({"unbind", "--hardware-id", id});
}

bool UsbIp::unbindDeviceByBusID(QString id)
{
    return runElevated({"unbind", "--busid", id});
}


quint16 UsbIp::getPort() const
{
    // TODO: Hardcoded for now
    return 3240;
}

bool UsbIp::bindDevice(const USBDevice &dev) {
    return bindDeviceByBusID(dev.busnum());
}

bool UsbIp::bindDeviceByHardwareID(QString id)
{
    return runElevated({"bind", "--hardware-id", id});
}

bool UsbIp::bindDeviceByBusID(QString id)
{
    return runElevated({"bind", "--busid", id});
}





QProcess* UsbIp::createProcess()
{
    QProcess* proc = new QProcess(this);
    proc->setProgram(m_usbip_binary);

    QObject::connect(proc, &QProcess::started, this, &UsbIp::processStarted);
    QObject::connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,  &UsbIp::processFinished);
    QObject::connect(proc, &QProcess::readyReadStandardError, this, &UsbIp::processStderrReady);
    QObject::connect(proc, &QProcess::readyReadStandardOutput, this, &UsbIp::processStdoutReady);


    return proc;
}

int UsbIp::runElevated(const QStringList &args)
{
    QString params = args.join(" ");

    DWORD retval = 0;

    qInfo() << "Running elevated: " << m_usbip_binary << params;

    wchar_t *wFile = new wchar_t[m_usbip_binary.length()+1];
    wchar_t *wParams = new wchar_t[params.length()+1];

    m_usbip_binary.toWCharArray(wFile);
    wFile[m_usbip_binary.length()] = 0;

    params.toWCharArray(wParams);
    wParams[params.length()] = 0;



    SHELLEXECUTEINFOW ShExecInfo = {0};
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb =  L"runas";
    ShExecInfo.lpFile = wFile;
    ShExecInfo.lpParameters = wParams;
    ShExecInfo.lpDirectory = NULL;
    ShExecInfo.nShow = SW_HIDE;
    ShExecInfo.hInstApp = NULL;
    BOOL ret = ShellExecuteExW(&ShExecInfo);
    if (ret) {
        WaitForSingleObject(ShExecInfo.hProcess,INFINITE);


        if (GetExitCodeProcess(ShExecInfo.hProcess, &retval)) {
            qInfo() << "Process terminated with code" << retval;
        } else {
            qCritical() << "Failed to get return code with status" << GetLastError();
        }

    } else {
        qCritical() << "Execution failed with status" << GetLastError();
    }

    qInfo() << "Elevated command done";
    delete[] wFile;
    delete[] wParams;
    return retval;
}



#ifdef WIN32
void UsbIp::processStarted()
{
        qInfo() << "USBIP started";
}

void UsbIp::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
        qInfo() << "USBIP exited with code " << exitCode << ", status " << exitStatus;
}

void UsbIp::processStderrReady()
{
    QProcess *usbip_process = dynamic_cast<QProcess*>(sender());

    const QString error = usbip_process->readAllStandardError();
    qInfo() << "USBIP ERR: " << error;
}

void UsbIp::processStdoutReady()
{
        QProcess *usbip_process = dynamic_cast<QProcess*>(sender());
        QByteArray data = usbip_process->readAllStandardOutput();
        qInfo() << "USBIP OUT: " << data;
}

#endif
