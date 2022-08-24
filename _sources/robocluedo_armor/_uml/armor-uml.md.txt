
# UML -- aRMOR Interface design elements

---

```{toctree}
---
caption: Contents
maxdepth: 3
---
./armor-uml.md
```

---

## aRMOR interface services

here are some textUML diagrams ready to use concerning the interfaces exposed by this package. Remember to replace the symbol `???` with the proper name when required.

### ADD

```
@startuml

() "/cluedo_armor/add_hint" as SRV_ARMOR_ADD
''' SRV_ARMOR_ADD <-- "srv" NODE_???
''' SRV_ARMOR_ADD --> "cl" NODE_???
note on link: robocluedo_armor_msgs/AddHint.srv

@enduml
```

### FIND

```
@startuml

() "/cluedo_armor/find_consistent_h" as SRV_ARMOR_FIND
''' SRV_ARMOR_FIND <-- "srv" NODE_???
''' SRV_ARMOR_FIND --> "cl" NODE_???
note on link: robocluedo_armor_msgs/FindConsistentHypotheses.srv

@enduml
```

### DEL

```
@startuml

() "/cluedo_armor/wrong_hypothesis" as SRV_ARMOR_DEL
SRV_ARMOR_DEL <-- "srv" NODE_???
SRV_ARMOR_DEL --> "cl" NODE_???
note on link: robocluedo_armor_msgs/DiscardHypothesis.srv

@enduml
```

### BACKUP

```
@startuml

() "/cluedo_armor/backup" as SRV_ARMOR_BACKUP
SRV_ARMOR_BACKUP <-- "srv" NODE_???
SRV_ARMOR_BACKUP --> "cl" NODE_???
note on link: std_srvs/Trigger.srv

@enduml
```

## aRMOR services

the package uses only one service from aRMOR. 

```
@startuml

() "/armor_interface_srv" as SRV_ARMOR_DIRECTIVE
''' SRV_ARMOR_DIRECTIVE <-- "srv" NODE_???
''' SRV_ARMOR_DIRECTIVE --> "cl" NODE_???
note on link: armor_msgs/ArmorDirective.srv

@enduml
```

## aRMOR interfaces structure

### package aRMOR

here's a simplified representation of the base aRMOR, used by this package. 

```{uml}
@startuml

''' COMPONENTS
package "aRMOR" as ARMOR {
	database "knowledge base" <<aRMOR>> as ARMOR.KB
	component "services" <<aRMOR>> as ARMOR.NODE
	ARMOR.KB <-- ARMOR.NODE
	ARMOR.KB --> ARMOR.NODE
}

''' LINKS
() "/armor_interface_srv" as SRV_ARMOR_DIRECTIVE
SRV_ARMOR_DIRECTIVE <-- "srv" ARMOR.NODE
note on link: armor_msgs/ArmorDirective.srv

@enduml
```

### aRMOR interface component

here is the component diagram of the two main classes `armor_tools` and `armor_cluedo`.

```{uml}
@startuml

''' COMPONENTS
component "aRMOR Interface" as NODE_RCL_ARMOR {
	class "aRMOR Tools" as NODE_RCL_ARMOR.ARMOR_TOOLS {
		+ bool ConnectAndLoad( ... )
		+ bool LoadOntology( ... )
		+ bool Connect( float timeout = ARMOR_DEFAULT_TIMEOUT )
		+ bool Success( )
		+ bool LoadedOntology( )
		+ bool TestInterface( )
		+ bool SendCommand( ... )
		+ armor_msgs::ArmorDirective GetRequest( ... )
		+ bool CallArmor( armor_msgs::ArmorDirective& data )
		+ bool UpdateOntology( )
		+ bool SaveOntology( std::string path )
		+ void SwitchDebugMode( )
		+ int GetLastErrorCode( )
		+ std::string GetLastErrorDescription( )
	}
	
	class "aRMOR CLuedo" as NODE_RCL_ARMOR.ARMOR_CLUEDO {
		+ bool Init( std::string ontologyPath )
		+ bool AddIndiv( std::string indivname, std::string classname, bool makeDisjoint = true )
		+ std::vector<std::string> GetClassOfIndiv( std::string indivname, bool deep )
		+ std::vector<std::string> GetIndivOfClass( std::string classname )
		+ bool ExistsIndiv( std::string indivname )
		+ bool SetObjectProperty( std::string prop, std::string Aelem, std::string Belem )
		+ std::vector<std::string> GetValuedOfIndiv( std::string prop, std::string indivname )
		+ std::vector<std::string> FindCompleteHypotheses( )
		+ std::vector<std::string> FindInconsistentHypotheses( )
		+ bool RemoveHypothesis( std::string hypTag )
		+ std::string FilterValue( std::string raw )
		+ std::vector<std::string> FilterVector( std::vector<std::string>& itemlist )
	}
	
	NODE_RCL_ARMOR.ARMOR_TOOLS --|> NODE_RCL_ARMOR.ARMOR_CLUEDO
}

''' LINKS
() "/armor_interface_srv" as SRV_ARMOR_DIRECTIVE
SRV_ARMOR_DIRECTIVE --> "cl" NODE_RCL_ARMOR.ARMOR_CLUEDO

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

@enduml
```

