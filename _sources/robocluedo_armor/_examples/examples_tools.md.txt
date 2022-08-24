
# ARMOR TOOLS EXAMPLES -- 1 -- Essential usage of ArmorTools

```{warning}
this code was written only for your understanding. Probably it won't compile.
```

## Essential usage of ArmorTools

In this little example I want to show you which are the main features of the class ArmorTools. It can speed up really the usage of aRMOR, making easier the coding phase.

The proposed workflow is always the same:

1. Constructor, setting the user infos
2. Connection to the service
3. Loading the ontology
4. (optional) check the calidity of the connection

Done this, using armor is simple: just call a function! 

```c++

#include "ros/ros.h"
#include "armor_tools/armor_tools.h"
#include "armor_msgs/ArmorDirective.h"
#include "armor_msgs/ArmorDirectiveReq.h"
#include "armor_msgs/ArmorDirectiveRes.h"

#include <iostream>
#include <string>

int main( int argc, char* argv[] )
{
	ros::init( argc, argv, "example_armor_tools_1" );
	ros::NodeHandle nh;
	
	
	
	
	// STEP 0 === CONSTRUCTOR
	
	// constructor with no arguments: 
	//    set default client name and reference name
	//    debug mode is off 
	// infos are stored for later in the load step
	ArmorTools armor;
	// another way: set your client name and reference
	ArmorTools armor2( "my_client", "my_reference" );
	// only debug mode, with default infos
	ArmorTools armorDBmode( true );
	
	// you can toggle the debug mode in any time using
	armor.SwitchDebugMode( );
	
	
	
	
	// STEP 1 === CONNECTION
	
	// the connection to the server is stored inside the class
	//    using the default timeout
	if( !armor.Connect( ) )
	{
		std::cout << "connection error!" << std::endl;
		return 0;
	}
	// if you want to set another timeout, use this:
	//    armor.Connect( ros::Duration( 36 ) );
	
	
	
	
	// STEP 2 === LOAD ONTOLOGY FROM FILE
	
	// straightforward: use the dedicated function
	// alternatively, you can load manually the ontology; there are several ways
	if( !armor.LoadOntology( "/root/ontology/my_pizza_ontology.owl" ) )
	{
		// something went wrong...
	}
	// see ArmorTools::LoadOntology() for more details
	
	// check the calidity of the interface
	if( !armor.TestInterface( ) )
	{
		// something went wrong...
	}
	
	
	
	
	// STEP 3 === REQUEST & CALLS
	
	// quickest way to call aRMOR
	armor.SendCommand( "ADD", "IND", "CLASS", "Giampiero", "PERSON" );
	
	// manual aRMOR call
	armor_msgs::ArmorDirective srvmsg = armor.GetRequest( "ADD", "IND", "CLASS", "Susanna", "PERSON" );
	armor.CallArmor( srvmsg );
	
	// get the success flag
	//    WARNING: sometimes it doesn't indicate a failure, but a 
	//      common response of the aRMOR service (i.e. testing existence
	//      of an individual which doesn't exist makes the success flag false.)
	if( !armor.Success( ) ) { /* ... */ }
	
	// REMEMBER TO UPDATE after the callings!
	armor.UpdateOntology( );
	
	
	
	
	// STEP 4 === RESPONSE
	
	// check the last request
	armor_msgs::ArmorDirectiveReq& req = armor.GetLastReq( );
	armor_msgs::ArmorDirectiveRes& res = armor.GetLastRes( );
	
	// print one of the sections of the last service message
	armor.PrintLastReq( );
	armor.PrintLastRes( );
	
	// generic prints
	armor.PrintRequest( req );
	armor.PrintResponse( res );
	
	// error checking
	int last_err_code = armor.GetLastErrorCode( ); // 0:success
	std::string error_description = armor.GetLastErrorDescription( ); // "":success ... or strange error
	
	return 0;
}

```

## a more advanced example

```c++
std::string ontology_file_path = "/where/is/your/owl/file.owl";
ArmorTools armor( true );
if( !armor.ConnectAndLoad( ontology_file_path ) )
{
	ROS_WARN_STREAM( OUTLABEL << "ERROR: unable to ConnectAndLoad() aRMOR tools" );
	return 0;
}

armor.SendCommand( "DISJOINT", "CLASS", "", "PERSON", "WEAPON" );
armor.SendCommand( "DISJOINT", "CLASS", "", "WEAPON", "PLACE" );
armor.SendCommand( "DISJOINT", "CLASS", "", "PERSON", "PLACE" );
armor.UpdateOntology( );

armor.SendCommand( "ADD", "IND", "CLASS", "Jim", "PERSON" );
armor.SendCommand( "ADD", "OBJECTPROP", "IND", "who", "HP3", "Jim" );
armor.SendCommand( "ADD", "IND", "CLASS", "Gun", "WEAPON" );
armor.SendCommand( "ADD", "OBJECTPROP", "IND", "what", "HP3", "Gun" );
armor.SendCommand( "ADD", "IND", "CLASS", "Stadium", "PLACE" );
armor.SendCommand( "ADD", "OBJECTPROP", "IND", "where", "HP3", "Stadium" );
armor.UpdateOntology( );
armor.SendCommand( "QUERY", "IND", "CLASS", "COMPLETED" );
ROS_INFO_STREAM( OUTLABEL << "(1) completed hypotheses:" );
armor.PrintLastRes( );

```


# ARMOR TOOLS EXAMPLES -- 2 -- The quickest way to use ArmorTools

```{warning}
this code was written only for your understanding. Probably it won't compile.
```

## The quickest way to use ArmorTools

This class is a very powerful tool, as I want to show you here. Instead of writing hundreds of lines of code, you can interact with aRMOR in few function calls. The most of times you'll use this interface in this way.

```c++

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

```

