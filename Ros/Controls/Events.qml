import QtQuick 2.0

Column
{
  id: root
  property var topicNames: []
  Repeater
  {
    model: root.topicNames
    Row
    {
      Text
      {
        text: modelData + ": "
      }
      Event
      {
        topicName: modelData
      }
    }
  }
}
