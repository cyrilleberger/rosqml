import QtQuick 2.0
import QtQuick.Window 2.0
import QtLocation 5.6
import QtPositioning 5.6
import Ros 1.0

Window {
  width: 512
  height: 512
  visible: true

  Map
  {
    id: map
    anchors.fill: parent
    
    plugin: Plugin
    {
      id: mapPlugin
      name: "osm"
      PluginParameter { name: "osm.useragent"; value: "rosqml" }
      PluginParameter { name: "osm.mapping.custom.host"; value: "http://c.tile.openstreetmap.org/" }
    }
    activeMapType: supportedMapTypes[supportedMapTypes.length - 1]
    zoomLevel: 14

    Publisher
    {
      id: pub
      topicName: "map/polygon"
      dataType: "geographic_msgs/GeoPath"
    }
    
    Component
    {
      id: received
      MapPolygon
      {
        border.width: 2
        border.color: "green"
      }
    }
    
    Subscriber
    {
      id: sub
      topicName: "map/polygon"
      onMessageReceived:
      {
        var p = []
        message['poses'].forEach(function(pose) {
          var position = pose['pose']['position']
          p.push(QtPositioning.coordinate(position['latitude'], position['longitude']))
        })
        var item = received.createObject(map, {path: p})
        map.addMapItem(item)
      }
    }
    
    MouseArea
    {
      anchors.fill: parent
      hoverEnabled: true
      onClicked: {
        var coord = map.toCoordinate(Qt.point(mouse.x, mouse.y));
        if(polyline.path.length > 1)
        {
          var pt = map.fromCoordinate(polyline.path[0])
          if(Math.abs(pt.x - mouse.x) < 2 && Math.abs(pt.y - mouse.y) < 2)
          {
            __publish()
          } else {
            polyline.addCoordinate(coord)
          }
        } else {
          polyline.addCoordinate(coord)
        }
      }
      onPositionChanged:
      {
        if(polyline.path.length > 0)
        {
          polyline.replaceCoordinate(polyline.path.length - 1, map.toCoordinate(Qt.point(mouse.x, mouse.y)));
        }
      }
      onDoubleClicked:
      {
        __publish()
      }
      function __publish()
      {
        var poses = []
        
        polyline.path.forEach(function(element) {
          var pose = { 'pose': { 'position': { 'latitude': element.latitude, 'longitude': element.longitude } } }
          poses.push(pose)
        })
        
        pub.publish({'poses': poses})
        
        while(polyline.path.length > 1) // This is weird, should be 0, but if I do that the path is still shown on screen, so we always stays with one element in the path which get overriden in onPositionChanged
        {
          polyline.removeCoordinate(0)
        }
      }
    }
    
    MapPolyline
    {
      id: polyline
      line.width: 3
      line.color: "blue"
    }
  }
}

