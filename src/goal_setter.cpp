#include "ros/ros.h"
#include "hide_and_seek/tp_msg.h"
#include "hide_and_seek/tp_map_srv.h"
#include "nav_msgs/OccupancyGrid.h"//PICK AN ACTUAL MAP MESSAGE YO MAYBE OCCUPANCY GRID??
#include "nav_msgs/GetMap.h"

class goal_setter//Map Reciever and Tourist Point List Generator
{

public:
  GoalSetter()
  {
//tpl_service = n.advertiseService("generate_new_tp_list", generate_new_tpl);
    tpmap_reciever = n.serviceClient<tp_map>("tp_list_maker_server");//this cant be right
    sub = n.subscribe("amcl_pose", 10, &GoalSetter::current_pos_retriever, this);
  }
      //prototypes bb
  bool GoalSetter::set_goal(hide_and_seek::tp_map_srv::Request &req, hide_and_seek::tp_map_srv::Response &res);
  void current_pos_retriever(const geometry_msgs::PoseWithCovarianceStamped::ConstPtr& current_pos);
private:
  ros::NodeHandle n;
  ros::ServiceClient tpmap_reciever;
ros::Publisher goal_pub;
  geometry_msgs::PoseWithCovarianceStampedMessage current_position;
  //ros::ServiceClient current_position;
  ros::Subscriber sub;
};

void GoalSetter::current_pos_retriever(const geometry_msgs::PoseWithCovarianceStamped::ConstPtr& current_pos){
  this.current_position = current_pos;
}//JUST FINISHED THIS 12:36 AM December 2nd

//call back function for 

void GoalSetter::set_goal(hide_and_seek::tp_map_srv::Request &req, hide_and_seek::tp_map_srv::Response &res)
{
  if(tpmap_reciever.call()
  {
//find closest point and make that the new goal. 
  }
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "goal_setter");

  GoalSetter mratplgster;
  ros::spin();

  return 0;
}

