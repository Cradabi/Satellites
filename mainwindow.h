#include <QMainWindow>
#include <QTextEdit>
#include <QNetworkAccessManager>
#include <iostream>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void loadFromFile();
    void loadFromUrl();
    void processData(const QString &content);
    void saveStatistics();

private:
    QTextEdit *m_textEdit;
    QNetworkAccessManager *m_networkManager;
};
