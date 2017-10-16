import QtQuick 2.0
import QtQuick.Window 2.0
import QtLocation 5.6
import QtPositioning 5.6
import Ros 1.0

Window {
    width: 512
    height: 512
    visible: true

    Plugin {
        id: mapPlugin
        name: "osm"
         PluginParameter { name: "osm.useragent"; value: "tst_editor" }
         PluginParameter { name: "osm.mapping.custom.host"; value: "http://c.tile.openstreetmap.org/" }
   }

    Map {
      id: map
        anchors.fill: parent
        plugin: mapPlugin
        activeMapType: supportedMapTypes[supportedMapTypes.length - 1]
        zoomLevel: 14
    MapPolyline {
      id: path
        line.width: 3
        line.color: 'green'
    }
    }
    property bool __first: true
  Subscriber
  {
    id: sub
    topicName: "/gps"
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

}
