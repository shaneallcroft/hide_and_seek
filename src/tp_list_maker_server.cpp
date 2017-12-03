1//I'm here to  S E R V E  you ;) 
#include "ros/ros.h"
#include "hide_and_seek/tp_msg.h"
#include "hide_and_seek/tp_map_srv.h"
#include "nav_msgs/OccupancyGrid.h"//PICK AN ACTUAL MAP MESSAGE YO MAYBE OCCUPANCY GRID??
#include "nav_msgs/GetMap.h"

class Mratplg//Map Reciever and Tourist Point List Generator
{

public:
  Mratplg()
  {
    tpl_service = n.advertiseService("generate_new_tp_list", generate_new_tpl);
    map_reciever = n.serviceClient<map_server>("map_server");//this cant be right
    ROS_INFO("Ready to generate some A1 TPL's");
    sub = n.subscribe("amcl_pose", 10, &Mratplg::current_pos_retriever, this);
  }
      //prototypes bb
  bool Mratplg::generate_new_tpl(hide_and_seek::tp_map_srv::Request &req, hide_and_seek::tp_map_srv::Response &res);
  // void current_pos_retriever(const geometry_msgs::PoseWithCovarianceStamped::ConstPtr& current_pos);
private:
  ros::NodeHandle n;
  ros::ServiceServer tpl_service;
  ros::ServiceClient map_reciever;
  //geometry_msgs::PoseWithCovarianceStampedMessage current_position;
  //ros::ServiceClient current_position;
  ros::Subscriber sub;
};

