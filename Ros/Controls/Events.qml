import QtQuick 2.0

Column
{
  id: root
  property var topicNames: []
  property bool relativeTime: true
  Repeater
  {
    model: root.topicNames
    Event
    {
      relativeTime: root.relativeTime
      topicName: modelData
    }
  }
}
