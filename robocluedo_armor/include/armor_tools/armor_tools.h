
/********************************************//**
 *  
 * \file armor_tools.h
 * \brief A minimal C++ client for aRMOR
 * \authors Francesco Ganci (S4143910)
 * \version v1.0
 * 
 * @see ArmorCluedo
 * @see ArmorTools
 * 
 ***********************************************/

#ifndef __H_ARMOR_TOOLS_H__
#define __H_ARMOR_TOOLS_H__

#include "ros/ros.h"
#include "armor_msgs/QueryItem.h"
#include "armor_msgs/ArmorDirective.h"
#include "armor_msgs/ArmorDirectiveReq.h"
#include "armor_msgs/ArmorDirectiveRes.h"

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>

#define ARMOR_DEFAULT_URI "http://www.emarolab.it/cluedo-ontology"
#define ARMOR_DEFAULT_REASONER "PELLET"
#define ARMOR_DEFAULT_CLIENT "armor_client"
#define ARMOR_DEFAULT_REFERENCE "cluedo"
#define ARMOR_DEFAULT_TIMEOUT 5.00
#define ARMOR_DEFAULT_DEBUGMODE true

#define ARMOR_SERVICE_SINGLE_REQUEST "/armor_interface_srv"
#define ARMOR_SERVICE_MULTIPLE_REQUESTS "/armor_interface_serialized_srv"

#define ARMOR_CLASS_LABEL "[armor_tools]"
#define ARMOR_INFO( msg ) if( this->DebugMode ) ROS_INFO_STREAM( ARMOR_CLASS_LABEL << " " << msg )
#define ARMOR_ERR( msg ) if( this->DebugMode ) ROS_WARN_STREAM( ARMOR_CLASS_LABEL << " ERROR: " << msg )
#define ARMOR_CHECK_INTERFACE( returnval ) if( !IsLoadedInterface || !ArmorSrv.exists( ) ) { ARMOR_ERR( "bad interface!" ); return returnval; }
#define ARMOR_RES( msg ) msg.response.armor_response
#define ARMOR_RES_QUERY( msg ) msg.response.armor_response.queried_objects

#define SS( this_string ) std::string( this_string )
#define SSS( this_thing ) std::to_string( this_thing )
#define BOOL_TO_CSTR( booleanvalue ) ( booleanvalue ? "true" : "false" )
#define LOGSQUARE( str ) "[" << str << "] "




/********************************************//**
 *  
 * \brief A minimal C++ client for aRMOR
 * 
 * The class ArmorTools aims at giving a first abstraction of the service
 * aRMOR by providing few "shortcuts". A strong point of this interface
 * lies in the fact that a roscpp node can communicate with aRMOR through
 * functions instead of direct callings to the service. <br>
 * 
 * Another important feature is the debug mode: the programmer can turn 
 * on and off the messages inside the code, in order to understand what
 * is going on "underneath the hood". <br>
 * 
 * The class lets the programmer to not waste time in fill in the ArmorDirective
 * message each time: <i>client_name</i> and <i>reference_name</i> are set 
 * only once in the constructor, whereas the remaining fields can be 
 * set using functions. Also the connection is contained in the class, and
 * there are checks for understanding if the connection is valid or not. <br>
 * In the end, the class stores the last request and the last response at
 * each call, so the programmer can easily retrieve them and print, again 
 * using function calls.  <br>
 * 
 * @todo actually the class has some methods missing, for example methods
 * 		for modifying client name and reference name.
 * 
 * @todo sequential commands to aRMOR is not supported right now: only the
 * 		single command service is used. 
 * 
 * @todo the interface actually can load the ontology only from file. 
 * 		It could be a good idea to have tools also for loading it from 
 * 		another source. 
 * 
 * @see ArmorCluedo an applciation of this interface
 * @see <a href="https://github.com/EmaroLab/armor/blob/master/README.MD"> the official aRMOR documentation</a>
 * 
 ***********************************************/
