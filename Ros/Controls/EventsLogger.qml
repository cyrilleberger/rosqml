import QtQuick 2.0
import Ros 1.0

Item {
  id: root
  property var topicNames: []
  property var __received_events: []
  property bool relativeTime: true
  function __make_timestamp(timestamp)
  {
    if(relativeTime)
    {
      return timestamp - ROS_START_TIME
    } else {
      return timestamp
    }
  }
  Repeater
  {
    model: topicNames
    Item
    {
      Subscriber
      {
        topicName: modelData
        onMessageReceived:
        {
          var res = root.__received_events
          res.push({'timestamp': timestamp, 'publisher': publisher, 'topicName': modelData})
          root.__received_events = res
        }
      }
    }
  }
  Column
  {
    Repeater
    {
      model: root.__received_events.slice(0).reverse()
      Text
      {
        text: Time.format(__make_timestamp(modelData.timestamp)) + ":" + modelData.topicName
      }
    }
  }
}
