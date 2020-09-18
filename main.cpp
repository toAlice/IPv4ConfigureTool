#include <windows.h>
#include <shellapi.h>

#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    const auto elevated = []() {
        BOOL fRet = FALSE;
        HANDLE hToken = NULL;
        if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
            TOKEN_ELEVATION Elevation;
            DWORD cbSize = sizeof(TOKEN_ELEVATION);
            if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation),
                                    &cbSize)) {
                fRet = Elevation.TokenIsElevated;
            }
        }
        if (hToken) { CloseHandle(hToken); }
        return fRet;
    };

    if (!elevated()) {
        ::ShellExecuteA(NULL, "runas",
                        QCoreApplication::applicationFilePath().toStdString().c_str(), "", NULL,
                        SW_SHOWNORMAL);
        exit(0);
    }

    MainWindow w;
    w.show();
    return a.exec();
}
