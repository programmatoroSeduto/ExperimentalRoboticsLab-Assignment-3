
/********************************************//**
 *  
 * @file test_armor_tools.cpp
 * @brief Testing the interface ArmorCluedo
 * 
 * @authors Francesco Ganci (S4143910)
 * @version v1.0
 * 
 * This node performs a deep test on the interface ArmorCluedo. 
 *  
 * @see test_armor_tools.launch
 * 
 * @todo translate logs from italian to English and reorder
 * 
 ***********************************************/

#include "ros/ros.h"
#include "armor_tools/armor_tools.h"
#include "armor_tools/armor_cluedo.h"

#include "armor_msgs/ArmorDirective.h"
#include "armor_msgs/ArmorDirectiveList.h"
#include "armor_msgs/ArmorDirectiveReq.h"
#include "armor_msgs/ArmorDirectiveRes.h"
#include "armor_msgs/QueryItem.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define ONTOLOGY_PARAM "cluedo_path_owlfile"
#define SERVICE_ARMOR_SINGLE_REQUEST "/armor_interface_srv"
#define SERVICE_ARMOR_MULTIPLE_REQUESTS "/armor_interface_serialized_srv"
#define ARMOR_CLIENT_NAME "robocluedo"
#define ARMOR_REFERENCE_NAME "cluedo_ontology"
#define OUTLABEL "[test_armor_tools] "


// check if a given file exists
bool fileExist( std::string path )
{
    return (std::ifstream(path)).good();
}



void printList( std::vector<std::string> list, std::string intro = "" )
{
	if( intro != "" )
		ROS_INFO_STREAM( OUTLABEL << intro );
	
	if( list.size() == 0 )
	{
		ROS_INFO_STREAM( OUTLABEL << "size==0" );
		return;
	}
	
	int idx = 1;
	for( auto it = list.begin(); it != list.end( ); ++it )
	{
		ROS_INFO_STREAM( OUTLABEL << "\t" << "numero " << idx++ << " -> " << *it  );
	}
}


/*
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "ADD", "IND", "CLASS", armorGetArgs( "Jim", "PERSON" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "ADD", "OBJECTPROP", "IND", armorGetArgs( "who", "HP3", "Jim" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "ADD", "IND", "CLASS", armorGetArgs( "Gun", "WEAPON" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "ADD", "OBJECTPROP", "IND", armorGetArgs( "what", "HP3", "Gun" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "ADD", "IND", "CLASS", armorGetArgs( "Stadium", "PLACE" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "ADD", "OBJECTPROP", "IND", armorGetArgs( "where", "HP3", "Stadium" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "REASON", "", "", armorGetArgs( "" ) ) );
	* 
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "ADD", "IND", "CLASS", armorGetArgs( "Lounge", "PLACE" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "DISJOINT", "IND", "", armorGetArgs( "Lounge", "Stadium" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "ADD", "OBJECTPROP", "IND", armorGetArgs( "where", "HP3", "Lounge" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "REASON", "", "", armorGetArgs( "" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "SAVE", "INFERENCE", "", armorGetArgs( "/root/Desktop/ROBOCLUEDO_ONTOLOGY.owl" ) ) );
 */
 /*
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "QUERY", "IND", "CLASS", armorGetArgs( "COMPLETED" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "QUERY", "IND", "CLASS", armorGetArgs( "INCONSISTENT" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "QUERY", "OBJECTPROP", "IND", armorGetArgs( "who", "HP0" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "QUERY", "OBJECTPROP", "IND", armorGetArgs( "where", "HP0" ) ) );
	directives.push_back( armorGetRequest( ARMOR_CLIENT_NAME, ARMOR_REFERENCE_NAME, "QUERY", "OBJECTPROP", "IND", armorGetArgs( "what", "HP0" ) ) );
*/
/*
 * disjoint di classe da un'altra
 * 	"DISJOINT", "CLASS", "", "PERSON", "WEAPON"
 * 	--
 * 
 * disjoint individual qualunque da individual qualunque
 * 	"DISJOINT", "IND", "", "Lounge", "HP3"
 * 	--
 * 
 * aggiungi un individual ad una classe
 * 	"ADD", "IND", "CLASS", "Jim", "PERSON" 
 * 	--
 * 
 * ritorna tutti gli individual appartenenti ad una classe
 * 	"QUERY", "IND", "CLASS", "HYPOTHESIS"
 * 	<uri#value>
 * 
 * aggiungi una proprietà
 * 	"ADD", "OBJECTPROP", "IND", "who", "HP3", "Jim"
 * 	--
 * 
 * query sulle proprietà di uno specifico individual
 * (logica: "QUERY" - "su cosa voglio lavorare" - "cosa voglio ottenere")
 * 	"QUERY", "IND", "OBJECTPROP", "HP3" 
 * 	<uri#property>
 * 
 * update
 * 	"REASON"
 * 	--
 * 
 * save
 * 	"SAVE", "INFERENCE", "", "/root/Desktop/ROBOCLUEDO_ONTOLOGY.owl"
 * 	--
 * 
 * check ipotesi consistenti
 * 	"QUERY", "IND", "CLASS", "COMPLETED"
 * 	<uri#HP3>
 * 
 * classe/i di un individual
 * 	"QUERY", "CLASS", "IND", "HP3", "false"
 * 	<uri#HYPOTHESIS>
 * 
 * classe più profonda di un individual (caso bastardo, probabilmente bug)
 * 	"QUERY", "CLASS", "IND", "HP3", "true"
 * 	HYPOTHESIS
 * 
 * Ritorna i valori associati ad una certa proprietà 
 * 	"QUERY", "OBJECTPROP", "IND", "where", "HP3"
 * 	<http://www.emarolab.it/cluedo-ontology#Study>
 * 	corrisponde a: (HP3, Study):where
 * 
 */	
