# Gnss_serial_driver_in_ROS
Gnss_serial_driver_in_ROS

GNSS串口解析，转换成ROS话题输出。



### 运行

```
rosrun uart_rtk_driver rtk_driver_node
```

### 参数

uart_rtk_driver\config\config.yaml

```
uartCOM: /dev/ttyUSB1
baudrate: 115200
frame_id: /imu_link
rtk_topic: /rtk_data
rtk_yaw_topic: /rtk_yaw_data
```

### 话题

ros自带话题：

sensor_msgs::NavSatFix

自定义话题：

uart_rtk_driver::rtk



msg文件如下：

```
Header header

float64 yaw

uint64 status

uint64 utctime

uint64 satelite_num
```

