import QtQuick 2.0
import Ros 1.0

Rectangle
{
  id: root
  color: "#aaa"
  property real __stick_size: 0.25 * Math.min(width, height)
  property real __stick_center_x: width * 0.5
  property real __stick_center_y: height * 0.5
  property alias topicName: pub.topicName
  property real maxLinearVelocity: 1
  property real maxAngularVelocity: 1
  Publisher
  {
    id: pub
    dataType: "geometry_msgs/Twist"
  }
  Rectangle
  {
    id: stick
    color: "white"
    radius: 0.5 * root.__stick_size
    x: __stick_center_x - 0.5 * width
    y: __stick_center_y - 0.5 * height
    width: root.__stick_size
    height: root.__stick_size
    property variant stickPoint: { 'x':  -2.0 * stick.x / (parent.width-stick.width) + 1.0, 'y': -2.0 * stick.y / (parent.height-stick.height) + 1.0 }
    onStickPointChanged:
    {
      pub.publish({'linear': {'x': stickPoint.y * maxLinearVelocity }, 'angular': {'z': stickPoint.x * maxAngularVelocity} })
    }
  }
  MouseArea
  {
    anchors.fill: parent
    drag.target: stick
    drag.minimumX: 0
    drag.minimumY: 0
    drag.maximumX: parent.width - stick.width
    drag.maximumY: parent.height - stick.height
    onReleased:
    {
      stick.x = __stick_center_x - 0.5 * stick.width
      stick.y = __stick_center_y - 0.5 * stick.height
    }
  }
}
