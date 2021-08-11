#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>
#include <QCryptographicHash>
#include <QDebug>
#include <QLineEdit>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QRegExpValidator>


MainWindow::MainWindow(const QString &appId, const QString &securityKey, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mAppId(appId)
    , mSecurityKey(securityKey)
{
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);
    setWindowTitle("Translate");
    ui->setupUi(this);

    QRegExp rx("[a-zA-Z]{1,30}");
    QValidator *validator = new QRegExpValidator(rx, this);
    ui->lineEdit->setValidator(validator);
    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &MainWindow::slot_translate);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slot_translate()
{
    QCryptographicHash hash(QCryptographicHash::Md5);
    QString query = ui->lineEdit->text();
    QString salt = QString::number(QDateTime::currentMSecsSinceEpoch());

    QString signSrc = mAppId+query+salt+mSecurityKey;
    QString sign = QCryptographicHash::hash(signSrc.toLatin1(),QCryptographicHash::Md5).toHex();
    QString url = QString("http://api.fanyi.baidu.com/api/trans/vip/translate?q=%1&from=en&to=zh&appid=%2&salt=%3&sign=%4")
            .arg(query).arg(mAppId).arg(salt).arg(sign);


    QNetworkAccessManager *manager = new QNetworkAccessManager;
    QNetworkRequest req(url);
    manager->get(req);
    connect(manager, &QNetworkAccessManager::finished, this, [manager, this](QNetworkReply* reply){

        auto data = reply->readAll();
        int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if(code != 200) {
            setValue(data);
            return ;
        }
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(data, &err);
        if(err.error != QJsonParseError::NoError) {
            setValue(QString("json parse err : %1").arg(err.error));
            return ;
        }

        auto resJson = doc.object();
        if(resJson.contains("trans_result")) {
            auto arr = resJson.value("trans_result");
            if(arr.isArray()) {
                auto arrJson = arr.toArray();
                QString builder = "";
                for(int i = 0; i<arrJson.size(); i++) {
                    builder.append(arrJson.at(i).toObject().value("dst").toString());
                    builder.append(";");
                }
                setValue(builder);
            }
        }
        manager->deleteLater();

    });
}

void MainWindow::setValue(const QString &v)
{
    ui->label->setText(v);
}

