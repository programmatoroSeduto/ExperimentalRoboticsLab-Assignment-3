
# UML -- ROSPlann Knowledge base -- formal ROS description

## schema -- knowledge base

```{uml} 
@startuml

''' INFOS
title robocluedo_armor project structure
skinparam Padding 8
allow_mixing


''' COMPONENTS
database "knowledge base" <<ROSPlan>> as KB


''' SERVICES
/'
() "srv" as SRV_
KB "srv" --> "cl" SRV_
note on link: 
'/

' () "/rosplan_knowledge_base/clear" as SRV_KB_CLEAR
' KB "srv" --> "cl" SRV_KB_CLEAR
' note on link: std_srvs::Empty

() "/rosplan_knowledge_base/state/propositions" as SRV_KB_GET_PRED
KB "srv" --> "cl" SRV_KB_GET_PRED
note on link: rosplan_knowledge_msgs/GetAttributeService

() "/rosplan_knowledge_base/update_array" as SRV_KB_UPDATE
KB "srv" --> "cl" SRV_KB_UPDATE
note on link: rosplan_knowledge_msgs/KnowledgeUpdateService

() "/rosplan_knowledge_base/update" as SRV_KB_UPDATE_ARRAY
KB "srv" --> "cl" SRV_KB_UPDATE_ARRAY
note on link: rosplan_knowledge_msgs/KnowledgeUpdateServiceArray

() "/rosplan_knowledge_base/query_state" as SRV_QUERY
KB "srv" --> "cl" SRV_QUERY
note on link: rosplan_knowledge_msgs/KnowledgeQueryService


''' CONNECTIONS
' ...

@enduml
```

## schema -- CLASS kb_tools

```{uml} 
@startuml

''' INFOS
title robocluedo_armor project structure
skinparam Padding 8
allow_mixing


''' COMPONENTS
package "ROSPlan" {
database "knowledge base" <<ROSPlan>> as KB
}
class "kb_tools" as TOOLS {
+ get_predicate(string pname, map<keyvalue> args) : bool
+ set_predicate(string pname, map<keyvalue> args, bool newvalue) : bool
' + get_goal(string pname, map<keyvalue> args, bool newvalue) : bool
+ set_goal(string pname, map<keyvalue> args, bool newvalue) : bool
}


''' SERVICES
/'
() "srv" as SRV_
KB "srv" --> "cl" SRV_
note on link: 
'/

() "/rosplan_knowledge_base/update" as SRV_KB_UPDATE
KB "srv" --> SRV_KB_UPDATE
note on link: rosplan_knowledge_msgs/KnowledgeUpdateService
SRV_KB_UPDATE --> "cl" TOOLS

() "/rosplan_knowledge_base/query_state" as SRV_QUERY
KB "srv" --> SRV_QUERY
note on link: rosplan_knowledge_msgs/KnowledgeQueryService
SRV_QUERY --> "cl" TOOLS


''' CONNECTIONS
' ...

@enduml
```

## the long list of HOW TOs ...

### HOW TO read a predicate

libraries to import:

```c++
#include "diagnostic_msgs/KeyValue.h"
#include "rosplan_knowledge_msgs/KnowledgeItem.h"
#include "rosplan_knowledge_msgs/KnowledgeQueryService.h"

// kb knowledge type
#define KB_KTYPE_FLUENT 2
#define KB_KTYPE_PREDICATE 1
```

the client to open:

```c++
#define SERVICE_QUERY "/rosplan_knowledge_base/query_state"
ros::ServiceClient cl_query = nh.serviceClient<rosplan_knowledge_msgs::KnowledgeQueryService>( SERVICE_QUERY );
```

a more detailed way to open the service:

```c++
#define SERVICE_QUERY "/rosplan_knowledge_base/query_state"

// === Predicates Query service === //
TLOG( "Opening client " << LOGSQUARE( SERVICE_QUERY ) << "..." );
ros::ServiceClient cl_query = nh.serviceClient<rosplan_knowledge_msgs::KnowledgeQueryService>( SERVICE_QUERY );
if( !this->cl_query.waitForExistence( ros::Duration( TIMEOUT_QUERY ) ) )
{
	TERR( "unable to contact the server - timeout expired (" << TIMEOUT_QUERY << "s) " );
	// return;
}
TLOG( "Opening client " << LOGSQUARE( SERVICE_QUERY ) << "... OK" );
```

message **rosplan_knowledge_msgs/KnowledgeQueryService**

```text
rosplan_knowledge_msgs/KnowledgeItem[] knowledge
---
bool all_true
bool[] results
rosplan_knowledge_msgs/KnowledgeItem[] false_knowledge
```

