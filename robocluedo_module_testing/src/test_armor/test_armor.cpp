
/********************************************//**
 *  
 * @file test_armor.cpp
 * @brief A preliminary test on aRMOR service. 
 * 
 * @authors Francesco Ganci (S4143910)
 * @version v1.0
 * 
 * Sometimes it is convenient to directly test the interface without
 * using classes such as ArmorTools or ArmorCluedo. This file contains a
 * small test session on aRMOR with some minimal functions. 
 *  
 * @see test_armor.launch 
 * 
 ***********************************************/

#include "ros/ros.h"
#include "armor_tools/armor_tools.h"

#include "armor_msgs/ArmorDirective.h"
#include "armor_msgs/ArmorDirectiveList.h"
#include "armor_msgs/ArmorDirectiveReq.h"
#include "armor_msgs/ArmorDirectiveRes.h"
#include "armor_msgs/QueryItem.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define ONTOLOGY_PARAM "cluedo_path_owlfile"
#define SERVICE_ARMOR_SINGLE_REQUEST "/armor_interface_srv"
#define SERVICE_ARMOR_MULTIPLE_REQUESTS "/armor_interface_serialized_srv"
#define ARMOR_CLIENT_NAME "robocluedo"
#define ARMOR_REFERENCE_NAME "cluedo_ontology"
#define ARMOR_CMD_LOAD "LOAD"
#define ARMOR_TIMEOUT 5.00

#define OUTLABEL "[test_armor] "
#define OUTLOG std::cout << OUTLABEL << " "
#define OUTERR OUTLOG << "ERROR: "
#define LOGSQUARE( str ) "[" << str << "] "
#define SS( this_string ) std::string( this_string )
#define SSS( this_thing ) std::to_string( this_thing )

// check if a given file exists
bool fileExist( std::string path )
{
    return (std::ifstream(path)).good();
}

// get args to fill in the armor message
std::vector<std::string> armorGetArgs(
	std::string arg1,
	std::string arg2 = "",
	std::string arg3 = "",
	std::string arg4 = "",
	std::string arg5 = ""
)
{
	std::vector<std::string> to_return;
	to_return.push_back( arg1 );
	to_return.push_back( arg2 );
	to_return.push_back( arg3 );
	to_return.push_back( arg4 );
	to_return.push_back( arg5 );
	
	return to_return;
}

// armor call
armor_msgs::ArmorDirective armorGetRequest( 
	std::string client,
	std::string reference,
	std::string command,
	std::string first_spec = "",
	std::string second_spec = "",
	std::vector<std::string> args = std::vector<std::string>(5, "")
 )
{
	armor_msgs::ArmorDirective adsrv;
	armor_msgs::ArmorDirectiveReq ad;
	
	ad.client_name = client;
	ad.reference_name = reference;
	
	ad.command = command;
	ad.primary_command_spec = first_spec;
	ad.secondary_command_spec = second_spec;
	ad.args = args;
	
	adsrv.request.armor_request = ad;
	
	return adsrv;
}

// print the armor request
void armorPrintRequest( armor_msgs::ArmorDirective& d )
{
	std::string str = SS(OUTLABEL) + SS("Print Request: \n");
	armor_msgs::ArmorDirectiveReq r = d.request.armor_request;
	
	str += SS("\tclient_name : ") + SS( r.client_name ) + SS("\n");
	str += SS("\treference_name : ") + SS( r.reference_name ) + SS("\n");
	str += SS("\tcommand : ") + SS( r.command ) + SS("\n");
	str += SS("\tprimary_command_spec : ") + SS( r.primary_command_spec ) + SS("\n");
	str += SS("\tsecondary_command_spec : ") + SS( r.secondary_command_spec ) + SS("\n");
	
	str += SS("\targs : [");
	for( std::string arg : r.args )
		str += SS( arg ) + SS(" ");
	str += SS( "]" );
		
	str += SS( "\n\t---" );
	
	ROS_INFO_STREAM( str );
}

