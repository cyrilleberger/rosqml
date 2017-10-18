import Ros 1.0
import QtQuick 2.2
import QtQuick.Window 2.0
import QtCharts 2.1
import QtLocation 5.5
import QtPositioning 5.5


Map
{
  id: map
  property alias topicName: gps_sub.topicName
  Plugin
  {
    id: mapPlugin
    name: "osm"
    PluginParameter { name: "osm.useragent"; value: "tst_editor" }
    PluginParameter { name: "osm.mapping.custom.host"; value: "http://c.tile.openstreetmap.org/" }
  }
  Subscriber
  {
    id: gps_sub
    property bool __first: true
    topicName: "gps"
    queueSize: 10000
    onMessageReceived:
    {
      var c = QtPositioning.coordinate(message.latitude, message.longitude, message.altitude)
      if(!isNaN(c.latitude) && !isNaN(c.longitude))
      {
        path.addCoordinate(c)
        if(__first)
        {
          map.center = c 
          __first = false
        }
      }
    }
  }
  plugin: mapPlugin
  activeMapType: supportedMapTypes[supportedMapTypes.length - 1]
  zoomLevel: 14
  MapPolyline
  {
    id: path
    line.width: 3
    line.color: 'green'
  }
}
