import QtQuick 2.0

Column
{
  property var message
  property var messageDefinition
  Repeater
  {
    model: messageDefinition ? messageDefinition.fields : null
    Row
    {
      Text
      {
        text: modelData.name + ": "
      }
      Text
      {
        text: message[modelData.name]
      }
    }
  }
}