void armorTestSession( ArmorTools& armor )
{
	armor.SendCommand( "DISJOINT", "CLASS", "", "PERSON", "WEAPON" );
	armor.SendCommand( "DISJOINT", "CLASS", "", "WEAPON", "PLACE" );
	armor.SendCommand( "DISJOINT", "CLASS", "", "PERSON", "PLACE" );
	armor.UpdateOntology( );
	
	armor.SendCommand( "ADD", "IND", "CLASS", "Jim", "PERSON" );
	armor.SendCommand( "ADD", "OBJECTPROP", "IND", "who", "HP3", "Jim" );
	armor.SendCommand( "ADD", "IND", "CLASS", "Gun", "WEAPON" );
	armor.SendCommand( "ADD", "OBJECTPROP", "IND", "what", "HP3", "Gun" );
	armor.SendCommand( "ADD", "IND", "CLASS", "Stadium", "PLACE" );
	armor.SendCommand( "ADD", "OBJECTPROP", "IND", "where", "HP3", "Stadium" );
	armor.UpdateOntology( );
	armor.SendCommand( "QUERY", "IND", "CLASS", "COMPLETED" );
	ROS_INFO_STREAM( OUTLABEL << "(1) completed hypotheses:" );
	armor.PrintLastRes( );
	armor.SendCommand( "QUERY", "IND", "CLASS", "INCONSISTENT" );
	ROS_INFO_STREAM( OUTLABEL << "(1) inconsistent hypotheses:" );
	armor.PrintLastRes( );
	
	// NON VALE LA PROPRIETA' TRANSITIVA, PURTROPPO
	armor.SendCommand( "ADD", "IND", "CLASS", "Lounge", "PLACE" );
	// armor.SendCommand( "DISJOINT", "IND", "", "Lounge", "Stadium" );
	armor.SendCommand( "ADD", "IND", "CLASS", "Study", "PLACE" );
	// armor.SendCommand( "DISJOINT", "IND", "", "Lounge", "Study" );
	// armor.SendCommand( "DISJOINT", "IND", "", "Stadium", "Study" );
	armor.SendCommand( "DISJOINT", "IND", "CLASS" );
	armor.PrintLastReq( ); armor.PrintLastRes( );
	armor.SendCommand( "ADD", "OBJECTPROP", "IND", "where", "HP3", "Study" );
	armor.UpdateOntology( );
	ROS_INFO_STREAM( OUTLABEL << "(2) completed hypotheses:" );
	armor.SendCommand( "QUERY", "IND", "CLASS", "COMPLETED" );
	armor.PrintLastRes( );
	ROS_INFO_STREAM( OUTLABEL << "(2) inconsistent hypotheses:" );
	armor.SendCommand( "QUERY", "IND", "CLASS", "INCONSISTENT" );
	armor.PrintLastRes( );
	
	// però si può fare disjoint di qualunque cosa nell'universo con qualunque altra cosa
	//    è possibile almeno una strategia di forza bruta
	armor.SendCommand( "DISJOINT", "IND", "", "Lounge", "HP3" );
	armor.UpdateOntology( );
	ROS_INFO_STREAM( OUTLABEL << "(3) un disjoint strano:" );
	armor.PrintLastRes( );
	ROS_INFO_STREAM( OUTLABEL << "(3) inconsistent hypotheses:" );
	armor.SendCommand( "QUERY", "IND", "CLASS", "INCONSISTENT" );
	armor.PrintLastRes( );
	
	ROS_INFO_STREAM( OUTLABEL << "query elementi della classe PLACE" );
	armor.SendCommand( "QUERY", "IND", "CLASS", "PLACE" );
	armor.PrintLastRes( );
	ROS_INFO_STREAM( OUTLABEL << "query proprieta' di hp3" );
	armor.SendCommand( "QUERY", "IND", "OBJECTPROP", "HP3" );
	armor.PrintLastRes( );
	ROS_INFO_STREAM( OUTLABEL << "query di tutte le ipotesi" );
	armor.SendCommand( "QUERY", "IND", "CLASS", "HYPOTHESIS" );
	armor.PrintLastRes( );
	ROS_INFO_STREAM( OUTLABEL << "query tutte le classi a cui HP3 apartiene" );
	armor.SendCommand( "QUERY", "CLASS", "IND", "HP3", "false" );
	armor.PrintLastRes( );
	// ROS_INFO_STREAM( OUTLABEL << "query solo la classe piu' profonda" );
	// armor.SendCommand( "QUERY", "CLASS", "IND", "HP3", "true" );
	// armor.PrintLastRes( );
	
	ROS_INFO_STREAM( OUTLABEL << "salvataggio" );
	armor.SendCommand( "SAVE", "INFERENCE", "", "/root/Desktop/ROBOCLUEDO_ONTOLOGY.owl" );
}



