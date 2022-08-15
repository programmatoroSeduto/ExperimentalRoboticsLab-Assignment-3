
/********************************************//**
 *  
 * @file cluedo_armor_interface.cpp
 * <div><b>ROS Node Name</b> 
 *      <ul><li>cluedo_armor_interface</li></ul></div>
 * @brief Dedicated RobotCLuedo-aRMOR client
 * 
 * @authors Francesco Ganci (S4143910)
 * @version v1.0
 * 
 * <b>Description:</b> <br>
 * <p>
 * This node implements an inferface which simplifies and abstracts the 
 * communication with the aRMOR service, with particular attention to the
 * most common operations performed by RCL during the search of a solution.<br>
 * </p>
 * 
 * <b>UML components</b><br>
 * (See \ref robocluedo_arch the overal architecture, for further informations)<br>
 * <img src="UML_components_cluedo_armor_interface.png" /><br>
 * 
 * <b>Services:</b> <br>
 * <ul>
 *     <li>
 * 			<i>/cluedo_armor/add_hint</i> : AddHint.srv <br>
 * 			    register a hint in the ontology <br><br>
 * 		</li>
 * 		<li>
 * 			<i>/cluedo_armor/find_consistent_h</i> : FindConsistentHypotheses.srv <br>
 * 			     find all the consistent hypotheses <br><br>
 * 		</li>
 * 		<li>
 * 			<i>/cluedo_armor/wrong_hypothesis</i> : DiscardHypothesis.srv <br>
 * 			     discard an hypothesis, due to a negative response from the oracle <br><br>
 * 		</li>
 * 		<li>
 * 			<i>/cluedo_armor/backup</i> : <a href="http://docs.ros.org/en/api/std_srvs/html/srv/Trigger.html">std_srvs::Trigger</a> <br>
 * 			    export the ontology to file <br><br>
 * 		</li>
 * </ul>
 * 
 * <b>Hidden services and topics</b> <br>
 * <ul>
 * 		<li>
 * 			(from ArmorTools : client) <i>/armor_interface_srv</i> <br>
 * 		</li>
 * </ul>
 * 
 * <b>Parameters:</b> <br>
 * <ul>
 * 		<li>
 * 			[GET] <i> \ref ONTOLOGY_PARAM </i> : string <br>
 * 			   path of the ontology <br><br>
 * 		</li>
 * 		<li>
 * 			[GET] <i> \ref PARAM_ONTOLOGY_BACKUP_PATH </i> : string <br>
 * 			   name for the backup ontology file exported <br><br>
 * 		</li>
 * </ul>
 * 
 ***********************************************/
 
#include <stdlib.h>
#include <unistd.h>

#include "ros/ros.h"
#include "armor_tools/armor_tools.h"
#include "armor_tools/armor_cluedo.h"

#include "std_srvs/Trigger.h"
#include "armor_msgs/ArmorDirective.h"
#include "armor_msgs/ArmorDirectiveList.h"
#include "armor_msgs/ArmorDirectiveReq.h"
#include "armor_msgs/ArmorDirectiveRes.h"
#include "armor_msgs/QueryItem.h"
#include "robocluedo_armor_msgs/AddHint.h"
#include "robocluedo_armor_msgs/Hypothesis.h"
#include "robocluedo_armor_msgs/FindConsistentHypotheses.h"
#include "robocluedo_armor_msgs/DiscardHypothesis.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define SERVICE_INTERFACE_ADD_HINT "/cluedo_armor/add_hint"
#define SERVICE_INTERFACE_FIND_CONSISTENT_HYP "/cluedo_armor/find_consistent_h"
#define SERVICE_INTERFACE_WRONG_HYPOTHESIS "/cluedo_armor/wrong_hypothesis"
#define SERVICE_INTERFACE_SAVE_ONTOLOGY "/cluedo_armor/backup"

