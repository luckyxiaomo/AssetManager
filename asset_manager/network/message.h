#ifndef MESSAGE_H
#define MESSAGE_H

#include <QByteArray>
#include <QString>
#include <QMap>
#include <QUrl>
#include "qhttpabstracts.hpp"

namespace network {

/**
 * @brief The Message class represents requests and responses to be
 * sent via the network manager. It provides a common-ish interface
 * over the QHttp server and the standard Qt Client networking code.
 *
 * The idea is that a Message is a container around all of the
 * information required to send/receive a message of a particular type.
 * Moreover, the specific message types know how to convert the
 * supplied objects to the data to be sent over the network and vice
 * versa. For example, a message type may take a Custodian and
 * produce the byte data (accessible through the data() function)
 * as well as read the byte data and create a Custodian object.
 *
 * With the message configuration and content stored in a Message
 * object, the NetworkManager will then send the request or response.
 *
 * Note: that the Message only stores the @bold path portion of the
 * URL as the same message will be sent to @bold all peers in the
 * network.
 */
class Message
{
public:
  Message(const QString &urlPath = "/") : _urlPath{urlPath}, _headers{} {}
  Message(const QUrl &url) : _urlPath{url.path()}, _headers{} {}
  virtual ~Message() = default;

  /**
   * @brief urlPath Returns the path portion of the URL that is/was requested.
   * Will be combined with endpoint or IP address to complete the URL.
   * @return
   */
  virtual const QString& urlPath() const { return _urlPath; }

  /**
   * @brief data Returns the data associated with this message.
   * @return
   */
  virtual QByteArray data() const = 0;

  /**
   * @brief headers Returns the headers to be used for this request/response.
   * @return
   */
  virtual const QMap<QString, QString>& headers() { return _headers; }

  /// For convenience to modify headers
  QString& operator[] (const QString &key) { return _headers[key]; }

protected:
  QString _urlPath;
  QMap<QString, QString> _headers;
};

/**
 * @brief The HttpMethod enum is the subset of HTTP methods that we need
 * to support in this application.
 */
enum HttpMethod {
  GET = qhttp::THttpMethod::EHTTP_GET, ///< GET - for retrieving resources/entities (collections or individuals)
  PUT = qhttp::THttpMethod::EHTTP_PUT, ///< PUT - for update or replace a resource/entity
  POST = qhttp::THttpMethod::EHTTP_POST, ///< POST - add a new entity to a collection
  DELETE = qhttp::THttpMethod::EHTTP_DELETE ///< DELETE - delete a collection or resource
};

/**
 * @brief The StatusCode enum is the subset of HTTP Status Codes that we
 * will make use of this in application. We have a small selection that
 * allow us to report success as well as several error types.
 */
enum StatusCode {
  OK = qhttp::TStatusCode::ESTATUS_OK,           ///< OK - successful response
  CREATED = qhttp::TStatusCode::ESTATUS_CREATED, ///< CREATED - entity created
  BAD_REQUEST = qhttp::TStatusCode::ESTATUS_BAD_REQUEST, ///< BAD_REQUEST - unable to interpret request message
  NOT_FOUND = qhttp::TStatusCode::ESTATUS_NOT_FOUND,     ///< NOT_FOUND - unable to find specified entity
  CONFLICT = qhttp::TStatusCode::ESTATUS_CONFLICT,       ///< CONFLICT - attempted to update an entity that is newer
  SERVER_ERROR = qhttp::TStatusCode::ESTATUS_INTERNAL_SERVER_ERROR, ///< SERVER_ERROR - the server crapped itself
  NOT_IMPLEMENTED = qhttp::TStatusCode::ESTATUS_NOT_IMPLEMENTED     ///< NOT_IMPLEMENTED - the message type is not (yet) implemented
};

} // namespace network

Q_DECLARE_METATYPE(network::HttpMethod)
Q_DECLARE_METATYPE(network::StatusCode)

#endif // MESSAGE_H
