#include "tst_testexample.h"
#include "autotest.h"       // must include this in all tests

#include <QDebug>
#include <QApplication>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QTimer>
#include <QThread>

#include "qhttpserver.hpp"
#include "qhttpserverresponse.hpp"
#include "qhttpserverrequest.hpp"

using namespace qhttp::server;


TestExample::TestExample() : QObject{}
{}

TestExample::~TestExample()
{}

void TestExample::testExampleRequest_data()
{
  QTest::addColumn<QString>("path");
  QTest::addColumn<QString>("name");
  QTest::addColumn<QByteArray>("data");
  QTest::addColumn<HttpMethod>("method");

  QTest::newRow("simple-GET") << QStringLiteral("/sayHello")
                              << QString{} << QByteArray{} << GET;

  QTest::newRow("name-POST") << QStringLiteral("/sayHello")
                             << QStringLiteral("Matt")
                             << QByteArray{"Matt"} << POST;
}

void TestExample::testExampleRequest()
{
  QFETCH(QString, path);
  QFETCH(QString, name);
  QFETCH(QByteArray, data);
  QFETCH(HttpMethod, method);

  ExampleRequestMessage msg{name, path};
  QCOMPARE(msg.name(), name);
  QCOMPARE(msg.data(), data);
  QCOMPARE(msg.httpMethod(), method);

  ExampleRequestMessage msgreverse{data, path};
  QCOMPARE(msgreverse.name(), name);
  QCOMPARE(msgreverse.data(), data);
  QCOMPARE(msgreverse.httpMethod(), method);
}

void TestExample::testExampleResponse_data()
{
  QTest::addColumn<QString>("path");
  QTest::addColumn<QString>("name");
  QTest::addColumn<QByteArray>("data");
  QTest::addColumn<StatusCode>("method");

  QTest::newRow("simple-GET") << QStringLiteral("/sayHello")
                              << QString{} << QByteArray{"Hello World!"} << OK;

  QTest::newRow("name-POST") << QStringLiteral("/sayHello")
                             << QStringLiteral("Matt") << QByteArray{"Hello Matt!"} << OK;
}

void TestExample::testExampleResponse()
{
  QFETCH(QString, path);
  QFETCH(QString, name);
  QFETCH(QByteArray, data);
  QFETCH(StatusCode, method);

  ExampleResponseMessage msg(path, method, name);
  QCOMPARE(msg.name(), name);
  QCOMPARE(msg.data(), data);
  QCOMPARE(msg.statusCode(), method);

  ExampleResponseMessage msgreverse{data, path};
  QCOMPARE(msgreverse.name(), name);
  QCOMPARE(msgreverse.data(), data);
  QCOMPARE(msgreverse.statusCode(), method);
}


