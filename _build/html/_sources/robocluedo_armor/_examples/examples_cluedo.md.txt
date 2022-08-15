
# ARMOR CLUEDO EXAMPLES -- 1 -- Working on Individuals

```{warning}
this code was written only for your understanding. Probably it won't compile.
```

## Working on Individuals

The class ArmorCluedo provides some powerful methods, specific for individuals. RCL requeires a limited numer of operations right now. 

Here is a simple usage of this class, employed in manipulating

```c++

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

```



# ARMOR CLUEDO EXAMPLES -- 2 -- Working with properties

```{warning}
this code was written only for your understanding. Probably it won't compile.
```

## Working with properties

RCL requires not only adding individuals, but also working with the properties of the individuals. You can get all the available methods from this small example.

```c++

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

```


# ARMOR TOOLS EXAMPLES -- 3 -- Reasoning Workflow

```{warning}
this code was written only for your understanding. Probably it won't compile.
```

## Reasoning Workflow

In this example I'm going to show how the combination of the two classes ArmorTools and ArmorCluedo structure the reasoning process, which is at the base of how RCL works.

```c++

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

```
