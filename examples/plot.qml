import Ros 1.0
import QtQuick 2.0
import QtQuick.Window 2.0
import QtCharts 2.1
  
Window {
  width: 512
  height: 512
  visible: true
  property bool __first: true
  Subscriber
  {
    topicName: "/data"
    onMessageReceived:
    {
      var x = timestamp
      var y = message["fluid_pressure"]
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
      lineSerie.append(x, y)
    }
  }
  ChartView
  {
    id: chartView
    anchors.fill: parent
    ValueAxis {
      id: axisY
    }
    ValueAxis {
      id: axisX
    }
    LineSeries {
      id: lineSerie
      axisX: axisX
      axisY: axisY
    }
  }
}
