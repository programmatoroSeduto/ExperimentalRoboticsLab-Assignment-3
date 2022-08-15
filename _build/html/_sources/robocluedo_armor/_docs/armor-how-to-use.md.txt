
# DOCS -- HOW TO use RoboCLuedo aRMOR interface

```{toctree}
./armor-services.md
```

the interface provides 4 main directives:

- **ADD**  -- put a hint into the aRMOR ontology
- **FIND** -- find the hypotheses which are CONSISTENT (i.e. COMPLETE but not INCONSISTENT)
- **DEL**  -- mark a hypothesis as wrong, i.e. *discard it*
- **BACKUP** -- save the ontology on file.

## services schematics

```{uml} 
@startuml

''' INFOS
title package robocluedo_armor
skinparam Padding 8
allow_mixing

''' COMPONENTS
component "aRMOR interface" <<RCL>> as NODE_RCL_ARMOR
component "aRMOR service" <<aRMOR>> as NODE_ARMOR

''' INTERFACES
() "/cluedo_armor/add_hint" as SRV_ARMOR_ADD
SRV_ARMOR_ADD <-- "srv" NODE_RCL_ARMOR
note on link: robocluedo_armor_msgs/AddHint.srv

() "/cluedo_armor/find_consistent_h" as SRV_ARMOR_FIND
SRV_ARMOR_FIND <-- "srv" NODE_RCL_ARMOR
note on link: robocluedo_armor_msgs/FindConsistentHypotheses.srv

() "/cluedo_armor/wrong_hypothesis" as SRV_ARMOR_DEL
SRV_ARMOR_DEL <-- "srv" NODE_RCL_ARMOR
note on link: robocluedo_armor_msgs/DiscardHypothesis.srv

() "/cluedo_armor/backup" as SRV_ARMOR_BACKUP
SRV_ARMOR_BACKUP <-- "srv" NODE_RCL_ARMOR
note on link: std_srvs/Trigger.srv

() "/armor_interface_srv" as SRV_ARMOR
SRV_ARMOR ..> "cl" NODE_RCL_ARMOR
SRV_ARMOR <.. "srv" NODE_ARMOR

/' 
service `` of type ``
'/

@enduml
```

## ADD -- register a new hint into the Ontology

To add a new hint (from the Oracle?) the node provides the service `/cluedo_armor/add_hint` of type `robocluedo_armor_msgs/AddHint`.

**Note that** adding the same hint twice *doesn't affect the content of the ontology*. The request will retur true even in this case. 

here's the format of the service:

```yaml
## file 'AddHint.srv'
# it represents a hint of the type -> (Aelem, Belem):property

# the numeric ID of the hint
int32 hypID

# fields of the property
string property
string Belem

---

bool success

```

fields:

- `hypID` is the integer identifier of the hypothesis ID
- `property` indicates the role to define: it corresponds to the 'key' element of the hint message
- `Belem` is the content of the property: it corresponds to the field `value` from the Oracle

the ontology allows to define three roles: (`Aelem` is always the label of the hypothesis, whereas `Belem` is the value of the property)

- `(who "HP<hypID>" ?Belem)`
- `(where "HP<hypID>" ?Belem)`
- `(what "HP<hypID>" ?Belem)`

**very important: the node allows only these properties!** it will raise an error if the property hasn't one of the abovementioned keys; remember to check the `success` flag of the message.

**property names must be written in lowercase!**


## FIND -- ask for a consistent hint

the service `/cluedo_armor/find_consistent_h` of type `robocluedo_armor_msgs/FindConsistentHypotheses` is what you need to ask the Ontology for a admissible solution. 

here's the service file employed by the service:

```yaml
## file 'FindConsistentHypotheses.srv'

# empty request

---

# list of consistent hypotheses from the ontology
Hypothesis[] hyp

```

when called, the service returns the entire list of the hypotheses *belonging to the COMPLETED set and not belonging to the INCONSISTENT one*. Remember that the ontology has the following structure:

- THINGs -- `owl:thing`
	- HYPOTHESIS -- `(what only WEAPON) and (where only PLACE) and (who only PERSON)`
		- COMPLETED -- `(what min 1 WEAPON) and (where min 1 PLACE) and (who min 1 PERSON)`
			- INCONSISTENT  -- `(what min 2 WEAPON) or (where min 2 PLACE) or (who min 2 PERSON)`
	- WHO
	- WHERE
	- WHAT

Hence the *COMPLETED* set will contain not only the complete hypotheses, but also the *INCONSISTENT* ones. The service performs a set difference *COMPLETED - INCONSISTENT* which is the set of the admissible solutions, if any.

**don't forget to check the length of the list!** The method could return a empty list of complete hypotheses. 

here's the type `robocluedo_armor_msgs/Hypothesis`:

```yaml
## file 'Hypothesis.msg'

# DEPRECATED
string tag

# the integer identifier of the hypothesis (-1 if the identifier is not standard)
int32 ID

# who killed Dr. Black
string who
# where the murder happened
string where
# what is the murder weapon
string what

```


## DEL -- discard a hypothesis

use the service `/cluedo_armor/wrong_hypothesis` of type `robocluedo_armor_msgs/DiscardHypothesis`, whose structure is reported here below:

```yaml
## file 'DiscardHypothesis.srv'

int32 ID

---

bool success

```

**the request could fail** due to, for instance, the unexistence of the indicated hypothesis. 

