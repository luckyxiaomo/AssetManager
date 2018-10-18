#ifndef REQUESTMESSAGE_H
#define REQUESTMESSAGE_H
#include "message.h"
#include <QByteArray>
#include <QString>
#include <QUrl>

namespace network {

/**
 * @brief The RequestMessage class Base class for request messages,
 * can also be used directly for simple GET requests.
 *
 * You can modify this class as you require.
 *
 * Note: the data() function will always return empty and must be
 * overridden on derived class if data needs to be included in the
 * request.
 */
class RequestMessage : public Message
{
public:
  RequestMessage(const QString &urlPath = QStringLiteral("/"), HttpMethod httpMethod = GET);
  RequestMessage(const QUrl &url, HttpMethod httpMethod = GET);

  /**
   * @brief httpMethod Return the HTTP method (e.g., GET, POST, etc.) this message is to be sent using.
   * @return the HTTP method of this message
   */
  HttpMethod httpMethod() const;

  /**
   * @brief data Returns an empty QByteArray. Override in subclasses that actually provide data.
   * @return nothing
   */
  QByteArray data() const override;

private:
  HttpMethod _method;
};

} // namespace network

#endif // REQUESTMESSAGE_H
