/****************************************************************************
The original script used was from https://answers.ros.org/question/11545/plotprint-rpy-from-quaternion/#17106
and it was modified to be a service.

Conversion from a quaternion to roll, pitch and yaw.

Nodes:
subscribed /rotation_quaternion (message of type geometry_msgs::Quaternion)
published /rpy_angles (message oftype geometry_msgs::Vector3.h)

****************************************************************************/

#include "ros/ros.h"
#include "geometry_msgs/Vector3.h"
#include "geometry_msgs/Quaternion.h"
#include "tf/transform_datatypes.h"
#include "tf/LinearMath/Matrix3x3.h"
#include "seyo_assignment/angle_conversions.h"

// Here I use global publisher and subscriber, since I want to access the
// publisher in the function MsgCallback:
//ros::Publisher rpy_publisher;
//ros::Subscriber quat_subscriber;

// Function for conversion of quaternion to roll pitch and yaw.
bool convert(seyo_assignment::angle_conversions::Request  &req,
          seyo_assignment::angle_conversions::Response &res)
 {
   //res.sum = req.a + req.b;
   //ROS_INFO("request: x=%f, y=%f, z=%f, w=%f", (float)req.pose.pose.orientation.x, (float)req.pose.pose.orientation.y, (float)req.pose.pose.orientation.z, (float)req.pose.pose.orientation.w);
   // the incoming geometry_msgs::Quaternion is transformed to a tf::Quaterion
   tf::Quaternion quat;
   tf::quaternionMsgToTF(req.pose.pose.orientation, quat);

   // the tf::Quaternion has a method to acess roll pitch and yaw
   double roll, pitch, yaw;
   tf::Matrix3x3(quat).getRPY(roll, pitch, yaw);
   res.roll = (float) roll;
   res.pitch = (float) pitch;
   res.yaw = (float) yaw;
   //ROS_INFO("sending back response y:%f p:%f r:%f", res.yaw, res.pitch, res.roll);
   return true;
 }

int main(int argc, char **argv)
{
    ros::init(argc, argv, "rotation_calcs_server");
    ros::NodeHandle n;

    ros::ServiceServer service = n.advertiseService("angle_conversions", convert);
    //rpy_publisher = n.advertise<geometry_msgs::Vector3>("rpy_angles", 1000);
    //quat_subscriber = n.subscribe("rotation_quaternion", 1000, MsgCallback);

    // check for incoming quaternions until ctrl+c is pressed
    //ROS_INFO("waiting for quaternion");
    ros::spin();
    return 0;
}
