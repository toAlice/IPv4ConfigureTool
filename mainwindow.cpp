#include "mainwindow.h"

#include <windows.h>
#include <shellapi.h>

#include <QAction>
#include <QDebug>
#include <QFile>
#include <QIntValidator>
#include <QMessageBox>
#include <QRegularExpression>
#include <QScrollBar>
#include <QTranslator>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "ui_mainwindow.h"

const QString MainWindow::NETSH = "C:\\Windows\\System32\\netsh.exe";
const QString MainWindow::ARG_PART_1 = "interface ipv4 set";
const QString MainWindow::ARG_PART_1_ADDR = "address name=";
const QString MainWindow::ARG_PART_1_DNS = "dns name=";
const QString MainWindow::ARG_SOURCE_DHCP = "source=dhcp";
const QString MainWindow::ARG_SOURCE_STATIC = "source=static";
const QString MainWindow::ARG_SOURCE_STATIC_CIDR = "address=";
const QString MainWindow::ARG_SOURCE_STATIC_GATEWAY = "gateway=";
const QString MainWindow::ARG_DNS_STATIC = "static";
const QString MainWindow::CONFIG_FILE_NAME = "settings.txt";

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), counter(0) {
    ui->setupUi(this);

    QTranslator translator;
    translator.load("SystemCallGenerateNetshObject_en_US");

    base_layout = new QHBoxLayout;
    ui->centralwidget->setLayout(base_layout);

    left_layout = new QVBoxLayout;
    base_layout->addLayout(left_layout);

    scroll = new QScrollArea;
    left_layout->addWidget(scroll);

    scroll_base_widget = new QWidget;
    scroll->setWidget(scroll_base_widget);

    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
    scroll->horizontalScrollBar()->setDisabled(true);

    profile_layout = new QVBoxLayout;
    scroll_base_widget->setLayout(profile_layout);

    DHCP_interface_edit = new QLineEdit;
    DHCP_interface_edit->setPlaceholderText(tr("Interface Name"));
    left_layout->addWidget(DHCP_interface_edit);
    auto *DHCP_button = new QPushButton(tr("Back to DHCP"));
    left_layout->addWidget(DHCP_button);
    connect(DHCP_button, SIGNAL(clicked()), this, SLOT(onDHCPPushButtonClicked()));

    right_layout = new QVBoxLayout;
    base_layout->addLayout(right_layout);

    settings_layout = new QGridLayout;
    right_layout->addLayout(settings_layout);

    interface_edit = new QLineEdit;
    settings_layout->addWidget(new QLabel(tr("Interface: ")), 0, 0, 1, 1, Qt::AlignRight);
    settings_layout->addWidget(interface_edit, 0, 1, 1, 4);

    IP_edit = new QLineEdit;
    subnet_edit = new QLineEdit;
    subnet_edit->setValidator(new QIntValidator(0, 32, this));
    settings_layout->addWidget(new QLabel(tr("IP: ")), 1, 0, 1, 1, Qt::AlignRight);
    auto *CIDR_layout = new QHBoxLayout;
    CIDR_layout->addWidget(IP_edit);
    CIDR_layout->addWidget(new QLabel(tr("/")));
    CIDR_layout->addWidget(subnet_edit);
    CIDR_layout->setStretch(0, 5);
    CIDR_layout->setStretch(1, 0);
    CIDR_layout->setStretch(2, 1);
    settings_layout->addLayout(CIDR_layout, 1, 1, 1, 4);

    gateway_edit = new QLineEdit;
    settings_layout->addWidget(new QLabel(tr("Gateway: ")), 2, 0, 1, 1, Qt::AlignRight);
    settings_layout->addWidget(gateway_edit, 2, 1, 1, 4);

    DNS_edit = new QLineEdit;
    settings_layout->addWidget(new QLabel(tr("DNS: ")), 3, 0, 1, 1, Qt::AlignRight);
    settings_layout->addWidget(DNS_edit, 3, 1, 1, 4);

    right_layout->addStretch();

    auto *save_button = new QPushButton;
    save_button->setText(tr("Save"));
    right_layout->addWidget(save_button);

    connect(save_button, SIGNAL(clicked()), this, SLOT(onSavePushButtonClicked()));

    QFile conf(CONFIG_FILE_NAME);
    if (conf.open(QIODevice::ReadOnly | QIODevice::Text)) {
        constexpr int BUFFER_SIZE = 64;
        constexpr int LINE_BUFFER_SIZE = 1024;
        char line_buffer[LINE_BUFFER_SIZE];
        char interface_buffer[BUFFER_SIZE];
        char IP_buffer[BUFFER_SIZE];
        char subnet_buffer[BUFFER_SIZE];
        char gateway_buffer[BUFFER_SIZE];
        char DNS_buffer[BUFFER_SIZE];
        qint64 size;
        while ((size = conf.readLine(line_buffer, LINE_BUFFER_SIZE)) > -1 &&
               size < LINE_BUFFER_SIZE) {
            ::memset(interface_buffer, 0, BUFFER_SIZE);
            ::memset(IP_buffer, 0, BUFFER_SIZE);
            ::memset(subnet_buffer, 0, BUFFER_SIZE);
            ::memset(gateway_buffer, 0, BUFFER_SIZE);
            ::memset(DNS_buffer, 0, BUFFER_SIZE);
            if (::strlen(line_buffer) > 0) {
                char *pl;
                char *pr;

                // interface
                {
                    pl = ::strchr(line_buffer, '\"');
                    if (!pl) { continue; }
                    pr = ::strchr(pl + 1, '\"');
                    if (!pr) { continue; }
                    ::memcpy(interface_buffer, pl + 1, pr - pl - 1);
                }

                // ip
                {
                    pl = ::strchr(pr + 1, '\"');
                    if (!pl) { continue; }
                    pr = ::strchr(pl + 1, '\"');
                    if (!pr) { continue; }
                    ::memcpy(IP_buffer, pl + 1, pr - pl - 1);
                }

                // subnet
                {
                    pl = ::strchr(pr + 1, '\"');
                    if (!pl) { continue; }
                    pr = ::strchr(pl + 1, '\"');
                    if (!pr) { continue; }
                    ::memcpy(subnet_buffer, pl + 1, pr - pl - 1);
                }

                // gateway
                {
                    pl = ::strchr(pr + 1, '\"');
                    if (!pl) { continue; }
                    pr = ::strchr(pl + 1, '\"');
                    if (!pr) { continue; }
                    ::memcpy(gateway_buffer, pl + 1, pr - pl - 1);
                }

                // DNS
                {
                    pl = ::strchr(pr + 1, '\"');
                    if (!pl) { continue; }
                    pr = ::strchr(pl + 1, '\"');
                    if (!pr) { continue; }
                    ::memcpy(DNS_buffer, pl + 1, pr - pl - 1);
                }

                QString interface_string = QString(interface_buffer).trimmed();
                QString IP_string = QString(IP_buffer).trimmed();
                QString subnet_string = QString(subnet_buffer).trimmed();
                QString gateway_string = QString(gateway_buffer).trimmed();
                QString DNS_string = QString(DNS_buffer).trimmed();

                try {
                    validate(interface_string, IP_string, subnet_string, gateway_string,
                             DNS_string);
                } catch (int) { continue; }

                addProfileWidget(interface_string, IP_string, subnet_string, gateway_string,
                                 DNS_string);
            }
        }

        conf.close();
    }

    QAction *quitAction = new QAction(tr("E&xit"), this);
    quitAction->setShortcut(tr("Q"));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    addAction(quitAction);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::closeEvent(QCloseEvent *event) {
    QFile config(CONFIG_FILE_NAME);
    if (!config.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error: cant open file" << CONFIG_FILE_NAME << "for write";
    } else {
        for (int i = 0; i < interface_strings.size(); ++i) {
            config.write("\"");
            config.write(interface_strings[i].toStdString().c_str());
            config.write("\", \"");
            config.write(IPs[i].toStdString().c_str());
            config.write("\", \"");
            config.write(subnets[i].toStdString().c_str());
            config.write("\", \"");
            config.write(gateways[i].toStdString().c_str());
            config.write("\", \"");
            config.write(DNSs[i].toStdString().c_str());
            config.write("\"\n");
        }

        config.close();
    }

    QMainWindow::closeEvent(event);
}

void MainWindow::setDHCP(const QString interface_string) {
    QString IP_parameters = QString(" %1 %2\"%3\" %4")
                                .arg(ARG_PART_1)
                                .arg(ARG_PART_1_ADDR)
                                .arg(interface_string)
                                .arg(ARG_SOURCE_DHCP);

    QString DNS_parameters = QString(" %1 %2\"%3\" %4")
                                 .arg(ARG_PART_1)
                                 .arg(ARG_PART_1_DNS)
                                 .arg(interface_string)
                                 .arg(ARG_SOURCE_DHCP);

    int result = reinterpret_cast<intptr_t>(
        ::ShellExecuteA(NULL, "runas", NETSH.toStdString().c_str(),
                        IP_parameters.toStdString().c_str(), NULL, SW_SHOWNA));
    ::ShellExecuteA(NULL, "runas", NETSH.toStdString().c_str(),
                    DNS_parameters.toStdString().c_str(), NULL, SW_SHOWNA);

    if (!(result & SE_ERR_ACCESSDENIED)) {
        ui->statusbar->showMessage(IP_parameters, 5000);
    } else {
        ui->statusbar->showMessage(tr("Ignoring..."), 3000);
    }
}

void MainWindow::addProfileWidget(const QString interface_string, const QString IP_string,
                                  const QString subnet_string, const QString gateway_string,
                                  const QString DNS_string) {
    auto *profile = new QFrame;
    auto *profile_base_layout = new QVBoxLayout;
    profile->setLayout(profile_base_layout);
    profile_layout->addWidget(profile);

    auto *interface_label = new QLabel(tr("Interface: ") + interface_string);
    auto *CIDR_label = new QLabel(tr("CIDR: ") + IP_string + tr("/") + subnet_string);
    auto *gateway_label = new QLabel(tr("Gateway: ") + gateway_string);
    auto *DNS_label = new QLabel(tr("DNS: ") + DNS_string);
    auto *button_layout = new QHBoxLayout;
    auto *confirm_button = new QPushButton(tr("Confirm"));
    auto *delete_button = new QPushButton(tr("Delete"));

    profile_base_layout->addWidget(interface_label);
    profile_base_layout->addWidget(CIDR_label);
    profile_base_layout->addWidget(gateway_label);
    if (DNS_string.length()) {
        profile_base_layout->addWidget(DNS_label);
    } else {
        delete DNS_label;
    }
    profile_base_layout->addLayout(button_layout);
    button_layout->addWidget(confirm_button);
    button_layout->addWidget(delete_button);
    button_layout->setStretch(0, 3);
    button_layout->setStretch(1, 1);

    auto *confirm_trigger = new SettingsSignalAdapter(interface_string, IP_string, subnet_string,
                                                      gateway_string, DNS_string, this);

    connect(confirm_button, SIGNAL(clicked()), confirm_trigger, SLOT(slot()));

    connect(
        confirm_trigger, SIGNAL(signal(const QString, const QString, const QString, const QString)),
        this,
        SLOT(onProfileWidgetClicked(const QString, const QString, const QString, const QString)));

    settings_signal_adapters.append(confirm_trigger);

    auto *id_trigger = new IDSignalAdapter(counter++, this);

    connect(delete_button, SIGNAL(clicked()), id_trigger, SLOT(slot()));
    connect(id_trigger, SIGNAL(signal(int)), this, SLOT(onDeletePushButtonClicked(int)));

    id_signal_adapters.append(id_trigger);

    interface_strings.append(interface_string);
    IPs.append(IP_string);
    subnets.append(subnet_string);
    gateways.append(gateway_string);
    DNSs.append(DNS_string);
}

bool MainWindow::validate(const QString interface_string, const QString IP_string,
                          const QString subnet_string, const QString gateway_string,
                          const QString DNS_string) {
    int exception = 0;
    // validate interface
    {
        if (!interface_string.length()) {
            exception |= ValidationException::InvalidInterfaceNameException;
        }
        std::for_each(interface_string.begin(), interface_string.end(),
                      [&exception](const QChar &ch) {
                          if (!(ch.isDigit() || ch.isLower() || ch.isUpper() || ch == '-' ||
                                ch == '_' || ch == ' ' || ch == '.')) {
                              exception |= ValidationException::InvalidInterfaceNameException;
                          }
                      });
    }

    // validate ip
    {
        QRegularExpression re(
            "^(25[0-5]|2[0-4][0-9]|[0-1]?[0-9]?[0-9])\\."
            "(25[0-5]|2[0-4][0-9]|[0-1]?[0-9]?[0-9])\\."
            "(25[0-5]|2[0-4][0-9]|[0-1]?[0-9]?[0-9])\\."
            "(25[0-5]|2[0-4][0-9]|[0-1]?[0-9]?[0-9])$");
        if (!re.match(IP_string).hasMatch()) { exception |= InvalidIPAddressException; }
    }

    // validate subnet
    {
        QRegularExpression re("^(3[0-2]|[0-2]?[0-9])$");
        if (!re.match(subnet_string).hasMatch()) { exception |= InvalidSubnetException; }
    }

    // validate gateway
    {
        QRegularExpression re(
            "^(25[0-5]|2[0-4][0-9]|[0-1]?[0-9]?[0-9])\\."
            "(25[0-5]|2[0-4][0-9]|[0-1]?[0-9]?[0-9])\\."
            "(25[0-5]|2[0-4][0-9]|[0-1]?[0-9]?[0-9])\\."
            "(25[0-5]|2[0-4][0-9]|[0-1]?[0-9]?[0-9])$");
        if (!re.match(gateway_string).hasMatch()) { exception |= InvalidGatewayException; }
    }

    // validate DNS
    {
        QRegularExpression re(
            "^((25[0-5]|2[0-4][0-9]|[0-1]?[0-9]?[0-9])\\."
            "(25[0-5]|2[0-4][0-9]|[0-1]?[0-9]?[0-9])\\."
            "(25[0-5]|2[0-4][0-9]|[0-1]?[0-9]?[0-9])\\."
            "(25[0-5]|2[0-4][0-9]|[0-1]?[0-9]?[0-9]))?$");
        if (!re.match(DNS_string).hasMatch()) { exception |= InvalidDNSAddressException; }
    }

    if (exception) { throw exception; }

    return true;
}

void MainWindow::onDHCPPushButtonClicked() {
    QString interface_string = DHCP_interface_edit->text().trimmed();

    setDHCP(interface_string);
}

void MainWindow::onProfileWidgetClicked(const QString interface_string, const QString CIDR_string,
                                        const QString gateway_string, const QString DNS_string) {
    QString IP_parameters = QString(" %1 %2\"%3\" %4 %5\"%6\" %7\"%8\" ")
                                .arg(ARG_PART_1)
                                .arg(ARG_PART_1_ADDR)
                                .arg(interface_string)
                                .arg(ARG_SOURCE_STATIC)
                                .arg(ARG_SOURCE_STATIC_CIDR)
                                .arg(CIDR_string)
                                .arg(ARG_SOURCE_STATIC_GATEWAY)
                                .arg(gateway_string);

    QString DNS_parameters = QString(" %1 %2\"%3\" %4 %5 ")
                                 .arg(ARG_PART_1)
                                 .arg(ARG_PART_1_DNS)
                                 .arg(interface_string)
                                 .arg(ARG_DNS_STATIC)
                                 .arg(DNS_string);

    int result = reinterpret_cast<intptr_t>(
        ::ShellExecuteA(NULL, "runas", NETSH.toStdString().c_str(),
                        IP_parameters.toStdString().c_str(), NULL, SW_SHOWNA));

    if (DNS_string.length()) {
        ::ShellExecuteA(NULL, "runas", NETSH.toStdString().c_str(),
                        DNS_parameters.toStdString().c_str(), NULL, SW_SHOWNA);
    }
    if (!(result & SE_ERR_ACCESSDENIED)) {
        ui->statusbar->showMessage(IP_parameters, 5000);
    } else {
        ui->statusbar->showMessage(tr("Ignoring..."), 3000);
    }
}

void MainWindow::onSavePushButtonClicked() {
    QString interface_string = interface_edit->text().trimmed();
    QString IP_string = IP_edit->text().trimmed();
    QString subnet_string = subnet_edit->text().trimmed();
    QString gateway_string = gateway_edit->text().trimmed();
    QString DNS_string = DNS_edit->text().trimmed();

    try {
        validate(interface_string, IP_string, subnet_string, gateway_string, DNS_string);
    } catch (int exception) {
        QString msg;
        bool first_line = true;
        if (exception & ValidationException::InvalidInterfaceNameException) {
            msg += tr("Invalid Interface Name.");
            first_line = false;
        }
        if (exception & ValidationException::InvalidIPAddressException) {
            if (!first_line) {
                msg += tr("\nInvalid IP Address.");
            } else {
                msg += tr("Invalid IP Address.");
            }
            first_line = false;
        }
        if (exception & ValidationException::InvalidSubnetException) {
            if (!first_line) {
                msg += tr("\nInvalid subnet.");
            } else {
                msg += tr("Invalid subnet.");
            }
            first_line = false;
        }
        if (exception & ValidationException::InvalidGatewayException) {
            if (!first_line) {
                msg += tr("\nInvalid Gateway Address.");
            } else {
                msg += tr("Invalid Gateway Address.");
            }
            first_line = false;
        }
        if (exception & ValidationException::InvalidDNSAddressException) {
            if (!first_line) {
                msg += tr("\nInvalid DNS Address.");
            } else {
                msg += tr("Invalid DNS Address.");
            }
            first_line = false;
        }
        QMessageBox msg_box;
        msg_box.setText(msg);
        msg_box.exec();
        return;
    }

    DHCP_interface_edit->setText(interface_string);

    addProfileWidget(interface_string, IP_string, subnet_string, gateway_string, DNS_string);
}

void MainWindow::onDeletePushButtonClicked(int id) {
    auto *frame = profile_layout->takeAt(id)->widget();
    assert(frame);
    auto *layout = frame->layout();
    assert(layout);
    auto *interface_label = layout->takeAt(0);
    assert(interface_label);
    auto *CIDR_label = layout->takeAt(0);
    assert(CIDR_label);
    auto *gateway_label = layout->takeAt(0);
    assert(gateway_label);
    auto *DNSorButton = layout->takeAt(0);
    assert(DNSorButton);
    if (DNSorButton->widget()) {
        auto *button_layout = layout->takeAt(0)->layout();
        assert(button_layout);
        auto *confirm_button = button_layout->takeAt(0);
        assert(confirm_button);
        auto *remove_button = button_layout->takeAt(0);
        assert(remove_button);
        delete remove_button;
        delete confirm_button;
        delete button_layout;
    } else {
        auto *confirm_button = DNSorButton->layout()->takeAt(0);
        assert(confirm_button);
        auto *remove_button = DNSorButton->layout()->takeAt(0);
        assert(remove_button);
        delete remove_button;
        delete confirm_button;
    }
    delete DNSorButton;
    delete gateway_label;
    delete CIDR_label;
    delete interface_label;
    delete layout;
    delete frame;

    assert(id < id_signal_adapters.size());
    delete id_signal_adapters.takeAt(id);
    assert(id < settings_signal_adapters.size());
    delete settings_signal_adapters.takeAt(id);

    assert(id < interface_strings.size());
    interface_strings.takeAt(id);
    assert(id < IPs.size());
    IPs.takeAt(id);
    assert(id < subnets.size());
    subnets.takeAt(id);
    assert(id < gateways.size());
    gateways.takeAt(id);
    assert(id < DNSs.size());
    DNSs.takeAt(id);

    for (int i = id; i < id_signal_adapters.size(); ++i) {
        assert(id_signal_adapters[i]);
        id_signal_adapters[i]->id -= 1;
    }
    --counter;
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        drag_position = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - drag_position);
        event->accept();
    }
    QMainWindow::mouseMoveEvent(event);
}
