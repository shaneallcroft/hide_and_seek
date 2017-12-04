#include "visualization_msgs/Marker.h"
#include "people_msgs/PositionMeasurement.h"
#include "geometry_msgs/Twist.h"
#include "face_detector.h"
#include <tf/transform_listener.h>
#include "std_msgs/Bool.h"

//TODO(shaneallcroft) correct all pose.y or pose.x to pose.position.y or x
//correct all incorrect uses of '.' to '->'  when the message being input is the const pointer thing

class PeopleFinder()
{
 public:
  PeopleFinder()  //maybe intake an integer number of players
  {
    player_count = 1;
    found_count = 0;
    legs_found = false;
    current_location_tracker = n.subscribe("amcl_pose", 10, &PeopleFinder::current_pos_retriever, this);
    velocity_control = n.advertise<geometry_msgs::Twist>("/cmd_vel" , 10);
    person_present_notifier = n.advertise<std_msgs::Bool>("person_present", 100);
    person_leg_detector = n.subscribe("leg_detector/leg_tracker_measurements", 10, &legs_detect_callback, this);
    person_face_detector = n.subscribe("face_detector/people_tracker_measurements", 10, &face_detect_callback, this);
  }

  void current_pos_retriever(const geometry_msgs::PoseWithCovarianceStamped::ConstPtr& current_pos);
  void PeopleFinder::legs_detect_callback(const people_msgs::PositionMeasurementArray::ConstPtr& leg_place);
  void PeopleFinder::face_detect_callback(const people_msgs::PositionMeasurementConstPtr& face_place);
  
 private:
  int player_count;
  int found_count;
  bool legs_found;
  ros::NodeHandle n;
  ros::Publisher person_present_notifier;
  ros::Publisher velocity_control;
  ros::Subscriber person_leg_detector;
  ros::Subscriber person_face_detector;
  ros::Subscriber current_location_tracker;
  geometry_msgs::PoseWithCovarianceStamped current_position;
}

void PeopleFinder::legs_detect_callback(const people_msgs::PositionMeasurementArray::ConstPtr& leg_place)
{
  if(/*The people array is not empty */ )  //no legs afoot 
  {
    //this.legs_detected
    legs_found = true;
    geometry_msgs::Twist vel_msg;
    vel_msg.linear.x = 0;
    vel_msg.linear.y = 0;
    vel_msg.angular.z = 0;
    velocity_control.publish(vel_msg);  //STOP!
    ROS_INFO("WHOA I SAW LEGS!!! STOPPING NOW!!!");
    std_msgs::Bool seen_msg;
    seen_msg.data = true;
    person_present_notifier.publish(seen_msg);
    velocity_control.publish(vel_msg);
    ros::Duration(.1).sleep();
    vel_msg.angular.z = .1;  
    velocity_control.publish(vel_msg);  //turn around and around...
    ros::Duration(4.5).sleep();
    seen_msg.data = false;
    //TODO Add a random quote from a skyrim guard like "Must've been my imagination" to ROS_INFO as a flavorful "give up" text
    person_present_notifier.publish(seen_msg);
    
  }
}

void PeopleFinder::face_detect_callback(const people_msgs::PositionMeasurementConstPtr& face_place)
{
  if(legs_found)  // I dont want it to have to find legs first, but structurally this is simpler rn
  {
    if(/*face place people array is not empty*/)
    {
      geometry_msgs::Twist vel_msg;  //I feel like an additional affirmation layer would be better to make sure there is someone really there
      vel_msg.linear.x = 0;
      vel_msg.linear.y = 0;
      vel_msg.angular.z = 0;
      velocity_control.publish(vel_msg);
      ROS_INFO("I FOUND YOUUUUUUUUUU~~~~~~~~~~~");
      ++found_count;
      if( found_count == player_count)
      {
	ROS_INFO("I WIN!!!!!!!!");
      }
    }
  }
}

void PeopleFinder::current_pos_retriever(const geometry_msgs::PoseWithCovarianceStamped::ConstPtr& current_pos){
  this.current_position = current_pos->pose;
} 

int main(int argc, char **argv)
{
  ros::init(argc, argv, "person_detector");

  PeopleFinder();

  ros::spin();
  
  return 0;
}
