
/********************************************//**
 *  
 * @file armor_tools.cpp
 * @brief implementation of the class ArmorTools
 * @author Francesco Ganci (S4143910)
 * @version v1.0
 *  
 * @see armor_tools.h
 * 
 ***********************************************/

#include "armor_tools/armor_tools.h"



// constructor with three arguments
ArmorTools::ArmorTools(
		std::string client,
		std::string reference,
		bool dbmode
	):
	ClientName( client ),
	ReferenceName( reference ),
	DebugMode( dbmode )
{
	// stub init of the last request
	LastReq.client_name = client;
	LastReq.reference_name = reference;
	LastReq.command = "";
	LastReq.primary_command_spec = "";
	LastReq.secondary_command_spec = "";
	// LastReq.args = std::vector<std::string>( 5, "" );
	
	// stub init of the last response
	LastRes.success = true;
	LastRes.timeout = false;
	LastRes.exit_code = 0;
	LastRes.error_description = "";
	LastRes.is_consistent = true;
	// LastRes.queried_objects = std::vector<std::string>( );
	// LastRes.sparql_queried_objects = std::vector<armor_msgs::QueryItem>( );
}



// constructor with one argument
ArmorTools::ArmorTools( bool dbmode ):
	ClientName( ARMOR_DEFAULT_CLIENT ),
	ReferenceName( ARMOR_DEFAULT_REFERENCE ),
	DebugMode( dbmode )
{
	// stub init of the last request
	LastReq.client_name = ClientName;
	LastReq.reference_name = ReferenceName;
	LastReq.command = "";
	LastReq.primary_command_spec = "";
	LastReq.secondary_command_spec = "";
	// LastReq.args = std::vector<std::string>( 5, "" );
	
	// stub init of the last response
	LastRes.success = true;
	LastRes.timeout = false;
	LastRes.exit_code = 0;
	LastRes.error_description = "";
	LastRes.is_consistent = true;
	// LastRes.queried_objects = std::vector<std::string>( );
	// LastRes.sparql_queried_objects = std::vector<armor_msgs::QueryItem>( );
}



// destructor
ArmorTools::~ArmorTools()
{
	// ...
}



// fill in the request
armor_msgs::ArmorDirective ArmorTools::GetRequest(
		std::string command,
		std::string first_spec,
		std::string second_spec,
		std::string arg1,
		std::string arg2,
		std::string arg3,
		std::string arg4,
		std::string arg5
	)
{
	armor_msgs::ArmorDirective adsrv;
	armor_msgs::ArmorDirectiveReq ad;
	
	ad.client_name = this->ClientName;
	ad.reference_name = this->ReferenceName;
	
	ad.command = command;
	ad.primary_command_spec = first_spec;
	ad.secondary_command_spec = second_spec;
	
	ad.args = std::vector<std::string>();
	ad.args.push_back( arg1 );
	ad.args.push_back( arg2 );
	ad.args.push_back( arg3 );
	ad.args.push_back( arg4 );
	ad.args.push_back( arg5 );
	
	adsrv.request.armor_request = ad;
	
	return adsrv;
}



// call aRMOR
bool ArmorTools::CallArmor( armor_msgs::ArmorDirective& data )
{
	// if there is no server, don't call it
	if( !this->ArmorSrv.exists() )
	{
		ARMOR_ERR( "no service found!" );
		return false;
	}
	
	// save the received request
	this->LastReq = data.request.armor_request;
	
	// try to call it
	if( !this->ArmorSrv.call( data ) )
	{
		ARMOR_ERR( "unable to call ArmorService" << LOGSQUARE( ARMOR_SERVICE_SINGLE_REQUEST ) );
		return false;
	}
	
	// save the last received response
	this->LastRes = data.response.armor_response;
	
	return true;
}



// load the ontology from file
bool ArmorTools::LoadOntology( 
		std::string path, 
		std::string uri,
		bool manipulationFlag,
		std::string reasoner,
		bool buffered_reasoner
	)
{
	// if there is no server, don't call it
	if( !this->ArmorSrv.exists() )
	{
		ARMOR_ERR( "no service found!" );
		return false;
	}
	
	// check if the file exists
	if( !this->FileExist( path ) )
	{
		ARMOR_ERR( "OWL file not found. " << LOGSQUARE( path ) );
		return false;
	}
	
	// prepare the command load
	armor_msgs::ArmorDirective load_cmd = GetRequest( 
		"LOAD", "FILE", "", 
		path, uri, BOOL_TO_CSTR( manipulationFlag ), reasoner, BOOL_TO_CSTR( buffered_reasoner ) 
	);
	
	// call the service
	if( !CallArmor( load_cmd ) ) 
	{
		ARMOR_ERR( "unable to call the service!" );
		return false;
	}
	
	return (IsLoadedInterface = load_cmd.response.armor_response.success);
}