#define ONTOLOGY_PARAM "cluedo_path_owlfile"
#define PARAM_ONTOLOGY_BACKUP_PATH "cluedo_path_owlfile_backup"
#define OUTLABEL "[cluedo_armor_interface]"
#define OUTLOG( msg ) ROS_INFO_STREAM( OUTLABEL << " " << msg );
#define LOGSQUARE( str ) "[" << str << "] "
#define SS( this_string ) std::string( this_string )
#define SSS( this_thing ) std::to_string( this_thing )



/// \private global connection to aRMOR interface
ArmorCluedo* armor;



/// \private check if a given file exists
bool fileExist( std::string path )
{
    return (std::ifstream(path)).good();
}



/********************************************//**
 *  
 * \brief delete all the occurrences of list2 inside list1
 * 
 * The function is employed in order to find the 
 * truly COMPLETE hypotheses, excluding the INCONSISTENT ones. 
 * 
 * @param list1 the array to be reduced
 * @param list2 the occurrences to delete from list1
 * 
 * @returns a copy of list1 without the elements of list2.
 * 
 ***********************************************/
std::vector<std::string> PerformDifferenceBetween( std::vector<std::string> list1, std::vector<std::string> list2 )
{
	std::vector<std::string> to_return;
	
	for( auto it1 = list1.begin(); it1 != list1.end(); ++it1 )
	{
		bool found = false;
		for( auto it2 = list2.begin(); it2 != list2.end(); ++it2 )
		{
			if( *it2 == *it1 ) 
			{
				found = true;
				break;
			}
		}
		
		if( !found )
			to_return.push_back( *it1 );
	}
	
	return to_return;
}



/********************************************//**
 *  
 * \brief implementation of service \ref SERVICE_INTERFACE_ADD_HINT
 * 
 * The service registers a property with a value referred to a hypothesis
 * given its numeric ID. Here is the followed procedure:
 * <ol>
 * <li>check if the hypothesis ID exists</li>
 * <li>if it doesn't exist, create it</li>
 * <li>check if the value exists</li>
 * <li>if the value doesn't exist, create it</li>
 * <li>set the property</li>
 * <li>update the ontology</li>
 * </ol>
 * 
 * @param hint    the hint to add to the ontology
 * @param success request accomplished or not
 * 
 * @see AddHint.srv
 * 
 * @note the DISJOINT on hypotheses is not performed; see ArmorCluedo::AddIndiv
 *     about the parameter 'makeDisjoint'
 * 
 ***********************************************/
bool ServiceAddHint( robocluedo_armor_msgs::AddHint::Request& hint, robocluedo_armor_msgs::AddHint::Response& success )
{
	OUTLOG( "called service " << SERVICE_INTERFACE_ADD_HINT );
	
	// check for the validity of the hint
	if( hint.hypID < 0 )
	{
		OUTLOG( "ERROR: not a valid hypothesis ID (got " << hint.hypID << ")" );
		
		success.success = false;
		return true;
	}
	if( 
		(hint.property == "") || 
		// (hint.Aelem == "") || 
		(hint.Belem == "")
	)
	{
		OUTLOG( "ERROR: not a valid hint (received empty fields)" );
		
		success.success = false;
		return true;
	}
	
	// check for the existence of the given hypothesis ID (in case, create it)
	/*
	std::string hypname = "";
	hypname += SS("HP") + SSS( hint.hypID );
	*/
	std::string hypname = SS("HP") + SSS( hint.hypID );
	if( !armor->ExistsIndiv( hypname ) )
		armor->AddIndiv( hypname, "HYPOTHESIS", false );
	
	// if the Belem is not defined, add it
	if( !armor->ExistsIndiv( hint.Belem ) )
	{
		if( hint.property == "where" )
			armor->AddIndiv( hint.Belem, "PLACE" );
		else if( hint.property == "who" )
			armor->AddIndiv( hint.Belem, "PERSON" );
		else if( hint.property == "what" )
			armor->AddIndiv( hint.Belem, "WEAPON" );
		else
		{
			OUTLOG( "ERROR: not a valid hint property (received " << hint.property << ")" );
			
			success.success = false;
			return true;
		}
	}
	
	// add the predicate
	OUTLOG( "set object property " << "(" << hypname << ", " << hint.Belem << "):" << hint.property );
	armor->SetObjectProperty( hint.property, hypname, hint.Belem );
	
	// perform the update
	armor->UpdateOntology( );
	
	success.success = true;
	return true;
}