## THE COMPLETE SCHEMATIC

```{uml}
@startuml

allow_mixing

''' COMPONENTS
package "aRMOR" as ARMOR {
	database "knowledge base" <<aRMOR>> as ARMOR.KB
	component "services" <<aRMOR>> as ARMOR.NODE
	ARMOR.KB <-- ARMOR.NODE
	ARMOR.KB --> ARMOR.NODE
}

component "aRMOR Interface" as NODE_RCL_ARMOR {
	class "aRMOR Tools" as NODE_RCL_ARMOR.ARMOR_TOOLS {
		+ bool ConnectAndLoad( ... )
		+ bool LoadOntology( ... )
		+ bool Connect( float timeout = ARMOR_DEFAULT_TIMEOUT )
		+ bool Success( )
		+ bool LoadedOntology( )
		+ bool TestInterface( )
		+ bool SendCommand( ... )
		+ armor_msgs::ArmorDirective GetRequest( ... )
		+ bool CallArmor( armor_msgs::ArmorDirective& data )
		+ bool UpdateOntology( )
		+ bool SaveOntology( std::string path )
		+ void SwitchDebugMode( )
		+ int GetLastErrorCode( )
		+ std::string GetLastErrorDescription( )
	}
	
	class "aRMOR CLuedo" as NODE_RCL_ARMOR.ARMOR_CLUEDO {
		+ bool Init( std::string ontologyPath )
		+ bool AddIndiv( std::string indivname, std::string classname, bool makeDisjoint = true )
		+ std::vector<std::string> GetClassOfIndiv( std::string indivname, bool deep )
		+ std::vector<std::string> GetIndivOfClass( std::string classname )
		+ bool ExistsIndiv( std::string indivname )
		+ bool SetObjectProperty( std::string prop, std::string Aelem, std::string Belem )
		+ std::vector<std::string> GetValuedOfIndiv( std::string prop, std::string indivname )
		+ std::vector<std::string> FindCompleteHypotheses( )
		+ std::vector<std::string> FindInconsistentHypotheses( )
		+ bool RemoveHypothesis( std::string hypTag )
		+ std::string FilterValue( std::string raw )
		+ std::vector<std::string> FilterVector( std::vector<std::string>& itemlist )
	}
	
	NODE_RCL_ARMOR.ARMOR_TOOLS --|> NODE_RCL_ARMOR.ARMOR_CLUEDO
}

''' LINKS
() "/armor_interface_srv" as SRV_ARMOR_DIRECTIVE
SRV_ARMOR_DIRECTIVE --> "cl" NODE_RCL_ARMOR.ARMOR_CLUEDO
SRV_ARMOR_DIRECTIVE <-- "srv" ARMOR.NODE
note on link: armor_msgs/ArmorDirective.srv

() "/cluedo_armor/add_hint" as SRV_ARMOR_ADD
SRV_ARMOR_ADD <-up- "srv" NODE_RCL_ARMOR
note on link: robocluedo_armor_msgs/AddHint.srv

() "/cluedo_armor/find_consistent_h" as SRV_ARMOR_FIND
SRV_ARMOR_FIND <-up- "srv" NODE_RCL_ARMOR
note on link: robocluedo_armor_msgs/FindConsistentHypotheses.srv

() "/cluedo_armor/wrong_hypothesis" as SRV_ARMOR_DEL
SRV_ARMOR_DEL <-up- "srv" NODE_RCL_ARMOR
note on link: robocluedo_armor_msgs/DiscardHypothesis.srv

() "/cluedo_armor/backup" as SRV_ARMOR_BACKUP
SRV_ARMOR_BACKUP <-up- "srv" NODE_RCL_ARMOR
note on link: std_srvs/Trigger.srv

@enduml
```



