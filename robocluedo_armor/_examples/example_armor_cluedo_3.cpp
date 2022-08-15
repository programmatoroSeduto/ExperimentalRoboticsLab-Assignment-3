
/********************************************//**
 * @example "ArmorCluedo - 003 - Reasoning Workflow"
 * 
 * In this example I'm going to show how the combination of the two classes
 * ArmorTools and ArmorCluedo structure the reasoning process, which is 
 * at the base of how RCL works. 
 * 
 * @warning this code was written only for your understanding. Probably
 * 		it won't compile. 
 * 
 * \include example_armor_cluedo_3.cpp
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
	ros::init( argc, argv, "example_armor_cluedo_3" );
	ros::NodeHandle nh;
	
	
	
	
	// === before starting (see the previous examples)
	
	ArmorCluedo armor;
	armor.Init( "/root/ontologies/my_super_ontology.owl" );
	armor.AddIndiv( "John", "PERSON" );
	armor.AddIndiv( "Pippo", "PERSON" );
	armor.AddIndiv( "Broccoli", "WEAPON" );
	armor.AddIndiv( "AK-47", "WEAPON" );
	armor.AddIndiv( "Stadium", "PLACE" );
	armor.AddIndiv( "Bathroom", "PLACE" );
	armor.AddIndiv( "SmartHypothesis", "HYPOTHESIS" );
	armor.UpdateOntology( );
	armor.SetObjectProperty( "where", "Smarthypothesis", "Stadium" );
	armor.SetObjectProperty( "who", "Smarthypothesis", "John" );
	armor.SetObjectProperty( "what", "Smarthypothesis", "Broccoli" );
	armor.UpdateOntology( );
	
	
	
	
	// === QUERYING HYPOTHESES
	
	// find the tags of the complete hypotheses
	//    in this example, the tag 'SmartHypothesis' is complete
	std::vector<std::string> complete_hypotheses = FindCompleteHypotheses( );
	
	
	
	
	// === INCONSISTENT HYPOTHESES
	
	// you can also find the inconsistent hypotheses
	//    ATTENTION! In this implementation there's a difference between
	//       "discarded" hypotheses and "inconsistent" hypotheses, due to
	//       some issues inside aRMOR.
	//    in any case, using FindCompleteHypotheses() is recommended. 
	std::vector<std::string> inconsistent_hypotheses = FindCompleteHypotheses( );
	
	// let's make 'SmartHypothesis' inconsistent
	armor.SetObjectProperty( "where", "SmatHypothesis", "Bathroom" );
	armor.UpdateOntology( );
	// now the hypothesis doesn't appear when calling FindCompleteHypotheses( )
	
	
	
	
	// === DISCARD HYPOTHESES
	
	// the implementation of ArmorCluedo contains a workaroud, due to an 
	//    issue in the implementation of aRMOR in "REMOVE" commands. 
	// let's suppose that there's another hypothesis, completed, but
	//    discarded due to a negative answer by the oracle
	
	armor.AddIndiv( "AnotherSmartHypothesis", "HYPOTHESIS" );
	armor.SetObjectProperty( "where", "AnotherSmarthypothesis", "Stadium" );
	armor.SetObjectProperty( "who", "AnotherSmarthypothesis", "John" );
	armor.SetObjectProperty( "what", "AnotherSmarthypothesis", "Broccoli" );
	armor.UpdateOntology( );
	
	// let's discard it with the function
	armor.RemoveHypothesis( "AnotherSmarthypothesis" );
	// THE HYPOTHESIS IS STILL PRESENT IN THE ONTOLOGY! 
	//    but now it is tracked as deleted, so the FindCompleteHypotheses()
	//    won't return it later. 
	// you cannot de-discard hypotheses. 
	
	return 0;
}
