
function format(value) {
  var secs  = Math.floor(value / 1e9)
  var nsecs = value % 1e9
  return secs + "s " + nsecs + "ns"
}
