#include "AirlineSystem.h"
#include "customsqlquerymodel.h"
#include "bookbuttondelegate.h"

using namespace std;

AirlineSystem::AirlineSystem(QWidget* parent)
    : QMainWindow(parent)
{
    login_ui.setupUi(this);
    connect(login_ui.loginBtn, SIGNAL(clicked()), this, SLOT(onLoginButtonClicked()));
    connect(login_ui.createAccBtn, SIGNAL(clicked()), this, SLOT(onCreateAccBtnClicked()));
    connect(login_ui.googleBtn, SIGNAL(clicked()), SLOT(on_googleBtnClicked()));
    connect(login_ui.resetPwBtn, SIGNAL(clicked()), SLOT(onResetPwBtnClicked()));

    database = QSqlDatabase::addDatabase("QMYSQL");
    database.setDatabaseName("airlinesystem");
    database.setPort(3306);
    database.setHostName("127.0.0.1");
    database.setUserName("root");
    database.setPassword("Eyad1634!e");
    if (!database.open()) {
        qDebug() << "Database not connected!";
    }
    else {
        qDebug() << "Database connected!";
    }   
}

AirlineSystem::~AirlineSystem()
{}


void AirlineSystem::onCreateAccBtnClicked() {
    login_ui.centralWidget->hide();
    signup_ui.setupUi(this);
    connect(signup_ui.signupBtn, SIGNAL(clicked()), this, SLOT(on_signupBtnClicked()));
}


void AirlineSystem::on_signupBtnClicked() {
    // Get details entered
    QString fname = signup_ui.fnameLineEdit->text();
    QString username = signup_ui.usernameLineEdit->text();
    QString email = signup_ui.emailLineEdit->text();
    QString password = signup_ui.passwordLineEdit->text();

    // Hashing password
    QByteArray passwordHash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();
    QString passwordHashStr(passwordHash);

    // Validate details
    if (fname.isEmpty() || username.isEmpty() || email.isEmpty() || password.isEmpty()) {
        QMessageBox::information(this, tr("Failed"), tr("Please fill all the details... Try again!"));
    }
    else if (!email.contains("@") || !email.contains(".")) {
        QMessageBox::information(this, tr("Failed"), tr("Your email is incorrect... Try again!"));
    }
    else {
        QSqlQuery query;
        query.prepare("INSERT INTO customers (full_name, username, email, password_hash) VALUES (:full_name, :username, :email, :password_hash)");
        query.bindValue(":full_name", fname);
        query.bindValue(":username", username);
        query.bindValue(":email", email);
        query.bindValue(":password_hash", passwordHashStr);

        if (!query.exec()) {
            QMessageBox::information(this, tr("Failed"), tr("Error adding to the database."));
        }
        else {
            QMessageBox::information(this, tr("Success"), tr("Your account was made! You can login now."));
            signup_ui.centralwidget->hide();
            login_ui.setupUi(this);
            connect(login_ui.loginBtn, SIGNAL(clicked()), this, SLOT(onLoginButtonClicked()));
        }
        
    }
}