void TestExample::testQhttp()
{
  qDebug() << "Starting qhttp test.";


  // ================ Server code ==================
  // We are using the QHttp open source library for the server side.
  // This will be encapsulated in a class for the real code.

  qDebug() << "Initialising the server.";
  std::unique_ptr<qhttp::server::QHttpServer> server{std::make_unique<QHttpServer>(QApplication::instance())};

  // This port range is essential if you want to be able to test across
  // computers in the computer pools at Uni.
  quint16 port{8101};
  quint16 topPort{8103};


  // Start listening: attempt a few ports incase the first part is already
  // in use by another application.
  // (Allows testing multiple instances on the same computer.)
  do {
    server->listen(QHostAddress::Any, port,
    // We are using a lambda to respond to requests to the server,
    // you will likely use a slot for an appropriate class in the
    // real implementation.
    // The emitted signal provides the request it received as well
    // as an object that needs to be filled in with the response.
    [](QHttpRequest *qRequest, QHttpResponse *qResponse)
    {
      // This just logs some details so we can see what is going on.
      qDebug("Received Request\n  from %s:%d\n  method: %s url: %s",
             qRequest->remoteAddress().toUtf8().constData(),
             qRequest->remotePort(),
             qhttp::Stringify::toString(qRequest->method()),
             qPrintable(qRequest->url().toString()) );

      // Collect all HTTP body (data).
      // If you forget to do this, qRequest->collectedData() will be empty.
      qRequest->collectData();

      // In the real code you could decide whether you need to wait for
      // the onEnd signal based on the type of message (GET vs. POST, etc.)
      // I will not bother for this example, and just use the onEnd for all.
      // (You can also process data bit by bit with 'onData', but we should
      // be dealing with relatively small messages, not large uploads.)
      qRequest->onEnd([qRequest, qResponse]() {
        qDebug() << "data? " << qRequest->collectedData();

        std::unique_ptr<ResponseMessage> response{nullptr};

        if (qRequest->url().path() == "/sayHello") {
          // Process the request data using our ExampleRequestMessage
          ExampleRequestMessage exampleReq{qRequest->collectedData(), qRequest->url().path()};

          // Maybe do some validation on resulting message (i.e., check if it processed
          // the data correctly, if any). This example does not have any strict requirements
          // on the content, so we do not do anything.
          if (exampleReq.httpMethod() != static_cast<HttpMethod>(qRequest->method())) {
            QFAIL("Inferred HTTP method and actually received request do not match.");
          }

          // Prepare the response
          response = std::make_unique<ExampleResponseMessage>(exampleReq.urlPath(), OK, exampleReq.name());
        }
        else {
          // Be polite and tell the client we do not know what it is talking about
          response = std::make_unique<ResponseMessage>(qRequest->url(), NOT_FOUND);
        }

        // Configure and send the QHttp response using out response message.
        // response status code: e.g., 200 OK
        qResponse->setStatusCode(response->qhttpStatusCode());

        // HTTP headers, e.g., Content-Type: text/plain
        for (auto k : response->headers().keys()) {
          qResponse->addHeader(k.toUtf8(), (*response)[k].toUtf8());
        }

        // Specify the Content-Length, it is good practice, and necessary if were
        // were usign Keep-Alive.
        qResponse->addHeaderValue("Content-Length", response->data().size());

        // Response body data
        qResponse->end(response->data());
      } );
    } );
  } while (!server->isListening() and ++port <= topPort);

  // Remove this in the real code...
  QVERIFY2(server->isListening(), "Unable to start QHttpServer.");

  // But you can keep this logging in.
  if (server->isListening()) {
    qDebug() << "Server listening on port " << port;
  }
  else {
    qDebug() << "Unable to start QHttpServer.";
  }

  // ================ Client code ==================
  // We are using the standard QtNetworkAccessManager for the client side.
  // This will be encapsulated in a class for the real code.

  // For HTTP messages, get, put, post, etc.
  QNetworkAccessManager nm{QApplication::instance()};

  int done{0};

  // Send example message to self (the timer is just to put in on the event
  // queue so that this function can finish executing and the server can
  // actually respond; you will not need to use the timer in the real code).
  QTimer::singleShot(1, [&nm, port, &done]() {
    ExampleRequestMessage request(QStringLiteral("/sayHello"));

    // Configure the standard Qt request with out ExampleRequestMessage

    // Construct the complete URL: the real code will send messages out to multiple servers.
    QString urlString{QString{"http://%1:%2%3"}.arg("localhost").arg(port).arg(request.urlPath())};
    qDebug() << "Sending request to : " <<  urlString;

    // Set the url: e.g., http://localhost:8101/sayHello
    QNetworkRequest qRequest{QUrl{urlString}};

    // HTTP headers, e.g., Content-Type: text/plain
    for (auto k : request.headers().keys()) {
      qRequest.setRawHeader(k.toUtf8(), request[k].toUtf8());
    }

    // Send the request and get the placeholder reply object.
    // You attach a listener to the reply object so that your code
    // is notified when it receives the response from the server.
    QNetworkReply *reply{nullptr};
    if (request.httpMethod() == GET) {
      // nm.get(...) sends the request as a GET,
      qDebug() << "GETing resource";
      reply = nm.get(qRequest);
    }
    else if (request.httpMethod() == POST) {
      // nm.post(...) sends the request as a POST with the specified data
      qDebug() << "POSTing data: " << request.data();
      reply = nm.post(qRequest, request.data());
    }
    else {
      // You would also include PUT and DELETE in the complete application.
      // These would be done with nm.put(...) and nm.deleteResource(...), respectively.
      // But in this case we know it will either be GET or POST, so anything
      // else is a problem, so just bail.
      QFAIL("Unexpected HTTP Method, should have been GET or POST"); // for the test
      return;
    }

    // Connect a slot to the reply so we can handle the response from the server.
    QObject::connect(reply, &QNetworkReply::finished, [reply, &done](){
      // Retrieve the data once (we use in the logging below)
      QByteArray data{reply->readAll()};

      // You can keep this logging so you know what is happening.
      qDebug() << "Received reponse:\n  from: " << reply->url()
               << "\n  content: " << data;

      // Convert the way status is reported by the Qt networking stuff into our enum
      StatusCode status{static_cast<StatusCode>(reply->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toInt())};

      // Read the response using our messages
      ExampleResponseMessage response{data, reply->url().path(), status};

      // Schedules the reply object to be deleted. Do not forget to
      // do this, otherwise you will begin to run out of memoery if
      // your application is running for too long.
      reply->deleteLater();

      // Process the response. Here we just display the name the server echoed back, which should be empty here
      // You might also want to validate that the Message could acutally interpret the data correctly.
      // Just incase the content is in the wrong format or something.
      if (response.statusCode() == OK and reply->error() == QNetworkReply::NoError) {
        qDebug() << "The server spoke to: " << response.name();
        QCOMPARE(response.name(), QStringLiteral(""));
      }
      else {
        // Handle error
        qDebug() << "Maybe do something with the read content here.\n"
                    "Errors can still return information from the server";
        switch (response.statusCode()) {
        case OK:
          qDebug() << "We recieved OK, why an error?";
          break;
        case CREATED:
          qDebug() << "CREATED is technically not an error what what is expected from a POST in our API.";
          break;
        case BAD_REQUEST:
        case CONFLICT:
        case NOT_FOUND:
        case NOT_IMPLEMENTED:
        case SERVER_ERROR:
          qDebug() << "Really you would do something use ful here.";
          break;
        }
        QFAIL("An unexpected error occurred.");
      }

      ++done; // this is just for testing, so we can assert that something actually happened
      // finish the test as quickly as possible, real code would not include this.
      if (done == 2) QCoreApplication::instance()->quit();
    } );

  } );

  // *** Send a second message, this time we send our name as part of the message.
  // *** for brevity all of the comments ahve been removed, see the above example.
  QTimer::singleShot(1, [&nm, port, &done]() {
    // *** this is basically the only difference this time
    // *** your real-code should not have this type of duplication.
    ExampleRequestMessage request(QStringLiteral("Matt"), QStringLiteral("/sayHello"));

    QString urlString{QString{"http://%1:%2%3"}.arg("localhost").arg(port).arg(request.urlPath())};
    qDebug() << "Sending request to : " <<  urlString;
    QNetworkRequest qRequest{QUrl{urlString}};
    for (auto k : request.headers().keys()) {
      qRequest.setRawHeader(k.toUtf8(), request[k].toUtf8());
    }
    QNetworkReply *reply{nullptr};
    if (request.httpMethod() == GET) {
      qDebug() << "GETing resource";
      reply = nm.get(qRequest);
    }
    else if (request.httpMethod() == POST) {
      qDebug() << "POSTing data: " << request.data();
      reply = nm.post(qRequest, request.data());
    }
    else {
      QFAIL("Unexpected HTTP Method, should have been GET or POST"); // for the test
      return;
    }
    QObject::connect(reply, &QNetworkReply::finished, [reply, &done](){
      QByteArray data{reply->readAll()};
      qDebug() << "Received reponse:\n  from: " << reply->url()
               << "\n  content: " << data;
      ExampleResponseMessage response{data, reply->url().path(), ResponseMessage::statusCodeFrom(reply)};
      reply->deleteLater();
      if (response.statusCode() == OK and reply->error() == QNetworkReply::NoError) {
        qDebug() << "The server spoke to: " << response.name();
        QCOMPARE(response.name(), QStringLiteral("Matt"));
      }
      else {
        qDebug() << "Maybe do something with the read content here.\n"
                    "Errors can still return information from the server";
        switch (response.statusCode()) {
        case OK:
          qDebug() << "We recieved OK, why an error?";
          break;
        case CREATED:
          qDebug() << "CREATED is technically not an error what what is expected from a POST in our API.";
          break;
        case BAD_REQUEST:
        case CONFLICT:
        case NOT_FOUND:
        case NOT_IMPLEMENTED:
        case SERVER_ERROR:
          qDebug() << "Really you would do something use ful here.";
          break;
        }
        QFAIL("An unexpected error occurred.");
      }

      ++done; // this is just for testing, so we can assert that something actually happened
      if (done == 2) QCoreApplication::instance()->quit();
    } );

  } );

  // This just ensures the test will finish if there is an error with request/response
  QTimer::singleShot(2000, []() { QApplication::instance()->quit(); } );
  QApplication::instance()->exec();
  QCOMPARE(done, 2);
}



// The following ensures the TestCase is run.
DECLARE_TEST(TestExample)

// The following allows Qt Creator to pick up the tests
QTEST_APPLESS_MAIN(TestExample)
