
/********************************************//**
 * @example "ArmorCluedo - 002 - Working with properties"
 * 
 * RCL requires not only adding individuals, but also working with the 
 * properties of the individuals. You can get all the available methods
 * from this small example. 
 * 
 * @warning this code was written only for your understanding. Probably
 * 		it won't compile. 
 * 
 * \include example_armor_cluedo_2.cpp
 * 
 ***********************************************/

#include "ros/ros.h"
#include "armor_tools/armor_tools.h"
#include "armor_tools_armor_cluedo.h"
#include "armor_msgs/ArmorDirective.h"
#include "armor_msgs/ArmorDirectiveReq.h"
#include "armor_msgs/ArmorDirectiveRes.h"

#include <iostream>
#include <string>

int main( int argc, char* argv[] )
{
	ros::init( argc, argv, "example_armor_cluedo_2" );
	ros::NodeHandle nh;
	
	
	
	
	// === before starting...
	
	ArmorCluedo armor;
	armor.Init( "/root/ontologies/my_super_ontology.owl" );
	armor.AddIndiv( "John", "PERSON" );
	armor.AddIndiv( "Broccoli", "WEAPON" );
	armor.AddIndiv( "Stadium", "PLACE" );
	armor.AddIndiv( "SmartHypothesis", "HYPOTHESIS" );
	
	// REMEMBER TO UPDATE!
	armor.UpdateOntology( );
	
	
	
	
	// === ADD SOME PROPERTIES
	
	armor.SetObjectProperty( "where", "Smarthypothesis", "Stadium" );
	armor.SetObjectProperty( "who", "Smarthypothesis", "John" );
	armor.SetObjectProperty( "what", "Smarthypothesis", "Broccoli" );
	
	// REMEMBER TO UPDATE!
	armor.UpdateOntology( );
	
	
	
	
	// === CHECKINGS ON PROPERTIES
	
	// the main function is
	std::vector<std::string> values = GetValuedOfIndiv( "where", "SmartHypothesis" );
	
	
	
	
	return 0;
}