/*  
void Mratplg::current_pos_retriever(const geometry_msgs::PoseWithCovarianceStamped::ConstPtr& current_pos){
  this.current_position = current_pos;
}//JUST FINISHED THIS 12:36 AM December 2nd
*/  
bool Mratplg::generate_new_tpl(hide_and_seek::tp_map_srv::Request &req, hide_and_seek::tp_map_srv::Response &res)
{
  nav_msgs::GetMap get_map_srv;//no values needed since this service only gets the map B)
  if(map_reciever.call(get_map_srv))
  {
    OccupancyGrid map = get_map_srv.response.map;//not positive if this is completely correct
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
      if(map.data[idx] == 0 && x_min == -2){  /* weird if it finds an isolated open space it cant get to but i dont think thats very plausible*/
	x_min = x;
	y_min = y;
      }
      else if(map.data[idx] == 0){
	x_max = x;  //no other requirements besides this because the x and y will always be at their highest
	y_max = y;
      }
    }
  }

  res.miniverse[] = new hide_and_seek::tp_msg[ (x_max - x_min) * (y_max - y_min) ];  //tp miniverse

  for(int x = x_min; x <= x_max; ++x)
  {
    for(int y = y_min; y <= y_max; ++y)
    {
      /*
	the following code iterates thru every spot on the grid in the room to see if it is occupied
	if it is it the 8 surrounding spots on the grid are analyzed, if they are free, a tourist point is marked at the central spot first checked at the beginning of the loop
	it could be done with a single if statement with a ton of && operators but thats so fucking ugly and this way shorts circuits anyway soooo.......
*/
      
      size_t idx = map.info.width * x + y;/* Thank you Hunter! :) */
      if(prev_check)
      {
	if(map.data[idx - map.info.width + 1] == 0)
	{
	  if(map.data[idx + 1] == 0)
	  {
	    if(map.data[idx + map.info.width + 1] == 0)
	    {
		hide_and_seek::tp_msg tourist_point;
		tourist_point.tp_pos.x = x;
		tourist_point.tp_pos.y = y;
		tourist_point.visited = false;
		res.miniverse[miniverse_index] = tourist_point;
		++miniverse_index;
		prev_check = true;
		continue;
	    }
	  }
	}        
      }
      if(map.data[idx] == 0)
      {
	if(map.data[idx - map.info.width - 1] == 0)
	{
	  if(map.data[idx - map.info.width] == 0)
	  {
	    if(map.data[idx - map.info.width + 1] == 0)
	    {
	      if(map.data[idx - 1] == 0)
	      {
		if(map.data[idx + 1] == 0)
		{
		  if(map.data[idx + map.info.width - 1] == 0)
		  {
		    if(map.data[idx + map.info.width] == 0)
		    {
		      if(map.data[idx + map.info.width + 1] == 0)
		      {
			 ROS_INFO("Tourist point marked at ( %ld , %ld )",(long int) x, (long int) y);
			 hide_and_seek::tp_msg tourist_point;
			 tourist_point.tp_pos.x = x;
			 tourist_point.tp_pos.y = y;
			 tourist_point.visited = false;
			 res.miniverse[miniverse_index] = tourist_point;
			 ++miniverse_index;
			 prev_check = true;
			 continue;
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}  
      }
      prev_check = false;
    }
  }
  
  
  /* SHANE'S WEIRD OCCUPANCY GRID FORCE CODE GRAVEYARD BELOW PAY YOUR RESPECTS HERE

     "COME IN PEACE

     REST IN PIECES"

     "On the shoulders of giants..... In the arms of an angel.............."

     "If something is learned, nothing is truly lost.... :)"

int[map.info.width * map.info.height][4] forces_at_play = { };//Seed the documentation for a fly af explanation
  int distance_to_left_edge = 0;
  int distance_to_right_edge = map.info.width;
  for( int i = 0; i < map.info.width * map.info.height; i++)
  {
    if(map.data[i] != 100)
    {
      continue;
    }
    //THE STUFF BELOW ALMOST DEF DOESNT WORK: I NEED TO FIX THIS SHIT
    distance_to_left_edge = (i >= map.info.width) ? i % map.info.width : i;
    if(distance_to_left_edge > 3) distance_to_left_edge = 3;
    distance_to_right_edge = (i >= map.info.width) ? map.info.width - ((i % map.info.width) + 1) : map.info.width - i;
    if(distance_to_right_edge > 3) distance_to_right_edge = 3;
    distance_to_top_edge = ((i - (map.info.width * 3)) > map.info.width) ? 3 : ((i - (map.info.width * 2) > map.info.width)) ? 2 : ( i > map.info.width) ? 1 : 0;//they call me tsar of the ternaries babEEEE!
    distance_to_bottom_edge = ((i + (map.info.width * 3)) < map.info.width * map.info.length) ? 3 : ((i + (map.info.width * 2) < map.info.width * map.info.length)) ? 2 : ( map.info.width * map.info.length > i + map.info.width) ? 1 : 0;//shoouuuuuuld work
    //we actually dont care if it gives force values to occupied spaces, there should be no way for the bot to get in.
    
    switch(distance_to_left_edge)
     {
    case 3:
      if(map.data[i - 1] == 100) break;
      forces_at_play[i-3][1] -= 1;
    case 2:
      forces_at_play[i-2][1] -= 2;
    case 1:
      forces_at_play[i-1][1] -= 3;
    }
    switch(distance_to_right_edge)
    {
    case 3:
      if(map.data[i + 1] == 100) break;
      forces_at_play[i+3][1] += 1;
    case 2:
      if(map.data[i + 2] == 100);//NEED TO EDGE CASE FOOL PROOTH THESE BAD BOIS MORE BUT IM WASTING TOO MUCH TIME RN GOTTA MOVE ON
      elseforces_at_play[i+2][1] += 2;
    case 1:
      forces_at_play[i+1][1] += 3;
    }
    switch(distance_to_top_edge)
    {
    case 3:
      forces_at_play[i - (map.info.width * 3)][3] += 1;
    case 2:
      if(distance_to_left_edge >= 2) forces_at_play[i - (map.info.width * 2) - 2][2] -= 1;
      if(distance_to_right_edge >= 2) forces_at_play[i - (map.info.width) + 1][0] += 1;
      forces_at_play[i - (map.info.width * 2)][3] += 2;
    case 1:
      if(distance_to_left_edge >= 1) forces_at_play[i - (map.info.width) - 1][2] -= 2;
      if(distance_to_right_edge >= 1) forces_at_play[i - (map.info.width) + 1][0] += 2;
      forces_at_play[i - (map.info.width * 1)][3] += 3;
    }
    switch(distance_to_bottom_edge)
    {
    case 3:
      forces_at_play[i + (map.info.width * 3)][3] -= 1;
    case 2:
      if(distance_to_left_edge >= 2) forces_at_play[i + (map.info.width * 2) - 2][0] -= 1;
      if(distance_to_right_edge >= 2) forces_at_play[i + (map.info.width * 2) + 2][2] += 2;
      forces_at_play[i + (map.info.width * 2)][3] -= 2;
    case 1:
      if(distance_to_left_edge >= 1) forces_at_play[i + map.info.width - 1 ][0] -= 2;
      if(distance_to_right_edge >= 2) forces_at_play[i + (map.info.width * 1) + 1][2] += 2;
      forces_at_play[i + (map.info.width * 1)][3] -= 3;
    }
    
  }
  //collapse forces into single direction bubgers
  
  //RELEASE THE TOURIST POINTS
  for(int i = 0; i < NUM_OF_TURNS; i++)
  {
    for(int j = 0; j < map.info.width * map.info.length; j++)
    {
      for(int k = 0; k < map.info.width * map.info.length; k++)
      {
	int highest_scoring_direction;
	int highest_force_strength = 0;
	int lowest_scoring_direction;
	int lowest_force_strength = 3;
	int num_of_active_directions = 0;
	for(int m = 0; m < 4; m++){
	  if(forces_at_work[k][m] > 0){
	    num_of_active_directions++;
	  }
	}
	if(num_of_active_directions == 1 || num_of_active_directions == 0)
	{
	  continue;
	}
	//coulda made these a switch statement. Oh shwell
	
	for(int m = 0; m < 4; m++)
	{
	  if(highest_force_strength < forces_at_work[k][m])
	  {
	    highest_force_strength = forces_at_work[k][m];
	    highest_scoring_direction = m;
	  }
	  // = (forces_at_work[k][m] > highest_force_strength) ? forces_at_work[k][m] : highest_force_strength;
	}
	for(int m = 0; m < 4; m++)
	{
	  if(lowest_force_strength > forces_at_work[k][m])
	  {
	    lowest_force_strength = forces_at_work[k][m];
	    lowest_scoring_direction = m;
	  } 
	  // = (forces_at_work[k][m] > highest_force_strength) ? forces_at_work[k][m] : highest_force_strength;
	}
	if(num_of_active_directions == 2)
	{
	  boolean same_sign = false;
	  if(highest_force_stength / lowest_force_strength >= 0){
	    same_sign = true;
	  }
	  //OKAY SO ANOTHER TO DO FOR ME. THIS PART IS TAKING TOO LONG GONNA COME BACK AND FINISH THE PROCESS OF THEM MERGING WHEN IM NOT SO SLEEPY AND AGAINST THE NOTION OF MAKING METHODS FOR CODE REUSE
	  switch(((highest_scoring_direction - lowest_scoring_direction) >= 0 ) ? (highest_scoring_direction - lowest_scoring_direction) : (highest_scoring_direction - lowest_scoring_direction) * -1)
	  {
	  case 0:
	    break;
	  case 1:
	    if(same_sign)
	    {
	      forces_at_play[k][lowest_scoring_direction] = 0;
	      break;
	    }
	    forces_at_play[k][highest_scoring_direction] = 0;
	    forces_at_play[k][lowest_scoring_direction] = 0;
	    forces_at_play[k][(highest_scoring_direction > lowest_scoring_direction) ? highest_scoring_direction - 1 : lowest_scoring_direction - 1] = highest_force_strength;
	    break;//All of the stuff in this switch does not work currently but will soon

	    
	  case 2:
	    forces_at_play[k][highest_scoring_direction] = 0;
	    forces_at_play[k][lowest_scoring_direction] = 0;
	    forces_at_play[k][(highest_scoring_direction > lowest_scoring_direction) ? highest_scoring_direction - 1 :lowest_scoring_direction - 1] = highest_force_strength;
	    break; 
	  }
	 
	}
	
      }
    }
    int position_in_1d = ((int) current_position.position.x + (int) (current_position.position.y * map.info.width * -1));
    if(i == 0)
    {
      //Gotta convert tp_pos otherwise this wont work
      tp_msg tp_first;
      tp_msg tp_second;
      tp_msg tp_third;
      tp_msg tp_fourth;
      tp_first.tp_pos = position_in_1d - map.info.width;
      tp_second.tp_pos = position_in_1d - 1;
      tp_third.tp_pos = position_in_1d + map.info.width;
      tp_fourth.tp_pos = position_in_1d +1;
      res.miniverse[0] = tp_first;
      res.miniverse[1] = tp_second;
      res.miniverse[2] = tp_third;
      res.miniverse[3] = tp_fourth;
    }  
    for(int z = 0; z < 4; z++)
    {
      res.miniverse[z].tp_pos = res.miniverse[z].tp_pos + forces_at_play[position_1d][2];//FINISH THIS TODO!!!!
										     
    }
  }
  */
  return true;
}


//void mark_tp( hide_and_seek::tp_map_srv::Response &res , int x, int y);

int main(int argc, char **argv)
{
  ros::init(argc, argv, "tp_list_maker_server");

  Mratplg mratplgster;
  ros::spin();

  return 0;
}

/*int mark_tp( hide_and_seek::tp_map_srv::Response &res, int x, int y, int  )  //ehhhhh not really nessesary to make this a function but its neato
{
  hide_and_seek::tp_msg tourist_point;
  tourist_point.tp_pos.x = x;
  tourist_point.tp_pos.y = y;
  tourist_point.visited = false;
  res.miniverse[miniverse_index] = tourist_point;
  ++miniverse_index;
}
*/
// distance_to_left_edge = ( (((int) current_position.position.x + (int) (current_position.position.y * map.info.width * -1)) % map.info.width) - map.info.width;........soon
