#include "internetchecker.h"

internetChecker::internetChecker(QObject *parent) : QObject(parent)
{
    checkProcess = new QProcess(this);
    QSettings checkerStn(CHECKER_CONFIG, QSettings::IniFormat);
    checkerStn.value("checker/pingsNum").toInt()>0 ? pingsNum = checkerStn.value("checker/pingsNum").toInt() : pingsNum=3;
    checkerStn.value("checker/pingTimeoutMs").toInt()>0 ? pingProcTimeoutMs = checkerStn.value("checker/pingTimeoutMs").toInt() : pingProcTimeoutMs = 7000;
    checkerStn.value("checker/hostToPing").toString().trimmed().length()? host2ping.setAddress(checkerStn.value("checker/hostToPing").toString().trimmed()):
                                                                            host2ping.setAddress("8.8.8.8");
    connect(checkProcess, SIGNAL(finished(int)), this, SLOT(onCheckProcFinish()));
}

void internetChecker::outputCheckResult(netStatus status, int packetLoss)
{
    qDebug() << "Internet Connection Status:" << QVariant::fromValue(status).toString();
    qDebug() << "Packet Loss" << packetLoss << "%";
}

void internetChecker::check()
{
    qDebug() << "Start checking internet" << "[Pings num]" << pingsNum << "[Timeout]" << pingProcTimeoutMs << "[Host to ping]" << host2ping.toString();
    checkProcess->start("ping", QStringList() << "-c" << QString::number(pingsNum) << host2ping.toString());
    if (!checkProcess->waitForFinished(pingProcTimeoutMs))
    {
         checkProcess->close();
    }
}

void internetChecker::onCheckProcFinish()
{
     int packetLoss = 100;
     enum netStatus internetStatus = NO_INTERNET_OR_SERVER2PING_DOWN;

    if (checkProcess->isReadable())
    {
        QString procResult = checkProcess->readAll();
        packetLoss = getPacketLoss(procResult);
        if (!packetLoss) {
            internetStatus = OK;
        }
        else if (packetLoss!=100){
            internetStatus= UNSTABLE_INTERNET;
        }
        else {
            internetStatus= NO_INTERNET_OR_SERVER2PING_DOWN;
        }
    }

    outputCheckResult(internetStatus, packetLoss);
}

int internetChecker::getPacketLoss(QString rawData)
{
    int packetLoss = 100;
    if (rawData.contains("packet loss"))
    {
        QRegularExpression lossRegex;
        lossRegex.setPattern("(\\d+)% packet loss");
        QRegularExpressionMatch match = lossRegex.match(rawData);
        if (match.hasMatch())
        {
            packetLoss = match.captured(1).toInt();
        }
    }
    else
    {
        //packet loss string not found. Probably the process has been finished by timeout before packet loss string output
        int receivedPings = rawData.count("ttl=");
        packetLoss = 100 - qCeil((float)receivedPings/pingsNum*100);
    }

    return packetLoss;
}

internetChecker::~internetChecker(){
}
