#include "mainwindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QNetworkReply>
#include <QUrl>
#include <QDateTime>
#include <QMap>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    QPushButton *fileButton = new QPushButton("Load from File", this);
    QPushButton *urlButton = new QPushButton("Load from URL", this);
    QPushButton *saveButton = new QPushButton("Save Statistics", this);
    m_textEdit = new QTextEdit(this);
    m_textEdit->setReadOnly(true);

    layout->addWidget(fileButton);
    layout->addWidget(urlButton);
    layout->addWidget(m_textEdit);
    layout->addWidget(saveButton);

    connect(fileButton, &QPushButton::clicked, this, &MainWindow::loadFromFile);
    connect(urlButton, &QPushButton::clicked, this, &MainWindow::loadFromUrl);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveStatistics);

    m_networkManager = new QNetworkAccessManager(this);
}

void MainWindow::loadFromFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open TLE File", "", "TLE Files (*.txt);;All Files (*)");
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Error", "Could not open file: " + file.errorString());
        return;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    processData(content);
}

void MainWindow::loadFromUrl()
{
    QString url = QInputDialog::getText(this, "Enter URL", "URL:");
    if (url.isEmpty())
        return;

    QNetworkReply *reply = m_networkManager->get(QNetworkRequest(QUrl(url)));
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError)
        {
            QMessageBox::critical(this, "Error", "Could not load data: " + reply->errorString());
            reply->deleteLater();
            return;
        }

        QString content = QString::fromUtf8(reply->readAll());
        reply->deleteLater();
        processData(content);
    });
}

void MainWindow::processData(const QString &content)
{
    QStringList lines = content.split("\n", QString::SkipEmptyParts);
    int satelliteCount = lines.size() / 3;
    QDateTime oldestDate = QDateTime::currentDateTime();
    QMap<int, int> launchYears;
    QMap<int, int> inclinations;

    for (int i = 0; i < lines.size(); i += 3)
    {
        QString line1 = lines[i + 1].trimmed();
        QString line2 = lines[i + 2].trimmed();

        // Extract launch year
        int year = line1.mid(9, 2).toInt();
        year += (year < 57) ? 2000 : 1900;
        launchYears[year]++;

        // Extract inclination
        int inclination = qRound(line2.mid(8, 8).toDouble());
        inclinations[inclination]++;

        // Check for oldest date
        QDateTime epochDate = QDateTime::fromString(line1.mid(18, 14), "yyDDD.dddddddd");
        if (epochDate < oldestDate)
            oldestDate = epochDate;
    }

    QString statistics;
    statistics += QString("Total satellites: %1\n").arg(satelliteCount);
    statistics += QString("Oldest data date: %1\n").arg(oldestDate.toString(Qt::ISODate));

    statistics += "\nLaunches by year:\n";
    for (auto it = launchYears.constBegin(); it != launchYears.constEnd(); ++it)
        statistics += QString("%1: %2\n").arg(it.key()).arg(it.value());

    statistics += "\nInclinations:\n";
    for (auto it = inclinations.constBegin(); it != inclinations.constEnd(); ++it)
        statistics += QString("%1°: %2\n").arg(it.key()).arg(it.value());

    m_textEdit->setText(statistics);
}

void MainWindow::saveStatistics()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Statistics", "", "Text Files (*.txt);;All Files (*)");
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Error", "Could not save file: " + file.errorString());
        return;
    }

    QTextStream out(&file);
    out << m_textEdit->toPlainText();
    file.close();

    QMessageBox::information(this, "Success", "Statistics saved successfully.");
}
