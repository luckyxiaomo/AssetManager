#include "requestmessage.h"

using network::RequestMessage;

RequestMessage::RequestMessage(const QString &urlPath, network::HttpMethod httpMethod)
  : Message{urlPath}, _method{httpMethod}
{}

RequestMessage::RequestMessage(const QUrl &url, network::HttpMethod httpMethod)
  : Message{url}, _method{httpMethod}
{}

network::HttpMethod RequestMessage::httpMethod() const
{
  return _method;
}

QByteArray RequestMessage::data() const
{
  // Simple GET messages contain no data
  return QByteArray{};
}
