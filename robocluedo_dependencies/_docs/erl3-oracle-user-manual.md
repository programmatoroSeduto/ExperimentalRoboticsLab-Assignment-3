
# Oracle3 -- User Documentation

---

```{toctree}
---
caption: Contents
---
./erl2-oracle-user-manual.md
```

---

differently from the oracle of for the second part of the project, this is nothing more than *a service* aware of the solution and capable to provide hints (also corrupted hints as before).

it is meant to be used with ArUco: the robot take sthe IDs detecting arUco markers spread across the environment, and asks for their meaning to the Oracle, which returns the message containing that meaning. 

```{warning}
The third Oracle provides the name */oracle_hint* as a service, and no longer as a topic as in the second version of the Oracle. It could be required a remapping. 
```

## Observations

- **the node contains up to 30 precomputed hints**, see in the code: `erl3::ErlOracle oracle_msgs[30];`
	sometimes in the space, the robot could find a ID greater than 30: *it must be discarded*. 
	
- the array `int markerID[30];` contains the ID associated to the respective message into `erl3::ErlOracle oracle_msgs[30];`. For instance, the id of the i-th hint `oracle_msgs[i]` is `markerID[i]`
- in particular, `winID` is the ID of the solution of the case
- there are two types of ID: there's the one in the message *Marker* which is an index of the array of hints; and there's the ID of the other message *ErlOracle* that is an identifier of a hypothesis of the case. 
- each hypothesis identifier goes from 0 up to 5

## SCHEMA -- node *simulation.cpp*

```{note}
- run the node referring to it with *final_oracle*
- the node name at runtime is *assignment3*
- the source file is called *simulation3.cpp*
```

```{uml} 
@startuml

''' INFOS
title (erl2) node simulation3.cpp
skinparam Padding 8
allow_mixing


''' COMPONENTS
component "simulation3.cpp" <<ERL3 Oracle>> as NODE_ERL3_ORACLE


''' LINKS 
() "/oracle_solution" as SRV_ORACLE_SOLUTION
NODE_ERL3_ORACLE "srv" --> SRV_ORACLE_SOLUTION
note on link: erl2/Oracle.srv

() "/oracle_hint" as SRV_HINT
NODE_ERL3_ORACLE "srv" --> SRV_HINT
note on link: exp_assignment3/Marker.srv

@enduml
```


### service *erl2::Oracle*

this service is used to check the validity of a solution in "solve" phase. it returns *the winner ID* choosed at the beginning of the game by the Oracle. 

the system should check wether the ID of the solution is equal to the one returned by this service. *the returned value is never corrupted* concerning this particular type of message.

```yaml
---
int32 ID
```

note: in the actual implementation, the ID goes from 0 included to 5 included. 

### message *erl2::ErlOracle*

this message is sent every time the **cluedo_link** is at a distance up to *0.25m*. 

```yaml
int32 ID
string key
string value
```

*the message could be corrupted* in many ways:

- empty field(s)
- one or more fields could be "-1"
- the field "key" could contain a string different from the allowed ones: who, where, and what

so, *remember to check the validity of the message*.


## HOW TO receive a hint

the robot goes around in search of a ArUco code containing the ID of the hint. When it finds one hint, it makes a service request to the Oracle through `/oracle_hint` service,  declaring the found hint. This request is of type `exp_assignment3/Marker`, made as follows:

```yaml
int32 markerId

---

erl2/ErlOracle oracle_hint
```

the field of type *erl2/ErlOracle* has these fields:

```yaml

int32 ID
string key
string value

```

## HOW TO check the solution

to check whether a specific ID is the solution of the mystery, call the service `/oracle_solution` of type `erl2/Oracle`. Here's the prototype of the service:

```yaml
---
int32 ID
```


## Code Reference

```{doxygenfile} simulation3.cpp
---
project: robocluedo
---
```