void armorTestSession2( std::string ontology_file_path )
{
	ArmorCluedo armor( true );
	armor.Init( ontology_file_path );
	
	armor.SendCommand( "DISJOINT", "CLASS", "", "PERSON", "WEAPON" );
	armor.SendCommand( "DISJOINT", "CLASS", "", "WEAPON", "PLACE" );
	armor.SendCommand( "DISJOINT", "CLASS", "", "PERSON", "PLACE" );
	armor.UpdateOntology( );
	
	ROS_INFO_STREAM( OUTLABEL << "\n\n\n TEST COMANDI DIRETTI ARMOR \n\n\n" );
	
	armor.AddIndiv( "Jim", "PERSON" );
	armor.AddIndiv( "Gun", "WEAPON" );
	// armor.AddIndiv( "study", "PLACE" );
	armor.AddIndiv( "HP3", "HYPOTHESIS" );
	armor.SetObjectProperty( "who", "HP3", "Jim" );
	armor.SetObjectProperty( "what", "HP3", "Gun" );
	// armor.SetObjectProperty( "where", "HP3", "study" );
	armor.UpdateOntology( );
	
	armor.SendCommand( "QUERY", "IND", "CLASS", "COMPLETED" );
	ROS_INFO_STREAM( OUTLABEL << "(0) completed hypotheses: (dovrebbe essere vuoto)" );
	armor.PrintLastRes( );
	armor.SendCommand( "QUERY", "IND", "CLASS", "INCONSISTENT" );
	ROS_INFO_STREAM( OUTLABEL << "(0) inconsistent hypotheses: (dovrebbe essere vuoto)" );
	armor.PrintLastRes( ); 
	
	
	armor.AddIndiv( "Stadium", "PLACE" );
	armor.SetObjectProperty( "where", "HP3", "Stadium" );
	armor.UpdateOntology( );
	
	armor.SendCommand( "QUERY", "IND", "CLASS", "COMPLETED" );
	ROS_INFO_STREAM( OUTLABEL << "(1) completed hypotheses: (dovrebbe comparire un'ipotesi)" );
	armor.PrintLastRes( );
	armor.SendCommand( "QUERY", "IND", "CLASS", "INCONSISTENT" );
	ROS_INFO_STREAM( OUTLABEL << "(1) inconsistent hypotheses: (la lista dovrebbe essere vuota)" );
	armor.PrintLastRes( ); 
	
	
	armor.AddIndiv( "Lounge", "PLACE" );
	armor.AddIndiv( "Study", "PLACE" );
	armor.SetObjectProperty( "where", "HP3", "Study" );
	armor.UpdateOntology( );
	
	armor.SendCommand( "QUERY", "IND", "CLASS", "COMPLETED" );
	ROS_INFO_STREAM( OUTLABEL << "(2) completed hypotheses: (sempre una sola ipotesi)" );
	armor.PrintLastRes( );
	armor.SendCommand( "QUERY", "IND", "CLASS", "INCONSISTENT" );
	ROS_INFO_STREAM( OUTLABEL << "(2) inconsistent hypotheses: (dovrebbe spuntare un'ipotesi inconsistente)" );
	armor.PrintLastRes( ); 
	
	
	armor.GetValuedOfIndiv( "where", "HP3" );
	ROS_INFO_STREAM( OUTLABEL << "proprieta' where dell'ipotesi HP3:" );
	armor.PrintLastRes( ); 
	/*
	ROS_INFO_STREAM( OUTLABEL << "esiste HP3? -> " << ( armor.ExistsIndiv( "HP3" ) ? "yes" : "no" ) );
	armor.PrintLastRes( ); 
	*/
	ROS_INFO_STREAM( OUTLABEL << "test esistenza di HP3:" );
	armor.SendCommand( "QUERY", "IND", "", "HP3", "", "", "", "", true );
	ROS_INFO_STREAM( OUTLABEL << "esiste HP3? (atteso: yes) -> " << ( armor.ExistsIndiv( "HP3" ) ? "yes" : "no" ) );
	armor.PrintLastRes( ); 
	
	
	
	ROS_INFO_STREAM( OUTLABEL << "\n\n\n TEST COMPLETO INTERFACCIA \n\n\n" );
	
	ROS_INFO_STREAM( OUTLABEL << "\n\n\t======\tTEST GetClassOfIndiv()" );
	ROS_INFO_STREAM( OUTLABEL << "classe dell'individual " << "Stadium" << "(con la maiuscola, scritto correttamente, no deep search, atteso un solo elemento)" );
	printList( armor.GetClassOfIndiv( "Stadium", false ) );
	ROS_INFO_STREAM( "" );
	ROS_INFO_STREAM( OUTLABEL << "classe dell'individual " << "Stadium" << "(con la maiuscola, scritto correttamente, usando la deep search, atteso un solo elemento)" );
	printList( armor.GetClassOfIndiv( "Stadium", true ) );
	ROS_INFO_STREAM( "" );
	ROS_INFO_STREAM( OUTLABEL << "classe dell'individual " << "stadium" << "(senza la maiuscola, sbagliato, no deep search, atteso 0)" );
	printList( armor.GetClassOfIndiv( "stadium", false ) );
	ROS_INFO_STREAM( "" );
	
	
	
	ROS_INFO_STREAM( OUTLABEL << "\n\n\t======\tTEST ExistsIndiv()" );
	ROS_INFO_STREAM( OUTLABEL << "esiste '" << "stadium" << "' senza la maiuscola? -> " << ( armor.ExistsIndiv( "stadium" ) ? "yes" : "no" ) );
	ROS_INFO_STREAM( OUTLABEL << "esiste '" << "Stadium" << "' con la maiuscola? -> " << ( armor.ExistsIndiv( "Stadium" ) ? "yes" : "no" ) );
	ROS_INFO_STREAM( OUTLABEL << "esiste '" << "HP0" << "' ? -> " << ( armor.ExistsIndiv( "HP0" ) ? "yes" : "no" ) );
	
	
	
	ROS_INFO_STREAM( OUTLABEL << "\n\n\t======\tTEST GetIndivOfClass()" );
	ROS_INFO_STREAM( OUTLABEL << "tutti gli individuals della classe HYPOTHESIS" << "(attesa ... un bel po' di roba)" );
	printList( armor.GetIndivOfClass( "HYPOTHESIS" ) );
	ROS_INFO_STREAM( "" );
	ROS_INFO_STREAM( OUTLABEL << "tutti gli individuals della classe PLACE" << "(attesi 3 elementi)" );
	printList( armor.GetIndivOfClass( "PLACE" ) );
	ROS_INFO_STREAM( "" );
	
	
	ROS_INFO_STREAM( OUTLABEL << "\n\n\t======\tTEST GetValuedOfIndiv()" );
	ROS_INFO_STREAM( OUTLABEL << "proprietà where dell'ipotesi HP3" << "(attesi due elementi)" );
	printList( armor.GetValuedOfIndiv( "where", "HP3" ) );
	ROS_INFO_STREAM( "" );
	
	
	
	ROS_INFO_STREAM( OUTLABEL << "\n\n\t======\tTEST RemoveHypothesis()" );
	ROS_WARN_STREAM( OUTLABEL << "NOTA BENE: il comando REMOVE di aRMOR è buggato, non funziona. Per farlo funzionare occorre trovare un workaround nell'interfaccia" );
	
	ROS_INFO_STREAM( OUTLABEL << "aggiungo l'ipotesi HPtoremove (uso qui solo comandi dell'interfaccia, non i diretti di aRMOR)" );
	armor.AddIndiv( "HPtoremove", "HYPOTHESIS", false );
	armor.UpdateOntology( );
	ROS_INFO_STREAM( OUTLABEL << "esiste '" << "HPtoremove" << "' ? -> " << ( armor.ExistsIndiv( "HPtoremove" ) ? "yes" : "no" ) );
	ROS_INFO_STREAM( OUTLABEL << "classe dell'individual " << "HPtoremove" << "? (atteso un solo elemento)" );
	printList( armor.GetClassOfIndiv( "HPtoremove", false ) );
	
	ROS_INFO_STREAM( OUTLABEL << "rimozione" );
	// armor.SendCommand( "REMOVE", "IND", "CLASS", "HPtoremove", "HYPOTHESIS" );
	// armor.PrintLastReq( );
	// armor.PrintLastRes( );
	ROS_INFO_STREAM( OUTLABEL << "RemoveHypothesis( ) = " << ( armor.RemoveHypothesis( "HPtoremove" ) ? "yes" : "no" ) );
	armor.UpdateOntology( );
	ROS_INFO_STREAM( OUTLABEL << "esiste ancora '" << "HPtoremove" << "' ? -> " << ( armor.ExistsIndiv( "HPtoremove" ) ? "yes" : "no" ) );
	
	
	
	ROS_INFO_STREAM( OUTLABEL << "\n\n\t======\tTEST FindCompleteHypotheses()" );
	printList( armor.FindCompleteHypotheses( ), "ricerca delle ipotesi complete (atteso: una sola ipotesi, escludendo quella scartata" );
	
	
	
	ROS_INFO_STREAM( OUTLABEL << "\n\n\t======\tTEST FindInconsistentHypotheses()" );
	printList( armor.FindCompleteHypotheses( ), "ricerca delle ipotesi inconsistenti (atteso: solo una, o anche due, includendo quella scartata)" );
	
	
	
	std::string save_path;
	ros::param::get( "cluedo_path_owlfile_backup", save_path );
	armor.SaveOntology( save_path ); 
	return;
	
}