message **rosplan_knowledge_msgs/KnowledgeItem** (needed to perform the request)

```text
# A knowledge item used to represent a piece of the state in ROSPlan

uint8 INSTANCE = 0
uint8 FACT = 1
uint8 FUNCTION = 2
uint8 EXPRESSION = 3
uint8 INEQUALITY = 4

uint8 knowledge_type

# time at which this knowledge becomes true
time initial_time

# knowledge is explicitly false
bool is_negative

#---------
# INSTANCE
#---------

# instance knowledge_type
string instance_type
string instance_name

#----------------------
# PREDICATE OR FUNCTION
#----------------------

# attribute knowledge_type
string attribute_name
diagnostic_msgs/KeyValue[] values

#---------
# FUNCTION
#---------

# function value
float64 function_value

#-----------
# EXPRESSION
#-----------

string optimization
rosplan_knowledge_msgs/ExprComposite expr

#-----------
# INEQUALITY
#-----------

rosplan_knowledge_msgs/DomainInequality ineq
```

message **diagnostic_msgs/KeyValue** : 

```text
# what to label this value when viewing
string key
# a value to track over time
string value
```

how to perform the reading:

> reading predicates : if you're sure that the predicate you're asking for is unique, use the field response.all_true; or also .response.results\[] showing each truth value.

```c++
// data for request
std::string pname
std::map<std::string, std::string> params


// === PREPARE THE REQUEST === //

rosplan_knowledge_msgs::KnowledgeQueryService query;
rosplan_knowledge_msgs::KnowledgeItem kbm;

kbm.knowledge_type = KB_KTYPE_PREDICATE; // it corresponds to 1
kbm.attribute_name = pname; // the name of the predicate
for ( auto it=params.begin( ) ; it!=params.end( ) ; ++it )
{
	diagnostic_msgs::KeyValue kv;
	kv.key = it->first;
	kv.value = it->second;
	kbm.values.push_back( kv );
}

query.request.knowledge.push_back( kbm );

// ... the same thing for each predicate you want
//    n predicate in list -> n values inside the result

// === PERFORM THE REQUEST === //

cl_query.call( query );


// === EVALUATE THE RESULT === //

bool res = false;

if( query.response.results.size() == 1 )
{
	// only one predicate compatible
	
	// method 1
	res = query.response.results[0];
	// method 2
	res = query.response.all_true;
}
else
{
	// many predicates (many results...)
}
```

how to perform the reading using KB tools:

```c++
// just one predicate!
std::string pname
std::map<std::string, std::string> params

kb_tools kbt;

bool res = kbt.get_predicate( pname, params );
if(!kbt->ok())
	ROS_ERR("kb tools get predicate");
```

### HOW TO modify a predicate

libraries:

```c++
#include "diagnostic_msgs/KeyValue.h"
#include "rosplan_knowledge_msgs/KnowledgeItem.h"
#include "rosplan_knowledge_msgs/KnowledgeUpdateService.h"

// kb knowledge type
#define KB_KTYPE_FLUENT 2
#define KB_KTYPE_PREDICATE 1

// kb action
#define KB_ADD_KNOWLEDGE 0
#define KB_DEL_KNOWLEDGE 2
```

the service to interact with:

```c++
#define SERVICE_KB_UPDATE "/rosplan_knowledge_base/update"
ros::ServiceClient cl_kb_update = nh.serviceClient<rosplan_knowledge_msgs::KnowledgeUpdateService>( SERVICE_KB_UPDATE );
```

a more detailed way to open the service:

```c++
#define SERVICE_KB_UPDATE "/rosplan_knowledge_base/update"

// === update service === //
TLOG( "Opening client " << LOGSQUARE( SERVICE_KB_UPDATE ) << "..." );
ros::ServiceClient cl_kb_update = nh.serviceClient<rosplan_knowledge_msgs::KnowledgeUpdateService>( SERVICE_KB_UPDATE );
if( !this->cl_kb_update.waitForExistence( ros::Duration( TIMEOUT_KB_UPDATE ) ) )
{
	TERR( "unable to contact the server - timeout expired (" << TIMEOUT_KB_UPDATE << "s) " );
	
	this->success = false;
	return;
}
TLOG( "Opening client " << LOGSQUARE( SERVICE_KB_UPDATE ) << "... OK" );
```

service **rosplan_knowledge_msgs::KnowledgeUpdateService**

```text
uint8 update_type
rosplan_knowledge_msgs/KnowledgeItem knowledge
---
bool success
```

