//I'm here to  S E R V E  you ;)
#include "ros/ros.h"
#include "hide_and_seek/tp_msg.h"
#include "hide_and_seek/tp_map_srv.h"
#include "nav_msgs/OccupancyGrid.h"//PICK AN ACTUAL MAP MESSAGE YO MAYBE OCCUPANCY GRID??
#include "nav_msgs/GetMap.h"

#DEFINE PROXIMITY_THRESHOLD 4
class Mratplg//Map Reciever and Tourist Point List Generator
{

public:
  Mratplg() {
    tpl_service = n.advertiseService("generate_new_tp_list", generate_new_tpl);
    map_reciever = n.serviceClient < nav_msgs::GetMap > ("map_server");//this might be right
    ROS_INFO("Ready to generate some A1 TPL's");
    number_of_times_map_refreshed = 0;
  }
  //prototypes bb
  bool Mratplg::generate_new_tpl(
    hide_and_seek::tp_map_srv::Request & req,
    hide_and_seek::tp_map_srv::Response & res);

private:
  ros::NodeHandle n;
  ros::ServiceServer tpl_service;
  ros::ServiceClient map_reciever;
  ros::Subscriber sub;
  int num_of_times_map_refreshed;
};


bool Mratplg::generate_new_tpl(
  hide_and_seek::tp_map_srv::Request & req,
  hide_and_seek::tp_map_srv::Response & res)
{
  nav_msgs::GetMap get_map_srv;//no values needed since this service only gets the map B)

  if (map_reciever.call(get_map_srv)) {
    OccupancyGrid map = get_map_srv.response.map;//not positive if this is completely correct
    ++num_of_times_map_refreshed;
  } else {
    return false;
  }

  int x_min, x_max, y_min, y_max = -2;
  //pick a new reSOLUTIon hereish?

  boolean prev_check = false;
  int miniverse_index = 0;
  //TODO(shaneallcroft): (after launchpad presentations) accomodate a room system
  for (size_t x = 0; x < map.info.width; ++x) {
    for (size_t y = 0; y < map.info.height; ++y) {
      /* I _think_ this is right -- I'll check the GMapping source. */
      size_t idx = map.info.width * x + y;
      if (map.data[idx] == 0 && x_min == -2) { /* weird if it finds an isolated open space it cant get to but i dont think thats very plausible*/
        x_min = x;
        y_min = y;
      } else if (map.data[idx] == 0) {
        x_max = x;  //no other requirements, this is because the x and y will always be at their highest
        y_max = y;
      }
    }
  }

  res.miniverse[] = new hide_and_seek::tp_msg[(x_max - x_min) * (y_max - y_min)];    //tp miniverse
  // ^^ is this legal? i dont see why it would be
  for (int x = x_min; x <= x_max; ++x) {
    for (int y = y_min; y <= y_max; ++y) {
/*
        the following code iterates thru every spot on the grid in the room to see if it is occupied
        if it is it the 8 surrounding spots on the grid are analyzed, if they are free, a tourist point is marked at the central spot first checked at the beginning of the loop
        it could be done with a single if statement with a ton of && operators but thats so fucking ugly and this way shorts circuits anyway soooo.......
*/

      size_t idx = map.info.width * x + y;/* Thank you Hunter! :) */
      if (prev_check && !map.data[idx - map.info.width + 1] && !map.data[idx + 1] &&
        !map.data[idx + map.info.width + 1])
      {
        ROS_INFO("Tourist point marked at ( %ld , %ld )", (long int) x, (long int) y);
        hide_and_seek::tp_msg tourist_point;
        tourist_point.tp_pos.x = x;
        tourist_point.tp_pos.y = y;
        tourist_point.visited = req.miniverse[miniverse_index].visited;
        res.miniverse[miniverse_index] = tourist_point;
        ++miniverse_index;
        prev_check = true;
        continue;
      }
      if (!map.data[idx] && !map.data[idx - map.info.width - 1] &&
        !map.data[idx - map.info.width] && !map.data[idx - map.info.width + 1] &&
        !map.data[idx - 1] && !map.data[idx + 1] && !map.data[idx + map.info.width - 1] &&
        !map.data[idx + map.info.width] && !map.data[idx + map.info.width + 1])
      {

        ROS_INFO("Tourist point marked at ( %ld , %ld )", (long int) x, (long int) y);
        hide_and_seek::tp_msg tourist_point;
        tourist_point.tp_pos.x = x;
        tourist_point.tp_pos.y = y;
        tourist_point.visited = req.miniverse[miniverse_index].visited;
        res.miniverse[miniverse_index] = tourist_point;
        ++miniverse_index;
        prev_check = true;
        continue;
      }
      prev_check = false;
    }
  }
  res.miniverse_size = miniverse_index + 1;

  //the below, unfinished, code's purpose is to remap the tourist points to counterparts in another tpl after changes have been oberved to the map. TODO(shaneallcroft) do that yo!

  if (req.miniverse_size == res.miniverse_size &&
    (int)(res.miniverse[miniverse_index].tp_pos.x - req.miniverse[miniverse_index].tp_pos.x) /
    PROXIMITY_THRESHOLD == 0 &&
    (int)(res.miniverse[miniverse_index].tp_pos.y - req.miniverse[miniverse_index].tp_pos.y) /
    PROXIMITY_THRESHOLD == 0)                                                                                                                                                                                                                                                                 //ROS help us if this if evaluates false...
  {
    return true;
  }
  ROS_INFO(
    "THE MAP HAS CHANGED OR TURTLEBOT HAS BECOME CONFUSED ........ NOW ATTEMPTING TO RE MAP THE VISIT DATA");

  return true;
}

int main(int argc, char ** argv)
{
  ros::init(argc, argv, "tp_list_maker_server");

  Mratplg mratplgster;
  ros::spin();

  return 0;
}
