#ifndef TOPICWINDOW_H
#define TOPICWINDOW_H

#include <QWidget>
#include <QCloseEvent>

#include <string>

using namespace std;


namespace Ui {
    class TopicWindow;
}

class TopicWindow : public QWidget {
    Q_OBJECT
    
public:
    explicit TopicWindow(QWidget *parent = 0);
    ~TopicWindow();
    
    void setTopic(QString topic);
    
private:
    Ui::TopicWindow *ui;
    string topic;
    
private slots:
    void publishTopic();
    void subscriptionStatus(bool status);
    
public slots:
    void receiveMessage(string message);
    
signals:
    void newMessage(string topic, string message);
    void addSubscription(string topic);
    void removeSubscription(string topic);
    void windowClosing(string topic);
    
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // TOPICWINDOW_H
