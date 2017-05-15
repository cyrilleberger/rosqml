import QtQuick 2.0
import Ros 1.0
import Ros.Controls 1.0

MessageView
{
  width: 150
  height: 50
  Subscriber
  {
    id: sub
    topicName: "/string"
    onMessageReceived:
    {
      console.log("Received ", message, " at ", timestamp, " from ", publisher)
    }
  }
  message: sub.lastMessage
  messageDefinition: sub.messageDefinition
}
