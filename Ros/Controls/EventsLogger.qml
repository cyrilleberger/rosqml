import QtQuick 2.0
import Ros 1.0

Item {
  id: root
  height: column.height
  property var topicNames: []
  property var __received_events: []
  property bool relativeTime: true
  property int maximumLength: 100
  function __make_timestamp(timestamp)
  {
    if(relativeTime)
    {
      return timestamp - Ros.startTime
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
          while(root.maximumLength > 0 && res.length > root.maximumLength)
          {
            res.shift()
          }
          root.__received_events = res
        }
      }
    }
  }
  Column
  {
    id: column
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