// connection to the service aRMOR
bool ArmorTools::Connect( float timeout )
{
	// check if the service exists
	if( this->ArmorSrv.exists() )
	{
		ARMOR_INFO( "service already connected!" );
		return false;
	}
	
	// connect to the service
	ros::NodeHandle nh;
	ARMOR_INFO( "Requiring client " << LOGSQUARE( ARMOR_SERVICE_SINGLE_REQUEST ) << "..." );
	this->ArmorSrv = nh.serviceClient<armor_msgs::ArmorDirective>( ARMOR_SERVICE_SINGLE_REQUEST );
	if( !this->ArmorSrv.waitForExistence( ros::Duration( timeout ) ) )
	{
		ARMOR_ERR( "ERROR: unable to contact the server - timeout expired (" << timeout << "s) " );
		return false;
	}
	ARMOR_INFO( "-> OK" );
	
	return true;
}



// connect and load from file
bool ArmorTools::ConnectAndLoad( 
		std::string path, 
		std::string uri,
		bool manipulationFlag,
		std::string reasoner,
		bool buffered_reasoner
	)
{
	if ( Connect( ) && 
			LoadOntology( path, uri, manipulationFlag, reasoner, buffered_reasoner ) )
		return true;
	else
		return false;
}



// save the ontology on file
bool ArmorTools::SaveOntology( std::string path )
{
	ARMOR_CHECK_INTERFACE( false );
	
	// try to call the service
	auto srvdata = GetRequest( "SAVE", "INFERENCE", "", path );
	if( !CallArmor( srvdata ) ) 
		return false;
	
	return srvdata.response.armor_response.success;
}



// update the ontology
bool ArmorTools::UpdateOntology( )
{
	ARMOR_CHECK_INTERFACE( false );
	
	auto srvdata = GetRequest( "REASON" );
	if( !CallArmor( srvdata ) ) 
		return false;
	
	return ARMOR_RES( srvdata ).success;
}



// print a request to the screen
void ArmorTools::PrintRequest( armor_msgs::ArmorDirective& d )
{
	std::string str = SS("   Print Request: \n");
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



// print a response to the screen
void ArmorTools::PrintResponse( armor_msgs::ArmorDirective& d )
{
	std::string str = SS("   Print Response: \n");
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



// toggle debug mode
void ArmorTools::SwitchDebugMode( )
{
	this->DebugMode = !this->DebugMode;
}



// get the error code referred from the last response
int ArmorTools::GetLastErrorCode( )
{
	return this->LastRes.exit_code;
}



// get the error description from the last response
std::string ArmorTools::GetLastErrorDescription( )
{
	return SS( this->LastRes.error_description );
}



// last flag 'success'
bool ArmorTools::Success( )
{
	return this->LastRes.success;
}



// check if the ontology was loaded
bool ArmorTools::LoadedOntology( )
{
	return IsLoadedInterface;
}



// test the interface
bool ArmorTools::TestInterface( )
{
	ARMOR_CHECK_INTERFACE( false );
	return true;
}



// fill in the command and send it
bool ArmorTools::SendCommand(
		std::string command,
		std::string first_spec,
		std::string second_spec,
		std::string arg1,
		std::string arg2,
		std::string arg3,
		std::string arg4,
		std::string arg5, 
		bool printRequest
	)
{
	ARMOR_CHECK_INTERFACE( false );
	
	// build the message to send
	auto srvdata = GetRequest( command, first_spec, second_spec, arg1, arg2, arg3, arg4, arg5 );
	if( printRequest ) this->PrintRequest( srvdata );
	
	// and send it
	return CallArmor( srvdata );
}



// get a reference to the last response
armor_msgs::ArmorDirectiveRes& ArmorTools::GetLastRes( )
{
	armor_msgs::ArmorDirectiveRes& res = this->LastRes;
	return res;
}



// get a reference to the last request
armor_msgs::ArmorDirectiveReq& ArmorTools::GetLastReq( )
{
	armor_msgs::ArmorDirectiveReq& req = this->LastReq;
	return req;
}



// print to the screen the last response
void ArmorTools::PrintLastRes( )
{
	armor_msgs::ArmorDirective armordata;
	armordata.response.armor_response = this->LastRes;
	PrintResponse( armordata );
}



// print to the screen the last request
void ArmorTools::PrintLastReq( )
{
	armor_msgs::ArmorDirective armordata;
	armordata.request.armor_request = this->LastReq;
	PrintRequest( armordata );
}



// check if a file exists, given its path
bool ArmorTools::FileExist( std::string path )
{
	return (std::ifstream(path)).good();
}
