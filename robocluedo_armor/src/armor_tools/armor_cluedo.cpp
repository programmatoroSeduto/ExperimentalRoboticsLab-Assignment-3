
/********************************************//**
 *  
 * @file armor_cluedo.cpp
 * @brief implementation of the class ArmorCluedo
 * @author Francesco Ganci (S4143910)
 * @version v1.0
 *  
 * @see armor_cluedo.h
 * 
 ***********************************************/

#include "armor_tools/armor_cluedo.h"



// default constructor
ArmorCluedo::ArmorCluedo( bool debugmode ): ArmorTools( debugmode )
{
	// ...
}



// destructor
ArmorCluedo::~ArmorCluedo( )
{ 
	// ...
}



// init the interface
bool ArmorCluedo::Init( std::string ontologyPath )
{
	// try to connect to the service
	this->ConnectAndLoad( ontologyPath );
	
	// test for errors in the connection
	if( ( this->GetLastErrorCode( ) != 0 ) || !this->TestInterface( ) )
	{
		ARMOR_ERR( "in connecting and loading the ontology. " );
		return false;
	}
	
	// connection successful
	return true;
}



// add an individual
bool ArmorCluedo::AddIndiv( std::string indivname, std::string classname, bool makeDisjoint )
{
	// return false if the individual is a discarded hypothesis
	if( classname == "HYPOTHESIS" )
	{
		if( this->GetPositionOf( indivname, DiscardHypotheses ) != DiscardHypotheses.end( ) )
		 return false;
	}
	
	// track the individual
	if( !this->TrackIndiv( indivname ) ) return false;
	
	// send the ADD command
	if( !this->SendCommand( "ADD", "IND", "CLASS", indivname, classname ) )
		return false;
	
	// disjoint this individual from the other
	if( makeDisjoint ) DisjointAllIndiv( indivname );
	// this->SendCommand( "REASON" );
	// this->SendCommand( "DISJOINT", "IND", "CLASS" );
	// this->SendCommand( "REASON" );
	
	return true;
}



// check the class of an individual
std::vector<std::string> ArmorCluedo::GetClassOfIndiv( std::string indivname, bool deep = false )
{
	// ask for the class of a given individual
	if( !this->SendCommand( "QUERY", "CLASS", "IND", indivname, SS( deep? "true" : "false" ) ) )
	{
		ARMOR_ERR( "unable to find the classes of the individual " << indivname );
		return std::vector<std::string>();
	}
	
	// filter the response and return it
	return FilterVector( this->GetLastRes( ).queried_objects );
}



// find the individuals belonging to a class
std::vector<std::string> ArmorCluedo::GetIndivOfClass( std::string classname )
{
	if( !this->SendCommand( "QUERY", "IND", "CLASS", classname ) )
	{
		ARMOR_ERR( "unable to find the individuals inside the class " << classname );
		return std::vector<std::string>();
	}
	
	// filter the response and return it
	return FilterVector( this->GetLastRes( ).queried_objects );
}



// check if an individual exists
bool ArmorCluedo::ExistsIndiv( std::string indivname )
{
	this->SendCommand( "QUERY", "IND", "", indivname );
	
	if( this->GetLastRes( ).success )
	{
		// exclude discarded hypotheses
		if( this->GetPositionOf( indivname, DiscardHypotheses ) != DiscardHypotheses.end( ) )
			return false;
		else
			return true;
	}
	else 
		return false;
}



// set a property true
bool ArmorCluedo::SetObjectProperty( std::string prop, std::string Aelem, std::string Belem )
{
	// these elements must be added before calling this function
	if( !ExistsItem( Aelem, individuals ) || !ExistsItem( Belem, individuals ) )
		return false;
	
	// set the protery true by a service call
	if( !this->SendCommand( "ADD", "OBJECTPROP", "IND", prop, Aelem, Belem ) )
	{
		ARMOR_ERR( "unable to set the property " << "(" << Aelem << ", " << Belem << "):" << prop );
		return false;
	}
	
	return true;
}