message **rosplan_knowledge_msgs/KnowledgeItem** (see above)

message **diagnostic_msgs/KeyValue** (see above)

how to perform the update:

```c++
std::string pname
std::map<std::string, std::string> params
bool val = false; // the new val for the predicate


// === PREPARE THE REQUEST === //

rosplan_knowledge_msgs::KnowledgeUpdateService kbm;

kbm.request.update_type = ( val ? KB_ADD_KNOWLEDGE : KB_DEL_KNOWLEDGE );
kbm.request.knowledge.knowledge_type = KB_KTYPE_PREDICATE;
kbm.request.knowledge.attribute_name = pname;

for ( auto it=params.begin( ) ; it!=params.end( ) ; ++it )
{
	diagnostic_msgs::KeyValue kv;
	kv.key = it->first;
	kv.value = it->second;
	kbm.request.knowledge.values.push_back( kv );
}


// === PERFORM THE REQUEST === //

cl_kb_update.call( kbm );


// === EVALUATE THE RESULT === //

bool success = kbm.response.success;

```

how to perform the update with kb_tools:

```c++
std::string pname
std::map<std::string, std::string> params
bool val = false; // the new val for the predicate

kb_tools kbt;

bool res = kbt.set_predicate( pname, params, val );
if(!kbt->ok())
	ROS_ERR("kb tools set predicate");
```

## dealing with goals

libraries:

```c++
#include "diagnostic_msgs/KeyValue.h"
#include "rosplan_knowledge_msgs/KnowledgeItem.h"
#include "rosplan_knowledge_msgs/KnowledgeUpdateService.h"

// kb knowledge type
#define KB_KTYPE_FLUENT 2
#define KB_KTYPE_PREDICATE 1

// kb action
#define KB_ADD_KNOWLEDGE 0
#define KB_DEL_KNOWLEDGE 2
#define KB_ADD_GOAL 1
#define KB_DEL_GOAL 3

```

message **rosplan_knowledge_msgs/KnowledgeUpdate** (see above)

in order to make this update, open a client with `/rosplan_knowledge_base/update` of type `rosplan_knowledge_msgs::KnowledgeUpdate`

```c++
#define SERVICE_KB_UPDATE "/rosplan_knowledge_base/update"
ros::ServiceClient cl_kb_update = nh.serviceClient<rosplan_knowledge_msgs::KnowledgeUpdateService>( SERVICE_KB_UPDATE );
```

here's the code for the action: 

```c++
std::string pname
std::map<std::string, std::string> params
bool val = false; // the new val for the predicate


// === PREPARE THE REQUEST === //

rosplan_knowledge_msgs::KnowledgeUpdateService kbm;

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


// === PERFORM THE REQUEST === //

cl_kb_update.call( kbm );


// === EVALUATE THE RESULT === //

bool success = kbm.response.success;

```

## schema -- NODE kb_interface

```{uml} 
@startuml

''' INFOS
title robocluedo_armor project structure
skinparam Padding 8
allow_mixing


''' COMPONENTS
component "KB interface" <<node>> as KB_INTERFACE

class "kb_tools" as TOOLS {
+ get_predicate(string pname, map<keyvalue> args) : bool
+ set_predicate(string pname, map<keyvalue> args, bool newvalue) : bool
' + get_goal(string pname, map<keyvalue> args, bool newvalue) : bool
+ set_goal(string pname, map<keyvalue> args, bool newvalue) : bool
}
KB_INTERFACE <|-- TOOLS

() "/replan" as SRV_REPLAN
KB_INTERFACE "srv" --> SRV_REPLAN
note on link: std_msgs::Empty

() "/update_goal" as SRV_UPDATE_GOAL
KB_INTERFACE "srv" --> SRV_UPDATE_GOAL
note on link: robocluedo_rosplan_msgs::UpdateGoal


''' SERVICES
/'
() "srv" as SRV_
KB_INTERFACE "srv" <-- "cl" SRV_
note on link: 
'/

' () "/rosplan_knowledge_base/clear" as SRV_KB_CLEAR
' KB "srv" --> "cl" SRV_KB_CLEAR
' note on link: std_srvs::Empty

() "/rosplan_knowledge_base/update_array" as SRV_KB_UPDATE
TOOLS "cl" <-- SRV_KB_UPDATE
note on link: rosplan_knowledge_msgs/KnowledgeUpdateService

() "/rosplan_knowledge_base/query_state" as SRV_QUERY
TOOLS "cl" <-- SRV_QUERY
note on link: rosplan_knowledge_msgs/KnowledgeQueryService


''' CONNECTIONS
' ...

@enduml
```