void AirlineSystem::on_googleBtnClicked()
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);

    // Construct the URL for the OAuth 2.0 authorization endpoint
    QString authUrl = "https://accounts.google.com/o/oauth2/auth?";
    authUrl += "client_id=1071715202859-srdkunnaa6abndt7g1sq7mmhia941hbo.apps.googleusercontent.com";
    authUrl += "&redirect_uri=urn:ietf:wg:oauth:2.0:oob";
    authUrl += "&response_type=code";
    authUrl += "&scope=https://www.googleapis.com/auth/userinfo.email https://www.googleapis.com/auth/userinfo.profile";
    authUrl += "&access_type=offline";

    // Open the user's default web browser and redirect them to the authorization endpoint
    QDesktopServices::openUrl(QUrl(authUrl));

    // Instruct the user to copy and paste the authorization code from the browser
    QMessageBox::StandardButton reply = QMessageBox::information(this, "Authorization Required", "Please authorize the application by visiting the URL you were directed to and copying the authorization code.", QMessageBox::Ok);

    if (reply == QMessageBox::Ok) {
        // Prompt the user to enter the authorization code
        QString authorizationCode = QInputDialog::getText(this, "Enter Authorization Code", "Please enter the authorization code:");

        // Construct the URL for the OAuth 2.0 token endpoint
        QUrl tokenUrl("https://oauth2.googleapis.com/token");

        // Construct the POST request body with the authorization code and other required parameters
        QUrlQuery postData;
        postData.addQueryItem("code", authorizationCode);
        postData.addQueryItem("client_id", "1071715202859-srdkunnaa6abndt7g1sq7mmhia941hbo.apps.googleusercontent.com");
        postData.addQueryItem("client_secret", "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");
        postData.addQueryItem("redirect_uri", "urn:ietf:wg:oauth:2.0:oob");
        postData.addQueryItem("grant_type", "authorization_code");

        // Send the POST request to the token endpoint to exchange the authorization code for an access token and refresh token
        QNetworkReply* reply = manager->post(QNetworkRequest(tokenUrl), postData.toString(QUrl::FullyEncoded).toUtf8());
        if (reply) {
            while (!reply->isFinished()) {
                qApp->processEvents();
            }

            // Parse the JSON response and extract the access token and refresh token
            QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
            QString accessToken = json.object().value("access_token").toString();
            QString refreshToken = json.object().value("refresh_token").toString();

            // Use the access token to make a request to the Google People API to retrieve the user's profile information
            QUrl peopleUrl("https://people.googleapis.com/v1/people/me?personFields=emailAddresses,names");
            QNetworkRequest peopleRequest(peopleUrl);
            QString apiKey = "AIzaSyByUDETi4-wDG9QlvuT0Q416ChepJpFq2I";
            peopleRequest.setRawHeader("Authorization", QString("Bearer %1").arg(accessToken).toUtf8());
            peopleRequest.setRawHeader("X-Goog-Api-Key", apiKey.toUtf8());

            QNetworkReply* peopleReply = manager->get(peopleRequest);
            while (!peopleReply->isFinished()) {
                qApp->processEvents();
            }

            // Parse the JSON response and extract the user's email and name
            QJsonDocument det_json = QJsonDocument::fromJson(peopleReply->readAll());
            qDebug() << "JSON Response:" << det_json.toJson(QJsonDocument::Indented);

            if (peopleReply->error() == QNetworkReply::NoError) {
                // extract email and name from the json response
                QString email = det_json.object().value("emailAddresses").toArray().at(0).toObject().value("value").toString();
                QString name = det_json.object().value("names").toArray().at(0).toObject().value("displayName").toString();

                //TO BE ADDED (ADDS DETAILS TO DATABASE)
                QMessageBox::information(this, tr("Success"), tr("Your account was made! You have been logged in!"));
                signup_ui.centralwidget->hide();
                search_ui.setupUi(this);
                connect(search_ui.searchBtn, SIGNAL(clicked()), this, SLOT(onSearchBtnClicked()));
            }
            else {
                // Handle any errors from the Google People API
                QMessageBox::critical(this, tr("Error"), tr("Error retrieving profile information from Google People API: %1").arg(peopleReply->errorString()));
            }

            // Clean up
            delete manager;
        }
    }
}



void AirlineSystem::onLoginButtonClicked() {
    QString email = login_ui.emailline->text();
    QString password = login_ui.passwordline->text();

    // Hashing password
    QByteArray passwordHash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();
    QString passwordHashStr(passwordHash);

    if (email.isEmpty() || password.isEmpty()) {
        QMessageBox::information(this, tr("Failed"), tr("Please fill all the details... Try again!"));
    }
    else {
        QSqlQuery query;
        query.prepare("SELECT * FROM customers WHERE email = :email AND password_hash = :passwordHash");
        query.bindValue(":email", email);
        query.bindValue(":passwordHash", passwordHashStr);
        query.exec();

        if (query.next()) {
            QMessageBox::information(this, tr("Success"), tr("You are logged in!"));
            login_ui.centralWidget->hide();

            // Save customer ID
            loggedinCustomerID = query.value("customer_id").toInt();

            bool isAdmin = query.value("admin").toBool();

            if (isAdmin) {
                admin_ui.setupUi(this);
                // Connect signals and slots for the admin UI
            }
            else {
                search_ui.setupUi(this);
                connect(search_ui.searchBtn, SIGNAL(clicked()), this, SLOT(onSearchBtnClicked()));
            }
        }
        else {
            QMessageBox::information(this, tr("Failed"), tr("Invalid username or password."));
        }

    }
}

void AirlineSystem::onResetPwBtnClicked() {
    login_ui.centralWidget->hide();
    reset_ui.setupUi(this);
    // Connect reset ui button
}

