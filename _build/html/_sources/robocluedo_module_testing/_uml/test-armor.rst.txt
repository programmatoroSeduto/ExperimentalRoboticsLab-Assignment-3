
UML -- schematic of test_armor.cpp
========================================

this test communicates directly with aRMOR in order to test out the functionalities of the ontology.

This is how aRMOR is seen for this test:

.. uml::
			
	@startuml

	''' DIAGRAM INFOS
	title "aRMOR model"
	skinparam Padding 8


	''' DIAGRAM ELEMENTS
	database "armor ontology" as ARMOR {
		portout "srv" as ARMOR_P1
	}

	interface "/armor_interface_srv" as ARMOR_SRV

	''' DIAGRAM CONNECTIONS
	ARMOR_P1 --> ARMOR_SRV
	note left on link : "armor_msgs::ArmorDirective"

	@enduml

representation of the test node:

.. uml::

	@startuml

	''' DIAGRAM INFOS
	title "test_armor.cpp node"
	skinparam Padding 8


	''' DIAGRAM ELEMENTS

	interface "/armor_interface_srv" as ARMOR_SRV

	component "test_armor.cpp" as TEST_ARMOR {
		portin "cl" as TEST_ARMOR_CLIENT
	}

	''' DIAGRAM CONNECTIONS
	ARMOR_SRV --> TEST_ARMOR_CLIENT
	note left on link : "armor_msgs::ArmorDirective"

	@enduml

overall arch:

.. uml::

	@startuml

	''' DIAGRAM INFOS
	title "entire test model"
	skinparam Padding 8


	''' DIAGRAM ELEMENTS
	database "armor ontology" as ARMOR {
		portout "srv" as ARMOR_P1
	}

	interface "/armor_interface_srv" as ARMOR_SRV

	component "test_armor.cpp" as TEST_ARMOR {
		portin "cl" as TEST_ARMOR_CLIENT
	}


	''' DIAGRAM CONNECTIONS
	ARMOR_P1 --> ARMOR_SRV
	note left on link : "armor_msgs::ArmorDirective"
	ARMOR_SRV --> TEST_ARMOR_CLIENT


	@enduml
