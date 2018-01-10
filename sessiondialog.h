#ifndef SESSIONDIALOG_H
#define SESSIONDIALOG_H

#include <QDialog>

#include <string>
#include <vector>

using namespace std;


// Types.
enum LoginType {
    LOGIN_TYPE_ANONYMOUS = 0,
    LOGIN_TYPE_PASSWORD,
    LOGIN_TYPE_CERTIFICATE
};

struct Session {
    string mqttHost;
    quint16 mqttPort;
    LoginType loginType;
    string username;
    string password;
    string caFile;
    string certFile;
    string keyFile;
    //vector<string> topics;
};


namespace Ui {
    class SessionDialog;
}

class SessionDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit SessionDialog(QWidget *parent = 0);
    ~SessionDialog();
    
signals:
    void updatedSession(Session s, bool newSession);
    
private slots:
    void okButton();
    void cancelButton();
    void browseCa();
    void browseCert();
    void browseKey();
    
public slots:
    void setNewSession(bool newSession = false);
    void setSessionData(Session s);
    
private:
    Ui::SessionDialog *ui;
    bool newSession;
};

#endif // SESSIONDIALOG_H