// get the values of a property related to a gven individual
std::vector<std::string> ArmorCluedo::GetValuedOfIndiv( std::string prop, std::string indivname )
{
	if( !this->SendCommand( "QUERY", "OBJECTPROP", "IND", prop, indivname ) )
	{
		ARMOR_ERR( "unable to find the values of the property " << prop << " related to the individual " << indivname );
		return std::vector<std::string>();
	}
	
	// filter the response and return it
	return FilterVector( this->GetLastRes( ).queried_objects );
}



// find all the complete hypotheses
std::vector<std::string> ArmorCluedo::FindCompleteHypotheses( )
{
	std::vector<std::string> temp = GetIndivOfClass( "COMPLETED" );
	
	// WORKAROUND for issue "REMOVE IND CLASS"
	//    remove the discarded hypotheses
	for( auto it = DiscardHypotheses.begin() ; it != DiscardHypotheses.end(); ++it )
	{
		auto pos = this->GetPositionOf( *it, temp );
		if( pos != temp.end( ) )
			temp.erase( pos );
	}
	
	return temp;
}



// find all the inconsistent hypotheses
std::vector<std::string> ArmorCluedo::FindInconsistentHypotheses( )
{
	return GetIndivOfClass( "INCONSISTENT" );
}



// remove one hyothesis
bool ArmorCluedo::RemoveHypothesis( std::string hypTag )
{
	// check if it exists
	if( !ExistsIndiv( hypTag ) ) return false;
	
	/* sarebbe molto bello ... ma purtroppo REMOVE IND CLASS è buggato.
	
	// delete the individual from all the classes
	std::vector<std::string> classnames = this->GetClassOfIndiv( hypTag );
	for( std::string cname : classnames )
		if( !this->SendCommand( "REMOVE", "IND", "CLASS", hypTag, cname ) )
			return false;
	
	*/
	
	// add the hypothesis to the list of the discarded hypotheses
	DiscardHypotheses.push_back( hypTag );
	
	// untrack the individual
	auto pos = this->GetPositionOf( hypTag, individuals );
	if( pos != individuals.end( ) )
	{
		individuals.erase( pos );
	}
	
	return true;
}



// rewrite a string like '<uri#value>' into 'value'
std::string ArmorCluedo::FilterValue( std::string raw )
{
	std::size_t pos;
	if( ( pos = raw.find( "#" ) ) == std::string::npos )
		return raw;
	else
		return raw.substr( pos+1, raw.length()-pos-2 );
}



// filter all the strings inside the array
std::vector<std::string> ArmorCluedo::FilterVector( std::vector<std::string>& itemlist )
{
	std::vector<std::string> returnlist;
	
	// filter each element
	for( std::string item : itemlist )
		returnlist.push_back( this->FilterValue( item ) );
	
	return returnlist;
}



// check if a string exists in one array
bool ArmorCluedo::ExistsItem( std::string item, const std::vector<std::string>& container )
{
	for( std::string s : container )
		if( s == item ) return true;
	
	return false;
}



// track individual
bool ArmorCluedo::TrackIndiv( std::string indivname )
{
	// check if the individual was already defined in the array
	if( ExistsItem( indivname, this->individuals ) )
	{
		// ARMOR_ERR( "the individual " << indivname << "is already tracked. " );
		return false;
	}
	
	// add the item to the list of items
	this->individuals.push_back( indivname );
	
	return true;
}



// disjoint all the individuals
void ArmorCluedo::DisjointAllIndiv( std::string from = "" )
{
	if( from == "" )
	{
		// iterative brute force disjoint
		for( std::string s : this->individuals )
			this->DisjointAllIndiv( s );
	}
	else
	{
		// disjoint wrt one element
		for( std::string s : this->individuals )
			if( s != from )
				this->SendCommand( "DISJOINT", "IND", "", s, from );
	}
}



// search for a specific iterator on a vector
//   it return vector::end() if the element is not contained
std::vector<std::string>::iterator ArmorCluedo::GetPositionOf( std::string tag, std::vector<std::string>& list )
{
	// ARMOR_INFO( "[ArmorCluedo::GetPositionOf] listsize=" << list.size() );
	for( auto it = list.begin(); it != list.end( ); ++it )
	{
		if( *it == tag )  return it;
	}
	
	// not found
	return list.end( );
}
