
/********************************************//**
*  
* @file kb_tools.cpp
* @brief implementation of the class \ref kb_tools
* 
* @authors Fracesco Ganci
* @version v1.0
*  
* @see kb_tools.h
* 
***********************************************/

#include "knowledge_base_tools/kb_tools.h"



// class constructor
kb_tools::kb_tools( ) : 
	success( true )
{
	// set the verbosity level
	this->set_debug_mode( DEBUG_MODE_ENABLED );
	
	// open the services
	this->open_services( );
}


// class destructor 
kb_tools::~kb_tools( ) 
{
	// ...
};


// check if the last action succeeded or not
bool kb_tools::ok( )
{
	if( debug_mode ) 
	{
		if( this->success )
			ROS_INFO( "kb_tools::ok( )" );
		else
			ROS_WARN( "kb_tools::ok( ) !!! FAILURE !!!" );
	}
	
	return this->success;
}


// set the log verosity level
void kb_tools::set_debug_mode( bool db_mode )
{
	// set the mode
	this->debug_mode = db_mode;
	
	// notify it
	if( db_mode ) ROS_INFO( "kb_tools::set_debug_mode : DEBUG MODE ENABLED" );
	else ROS_INFO( "kb_tools::set_debug_mode : debug mode not enabled" );
}




// === GETTERS

// get value of a predicate
bool kb_tools::get_predicate( 
	const std::string& pname, 
	std::map<std::string, std::string> params )
{
	// query message
	rosplan_knowledge_msgs::KnowledgeQueryService query = this->request_query(
		pname, params );
	
	if( debug_mode ) 
	{
		std::string pm = "";
		for ( auto it=params.begin( ) ; it!=params.end( ) ; ++it )
			pm += ", " + it->first + "=" + it->second;
		ROS_INFO_STREAM( "kb_tools::get_predicate" << "( " << pname << pm << " )"  );
	}

	// call the query service
	if( !this->cl_query.call( query ) ) 
	{ 
		ROS_WARN_STREAM( "unable to make a service request -- failed calling service " 
			<< SERVICE_QUERY
			<< (!this->cl_query.exists( ) ? " -- it seems not opened" : "") );
		
		this->success = false;
		return false;
	}
	
	this->success = true;
	
	if( debug_mode ) ROS_INFO_STREAM( "kb_tools::get_predicate" << " CALL SUCCESS with return " 
		<< (query.response.all_true ? "true" : "false")  );
	return query.response.all_true;
}




// === SETTERS

// set a predicate
bool kb_tools::set_predicate(
	const std::string& pname, 
	std::map<std::string, std::string> params,
	bool pvalue )
{
	// formulate the request
	rosplan_knowledge_msgs::KnowledgeUpdateService kbm = 
		this->request_update( pname, params, pvalue, false );
	
	if( debug_mode ) 
	{
		std::string pm = "";
		for ( auto it=params.begin( ) ; it!=params.end( ) ; ++it )
			pm += ", " + it->first + "=" + it->second;
		ROS_INFO_STREAM( "kb_tools::set_predicate" << "( " << pname << pm << ", pvalue=" << (pvalue ? "true" : "false" ) << " )"  );
	}
		
	// send command
	if( !this->cl_kb_update.call( kbm ) ) 
	{ 
		ROS_WARN_STREAM( "unable to make a service request -- failed calling service " 
			<< SERVICE_KB_UPDATE
			<< (!this->cl_kb_update.exists( ) ? " -- it seems not opened" : "") );
		
		this->success = false;
		return false;
	}
	
	this->success = kbm.response.success;
	if( debug_mode ) ROS_INFO_STREAM( "kb_tools::set_predicate" << " CALL SUCCESS with return " 
		<< (kbm.response.success ? "success" : "NOT success")  );
	return kbm.response.success;
}