void AirlineSystem::onSearchBtnClicked()
{
    QString departureAirportCode = search_ui.deptAirport->currentText();
    QString arrivalAirportCode = search_ui.arrAirport->currentText();
    QString departureDate = search_ui.deptDate->text();
    QString maxPrice = search_ui.maxPrice->text();
    QString sort = search_ui.sort->currentText();

    QSqlQuery query;
    query.prepare("SELECT * FROM flights WHERE departure_airport = :departure AND arrival_airport = :arrival AND departure_date = :date");
    query.bindValue(":departure", departureAirportCode);
    query.bindValue(":arrival", arrivalAirportCode);
    query.bindValue(":date", departureDate);
    

    if (!query.exec()) {
        qDebug() << "Error: Unable to execute search query.";
        return;
    }

    CustomSqlQueryModel* model = new CustomSqlQueryModel();
    model->setQuery(query);

    search_ui.centralWidget->hide();
    results_ui.setupUi(this);

    results_ui.resultsTableView->setModel(model);
    results_ui.resultsTableView->verticalHeader()->setVisible(false);

    // Set the custom delegate for the "Book" button column
    BookButtonDelegate* delegate = new BookButtonDelegate(results_ui.resultsTableView);
    results_ui.resultsTableView->setItemDelegateForColumn(model->columnCount() - 1, delegate);

    connect(delegate, &BookButtonDelegate::bookButtonClicked, this, &AirlineSystem::onBookButtonClicked);
}


void AirlineSystem::onBookButtonClicked(const QModelIndex& index)
{
    QString flightNumber = index.sibling(index.row(), 0).data().toString();

    // Get the customer ID from the logged-in user
    int customerId = loggedinCustomerID;

    // Add booking to the "reservations" table
    QSqlQuery query;
    query.prepare("INSERT INTO reservations (flight_number, customer_id) VALUES (:flightNumber, :customerID)");
    query.bindValue(":flightNumber", flightNumber);
    query.bindValue(":customerID", customerId);
    bool success = query.exec();

    if (success) {
        QMessageBox::information(this, tr("Success"), tr("Booking successful!"));
    }
    else {
        QMessageBox::warning(this, tr("Failed"), tr("Booking failed! Please try again."));
    }
}


void AirlineSystem::retrieveReservationData()
{
    // load sql query
    QSqlQuery query;
    query.prepare("SELECT * FROM reservations");
    if (!query.exec()) {
        qDebug() << query.lastError().text();
        return;
    }

    // print results
    while (query.next()) {
        int reservationId = query.value(0).toInt();
        int customerId = query.value(1).toInt();
        int flightId = query.value(2).toInt();
        qDebug() << "Reservation ID:" << reservationId << "Customer ID:" << customerId << "Flight ID:" << flightId;
    }
}



// Ahmed Taha's part

