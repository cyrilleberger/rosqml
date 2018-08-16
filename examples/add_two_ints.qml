// Run with: rosrun roscpp_tutorials add_two_ints_server
//

import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Ros 1.0

ColumnLayout
{
  width: 150
  height: 250
  ServiceClient
  {
    id: client1
    serviceName: "/add_two_ints"
    dataType: "roscpp_tutorials/TwoInts"
    onAnswerReceived: result1.text = answer.sum.toString()
    onCallFailed: result1.text = "Call failed"
  }
  ServiceClient
  {
    id: client2
    serviceName: "/add_two_ints"
    dataType: "roscpp_tutorials/TwoInts"
    onAnswerReceived: result2.text = answer.sum.toString()
    onCallFailed: result2.text = "Call failed"
  }
  SpinBox
  {
    id: a
  }
  SpinBox
  {
    id: b
  }
  Button
  {
    text: "add"
    onClicked: {
      client1.call({"a": a.value, "b": b.value})
      client2.call([a.value, b.value])
    }
    enabled: !client1.callInProgress && !client2.callInProgress
  }
  Text
  {
    id: result1
  }
  Text
  {
    id: result2
  }
}


