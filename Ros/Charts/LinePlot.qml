import Ros 1.0
import QtQuick 2.2
import QtQuick.Window 2.0
import QtCharts 2.1


ChartView
{
  id: root
  anchors.fill: parent
  
  property bool relativeTime: true
  property alias topics: instantiator.model
  
  ValueAxis {
    id: axisY
  }
  ValueAxis {
    id: axisX
  }
  
  property bool __first: true
  
  onTopicsChanged:
  {
    removeAllSeries()
    root.__first = true
    for(var topicIndex in root.topics)
    {
      var topicInfo = root.topics[topicIndex]
      
      var s = root.createSeries(ChartView.SeriesTypeLine, topicInfo.topicName, axisX, axisY)
      if(topicInfo.color)
      {
        s.color = topicInfo.color
      }
    }
  }
  
  Instantiator
  {
    id: instantiator
    delegate: Subscriber
    {
      topicName: modelData.topicName
      property variant __field_split: modelData.field.split(".")
      onMessageReceived:
      {
        var x = timestamp
        if(root.relativeTime)
        {
          x = x - Ros.startTime
        }
        var y = message
        
        for(var i in __field_split)
        {
          y = y[__field_split[i]]
        }
        
        if(__first)
        {
          axisX.min = x
          axisX.max = x
          axisY.min = y
          axisY.max = y
          __first = false
        } else {
          axisX.min = Math.min(axisX.min, x)
          axisX.max = Math.max(axisX.max, x)
          axisY.min = Math.min(axisY.min, y)
          axisY.max = Math.max(axisY.max, y)
        }
        root.series(modelData.topicName).append(x, y)
      }
    }
  }
  
}