/********************************************//**
 *  
 * \brief implementation of service \ref SERVICE_INTERFACE_FIND_CONSISTENT_HYP
 * 
 * The service returns a vector of hypotheses belonging to the class COMPLETE
 * and not to the class INCONSISTENT with an operation of difference. 
 * 
 * @param empty empty request
 * @param hyplist a vector of COMPLETE hypotheses
 * 
 * @see FindConsistentHypotheses.srv
 * @see Hypothesis.msg one element of the output vector from the service
 * 
 ***********************************************/
bool ServiceFindConsistentHypotheses( robocluedo_armor_msgs::FindConsistentHypotheses::Request& empty, robocluedo_armor_msgs::FindConsistentHypotheses::Response& hyplist )
{
	OUTLOG( "called service " << SERVICE_INTERFACE_FIND_CONSISTENT_HYP );
	
	// perform the update before starting
	armor->UpdateOntology( );
	
	std::vector<std::string> list_to_return;
	{
		// get all the consistent hypotheses
		std::vector<std::string> consistent_hyp = armor->FindCompleteHypotheses( );
		
		// get the inconsistent hypotheses
		std::vector<std::string> inconsistent_hyp = armor->FindInconsistentHypotheses( );
		
		// remove the intersection between the two arrays
		list_to_return = PerformDifferenceBetween( consistent_hyp, inconsistent_hyp );
	}
	
	// expand the list
	hyplist.hyp = std::vector<robocluedo_armor_msgs::Hypothesis>( );
	robocluedo_armor_msgs::Hypothesis h;
	for( std::string hptag : list_to_return )
	{
		h.tag = hptag;
		h.who = armor->GetValuedOfIndiv( "who", hptag )[0];
		h.where = armor->GetValuedOfIndiv( "where", hptag )[0];
		h.what = armor->GetValuedOfIndiv( "what", hptag )[0];
		
		if( (h.tag.find("HP") == std::string::npos) || (h.tag.find("HP") != (std::size_t) 0) )
		{
			OUTLOG( "ERROR: can't get the ID of the registered hypothesis with h.tag=" << h.tag );
			h.ID = -1;
		}
		else
			h.ID = atoi(hptag.substr(2).c_str());
		
		hyplist.hyp.push_back( h );
	}
	
	return true;
}



/********************************************//**
 *  
 * \brief implementation of service \ref SERVICE_INTERFACE_WRONG_HYPOTHESIS
 * 
 * Simple call of the methos ArmorCluedo::RemoveHypothesis. It is called
 * after a negative answer from the oracle related to a charge. 
 * 
 * @param tag     of the hypothesis to discard
 * @param success if the request has been accomplished or not
 * 
 * @see DiscardHypothesis.srv
 * 
 ***********************************************/
bool DiscardHypothesis( robocluedo_armor_msgs::DiscardHypothesis::Request& tag, robocluedo_armor_msgs::DiscardHypothesis::Response& success )
{
	OUTLOG( "called service " << SERVICE_INTERFACE_WRONG_HYPOTHESIS );
	
	if( tag.ID < 0 )
	{
		OUTLOG( "ERROR: received a negative ID!" );
		
		success.success = false;
		return true;
	}
	
	// remove the hypothesis from the database
	std::string hypothesisTag = SS("HP") + SSS( tag.ID );
	success.success = armor->RemoveHypothesis( hypothesisTag );
	
	return true;
}



/********************************************//**
 *  
 * \brief implementation of service \ref SERVICE_INTERFACE_SAVE_ONTOLOGY
 * 
 * The service exports the actual ontology to file. If the parameter
 * \ref PARAM_ONTOLOGY_BACKUP_PATH is defined in the parameter server, the 
 * ontology is exported in that path, otherwise the service returns with 
 * error.
 * 
 * @param emptyrequest empty request
 * @param success if the request has been accomplished or not
 * 
 * @see <a href="http://docs.ros.org/en/api/std_srvs/html/srv/Trigger.html">std_srvs::Trigger</a>
 * 
 ***********************************************/
