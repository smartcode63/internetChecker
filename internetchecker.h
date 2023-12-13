#ifndef INTERNETCHECKER_H
#define INTERNETCHECKER_H

#define CONF_DIRS_ROOT QCoreApplication::applicationDirPath()
#define CHECKER_CONFIG CONF_DIRS_ROOT+"/config.ini"

#include <QObject>
#include <QtCore>
#include <QHostAddress>
#include <QRegularExpression>
#include <QtMath>

class internetChecker : public QObject
{
    Q_OBJECT

private:
    QProcess* checkProcess;
    QHostAddress host2ping;
    int pingsNum;
    int pingProcTimeoutMs;

public:
    explicit internetChecker(QObject *parent = nullptr);
    ~internetChecker();
    void check();
    int getPacketLoss(QString rawData);
    enum netStatus {
        UNKNOWN,
        NO_INTERNET_OR_SERVER2PING_DOWN,
        UNSTABLE_INTERNET,
        OK
    };
    Q_ENUM (netStatus);
    void outputCheckResult(netStatus status, int packetLoss);

public slots:
    void onCheckProcFinish();

};

#endif // INTERNETCHECKER_H
