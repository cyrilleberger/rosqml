import QtQuick 2.0
import Ros 1.0

TextEdit
{
  width: 150
  height: 50
  Publisher
  {
    id: sub
    topicName: "/string"
    dataType: "std_msgs/String"
  }
  onTextChanged: sub.publish(text)
}

