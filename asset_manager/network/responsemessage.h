#ifndef RESPONSEMESSAGE_H
#define RESPONSEMESSAGE_H
#include "message.h"
#include "qhttpabstracts.hpp"
#include <QNetworkReply>

namespace network {

/**
 * @brief The ResponseMessage class - Base class for request messages,
 * can also be used directly for simple OK responses.
 *
 * You can modify this class as you require.
 *
 * Note: the data() function will always return empty and must be
 * overridden on derived class if data needs to be included in the
 * response.
 */
class ResponseMessage : public Message
{
public:
  ResponseMessage(const QString &urlPath = QStringLiteral("/"), StatusCode status = OK);
  ResponseMessage(const QUrl &url, StatusCode status = OK);

  /**
   * @brief statusCode Returns the StatusCode associated with this response.
   * @return the status code
   * @sa qhttpStatusCode()
   */
  StatusCode statusCode() const;

  /**
   * @brief qhttpStatusCode Returns the StatusCode associated with this response as
   * a type compatible with the QHttp request/responses.
   *
   * This is a convenience function. There is no need to modify it (or even use it).
   *
   * @return the status code as TStatusCode
   * @sa statusCode()
   */
  inline qhttp::TStatusCode qhttpStatusCode() const { return static_cast<qhttp::TStatusCode>(_status); }

  /**
   * @brief statusCodeFrom Returns the HTTP status of the specified QNetworkReply
   * a StatusCode, rather than a QVariant<int> used by the Qt Networking classes.
   *
   * This is a convenience function. There is no need to modify it (or even use it).
   *
   * @param reply the reply from which to retrieve the HTTP status
   * @return the status code as StatusCode
   */
  inline static StatusCode statusCodeFrom(const QNetworkReply *reply) {
    return static_cast<StatusCode>(reply->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toInt());
  }

  /**
   * @brief data Returns an empty QByteArray. Override in subclasses that actually provide data.
   * @return nothing
   */
  QByteArray data() const override;

private:
  StatusCode _status;
};

} // namespace name

#endif // RESPONSEMESSAGE_H