// set a goal value
bool kb_tools::set_goal( 
	const std::string& pname, 
	std::map<std::string, std::string> params,
	bool pvalue )
{
	rosplan_knowledge_msgs::KnowledgeUpdateService kbm = 
		this->request_update( pname, params, pvalue, true );
	
	if( debug_mode ) 
	{
		std::string pm = "";
		for ( auto it=params.begin( ) ; it!=params.end( ) ; ++it )
			pm += ", " + it->first + "=" + it->second;
		ROS_INFO_STREAM( "kb_tools::set_predicate" << "( " << pname << pm << ", pvalue=" << (pvalue ? "true" : "false" ) << " )"  );
	}
	
	// send command
	if( !this->cl_kb_update.call( kbm ) ) 
	{ 
		ROS_WARN_STREAM( "unable to make a service request -- failed calling service " 
			<< SERVICE_KB_UPDATE
			<< (!this->cl_kb_update.exists( ) ? " -- it seems not opened" : "") );
		
		this->success = false;
		return false;
	}
	
	this->success = kbm.response.success;
	if( debug_mode ) ROS_INFO_STREAM( "kb_tools::set_predicate" << " CALL SUCCESS with return " 
		<< (kbm.response.success ? "success" : "NOT success")  );
	return kbm.response.success;
}




// === PROTECTED METHODS

// build a query message (predicates only)
rosplan_knowledge_msgs::KnowledgeQueryService kb_tools::request_query(
	const std::string& pname, 
	std::map<std::string, std::string>& params )
{
	// query message
	rosplan_knowledge_msgs::KnowledgeQueryService query;
	rosplan_knowledge_msgs::KnowledgeItem kbm;

	kbm.knowledge_type = KB_KTYPE_PREDICATE;
	kbm.attribute_name = pname;

	for ( auto it=params.begin( ) ; it!=params.end( ) ; ++it )
	{
		diagnostic_msgs::KeyValue kv;
		kv.key = it->first;
		kv.value = it->second;
		kbm.values.push_back( kv );
	}

	query.request.knowledge.push_back( kbm );
	
	return query;
}


// build a update message for predicates only
rosplan_knowledge_msgs::KnowledgeUpdateService kb_tools::request_update(
	const std::string pname, 
	std::map<std::string, std::string>& params, 
	bool value,
	bool is_goal )
{
	rosplan_knowledge_msgs::KnowledgeUpdateService kbm;
	
	if(!is_goal)
		kbm.request.update_type = ( value ? KB_ADD_KNOWLEDGE : KB_DEL_KNOWLEDGE );
	else
		kbm.request.update_type = ( value ? KB_ADD_GOAL : KB_DEL_GOAL );
	
	kbm.request.knowledge.knowledge_type = KB_KTYPE_PREDICATE;
	kbm.request.knowledge.attribute_name = pname;

	for ( auto it=params.begin( ) ; it!=params.end( ) ; ++it )
	{
		diagnostic_msgs::KeyValue kv;
		kv.key = it->first;
		kv.value = it->second;
		kbm.request.knowledge.values.push_back( kv );
	}
	
	return kbm;
}


// cast a KeyValue message into a simple map
std::map<std::string, std::string> kb_tools::keyvalue2map( 
		const std::vector<diagnostic_msgs::KeyValue>& kv )
{
	std::map<std::string, std::string> res;
	
	for( auto it=kv.begin( ) ; it!=kv.end( ) ; ++it )
		res[it->key] = it->value;
	
	return res;
}




// === PRIVATE METHODS

// open the services with the knowledge base
void kb_tools::open_services( )
{
	// === Predicates Query service === //
	ROS_INFO_STREAM( "Opening client [" << SERVICE_QUERY << "]..." );
	this->cl_query = nh.serviceClient<rosplan_knowledge_msgs::KnowledgeQueryService>( SERVICE_QUERY );
	if( !this->cl_query.waitForExistence( ros::Duration( TIMEOUT_QUERY ) ) )
	{
		ROS_WARN_STREAM( "unable to contact the server - timeout expired (" << TIMEOUT_QUERY << "s) " );
		
		this->success = false;
		return;
	}
	ROS_INFO_STREAM( "Opening client [" << SERVICE_QUERY << "] ... OK" );
	
	
	// === update service === //
	ROS_INFO_STREAM( "Opening client [" << SERVICE_KB_UPDATE << "] ..." );
	this->cl_kb_update = nh.serviceClient<rosplan_knowledge_msgs::KnowledgeUpdateService>( SERVICE_KB_UPDATE );
	if( !this->cl_kb_update.waitForExistence( ros::Duration( TIMEOUT_KB_UPDATE ) ) )
	{
		ROS_WARN_STREAM( "unable to contact the server - timeout expired (" << TIMEOUT_KB_UPDATE << "s) " );
		
		this->success = false;
		return;
	}
	ROS_INFO_STREAM( "Opening client [" << SERVICE_KB_UPDATE << "] ... OK" );
}
