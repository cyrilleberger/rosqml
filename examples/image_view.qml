// Run with: rosrun image_publisher image_publisher __name:=test <some_test_image_or_video>
//


import Ros 1.0
import Ros.ImageTransport 1.0 as RIT
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0

GridLayout
{
  id: root
  columns: 2
  RIT.Subscriber
  {
    id: sub
    topicName: "/test/image_raw"
  }
  RIT.ImageView
  {
    Layout.columnSpan: 2
    image: sub.lastImage
    Layout.fillWidth: true
    Layout.fillHeight: true
  }
  Label
  {
    text: "Width: " 
  }
  Label
  {
    id: widthLabel
    text: sub.lastImage ? sub.lastImage.width.toString() : ""
  }
  Label
  {
    text: "Height: " 
  }
  Label
  {
    id: heightLabel
    text: sub.lastImage ? sub.lastImage.height.toString() : ""
  }
}

