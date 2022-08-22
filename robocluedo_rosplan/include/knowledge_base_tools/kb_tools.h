
/********************************************//**
*  
* @file kb_tools.h
* @brief basic tools for dealng with the ROSPLan
* 	knowledge base.
* 
* Essential tools for handling the informations inside the ROSPlan knowledge
* base in a more intuitive, convenient way. 
* 
* @authors Francesco Ganci
* @version v1.0.0
* 
* @todo the class right now has a very limited support for the fluents. 
* 
***********************************************/

#ifndef __H_KB_TOOLS__
#define __H_KB_TOOLS__ "__H_KB_TOOLS__"

#include "ros/ros.h"
#include "diagnostic_msgs/KeyValue.h"
#include "rosplan_knowledge_msgs/KnowledgeItem.h"
#include "rosplan_knowledge_msgs/KnowledgeQueryService.h"
#include "rosplan_knowledge_msgs/KnowledgeUpdateService.h"

#include <string>
#include <map>

#define DEBUG_MODE_ENABLED false

// === KB services

/// KB query (predicates only)
#define SERVICE_QUERY "/rosplan_knowledge_base/query_state"
#define TIMEOUT_QUERY 60

/// KB propositions query
#define SERVICE_QUERY_2 "/rosplan_knowledge_base/state/propositions"
#define TIMEOUT_QUERY_2 60

/// KB update (fluents and predicates)
#define SERVICE_KB_UPDATE "/rosplan_knowledge_base/update"
#define TIMEOUT_KB_UPDATE 60

/// KB query (fluents only)
#define SERVICE_KB_GET_FLUENT "/rosplan_knowledge_base/state/functions"
#define TIMEOUT_KB_GET_FLUENT 60

// === KB macros

// kb action
#define KB_ADD_KNOWLEDGE 0
#define KB_DEL_KNOWLEDGE 2
#define KB_ADD_GOAL 1
#define KB_DEL_GOAL 3

// kb knowledge type
#define KB_KTYPE_FLUENT 2
#define KB_KTYPE_PREDICATE 1

/********************************************//**
 *  
 * \class kb_tools
 * 
 * \brief base interface with the ROS plan knowledge base
 * 
 * this interface can be considered a abstraction of the ROS plan knowledge
 * base. the knowledge base is seen as a database containing predicates 
 * and fluents, which can be obtaine by GET methods, and set using SET 
 * methods. 
 * 
 * Moreover, the class opens and manages all the services needed to 
 * perform such communication with the database, hence the node using this
 * kind of functonality doesn't need to manually open the required 
 * interfaces. 
 * 
 ***********************************************/
class kb_tools
{
public:
	
	/// class constructor
	kb_tools( );
	
	/// class destructor 
	~kb_tools( );
	
	/********************************************//**
	 *  
	 * \brief check if the last action succeeded or not
	 * 
	 * \note this function should be called every time you perform a
	 * call to any service. 
	 * 
	 ***********************************************/
	bool ok( );
	
	/********************************************//**
	 *  
	 * \brief set the log verbosity level
	 * 
	 * with the debug mode disabled, the class will notify only
	 * the opening of the services; otherwise, each method contains a 
	 * lot of printable messages. 
	 * 
	 * @param dbmode if true, the class will print on the log/shell 
	 * <i>every operations</i> the system will perform. 
	 * 
	 ***********************************************/
	void set_debug_mode( bool dbmode );
	
	
	
	
	// === GETTERS
	
	/********************************************//**
	 *  
	 * \brief get value of a predicate
	 * 
	 * @param pname the name of the predicate to get
	 * 
	 * @param params the map of the parameters of the predicate
	 * to check 
	 * 
	 * @returns <ul>
	 * <li><b>true</b> if the predicate is true (and the call succeeded)</li>
	 * <li><b>false</b> if the predicate is false OR if the service call 
	 * failed, see \ref ok</li>
	 * 
	 * @note the boolean 'false' is returned also when the call fails, so
	 * remember to check the valdity of the value using \ref ok
	 * 
	 * @todo the method assumes that only one result is returned, which is
	 * true for fully defined queries, but what about partial queries?
	 * 
	 ***********************************************/
	bool get_predicate( 
		const std::string& pname, 
		std::map<std::string, std::string> params );
	
	
	
	
	// === SETTERS
	