//// Define a class to store flight information
//class Flight {
//public:
//    string flightNumber;
//    string departureCity;
//    string arrivalCity;
//    string airlineCompany;
//    double price;
//    int duration;
//    string departureTime;
//    bool bookingStatus;
//};
//
//vector<Flight> searchFlights(const vector<Flight>& flights, const string& departureCity, const string& arrivalCity, const string& airlineCompany, double maxPrice) {
//    vector<Flight> results;
//
//    for (const Flight& flight : flights) {
//        if ((departureCity.empty() || flight.departureCity == departureCity)
//            && (arrivalCity.empty() || flight.arrivalCity == arrivalCity)
//            && (airlineCompany.empty() || flight.airlineCompany == airlineCompany)
//            && (maxPrice == 0 || flight.price <= maxPrice)) {
//            results.push_back(flight);
//        }
//    }
//
//    return results;
//}
//
//// Define a function to sort flights by different criteria
//void sortFlights(vector<Flight>& flights, const string& sortCriteria) {
//    if (sortCriteria == "price") {
//        sort(flights.begin(), flights.end(), [](const Flight& a, const Flight& b) {
//            return a.price < b.price;
//            });
//    }
//    else if (sortCriteria == "duration") {
//        sort(flights.begin(), flights.end(), [](const Flight& a, const Flight& b) {
//            return a.duration < b.duration;
//            });
//    }
//    else if (sortCriteria == "departureTime") {
//        sort(flights.begin(), flights.end(), [](const Flight& a, const Flight& b) {
//            return a.departureTime < b.departureTime;
//            });
//    }
//}
//
//// Define a function to print flight information
//void printFlight(const Flight& flight) {
//    cout << "Flight number: " << flight.flightNumber << endl;
//    cout << "Departure city: " << flight.departureCity << endl;
//    cout << "Arrival city: " << flight.arrivalCity << endl;
//    cout << "Airline company: " << flight.airlineCompany << endl;
//    cout << "Price: " << flight.price << endl;
//    cout << "Duration: " << flight.duration << endl;
//    cout << "Departure time: " << flight.departureTime << endl;
//    cout << endl;
//}
//
//// Define a function to export the list of flights to an Excel file
//void exportFlightData(const vector<Flight>& Reservedflights, const string& filename) {
//    // Open the output file
//    ofstream outfile(filename);
//
//    // Write the header row to the file
//    outfile << "Flight Number,Departure City,Departure Time,Arrival City,Airline Name,Price,Duration,Booking Status" << endl;
//
//    // Write each flight to the file
//    for (const auto& flight : Reservedflights) {
//        outfile << flight.flightNumber << ","
//            << flight.departureCity << ","
//            << flight.departureTime << ","
//            << flight.arrivalCity << ","
//            << flight.airlineCompany << ","
//            << flight.price << ","
//            << flight.duration << ","
//            << flight.bookingStatus << endl;
//    }
//
//    // Close the output file
//    outfile.close();
//
//    // Print a message to the console
//    cout << "Export complete. File saved as " << filename << endl;
//}
//
//// The customer class including sample fields (to be updated at end to match requirements)
//struct Traveler {
//    string name;
//    string passport_number;
//    string phone_number;
//    string travel_date;
//    bool archived;
//};
//
//// Vector acting as a database until furhter implementation
//vector<Traveler> travelers;
//
//
//// Add traveler functionality
//void add_traveler() {
//    Traveler new_traveler;
//    cout << "Enter name: ";
//    cin >> new_traveler.name;
//    cout << "Enter passport number: ";
//    cin >> new_traveler.passport_number;
//    cout << "Enter phone number: ";
//    cin >> new_traveler.phone_number;
//    cout << "Enter travel date: ";
//    cin >> new_traveler.travel_date;
//    new_traveler.archived = false;
//    travelers.push_back(new_traveler);
//    cout << "Traveler added successfully!\n";
//}
//
//// Edit traveler functionality
//void edit_traveler() {
//    string passport_number;
//    cout << "Enter passport number of traveler to edit: ";
//    cin >> passport_number;
//    for (int i = 0; i < travelers.size(); i++) {
//        if (travelers[i].passport_number == passport_number) {
//            cout << "Enter new name: ";
//            cin >> travelers[i].name;
//            cout << "Enter new passport number: ";
//            cin >> travelers[i].passport_number;
//            cout << "Enter new phone number: ";
//            cin >> travelers[i].phone_number;
//            cout << "Enter new travel date: ";
//            cin >> travelers[i].travel_date;
//            cout << "Traveler edited successfully!\n";
//            return;
//        }
//    }
//    cout << "Passport number not found.\n";
//}
//
//// Archive traveler functionality
//void archive_traveler() {
//    string passport_number;
//    cout << "Enter passport number of traveler to archive: ";
//    cin >> passport_number;
//    for (int i = 0; i < travelers.size(); i++) {
//        if (travelers[i].passport_number == passport_number) {
//            travelers[i].archived = true;
//            cout << "Traveler archived successfully!\n";
//            return;
//        }
//    }
//    cout << "Passport number not found.\n";
//}
//
//// Display traveler functionality
//void display_travelers() {
//    int filter_choice, sort_choice;
//    cout << "Select filter:\n";
//    cout << "1. All travelers\n";
//    cout << "2. Archived travelers\n";
//    cin >> filter_choice;
//    vector<Traveler> filtered_travelers;
//    if (filter_choice == 1) {
//        filtered_travelers = travelers;
//    }
//    else if (filter_choice == 2) {
//        for (int i = 0; i < travelers.size(); i++) {
//            if (travelers[i].archived) {
//                filtered_travelers.push_back(travelers[i]);
//            }
//        }
//    }
//    else {
//        cout << "Invalid filter.\n";
//        return;
//    }
//    cout << "Select sort:\n";
//    cout << "1. Sort by name\n";
//    cout << "2. Sort by passport number\n";
//    cout << "3. Sort by phone number\n";
//    cout << "4. Sort by travel date\n";
//    cin >> sort_choice;
//    if (sort_choice == 1) {
//        sort(filtered_travelers.begin(), filtered_travelers.end(), [](Traveler a, Traveler b) { return a.name < b.name; });
//    }
//    else if (sort_choice == 2) {
//        sort(filtered_travelers.begin(), filtered_travelers.end(), [](Traveler a, Traveler b) { return a.passport_number < b.passport_number; });
//    }
//    else if (sort_choice == 3) {
//        sort(filtered_travelers.begin(), filtered_travelers.end(), [](Traveler a, Traveler b) { return a.phone_number < b.phone_number; });
//    }
//    else if (sort_choice == 4) {
//        sort(filtered_travelers.begin(), filtered_travelers.end(), [](Traveler a, Traveler b) { return a.travel_date < b.travel_date; });
//    }
//    else {
//        cout << "Invalid sort.\n";
//        return;
//    }
//    cout << "Name\t\tPassport\Phone Number\tTravel Date\tArchived\n";
//    for (int i = 0; i < filtered_travelers.size(); i++) {
//        Traveler traveler = filtered_travelers[i];
//        cout << traveler.name << "\t" << traveler.passport_number << "\t" << traveler.phone_number << "\t" << traveler.travel_date << "\t" << (traveler.archived ? "Yes" : "No") << "\n";
//    }
//}