// print the whole armor response
void armorPrintResponse( armor_msgs::ArmorDirective& d )
{
	std::string str = SS(OUTLABEL) + SS("Print Response: \n");
	armor_msgs::ArmorDirectiveRes r = d.response.armor_response;
	
	str += SS("\tsuccess : ") + ( r.success ? SS("true") : SS("false") ) + SS("\n");
	str += SS("\ttimeout : ") + ( r.timeout ? SS("true") : SS("false") ) + SS("\n");
	str += SS("\texit_code : ") + SSS(r.exit_code) + SS("\n");
	str += SS("\terror_description : ") + SS(r.error_description) + SS("\n");
	str += SS("\tis_consistent : ") + ( r.is_consistent ? SS("true") : SS("false") ) + SS("\n");
	
	str += SS("\tqueried_objects :\n");
	for( std::string s : r.queried_objects )
		str += SS("\t-\t") + s + SS("\n");
	
	str += SS("\tsparql_queried_objects : \n");
	for( armor_msgs::QueryItem s : r.sparql_queried_objects )
		str += SS("\t-\t") + SS("key: ") + SS( s.key ) + SS( " | value: " ) + SS( s.value ) + SS("\n");
	
	str += SS( "\t---" );
	
	ROS_INFO_STREAM( str );
}

// call to aRMOR service
bool armorCallService( ros::ServiceClient& armorClient, armor_msgs::ArmorDirective& armorsrvdata )
{
	if( armorClient.call( armorsrvdata ) )
	{
		/*
		if( armorsrvdata.response.armor_response.success )
		{
			return true;
		}
		else
			return false;
		*/
		return true;
	}
	else
		return false;
}

/*
 * nuovo indvidual:
 * 	ADD IND CLASS [ individual, class ]
 * 
 * nuova proprietà:
 *  ADD OBJECTPROP IND [ prop, domain, img ]
 * 
 * caricamento da file:
 * 	LOAD FILE [ path ]
 * 
 */
