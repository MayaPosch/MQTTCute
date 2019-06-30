#ifndef TOPICWINDOW_H
#define TOPICWINDOW_H

#include <QWidget>
#include <QCloseEvent>

#include <string>


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
    std::string topic;
    
private slots:
    void publishTopic();
    void subscriptionStatus(bool status);
    
public slots:
    void receiveMessage(std::string message);
    
signals:
    void newMessage(std::string topic, std::string message);
    void addSubscription(std::string topic);
    void removeSubscription(std::string topic);
    void windowClosing(std::string topic);
    
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // TOPICWINDOW_H
