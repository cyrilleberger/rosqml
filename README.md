ROS QML
=======

Requirements
------------

- `qt5`
- ROS

Installation
------------

```
> mkdir build
> cd build
> qmake ..
> make
> make install
```

Known Issue
-----------

Ros has a known error in the pc files, lib are specified as -l:/path/libname.so. So the "-l:" should be removed. This can be done by updating the pc files in ros:

```
> cd /opt/ros/kinetic/lib/pkgconfig/
> sudo sed -i "s/-l://g" *
```

On Debian:

```
> cd /usr/lib/x86-64/pkgconfig/
> sudo sed -i "s/-l://g" *
```

Usage
-----

See the example, they can be run with:

```
> qmlscene [filename]
```