void armorTestSession( ros::ServiceClient& cl, std::string path )
{
	//armor_msgs::ArmorDirective adsrv;
	//ROS_INFO_STREAM( OUTLABEL << "client_name" << LOGSQUARE( ARMOR_CLIENT_NAME ) << "reference_name" << LOGSQUARE( ARMOR_REFERENCE_NAME ) );
	
	std::vector<armor_msgs::ArmorDirective> directives;
	
	// caricamento dell'ontology da file
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "LOAD", "FILE", "", armorGetArgs( path, "http://www.emarolab.it/cluedo-ontology", "false", "PELLET", "true" ) ) );
	
	// ipotesi consistente: where:Stadium, who:Jim, what:Gun
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "ADD", "IND", "CLASS", armorGetArgs( "Jim", "PERSON" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "ADD", "OBJECTPROP", "IND", armorGetArgs( "who", "HP3", "Jim" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "ADD", "IND", "CLASS", armorGetArgs( "Gun", "WEAPON" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "ADD", "OBJECTPROP", "IND", armorGetArgs( "what", "HP3", "Gun" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "ADD", "IND", "CLASS", armorGetArgs( "Stadium", "PLACE" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "ADD", "OBJECTPROP", "IND", armorGetArgs( "where", "HP3", "Stadium" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "REASON", "", "", armorGetArgs( "" ) ) );
	
	// l'ipotesi diventa inconsistente quando viene aggiunto where:Lounge E SI FA DISJOINT con l'altro luogo
	//    (a quanto pare quando si aggiunge l'etichetta, aRMOR pensa sia la stessa cosa aggiunta prima ... perchè?)
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "ADD", "IND", "CLASS", armorGetArgs( "Lounge", "PLACE" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "DISJOINT", "IND", "", armorGetArgs( "Lounge", "Stadium" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "ADD", "OBJECTPROP", "IND", armorGetArgs( "where", "HP3", "Lounge" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "REASON", "", "", armorGetArgs( "" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "SAVE", "INFERENCE", "", armorGetArgs( "/root/Desktop/ROBOCLUEDO_ONTOLOGY.owl" ) ) );
	// directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "", "", "", armorGetArgs( "", "", "", "", "" ) ) );
	
	/*
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "QUERY", "IND", "CLASS", armorGetArgs( "COMPLETED" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "QUERY", "IND", "CLASS", armorGetArgs( "INCONSISTENT" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "QUERY", "OBJECTPROP", "IND", armorGetArgs( "who", "HP0" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "QUERY", "OBJECTPROP", "IND", armorGetArgs( "where", "HP0" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "QUERY", "OBJECTPROP", "IND", armorGetArgs( "what", "HP0" ) ) );
	*/
	
	for( auto adsrv = directives.begin(); adsrv != directives.end(); ++adsrv )
	{
		armorPrintRequest( *adsrv );
		armorCallService( cl, *adsrv );
		armorPrintResponse( *adsrv );
	}
}

// test section
int main( int argc, char* argv[] )
{
	ros::init( argc, argv, "test_armor" );
	ros::NodeHandle nh;
	
	// path of the ontology file from parameter server
	std::string ontology_file_path;
	if( !ros::param::has( ONTOLOGY_PARAM ) )
	{
		ROS_INFO_STREAM( OUTLABEL << "ERROR: parameter '" << ONTOLOGY_PARAM << "' not defined." );
		return 0;
	}
	ros::param::get( ONTOLOGY_PARAM, ontology_file_path );
	if( !fileExist( ontology_file_path ) )
	{
		ROS_INFO_STREAM( OUTLABEL << "ERROR: the file '" << ONTOLOGY_PARAM << "' doesn't exist." );
		return 0;
	}
	ROS_INFO_STREAM( OUTLABEL << "Ontology found! " << LOGSQUARE( ontology_file_path ) );
	
	// request for aRMOR service single request
	ROS_INFO_STREAM( OUTLABEL << "Requiring client " << LOGSQUARE( SERVICE_ARMOR_SINGLE_REQUEST ) << "..." );
	ros::ServiceClient cl_single = nh.serviceClient<armor_msgs::ArmorDirective>( SERVICE_ARMOR_SINGLE_REQUEST );
	if( !cl_single.waitForExistence( ros::Duration( ARMOR_TIMEOUT ) ) )
	{
		ROS_INFO_STREAM( OUTLABEL << "ERROR: unable to contact the server - timeout expired (" << ARMOR_TIMEOUT << "s) " );
		return 0;
	}
	ROS_INFO_STREAM( OUTLABEL << "Requiring client " << LOGSQUARE( SERVICE_ARMOR_SINGLE_REQUEST ) << "... OK" );
	
	// request for aRMOR service multiple requests
	ROS_INFO_STREAM( OUTLABEL << "Requiring client " << LOGSQUARE( SERVICE_ARMOR_MULTIPLE_REQUESTS ) << "..." );
	ros::ServiceClient cl_multi = nh.serviceClient<armor_msgs::ArmorDirectiveList>( SERVICE_ARMOR_MULTIPLE_REQUESTS );
	if( !cl_single.waitForExistence( ros::Duration( ARMOR_TIMEOUT ) ) )
	{
		ROS_INFO_STREAM( OUTLABEL << "ERROR: unable to contact the server - timeout expired (" << ARMOR_TIMEOUT << "s) " );
		return 0;
	}
	ROS_INFO_STREAM( OUTLABEL << "Requiring client " << LOGSQUARE( SERVICE_ARMOR_MULTIPLE_REQUESTS ) << "... OK" );
	
	// start testing phase
	armorTestSession( cl_single, ontology_file_path );
	
	ROS_INFO_STREAM( OUTLABEL << "closing..." );
	return 0;
}
