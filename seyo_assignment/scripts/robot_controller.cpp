#include "ros/ros.h"
#include "std_msgs/String.h"
#include "geometry_msgs/Twist.h"
#include "seyo_assignment/user_commands.h"
#include "seyo_assignment/angle_conversions.h"
#include "nav_msgs/Odometry.h"
#include "math.h"
#include "string"
#include <sstream>
using namespace std;

//Global variables
float target_yaw=0, odom_x=0, odom_y=0, target_yaw_rad=0, yaw=0, dist=0, goal_x=0, goal_y=0, delta_x=0, delta_y=0, delta_yaw=0, angleToGoal=0, travel=0, speed=0;
string action;
seyo_assignment::angle_conversions srv;
int direction=0, state=0;
bool standby = false, driving = false;
geometry_msgs::Twist cmd_msg; 

//Robot parameters
float yaw_precision = M_PI/180; //1 degree yaw precision;
float pos_precision = 0.05; // 1cm position precision

//Use global publisher, subscriber and services
ros::Publisher robotCommands_pub;
ros::Subscriber userCommands_sub;
ros::Subscriber odom_sub;
ros::ServiceClient client;

//Subscriber callback functions
void odomCallback(const nav_msgs::Odometry::ConstPtr& odom_msg)
{  
  odom_x = odom_msg->pose.pose.position.x;
  odom_y = odom_msg->pose.pose.position.y;
  srv.request.pose.pose.orientation = odom_msg->pose.pose.orientation;
  
  if (client.call(srv))
  {
    //ROS_INFO("Yaw: %f", srv.response.yaw);
    yaw = srv.response.yaw;
  }
  else
  {
    ROS_ERROR("Failed to call rotation_calcs service");
  }

  //ROS_INFO("Odom x: %f Odom y: %f OdomConv z: %f", odom_x, odom_y, yaw);
}

void userCommandCallback(const seyo_assignment::user_commands::ConstPtr& msg)
{
  action = msg->action.c_str();
  travel = msg->distance;
  ROS_INFO("Cmd received: [%s] [%f]", msg->action.c_str(), msg->distance);
  standby = false;
  
  if (action == "left") {
     state = 0; //turning state
     if (target_yaw == 180 || target_yaw == -180) {
       target_yaw = -90;
       direction = -1;
     } else if (target_yaw == 90) {
       target_yaw = 180;
       direction = -1;
     } else if (target_yaw == -90) {
       target_yaw = 0;
       direction = -1;
     }  else {
       target_yaw = 90;
       direction = -1;
     } 
     ROS_INFO("Turning left"); 
  
  } else if (action == "right") {
     state = 0; //turning state
     if (target_yaw == 180 || target_yaw == -180) {
       target_yaw = 90;
       direction = 1;
     } else if (target_yaw == 90) {
       target_yaw = 0;
       direction = 1;
     } else if (target_yaw == -90) {
       target_yaw = -180;
       direction = 1;
     }  else {
       target_yaw = -90;
       direction = 1;
     }
     ROS_INFO("Turning right");   

  } else if (action == "fwd") {    
     state = 1; //fix heading state 
     if (target_yaw == 180 || target_yaw == -180) {
       goal_x = goal_x-travel;
       goal_y = goal_y;
     } else if (target_yaw == 90) {
       goal_x = goal_x;
       goal_y = goal_y+travel;
     } else if (target_yaw == -90) {
       goal_x = goal_x;
       goal_y = goal_y-travel;
     }  else {
       goal_x = goal_x+travel;
       goal_y = goal_y;
     }
     ROS_INFO("Driving forward");   
  }
}

void turn()
{
  cmd_msg.angular.z = direction * abs(target_yaw_rad-yaw);
  if (abs(cmd_msg.angular.z) > yaw_precision) {
     //ROS_INFO("Cmd sent-> x: %f z: %f Dist: %f", cmd_msg.linear.x, cmd_msg.angular.z, dist);
     robotCommands_pub.publish(cmd_msg);
  } else {
     state = 3;        
  }
}