	/********************************************//**
	 *  
	 * \brief set the truth value of a predicate
	 * 
	 * @param pname the name of the predicate to get
	 * 
	 * @param params the map of the parameters of the predicate
	 * to check 
	 * 
	 * @param pvalue the boolean value to set for that predicate
	 * 
	 * @returns (bool) <b>true</b> if the operation has gone well, <b>false</b>
	 * otherwise.
	 * 
	 * @note no need here to check the success of the call using \ref ok:
	 * the return value is sufficient to understand what's going on.
	 * 
	 ***********************************************/
	bool set_predicate(
		const std::string& pname, 
		std::map<std::string, std::string> params,
		bool pvalue );
	
	/********************************************//**
	 *  
	 * \brief set the truth value of a goal predicate
	 * 
	 * @param pname the name of the goal predicate to get
	 * 
	 * @param params the map of the parameters of the goal 
	 * predicate to check 
	 * 
	 * @param pvalue the boolean value to set for that goal predicate
	 * 
	 * @returns (bool) <b>true</b> if the operation has gone well, <b>false</b>
	 * otherwise.
	 * 
	 * @note no need here to check the success of the call using \ref ok:
	 * the return value is sufficient to understand what's going on.
	 * 
	 ***********************************************/
	bool set_goal( 
		const std::string& pname, 
		std::map<std::string, std::string> params,
		bool pvalue );



protected:
	
	/// node handle reference
	ros::NodeHandle nh;

	/// debug mode status, @see set_debug_mode
	bool debug_mode; 
	
	/// last action success or not, @see ok
	bool success;
	
	/********************************************//**
	 *  
	 * \brief build a query message (predicates only)
	 * 
	 * the method returns a message ready for a simple predicate query.
	 * just give the name of the predicate you're searching, and its
	 * parameters. 
	 * 
	 * @param pname (string) the name of the predicate
	 * 
	 * @param params (std::map<std::string, std::string>&) the map of 
	 * parameters for the predicate
	 * 
	 * @returns a copy of the message <i>rosplan_knowledge_msgs::KnowledgeQueryService</i>,
	 * with the <i>.request</i> field ready for the service request. 
	 * 
	 ***********************************************/
	rosplan_knowledge_msgs::KnowledgeQueryService request_query(
		const std::string& pname, 
		std::map<std::string, std::string>& params );
	
	/********************************************//**
	 *  
	 * \brief build a update message for predicates
	 * 
	 * @param pname (string) the name of the predicate
	 * 
	 * @param params (std::map<std::string, std::string>&) the map of 
	 * parameters for the predicate
	 * 
	 * @param value (bool) the new value of the predicate
	 * 
	 * @returns a copy of the message <i>rosplan_knowledge_msgs::KnowledgeUpdateService</i>,
	 * with the <i>.request</i> field ready for the service request. 
	 * 
	 ***********************************************/
	rosplan_knowledge_msgs::KnowledgeUpdateService request_update(
		const std::string pname, 
		std::map<std::string, std::string>& params, 
		bool value,
		bool is_goal = false );
	
	/********************************************//**
	 *  
	 * \brief cast a KeyValue message into a simple map
	 * 
	 * this function is often useful, since the messages of the
	 * knowledge base use a lot the message type diagnostic_msgs::KeyValue. 
	 * 
	 * @param kv the vector of messages diagnostic_msgs::KeyValue
	 * 
	 * @returns a map from that vector
	 * 
	 ***********************************************/
	std::map<std::string, std::string> keyvalue2map( 
		const std::vector<diagnostic_msgs::KeyValue>& kv );

private:
	
	/// predicates query client handle
	ros::ServiceClient cl_query;
	
	/// another query client handle
	ros::ServiceClient cl_query_2;
	
	/// update clent handle
	ros::ServiceClient cl_kb_update;
	
	/// fluents query client handle
	ros::ServiceClient cl_kb_get_fluent;
	
	/********************************************//**
	 *  
	 * \brief open the services with the knowledge base
	 * 
	 * here is the list of the services opened by this function: 
	 * <ul>
	 * <li> \ref SERVICE_QUERY : rosplan_knowledge_msgs::KnowledgeQueryService> </li>
	 * <li> \ref SERVICE_KB_UPDATE : rosplan_knowledge_msgs::KnowledgeUpdateService </li>
	 * <li> ??? : ??? </li>
	 * </ul>
	 * 
	 ***********************************************/
	void open_services( );
	
};

#endif
