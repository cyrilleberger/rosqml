import QtQuick 2.0
import Ros 1.0

Text
{
  Subscriber
  {
    id: sub
    topicName: "/string"
    onMessageReceived:
    {
      console.log("Received ", message, " at ", timestamp, " from ", publisher)
    }
  }
  text: sub.lastMessage.data
}
