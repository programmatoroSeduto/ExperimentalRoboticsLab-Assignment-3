
/********************************************//**
 * @example "ArmorTools - 002 - The quickest way to use ArmorTools"
 * 
 * This class is a very powerful tool, as I want to show you here. Instead
 * of writing hundreds of lines of code, you can interact with aRMOR in 
 * few function calls. The most of times you'll use this interface in this
 * way. <br><br>
 * 
 * @warning this code was written only for your understanding. Probably
 * 		it won't compile. 
 * 
 * \include example_armor_tools_2.cpp
 * 
 ***********************************************/

#include "ros/ros.h"
#include "armor_tools/armor_tools.h"
#include "armor_msgs/ArmorDirective.h"
#include "armor_msgs/ArmorDirectiveReq.h"
#include "armor_msgs/ArmorDirectiveRes.h"

int main( int argc, char* argv[] )
{
	ros::init( argc, argv, "example_armor_tools_2" );
	ros::NodeHandle nh;
	
	// preparing the client
	ArmorTools armor( );
	if( !armor.ConnectAndLoad( "/root/ontologies/civetta_ontology.owl" ) )
	{
		// something went wrong...
	}
	
	// call aRMOR
	armor.SendCommand( "ADD", "IND", "CLASS", "pizza", "WEAPON" );
	
	// REMEMBER TO UPDATE after the callings!
	armor.UpdateOntology( );
	
	return 0;
}
