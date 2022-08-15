
# aRMOR UML CLASS -- robocluedo armor

```{uml} 
@startuml

''' INFOS
title package robocluedo_armor
skinparam Padding 8
allow_mixing

''' COMPONENTS
package "aRMOR" {
	database "knowledge base" <<aRMOR>> as A_KB
	component "services" <<aRMOR>> as A_SRV
	A_KB -right- A_SRV
}

package "aRMOR tools" {
	class "aRMOR tools" as A_TOOLS 
	class "aRMOR cluedo" as A_CLUEDO 
	A_TOOLS -up-|> A_CLUEDO
	component "aRMOR interface" <<ROS node>> as A_NODE
	A_CLUEDO -up-|> A_NODE
}

() "add hint" as SRV_ADD
() "mark wrong hint" as SRV_DELETE
() "check hints" as SRV_GET

''' CONNECTIONS
A_SRV -right-> A_TOOLS
A_NODE --> SRV_ADD
A_NODE --> SRV_DELETE
A_NODE --> SRV_GET

@enduml
```
