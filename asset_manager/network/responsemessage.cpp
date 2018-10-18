#include "responsemessage.h"

using network::ResponseMessage;

ResponseMessage::ResponseMessage(const QString &urlPath, network::StatusCode status)
  : Message{urlPath}, _status{status}
{}

ResponseMessage::ResponseMessage(const QUrl &url, network::StatusCode status)
  : Message{url}, _status{status}
{}

network::StatusCode ResponseMessage::statusCode() const
{
  return _status;
}

QByteArray ResponseMessage::data() const
{
  return QByteArray{};
}
