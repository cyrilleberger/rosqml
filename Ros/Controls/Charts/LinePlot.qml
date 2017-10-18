import Ros 1.0
import QtQuick 2.2
import QtQuick.Window 2.0
import QtCharts 2.1


ChartView
{
  id: root
  
  property bool relativeTime: true
  property alias topics: instantiator.model
  property bool inSeconds: true
  property int maximumPoints: -1
  property variant __counts: {}
  
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
    var counts = {}
    for(var topicIndex in root.topics)
    {
      var topicInfo = root.topics[topicIndex]
      var s_name = topicInfo.topicName + "." + topicInfo.field
      var s = root.createSeries(ChartView.SeriesTypeLine, s_name, axisX, axisY)
      counts[s_name] = 0
      if(topicInfo.color)
      {
        s.color = topicInfo.color
      }
      s.useOpenGL = true
    }
    root.__counts = counts
  }
  
  Instantiator
  {
    id: instantiator
    delegate: Subscriber
    {
      topicName: modelData.topicName
      skip: modelData.skip ? modelData.skip : 0
      property variant __field_split: modelData.field.split(".")
      property bool __first_nan: true
      onMessageReceived:
      {
        var x = timestamp
        if(root.relativeTime)
        {
          x = x - Ros.startTime
        }
        if(root.inSeconds)
        {
          x = x * 1e-9
        }
        var y = message
        for(var i in __field_split)
        {
          y = y[__field_split[i]]
        }
        if(!isNaN(x) && !isNaN(y))
        {
          __first_nan = true
          
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
          var s_name = modelData.topicName + "." + modelData.field
          var s = root.series(s_name)
          s.append(x, y)
          var counts = root.__counts
          counts[s_name] += 1
          if(root.maximumPoints > 0 && counts[s_name] > root.maximumPoints)
          {
            counts[s_name] -= 1
            s.remove(0)
            var minX = axisX.max
            for(var i = 0; i < root.count; ++i)
            {
              minX = Math.min(root.series(i).at(0).x, minX)
            }
            axisX.min = minX
          }
          root.__counts = counts
        }
        else if(__first_nan)
        {
          console.log("Warning: got NaN on topic ", modelData.topicName, " field ", modelData.field)
          __first_nan = false
        }
      }
    }
  }
  
}