void fixHeading()
{
  delta_yaw = angleToGoal - yaw; 
  
  if (driving == true) {
     cmd_msg.linear.x = speed;
  }
  
  if (abs(delta_yaw) > yaw_precision) {
     if (delta_yaw > 6) {
        cmd_msg.angular.z = 0.1;
     } else if (delta_yaw < -6) {
        cmd_msg.angular.z = -0.1;
     } else if (delta_yaw > 0) {
        cmd_msg.angular.z = -0.1;
     } else {
        cmd_msg.angular.z = 0.1;
     }     
  } 

  //ROS_INFO("FH Cmd sent-> x: %f z: %f Dist: %f DelYaw: %f", cmd_msg.linear.x, cmd_msg.angular.z, dist, delta_yaw);
  //ROS_INFO("Check-> A2G: %f target: %f convZ: %f", angleToGoal, target_yaw_rad, yaw);
  robotCommands_pub.publish(cmd_msg);

  if (abs(delta_yaw) <= yaw_precision) {
     //ROS_INFO("Yaw error: %f", delta_yaw);
     cmd_msg.angular.z = 0.0;
     state = 2;        
  }
}

void driveStraight()
{
  driving = true;
  delta_yaw = angleToGoal - yaw;
  
  if (dist >= pos_precision) {
     cmd_msg.linear.x = speed;
     //ROS_INFO("Drive Cmd sent-> x: %f z: %f Dist: %f DelYaw: %f", cmd_msg.linear.x, cmd_msg.angular.z, dist, delta_yaw);
     //ROS_INFO("Check-> A2G: %f target: %f convZ: %f", angleToGoal, target_yaw_rad, yaw);
     robotCommands_pub.publish(cmd_msg);
  } else {
        //ROS_INFO("Position error: %f", dist);
        state = 3;
  }      

  if (abs(delta_yaw) > yaw_precision) {
     state = 1;        
  }
}

void goalReached()
{
  cmd_msg.linear.x = 0.0;
  cmd_msg.angular.z = 0.0;
  robotCommands_pub.publish(cmd_msg);
  driving = false;
  if (standby == false) {     
     if ((goal_x == 0) && (goal_y == 0)) {
        ROS_INFO("START: Ready for the first command...");
     } else if ((14 <= goal_x && goal_x <= 15) && (-0.5 <= goal_y && goal_y <= 0.5)) {
        ROS_INFO("MISSION COMPLETE :)");
     } else {
       ROS_INFO("Ready for the next command...");
     }
     standby = true;
  }
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "robot_controller");

  ros::NodeHandle nh;

  odom_sub = nh.subscribe("sim_model/odom", 1, odomCallback);
  userCommands_sub = nh.subscribe("userCommands", 1, userCommandCallback);
  client = nh.serviceClient<seyo_assignment::angle_conversions>("angle_conversions");
  robotCommands_pub = nh.advertise<geometry_msgs::Twist>("sim_model/cmd_vel", 1);

  //seyo_assignment::user_commands msg;   

  ros::Rate loop_rate(50);

  while (ros::ok())
    {
      delta_x = goal_x - odom_x;
      delta_y = goal_y - odom_y;
      //ROS_INFO("delta_x: %f delta_y: %f", delta_x, delta_y);
      angleToGoal = atan2(delta_y, delta_x);
      dist = sqrt(delta_x*delta_x + delta_y*delta_y);
    
      //Turning angles
      target_yaw_rad = target_yaw*M_PI/180;
      
      //Speed adjustments
      if (driving == true) {
        if (dist <= 0.01 || standby == true) {
           speed = 0.0;
        } else if (dist < 0.1) {
           speed = -0.05;
        } else if (dist < 0.5) {
           speed = -0.2;
        } else {
           speed = -0.5;
        } 
      }
      
      //State control
      if (state == 0) {
         turn();
      } else if (state == 1) {
         fixHeading();
      } else if (state == 2) {
         driveStraight();
      } else if (state == 3) {
         goalReached();
      } else {
         ROS_INFO("Unknown state");
      }
      
      //ROS_INFO("Check-> A2G: %f target: %f convZ: %f", angleToGoal, target_yaw_rad, yaw);
      ros::spinOnce();

      loop_rate.sleep();

    }

  return 0;
}



