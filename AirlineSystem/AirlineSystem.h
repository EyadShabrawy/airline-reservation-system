#pragma once

#include <QtWidgets/QMainWindow>
#include <QtSql>
#include <QSqlDatabase>
#include <QTableView>
#include "QMessageBox"
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QByteArray>
#include <QDesktopServices>
#include <QJsonArray>
#include <QInputDialog>
#include <QSqlQuery>
#include <QCryptographicHash>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include "ui_Signup.h"
#include "ui_Login.h"
#include "ui_Search.h"
#include "ui_Results.h"
#include "ui_Admin.h"
#include "ui_User.h"
#include "ui_ResetPassword.h"

class AirlineSystem : public QMainWindow
{
    Q_OBJECT

public:
    AirlineSystem(QWidget *parent = nullptr);
    ~AirlineSystem();

private:
    Ui::SignupWindow signup_ui;
    Ui::LoginWindow login_ui;
    Ui::SearchWindow search_ui;
    Ui::ResultsWindow results_ui;
    Ui::AdminWindow admin_ui;
    Ui::UserWindow user_ui;
    Ui::ResetPWindow reset_ui;
    QSqlDatabase database;
    QTableView* tableView;
    QAbstractTableModel* flightModel;
    int loggedinCustomerID;

private slots:
    // Signup Functions
    void on_signupBtnClicked();
    void on_googleBtnClicked();

    // Login Functions
    void onLoginButtonClicked();
    void onCreateAccBtnClicked();
    void onResetPwBtnClicked();

    // Search Functions
    void onSearchBtnClicked();

    // Booking Functions
    void retrieveReservationData();
    void onBookButtonClicked(const QModelIndex& index);

    // Reset Password Functions
    /*void updatePassword();*/
};
