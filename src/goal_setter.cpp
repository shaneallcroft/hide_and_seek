#include "ros/ros.h"
#include "hide_and_seek/tp_msg.h"
#include "hide_and_seek/tp_map_srv.h"
#include "nav_msgs/OccupancyGrid.h"//PICK AN ACTUAL MAP MESSAGE YO MAYBE OCCUPANCY GRID??
#include "nav_msgs/GetMap.h"
#include "move_base_msgs/MoveBaseAction.h"
#include <actionlib/client/simple_action_client.h>
#include <math.h>

#define PROXIMITY_THRESHOLD 4

class GoalSetter//Map Reciever and Tourist Point List Generator
{

public:
  GoalSetter()
  {    
    sub = n.subscribe("amcl_pose", 10, &GoalSetter::current_pos_retriever, this);
    first_spin = true;
    goal_index = -1;
    ros::spinOnce();  //spun for her pleasure..... and to get the initial position cause set_goal needs it
    tpmap_reciever = n.serviceClient<hide_and_seek::tp_map>("generate_new_tp_list");  //this cant be right
    action_client("move_base", false);  //I'm not positive whether this should be true
    while(!action_client.waitForServer( ros::Duration(5.0) ))
    {
      ROS_INFO("Waiting for the move_base action server to come up");
    }  //in constructor so it doesnt wait 5 sec after every tourist point
    person_alert = n.subscribe("person_present", 10);
    this.set_goal();
  }
      //prototypes bb
  void GoalSetter::set_goal(hide_and_seek::tp_map_srv::Request &req, hide_and_seek::tp_map_srv::Response &res);
  void current_pos_retriever(const geometry_msgs::PoseWithCovarianceStamped::ConstPtr& current_pos);
  void GoalSetter::person_alert_callback(const std_msgs::Bool::ConstPtr& person_present_msg)
  
private:
  ros::NodeHandle n;
  ros::ServiceClient tpmap_reciever;
  ros::Publisher goal_pub;
  geometry_msgs::PoseWithCovarianceStampedMessage current_position;
  ros::Subscriber sub;
  ros::Subscriber person_alert;
  actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> action_client;
  move_base_msgs::MoveBaseGoal current_goal;
  hide_and_seek::tp_map_srv ottpm;  //The one true tourist point map
  bool first_spin;
  int goal_index;
};

void GoalSetter::current_pos_retriever(const geometry_msgs::PoseWithCovarianceStamped::ConstPtr& current_pos){  //I think the ConstPtr thing there is extraneous   LOL JK
  this.current_position = current_pos;
  if(first_spin)
  {
    first_spin = false;
    return;
  }  
  
  if( (int)((current_position.pose.position.x - current_goal.pose.position.x) + (current_position.pose.position.y - current_goal.pose.position.y)) / PROXIMITY_THRESHOLD == 0)// i.e. if(close enough) Since they're(x and y) both held to the same standard of closeness, we can add them together and divide by that standard (technically this should be divided by 2 again but threshold was pretty large before
  {
      this.set_goal();
  }
}//JUST FINISHED THIS 12:36 AM December 2nd

void GoalSetter::person_alert_callback(const std_msgs::Bool::ConstPtr& person_present_msg)
{
  if(person_present_msg->data)
  {
    move_base_msgs::MoveBaseGoal goal;
    current_goal.target_pose.header.frame_id = "base_link";
    current_goal.target_pose.header.stamp = ros::Time::now();
  }
}

//call back function for 

void GoalSetter::set_goal()
{ 
  if(tpmap_reciever.call(ottpm))
  {
//find closest point and make that the new goal.
    
    move_base_msgs::MoveBaseGoal goal;
    goal.target_pose.header.frame_id = "base_link";
    goal.target_pose.header.stamp = ros::Time::now();
    hide_and_seek::tp_msg tp_at_current_location;
    tp_at_current_location.tp_pos.x = current_position.pose.position.x;
    tp_at_current_location.tp_pos.y = current_position.pose.position.y;
    int index_of_closest_tp = 0;
    float closest_distance = pow(pow((tp_at_current_location.tp_pos.x - ottpm.miniverse[i].tp_pos.x),2) + pow((tp_at_current_location.tp_pos.y - ottpm.miniverse[index_of_closest_tp].y), 2), .5);
    if(ottpm.response.miniverse_size == 0)
    {
      ROS_INFO("I checked the WHOLE ROOM. YOU WON?");//this seems like a dumb ending
      return;
    }
    for(int i = 0; i < ottpm.response.miniverse_size; ++i)
    {
	hide_and_seek::tp_msg test_tp = ottpm.miniverse[i];
	
	if(test_tp.visited == true)
	{
	  continue;
	}
	float distance = pow(pow((tp_at_current_location.tp_pos.x - ottpm.miniverse[i].tp_pos.x),2) + pow((tp_at_current_location.tp_pos.y - ottpm.miniverse[index_of_closest_tp].y), 2), .5);
	if(distance < closest_distance)
	{
	    index_of_closest_tp = i;
	}
    }
    
    ottpm.request.miniverse = ottpm.response.miniverse;  
    goal_index = index_of_closest_tp;
    current_goal.target_pose.pose.position.x = ottpm.response.miniverse[goal_index].tp_pos.x;
    current_goal.target_pose.pose.position.y = ottpm.response.miniverse[goal_index].tp_pos.y;
    ROS_INFO("New goal chosen at x=%ld, y=%ld" (long int)current_goal.target_pose.pose.position.x, current_goal.target_pose.pose.position.y);
    action_client.sendGoal(current_goal);
  }
  return true;
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "goal_setter");

  GoalSetter goalSetterster;
  
  ros::spin();

  return 0;
}

