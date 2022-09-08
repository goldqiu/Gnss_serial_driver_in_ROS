#include "serial/serial.h"
#include <ros/ros.h>
#include <yaml-cpp/yaml.h>
#include "global_defination/global_defination.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <sstream>
#include "sensor_msgs/NavSatFix.h"
#include "uart_rtk_driver/rtk.h"
void Stringsplit(std::string str, const char split,std::vector<std::string>& rst)
{
	std::istringstream iss(str);	// 输入流
	std::string token;			// 接收缓冲区
	while (getline(iss, token, split))	// 以split为分隔符
	{
        rst.push_back(token);
	}
}

int main(int argc, char *argv[]) {
  
    ros::init(argc, argv, "rtk_driver_node");
    ros::NodeHandle nh;
    ros::Rate rate(100);
    serial::Serial ser;
    std::string str;  
    std::string config_file_path = WORK_SPACE_PATH + "/config/config.yaml";
    YAML::Node config_node = YAML::LoadFile(config_file_path);
    std::string uart_com = config_node["uartCOM"].as<std::string>();
    uint32_t uart_baud = config_node["baudrate"].as<uint32_t>();
    std::string rtk_topic_name = config_node["rtk_topic"].as<std::string>();
    std::string rtk_yaw_topic_name = config_node["rtk_yaw_topic"].as<std::string>();
    std::string frame_id_name = config_node["frame_id"].as<std::string>();


    ros::Publisher  rtk_pub = nh.advertise<sensor_msgs::NavSatFix>(rtk_topic_name, 1);
    ros::Publisher  rtk_yaw_pub = nh.advertise<uart_rtk_driver::rtk>(rtk_yaw_topic_name, 1);

  
    try
    {
        ser.setPort(uart_com);
        ser.setBaudrate(uart_baud);
        serial::Timeout to = serial::Timeout::simpleTimeout(1000);
        ser.setTimeout(to);
        ser.open();
    }
    catch (serial::IOException &e)
    {
        std::cout << "Unable to open port" << std::endl;
        return -1;
    }
    if (ser.isOpen())
    {
        std::cout << "Serial port initialized" << std::endl;
    }
    else
    {
        return -1;
    }
    while (ros::ok())
    {
        ros::spinOnce();
        std::string read;

        read = ser.read(ser.available());
        std::string foundGGA = "$GPGGA";
        if (read.size() != 0)
        {
            str.append(read);
            if(str.size() > 140)
            {
                 uint16_t loc1 = str.find(foundGGA,0);
                 uint16_t loc2 = str.find(foundGGA,loc1+1);
                   if(loc1 > 140)
                   {
                       std::cout<<"read GPS fail!!!!"<<std::endl;
                       str.erase();
                       continue;
                   }

                   if(loc2 > 140)
                   {
                       str.erase();
                       continue;
                   }
                std::string gpgga(str,loc1,loc2-loc1);
                str.erase(0, loc2);
                std::vector<std::string> strlist1,strlist2,strlist3;
	            Stringsplit(gpgga, '\n',strlist1);		
                // std::cout<<strlist1[0]<<std::endl;
                // std::cout<<strlist1[1]<<std::endl;
	            Stringsplit(strlist1[0], ',',strlist2);		
	            Stringsplit(strlist1[1], ',',strlist3);	
                sensor_msgs::NavSatFix temp_rtk_data;
                uart_rtk_driver::rtk temp_rtk_yaw_data;
                temp_rtk_yaw_data.status = stol(strlist2[6]);
                if(temp_rtk_yaw_data.status > 0)
                {
                    temp_rtk_data.altitude = stod(strlist2[9]);
                    temp_rtk_data.latitude = stod(strlist2[2])/100;
                    temp_rtk_data.longitude = stod(strlist2[4])/100;
                    temp_rtk_data.header.frame_id = frame_id_name;
                    temp_rtk_data.header.stamp = ros::Time::now();
                    temp_rtk_yaw_data.yaw = stod(strlist3[3]);
                    temp_rtk_yaw_data.header.frame_id = frame_id_name;
                    temp_rtk_yaw_data.utctime = stol(strlist2[1]);
                    temp_rtk_yaw_data.satelite_num = stol(strlist2[7]);
                    temp_rtk_yaw_data.header.stamp = ros::Time::now();

                    rtk_pub.publish(temp_rtk_data);
                    rtk_yaw_pub.publish(temp_rtk_yaw_data);
                }
           

                // for(int i = 0 ; i < strlist2.size(); i++)
                // {
                //     std::cout<<strlist2[i]<<std::endl;
                // }
                // for(int i = 0 ; i < strlist3.size(); i++)
                // {
                //     std::cout<<strlist3[i]<<std::endl;
                // }

            }

        }
        rate.sleep();
    }

    return 0;
}

