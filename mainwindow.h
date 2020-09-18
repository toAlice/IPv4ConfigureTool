#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMainWindow>
#include <QPoint>
#include <QPushButton>
#include <QScrollArea>
#include <QString>
#include <QVBoxLayout>

#include "signaladapters.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    static const QString NETSH;
    static const QString ARG_PART_1;
    static const QString ARG_PART_1_ADDR;
    static const QString ARG_PART_1_DNS;
    static const QString ARG_SOURCE_DHCP;
    static const QString ARG_SOURCE_STATIC;
    static const QString ARG_SOURCE_STATIC_CIDR;
    static const QString ARG_SOURCE_STATIC_GATEWAY;
    static const QString ARG_DNS_STATIC;
    static const QString CONFIG_FILE_NAME;

    Ui::MainWindow *ui;

    QList<QString> interface_strings;
    QList<QString> IPs;
    QList<QString> subnets;
    QList<QString> gateways;
    QList<QString> DNSs;

    QList<SettingsSignalAdapter *> settings_signal_adapters;
    QList<IDSignalAdapter *> id_signal_adapters;

    QHBoxLayout *base_layout;
    QVBoxLayout *left_layout;
    QScrollArea *scroll;
    QWidget *scroll_base_widget;
    QVBoxLayout *profile_layout;
    QLineEdit *DHCP_interface_edit;
    QVBoxLayout *right_layout;
    QGridLayout *settings_layout;
    QLineEdit *interface_edit;
    QLineEdit *IP_edit;
    QLineEdit *subnet_edit;
    QLineEdit *gateway_edit;
    QLineEdit *DNS_edit;

    int counter;

    void setDHCP(const QString interface_string);
    void addProfileWidget(const QString interfac_string, const QString IP_string,
                          const QString subnet_string, const QString gateway_string,
                          const QString DNS_string);
    bool validate(const QString interface_string, const QString IP_string,
                  const QString subnet_string, const QString gateway_string,
                  const QString DNS_string);

    enum ValidationException : int {
        InvalidInterfaceNameException = 1,
        InvalidIPAddressException = 1 << 1,
        InvalidSubnetException = 1 << 2,
        InvalidGatewayException = 1 << 3,
        InvalidDNSAddressException = 1 << 4,
    };

    QPoint drag_position;

private slots:
    void onDHCPPushButtonClicked();
    void onProfileWidgetClicked(const QString interface_string, const QString CIDR_string,
                                const QString gateway_string, const QString DNS_string);
    void onSavePushButtonClicked();
    void onDeletePushButtonClicked(int id);

protected:
    void closeEvent(QCloseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
};

#endif  // MAINWINDOW_H
