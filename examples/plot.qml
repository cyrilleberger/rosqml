// Example of use: ROS_ARGUMENTS="data:=/pressure _field:=fluid_pressure"  qmlscene plot.qml

import Ros 1.0
import QtQuick 2.0
import QtQuick.Window 2.0
import Ros.Charts 1.0
  
Window {
  width: 512
  height: 512
  visible: true
  
  LinePlot
  {
    topics: [ { topicName: "/data", field: Ros.getParam("~field") } ]
  }
}
