#ifndef TST_TESTEXAMPLE_H
#define TST_TESTEXAMPLE_H
#include <QObject>
#include <memory>
#include "network/requestmessage.h"
#include "network/responsemessage.h"

using namespace network;

class TestExample : public QObject
{
  Q_OBJECT

public:
  TestExample();
  ~TestExample();

private slots:
  void testExampleRequest_data();
  void testExampleRequest();
  void testExampleResponse_data();
  void testExampleResponse();
  void testQhttp();

};

// The following are example request and response message classes used by the example.

class ExampleRequestMessage : public RequestMessage {
public:
  /// Simple constructor, GET request with no content. (GET's cannot have content)
  ExampleRequestMessage(const QString &urlPath = QStringLiteral("/"))
    : RequestMessage{urlPath, GET}, _data{}
  {
    _headers["Content-Type"] = "text/plain";
    _headers["Accept"] = "text/plain";
  }

  /// Request containing the "name". Since this sends data it is a POST request type.
  /// The "name" string is the application object for the message.
  ExampleRequestMessage(const QString &name,
                        const QString &urlPath)
    : RequestMessage{urlPath, (name.isEmpty() ? GET : POST)}, _data{}
  {
    _headers["Content-Type"] = "text/plain";
    // This is just an example of "converting" the message into the data.
    // Note: does not have to be done in the constructor.
    _data.append(name);
  }

  /// Request built from the data itself. If the data is empty assume a simple GET,
  /// otherwise it must have been a POST request.
  ExampleRequestMessage(const QByteArray &data,
                        const QString &urlPath = QStringLiteral("/"))
    : RequestMessage{urlPath, (data.isEmpty() ? GET : POST)}, _data{data}
  {
    _headers["Content-Type"] = "text/plain";
    // We convert back to the application object when calling the name() function.
    // So no need to do anything here.
  }

  ~ExampleRequestMessage() override = default;

  /// Retrieve the application object.
  QString name() const {
    // There is a constructor for creating a String from a ByteArray
    return QString{_data};
  }

  /// Retrieve the underlying data.
  QByteArray data() const override {
    return _data;
  }

private:
  QByteArray _data;
};

class ExampleResponseMessage : public ResponseMessage {
public:
  /// Simple constructor, OK with generic response (if no name provided) otherwise
  /// a customised response message.
  ExampleResponseMessage(const QString &urlPath = QStringLiteral("/"),
                         StatusCode status = OK, const QString &name = QStringLiteral(""))
    : ResponseMessage{urlPath, status}, _name{name}
  {
    _headers["Content-Type"] = "text/plain";
    // We convert to the data when calling the data() function. So no need to do anything here.
    // For illustration purposes we do this the opposite to the ExampleRequestMessage.
  }

  /// Request built from the data itself. If the data is empty assume a simple GET,
  /// otherwise it must have been a POST request.
  ExampleResponseMessage(const QByteArray &data,
                         const QString &urlPath = QStringLiteral("/"),
                         StatusCode status = OK)
    : ResponseMessage{urlPath, status}, _name{}
  {
    _headers["Content-Type"] = "text/plain";

    // Extract the name from the received data.
    QString allData{data};
    if (allData != QStringLiteral("Hello World!")) {
      int nameStart{allData.indexOf(' ') + 1};
      // Retrieve the text after the first space, excluding the final exclamation mark
      // i.e., "Hello Matt!" => "Matt"
      _name = allData.mid(nameStart, allData.size() - 1 - nameStart);
    }
  }

  ~ExampleResponseMessage() override = default;

  /// Retrieve the application object.
  QString name() const {
    return _name;
  }

  /// Retrieve the data for the message exchange.
  QByteArray data() const override {
    QByteArray data{};
    if (_name.isEmpty()) {
      data.append("Hello World!");
    }
    else {
      data.append("Hello ").append(_name).append('!');
    }
    return data;
  }

private:
  QString _name;
};

#endif // TST_TESTEXAMPLE_H
