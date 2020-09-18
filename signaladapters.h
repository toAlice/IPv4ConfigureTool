#ifndef TRIGGER_H
#define TRIGGER_H

#include <QObject>

class SettingsSignalAdapter : public QObject {
    Q_OBJECT
public:
    explicit SettingsSignalAdapter(const QString interface_string, const QString IP_string,
                                   const QString subnet_string, const QString gateway_string,
                                   const QString DNS_string, QObject *parent = nullptr);

private:
    QString interface_string;
    QString CIDR_string;
    QString gateway_string;
    QString DNS_string;

private slots:
    void slot();

signals:
    void signal(const QString interface_string, const QString CIDR_string,
                const QString gateway_string, const QString DNS_string);
};

class IDSignalAdapter : public QObject {
    Q_OBJECT
public:
    explicit IDSignalAdapter(const int id, QObject *parent = nullptr);

    int id;

private slots:
    void slot();

signals:
    void signal(int id);
};

// class DeleteTrigger;

#endif  // TRIGGER_H
