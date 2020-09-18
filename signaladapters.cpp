#include "signaladapters.h"

SettingsSignalAdapter::SettingsSignalAdapter(const QString interface_string,
                                             const QString IP_string, const QString subnet_string,
                                             const QString gateway_string, const QString DNS_string,
                                             QObject *parent)
    : QObject(parent),
      interface_string(interface_string),
      gateway_string(gateway_string),
      DNS_string(DNS_string) {
    CIDR_string = IP_string + "/" + subnet_string;
}

void SettingsSignalAdapter::slot() {
    emit signal(interface_string, CIDR_string, gateway_string, DNS_string);
}

IDSignalAdapter::IDSignalAdapter(const int id, QObject *parent) : QObject(parent), id(id) {}

void IDSignalAdapter::slot() { emit signal(id); }