class ArmorTools
{
public:
	/********************************************//**
	 *  
	 * \brief Class Constructor, 3 arguments
	 * 
	 * Complete class constructor. Dummy init of the class in an invalid
	 * state. 
	 * 
	 * @param client    (optional) corresponding to the 'client_name' field
	 * @param reference (optional) corresponding to the 'reference_name' field
	 * @param dbmode    (optional) enable debug mode?
	 * 
	 * @see <a href="https://github.com/EmaroLab/armor#request">aRMOR official documentation</a> -- meaning of the parameters "client" and "reference"
	 * 
	 ***********************************************/
	ArmorTools(
		std::string client = ARMOR_DEFAULT_CLIENT,
		std::string reference = ARMOR_DEFAULT_REFERENCE,
		bool dbmode = false
	);
	
	
	/********************************************//**
	 *  
	 * \brief Class Constructor, only one argument
	 * 
	 * class constructor with only debug mode. Dummy init of the class 
	 * in an invalid state. 
	 * 
	 * @param dbmode debug mode?
	 * 
	 ***********************************************/
	ArmorTools( bool dbmode );
	
	
	/// class destructor (empty)
	~ArmorTools();
	
	
	/********************************************//**
	 *  
	 * \brief quick generation of an aRMOR request
	 * 
	 * This function writes the aRMOR directive request in one shot. 
	 * 
	 * @param command     (mandatory) the main command
	 * @param first_spec  (optional) the first specifier
	 * @param second_spec (optional) the second specifier
	 * @param arg         (optional) (from 1 to 5) the arguments of the request
	 * 
	 * @return the complete aRMOR directve message with the request part 
	 * 		ready to use. 
	 * 
	 * @see <a href="https://github.com/EmaroLab/armor/blob/master/commands.md">aRMOR table of commands</a>
	 * 
	 ***********************************************/
	armor_msgs::ArmorDirective GetRequest(
		std::string command,
		std::string first_spec = "",
		std::string second_spec = "",
		std::string arg1 = "",
		std::string arg2 = "",
		std::string arg3 = "",
		std::string arg4 = "",
		std::string arg5 = ""
	);
	
	
	/********************************************//**
	 *  
	 * \brief send a command to the aRMOR service.
	 * 
	 * This function performs the call to the aRMOR service \ref ARMOR_SERVICE_SINGLE_REQUEST
	 * given the entire message to send to. 
	 * 
	 * @param data reference to the request to send
	 * 
	 * @returns <b>false</b> if there's not coenction, or if the request
	 * 		went wrong. <b>true</b> otherwise. 
	 * 
	 * @todo there's no way to compose aRMOR commands in a list. actually
	 * 		\ref ARMOR_SERVICE_MULTIPLE_REQUESTS is unused. 
	 * 
	 ***********************************************/
	bool CallArmor( armor_msgs::ArmorDirective& data );
	
	
	/********************************************//**
	 *  
	 * \brief load the ontology from file. 
	 * 
	 * the function call the aRMOR service in order to make it load one
	 * file with extension .owl as ontology base. The file is only read, 
	 * so the ontology will not alter it. 
	 * 
	 * @param path               the path of the .owl ontology file
	 * @param uri                the URI of the ontology (the main identifier)
	 * @param manipulationFlag   if enabled, the ontology will wait for the
	 * 		command APPLY before executing the manipulation commands
	 * @param reasoner           one among "HERMIT", "PELLET" (default), "FACT", "SNOROCKET" 
	 * @param bufferend_reasoner if enabled, the ontology will wait for the
	 * 		command REASON before updating its internal state. 
	 * 
	 * @returns success or not
	 * 
	 * @note If you want to manage multiple aRMOR sessions, please consider to 
	 * 		construct more than one ArmorTools object. 
	 * 
	 ***********************************************/
	bool LoadOntology( 
		std::string path, 
		std::string uri = ARMOR_DEFAULT_URI,
		bool manipulationFlag = true,
		std::string reasoner = ARMOR_DEFAULT_REASONER,
		bool buffered_reasoner = true 
	);
	
	
	/********************************************//**
	 *  
	 * \brief open a connection with the aRMOR service. 
	 * 
	 * This function simply asks the aRMOR service client. The client handle
	 * is stored inside the class, so you don't have to worry about it. 
	 * 
	 * @param timeout how much time to wait until to stop the connection 
	 * 		attempt; see \ref ARMOR_DEFAULT_TIMEOUT . 
	 * 
	 * @returns success or not
	 * 
	 * @note The function returns <b>false</b> anso when, after opened the 
	 * 		connection, you try to connect again. This is not allowed: you
	 * 		cannot reconnect. 
	 * 
	 ***********************************************/
	bool Connect( float timeout = ARMOR_DEFAULT_TIMEOUT );
	
	
	/********************************************//**
	 *  
	 * \brief connect to the server and load the ontology from file. 
	 * 
	 * Just a shortcut for the couple ArmorTools::Connect then ArmorTools::LoadOntology .
	 * 
	 * @see ArmorTools::Connect      connection to the aRMOR service
	 * @see ArmorTools::LoadOntology loading ontology from file, and arguments
	 * 
	 * @returns success or not
	 * 
	 ***********************************************/
	bool ConnectAndLoad( 
		std::string path, 
		std::string uri = ARMOR_DEFAULT_URI,
		bool manipulationFlag = true,
		std::string reasoner = ARMOR_DEFAULT_REASONER,
		bool buffered_reasoner = true 
	);
	
	
	/********************************************//**
	 *  
	 * \brief save the ontology on file
	 * 
	 * Shortcut for the aRMOR command <code>SAVE INFERENCE</code>. It 
	 * writes the actual state of the ontology on a file. If the file 
	 * doesn't exist, it will be created. The ontology is updated before
	 * the writing on file. 
	 * 
	 * @param path where to save the .owl file
	 * 
	 * @returns success or not
	 * 
	 ***********************************************/
	bool SaveOntology( std::string path );
	
	
	/********************************************//**
	 *  
	 * \brief send the command REASON
	 * 
	 * Depending on the configuration used in ArmorTools::LoadOntology, 
	 * the aRMOR command <code>REASON</code> is needed in order to update
	 * the current state of the ontology (aka <i>infer something</i>). <br>
	 * This function is a shortcut for the command <code>REASON</code>. 
	 * 
	 * @returns success or not
	 * 
	 * @warning often not using this function is source of errors! Please
	 * remember to use it, because the other function won't do it 
	 * automatically. 
	 * 
	 ***********************************************/
	bool UpdateOntology( );
	
	
	/********************************************//**
	 *  
	 * \brief print a request to the screen.
	 * 
	 * @param d the entire aRMOR directive message.
	 * 
	 ***********************************************/
	void PrintRequest( armor_msgs::ArmorDirective& d );
	
	
	/********************************************//**
	 *  
	 * \brief print the response to the screen.
	 * 
	 * @param d the entire aRMOR directive message.
	 * 
	 ***********************************************/
	void PrintResponse( armor_msgs::ArmorDirective& d );
	
	
	/********************************************//**
	 *  
	 * \brief toggle the debug mode
	 * 
	 * This function switches the value ArmorTools::DebugMode each time
	 * it is called. 
	 * 
	 ***********************************************/
	void SwitchDebugMode( );
	
	
	/********************************************//**
	 *  
	 * \brief err code referred to the last call 
	 * 
	 * @returns the last error code
	 * 
	 ***********************************************/
	int GetLastErrorCode( );
	
	
	/********************************************//**
	 *  
	 * \brief last err description
	 * 
	 * @returns the last error description
	 * 
	 ***********************************************/
	std::string GetLastErrorDescription( );
	
	
	/********************************************//**
	 *  
	 * \brief check the 'success' flag referred to
	 * 			the last aRMOR call
	 * 
	 * @returns success or not
	 * 
	 * @warning sometimes aRMOR uses the <b>false</b> answer as the correct
	 * 		answer, violating the semantic of the flag 'success'. A case is
	 * 		the command <code>QUERY IND</code> which causes aRMOR to reply
	 * 		with <b>success=false</b>. 
	 * 
	 ***********************************************/
	bool Success( );
	
	
	/********************************************//**
	 *  
	 * \brief check if the ontology was loaded or not
	 * 
	 * @returns loaded or not
	 * 
	 ***********************************************/
	bool LoadedOntology( );
	
	
	/********************************************//**
	 *  
	 * \brief check the status of the interface
	 * 
	 * @returns valid inferface or not
	 * 
	 ***********************************************/
	bool TestInterface( );
	
	
	/********************************************//**
	 *  
	 * \brief fill in a command and send it to aRMOR
	 * 
	 * Shortcut for the couple ArmorTools::GetRequest and Armor::CallArmor. 
	 * 
	 * @param command (mandatory) the main command
	 * @param first_spec (optional) the first specifier
	 * @param second_spec (optional) the second specifier
	 * @param arg (optional) (from 1 to 5) the arguments of the request
	 * @param printResponse print the request before calling the service
	 * 
	 * @returns success or not
	 * 
	 ***********************************************/
	bool SendCommand(
		std::string command,
		std::string first_spec = "",
		std::string second_spec = "",
		std::string arg1 = "",
		std::string arg2 = "",
		std::string arg3 = "",
		std::string arg4 = "",
		std::string arg5 = "",
		bool printRequest = false
	);
	
	
	/********************************************//**
	 *  
	 * \brief get a reference to the last response
	 * 
	 * @returns reference to the last response
	 * 
	 ***********************************************/
	armor_msgs::ArmorDirectiveRes& GetLastRes( );
	
	
	/********************************************//**
	 *  
	 * \brief get a reference to the last request
	 * 
	 * @returns last sent request to aRMOR
	 * 
	 ***********************************************/
	armor_msgs::ArmorDirectiveReq& GetLastReq( );
	
	
	/********************************************//**
	 *  
	 * \brief print the last response
	 * 
	 ***********************************************/
	void PrintLastRes( );
	
	
	/********************************************//**
	 *  
	 * \brief print the last request
	 * 
	 ***********************************************/
	void PrintLastReq( );
	
protected:

	/// debug mode enabled or not
	bool DebugMode = false;
	
private:
	// aRMOR service
	ros::ServiceClient ArmorSrv;
	
	// client name
	std::string ClientName = ARMOR_DEFAULT_CLIENT;
	
	// reference name
	std::string ReferenceName = ARMOR_DEFAULT_REFERENCE;
	
	// uri
	std::string uri = ARMOR_DEFAULT_URI;
	
	// interface loaded? 
	//    l'ultimo comando load ha avuto successo?
	bool IsLoadedInterface = false;
	
	// last response from the server
	armor_msgs::ArmorDirectiveRes LastRes;
	
	// last request sent to the server
	armor_msgs::ArmorDirectiveReq LastReq;
	
	// controlla se il dato file esiste
	bool FileExist( std::string path );
};


#endif
