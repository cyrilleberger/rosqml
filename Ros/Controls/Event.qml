import QtQuick 2.0
import QtQuick.Controls 1.0
import Ros 1.0

Row {
  id: root
  property string topicName
  property bool relativeTime: true
  function __make_timestamp(timestamp)
  {
    if(relativeTime)
    {
      return timestamp - Ros.startTime
    } else {
      return timestamp
    }
  }
  Subscriber
  {
    topicName: root.topicName
    onMessageReceived: text.text = "Last received at " + Time.format(__make_timestamp(timestamp)) + " from " + publisher
  }
  Publisher
  {
    id: pub
    topicName: root.topicName
    dataType: "std_msgs/Empty"
  }
  Button
  {
    text: "Send " + topicName
    onClicked: pub.publish({})
  }
  Text {
    id: text
    text: "Not yet received"
  }
}
