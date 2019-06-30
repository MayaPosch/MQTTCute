#ifndef DISCOVERYWINDOW_H
#define DISCOVERYWINDOW_H

#include <QWidget>
#include <QCloseEvent>

#include <string>
#include <set>


namespace Ui {
    class DiscoveryWindow;
}

class DiscoveryWindow : public QWidget {
    Q_OBJECT

public:
    explicit DiscoveryWindow(QWidget *parent = 0);
    ~DiscoveryWindow();
    
    void setTopic(QString topic);

private:
    Ui::DiscoveryWindow *ui;
    std::string topic;
	std::set<std::string> topics;
    
private slots:
    void subscriptionStatus(bool status);
	void clearText();
    
public slots:
    void receiveMessage(std::string topic, std::string message);
    
signals:
    void addSubscription(std::string topic);
    void removeSubscription(std::string topic);
    void windowClosing(std::string topic);
    
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // DISCOVERYWINDOW_H