// test section
int main( int argc, char* argv[] )
{
	ros::init( argc, argv, "test_armor_tools" );
	ros::NodeHandle nh;
	
	// path of the ontology file from parameter server
	std::string ontology_file_path;
	if( !ros::param::has( ONTOLOGY_PARAM ) )
	{
		ROS_INFO_STREAM( OUTLABEL << "ERROR: parameter '" << ONTOLOGY_PARAM << "' not defined." );
		return 0;
	}
	ros::param::get( ONTOLOGY_PARAM, ontology_file_path );
	if( !fileExist( ontology_file_path ) )
	{
		ROS_INFO_STREAM( OUTLABEL << "ERROR: the file '" << ONTOLOGY_PARAM << "' doesn't exist." );
		return 0;
	}
	ROS_INFO_STREAM( OUTLABEL << "Ontology found! " << LOGSQUARE( ontology_file_path ) );
	
	ArmorTools tools( true );
	if( !tools.ConnectAndLoad( ontology_file_path ) )
	{
		ROS_WARN_STREAM( OUTLABEL << "ERROR: unable to ConnectAndLoad() aRMOR tools" );
		return 0;
	}
	armorTestSession( tools );
	// armorTestSession2( ontology_file_path );
	
	ROS_INFO_STREAM( OUTLABEL << "THAT'S ALL FOLKS!" );
	return 0;
}