bool ServiceBackupOntology( std_srvs::Trigger::Request& emptyrequest, std_srvs::Trigger::Response& success )
{
	OUTLOG( "called service " << SERVICE_INTERFACE_SAVE_ONTOLOGY );
	
	if( !ros::param::has( PARAM_ONTOLOGY_BACKUP_PATH ) )
	{
		success.success = false;
		success.message = SS( "unable to find the parameter" ) + SS( PARAM_ONTOLOGY_BACKUP_PATH );
		
		OUTLOG( "ERROR: " << success.message );
	}
	else
	{
		std::string save_path;
		ros::param::get( PARAM_ONTOLOGY_BACKUP_PATH, save_path );
		armor->SaveOntology( save_path );
		success.success = true;
		
		OUTLOG( "Ontology save here -> " << save_path );
	}
	
	return true;
}



 /********************************************//**
 *  
 * \brief ROS node main
 * 
 * Retrieving of the parameters from the parameter server, connection with 
 * aRMOR through C++ interface, opening of the service, and spin. 
 * 
 ***********************************************/
int main( int argc, char* argv[] )
{
	ros::init( argc, argv, "cluedo_armor_interface" );
	ros::NodeHandle nh;
	
	// percorso della oltology da parameter server
	std::string ontology_file_path;
	if( !ros::param::has( ONTOLOGY_PARAM ) )
	{
		OUTLOG( "ERROR: parameter '" << ONTOLOGY_PARAM << "' not defined." );
		return 0;
	}
	ros::param::get( ONTOLOGY_PARAM, ontology_file_path );
	if( !fileExist( ontology_file_path ) )
	{
		OUTLOG( "ERROR: the file '" << ONTOLOGY_PARAM << "' doesn't exist." );
		return 0;
	}
	OUTLOG( "Ontology found! " << LOGSQUARE( ontology_file_path ) );
	
	// connessione ad aRMOR
	OUTLOG( "loading armor ..." );
	ArmorCluedo armorcluedo;
	if( !armorcluedo.Init( ontology_file_path ) || !armorcluedo.TestInterface( ) )
	{
		OUTLOG( "ERROR: unable to load aRMOR!" );
		return 0;
	}
	armor = &armorcluedo;
	OUTLOG( "OK!" );
	
	// servizio per registrare un hint dall'oracolo
	OUTLOG( "opening server " << LOGSQUARE( SERVICE_INTERFACE_ADD_HINT ) << " ..." );
	ros::ServiceServer srv_add_hint = nh.advertiseService( SERVICE_INTERFACE_ADD_HINT , ServiceAddHint );
	OUTLOG( "OK!" );
	
	// servizio per ottenere tutte le ipotesi consistenti
	OUTLOG( "opening server " << LOGSQUARE( SERVICE_INTERFACE_FIND_CONSISTENT_HYP ) << " ..." );
	ros::ServiceServer srv_find_cons_hyp = nh.advertiseService( SERVICE_INTERFACE_FIND_CONSISTENT_HYP, ServiceFindConsistentHypotheses );
	OUTLOG( "OK!" );
	
	// servizio per scartare ipotesi
	OUTLOG( "opening server " << LOGSQUARE( SERVICE_INTERFACE_WRONG_HYPOTHESIS ) << " ..." );
	ros::ServiceServer srv_wrong_hyp = nh.advertiseService( SERVICE_INTERFACE_WRONG_HYPOTHESIS, DiscardHypothesis );
	OUTLOG( "OK!" );
	
	// servizio per scaricare la ontology su file
	OUTLOG( "opening server " << LOGSQUARE( SERVICE_INTERFACE_SAVE_ONTOLOGY ) << " ..." );
	ros::ServiceServer srv_backup = nh.advertiseService( SERVICE_INTERFACE_SAVE_ONTOLOGY, ServiceBackupOntology );
	OUTLOG( "OK!" );
	
	// spin
	OUTLOG( "ready!" );
	ros::spin( );
	
	return 0;
}
