
/********************************************//**
 * @example "ArmorCluedo - 001 - Working on Individuals"
 * 
 * The class ArmorCluedo provides some powerful methods, specific for 
 * RCL. Here is a simple usage of this class, employed in manipulating
 * individuals. RCL requeires a limited numer of operations right now. 
 * 
 * @warning this code was written only for your understanding. Probably
 * 		it won't compile. 
 * 
 * \include example_armor_cluedo_1.cpp
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
	ros::init( argc, argv, "example_armor_cluedo_1" );
	ros::NodeHandle nh;
	
	
	
	// === INIT
	
	// instanciation and connection
	ArmorCluedo armor;
	armor.Init( "/root/ontologies/my_super_ontology.owl" );
	
	
	
	// === ADDING INDIVIDUALS
	
	// add individuals
	armor.AddIndiv( "John", "PERSON" );
	armor.AddIndiv( "Broccoli", "WEAPON" );
	armor.AddIndiv( "Stadium", "PLACE" );
	armor.AddIndiv( "SmartHypothesis", "HYPOTHESIS" );
	
	// REMEMBER TO UPDATE!
	armor.UpdateOntology( );
	
	
	
	// === INFORMATIONS about the individuals
	
	// get the classes an individual belongs to
	std::vector<std::string> classes_of_john = armor.GetClassOfIndiv( "John" );
	
	// get the individuals belonging to a class
	std::vector<std::string> indiv_of_class_weapon = armor.GetIndivOfClass( "WEAPON" );
	
	// check if an individual exists
	if( armor.ExistsIndiv( "Tower" )   == false )  std::cout << "'Tower' not defined." << std::endl;
	if( armor.ExistsIndiv( "Stadium" ) == true  ) std::cout << "'Stadium' exists!" << std::endl;
	
	
	
	return 0;
}
