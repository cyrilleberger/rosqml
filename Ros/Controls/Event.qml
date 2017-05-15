import QtQuick 2.0
import QtQuick.Controls 1.0
import Ros 1.0

Row {
  id: root
  property string topicName
  Subscriber
  {
    topicName: root.topicName
    onMessageReceived: text.text = "Last received at " + timestamp + " from " + publisher
  }
  Publisher
  {
    id: pub
    topicName: root.topicName
    dataType: "std_msgs/Empty"
  }
  Text {
    id: text
    text: "Not yet received"
  }
  Button
  {
    text: "Send"
    onClicked: pub.publish({})
  }
}
