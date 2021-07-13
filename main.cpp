#include <QCoreApplication>
//#include <QLocale>
//#include <QTranslator>


//#include <ServerGUI.h>
#include <SirenShell.h>
#include <ConsoleManager.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

////--- GUI stuff
////    QTranslator translator;
////    const QStringList uiLanguages = QLocale::system().uiLanguages();
////    for (const QString &locale : uiLanguages) {
////        const QString baseName = "Siren-Server_" + QLocale(locale).name();
////        if (translator.load(":/i18n/" + baseName)) {
////            a.installTranslator(&translator);
////            break;
////        }
////    }
//    ServerGUI w;
//    w.show();

//--- Shell stuff

    QString hostname, username, password;
    int port = -1;
    std::string pass;
    DatabaseManager *db;

    for (int x = 1; x < argc; x++){
        QString aux = argv[x];
        if ((aux.toUpper() == "-H") && (x+1 < argc)){
            hostname = argv[x+1];
            x++;
        } else {
            if ((aux.toUpper() == "-U") && (x+1 < argc)){
                username = argv[x+1];
                x++;
            } else {
                if ((aux.toUpper() == "-P") && (x+1 < argc)){
                    port = QString(argv[x+1]).toInt();
                    x++;
                } else {
                    std::cout << "Invalid SIREN server setup. Exiting... \n" << std::endl;
                    return 0;
                }
            }
        }
    }

    if (hostname.isEmpty()){
        std::string aux;
        std::cout << "Hostname: ";
        std::cin >> aux;
        hostname.fromStdString(aux);
    }

    if (username.isEmpty()){
        std::string aux;
        std::cout << "User: ";
        std::cin >> aux;
        username.fromStdString(aux);
    }

    if (port == -1){
        std::string aux;
        std::cout << "Port: ";
        std::cin >> aux;
        port = QString::fromStdString(aux).toInt();
    }

    std::cout << "Password: ";
    char ch = (char) ConsoleManager::upDownArrowOrRegularChar();
    while (ch != '\n'){
        if (!((ch == 8) || (ch == 127))){
            pass += ch;
            std::cout << "*";
        } else {
            if (pass.size() > 0){
                std::cout << '\b';
                std::cout << " ";
                std::cout << '\b';
                std::string aux;
                for (size_t x = 0; x < pass.size()-1; x++){
                    aux += pass[x];
                }
                pass.clear();
                pass = aux;
            }
        }
        ch = (char) ConsoleManager::upDownArrowOrRegularChar();
    }
    std::cout << std::endl;
    password.fromStdString(pass);


    try {
        db = new DatabaseManager(hostname, username, password);
    } catch (std::invalid_argument *e){
        std::cout << "Fatal error: " << e->what();
        delete (e);
        return;
    }
    if (db->openConnection()){
        std::cout << "DB connection opened." << std::endl;
    } else {
        std::cout << "Can't establish the DB connection. " << std::endl;
        return 0;
    }

    QHostAddress address;
    address.setAddress(hostname);

    SirenShell shell(db, address, port);
    shell.startTcpServer();

    return a.exec();
}
