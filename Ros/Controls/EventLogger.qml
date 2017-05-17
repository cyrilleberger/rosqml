import QtQuick 2.0
import Ros 1.0

Item {
  id: root
  property var topicNames: []
  property var __received_events: []
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
        text: modelData.timestamp + ":" + modelData.topicName
      }
    }
  }
}
