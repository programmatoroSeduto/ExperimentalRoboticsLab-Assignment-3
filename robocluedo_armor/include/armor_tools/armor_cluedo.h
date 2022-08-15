
/********************************************//**
 *  
 * \file armor_cluedo.h
 * \brief additional aRMOR C++ interface
 * \authors Francesco Ganci (S4143910)
 * \version v1.0
 * 
 * @see ArmorCluedo
 * @see ArmorTools
 * 
 ***********************************************/

#ifndef __H_ARMOR_CLUEDO_H__
#define __H_ARMOR_CLUEDO_H__

#include "armor_tools/armor_tools.h"

#include <vector>
#include <iostream>
#include <string>




/********************************************//**
 *  
 * \brief additional utilities for aRMOR
 * \extends ArmorTools
 * 
 * Whereas the class ArmorTools provides the low-level tools for communicating 
 * with aRMOR, the class ArmorCluedo implements some specific methods for 
 * this project, and sme workaround due to some bugs in the actual 
 * implementation of aRMOR, in particular for the commands "REMOVE" and
 * "DISJOINT". <br>
 * 
 * The class ArmorCluedo provides methods for doing these operations: 
 * <ul>
 * <li>add individuals</li>
 * <li>check existence of individuals</li>
 * <li>find the classes of an individual</li>
 * <li>find the content of a class</li>
 * <li>set/get properties</li>
 * </ul>
 * In addition, the class implements some useful methods for managing the
 * hypotheses:
 * <ul>
 * <li>add hypotheses</li>
 * <li>queries of hypotheses</li>
 * <li>discard hypotheses</li>
 * </ul>
 * 
 * @todo maybe the issues with aRMOR are due to a lack in its documentation. The implemented workarounds could reduce the performances of the program. 
 * 
 ***********************************************/
class ArmorCluedo : public ArmorTools
{
public:
	
	/********************************************//**
	 *  
	 * \brief class constructor of ArmorCluedo
	 * 
	 * The constrctor simply instanciates the internal data structures
	 * and the ArmorTools class, without connection and loading phases. 
	 * 
	 * @param debugmode enable or not debug mode; see \ref ARMOR_DEFAULT_DEBUGMODE
	 * 
	 ***********************************************/
	ArmorCluedo( bool debugmode = ARMOR_DEFAULT_DEBUGMODE );
	
	
	/// class destructor
	~ArmorCluedo( );
	
	
	/********************************************//**
	 *  
	 * \brief initizalize the interface
	 * 
	 * The function performs both the first steps: connection to the aRMOR
	 * service, and the load of the file. A default loading phase is executed,
	 * with settings: <br>
	 * <b><code>LOAD FILE <i>ontologyPath</i> \ref ARMOR_DEFAULT_URI true 
	 * \ref ARMOR_DEFAULT_REASONER true</code></b> <br>
	 * No other types of initializations are needed right now by the project. 
	 * 
	 * @param ontologyPath the path of the .owl file
	 * 
	 * @returns success or not
	 * 
	 * @see <a href="https://github.com/EmaroLab/armor/blob/master/README.MD"> the official aRMOR documentation</a> for more informations
	 * 
	 * @note If you need to initialize the interface in another way, you
	 *    still can call the methods inside ArmorTools and setup the 
	 *    connection in the proper way. 
	 * 
	 ***********************************************/
	bool Init( std::string ontologyPath );
	
	
	
	// ======== individuals and classes
	//    methods for adding individuals to the ontology
	
	/********************************************//**
	 *  
	 * \brief add an individual to the ontology
	 * 
	 * In simple words, this function adds the individual to the database. <br>
	 * 
	 * Due to the need of the command "DISJOINT" in order to make the 
	 * reasoner able to classify the hypotheses, and due to an issue with
	 * the command "DISJOINT IND CLASS", this function works in a more 
	 * complex way. Here is how it works:
	 * <ol>
	 * <li> before starting, if the individual has class HYPOTHESIS and was
	 *       discarded, return false immediately </li>
	 * <li> The individual is <i>tracked</i>, i.e. put into an array which
	 * 	     contains all the defined individuals</li>
	 * <li> then, the individual is added </li>
	 * <li> call the command "REASON" </li>
	 * <li> if the last argument makeDisjoint is ture, the function also
	 *       performs "DISJOINT" between the element to add and all the 
	 *       other elements. It is the only <i>working and reliable way</i> 
	 *       to ensure a correct classification by the reasoner right now.</li>
	 * </ol>
	 * 
	 * Since in RCL, the hypotheses are the only individuals with properties,
	 * there's no need to DISJOINT them. Hence, that last option can be useful 
	 * to reduce the complexity of the method: if makeDisjoint is set to false,
	 * the function doesn't perform the last step. The element is tracked anyway. 
	 * 
	 * @param indivname    the individual to add
	 * @param classname    the class of the individual to add
	 * @param makeDisjoint perform disjoint or not?
	 * 
	 * @returns true if the individual doesn't appear in the database; 
	 *    false otherwise
	 * 
	 * @warning remember: call upload before any manipolation command! 
	 * 
	 ***********************************************/
	bool AddIndiv( std::string indivname, std::string classname, bool makeDisjoint = true );
	
	
	/********************************************//**
	 *  
	 * \brief get the class of a given individual
	 * 
	 * The function implements a simple query: find all the classes an 
	 * individual belongs to. It is performed using only one aRMOR call. <br>
	 * 
	 * using the <i>Deep search</i>, the function will return only the 
	 * deepest subclass among all the classes; it is a functionality
	 * provided by aRMOR. 
	 * 
	 * @param indivname the individual to be "classified"
	 * @param deep use deep search?
	 * 
	 * @returns a vector of classes (zero if the indiv. doesn't exist)
	 * 
	 ***********************************************/
	std::vector<std::string> GetClassOfIndiv( std::string indivname, bool deep );
	
	
	/********************************************//**
	 *  
	 * \brief find the individuals belonging to a class
	 * 
	 * @param classname the class
	 * 
	 * @returns a vector of individals inside the class (eventually empty)
	 * 
	 * @warning The element also returns the discarded hypotheses. <i>Don't use
	 * this method for inspecting the hypotheses! </i>
	 * 
	 ***********************************************/
	std::vector<std::string> GetIndivOfClass( std::string classname );
	
	
	/********************************************//**
	 *  
	 * \brief check if an individual exists
	 * 
	 * @param indivname the name of the individual
	 * 
	 * @returns if the individual exists or not; false if the individual
	 *     is a discarded hypothesis. 
	 * 
	 ***********************************************/
	bool ExistsIndiv( std::string indivname );
	
	
	
	// ======== properties
	//    methods for checking and adding properties between individuals
	
	/********************************************//**
	 *  
	 * \brief set a property true
	 * 
	 * The function sets a property like (Aelem, Belem):prop .
	 * 
	 * @param prop  the property to set
	 * @param Aelem the domain
	 * @param Belem the image
	 * 
	 * @returns false if one between Aelem and Belem doesn't exist.
	 * 
	 * @note if the prop is not valid, the aRMOR call will fail, hence
	 *    the method returns false. 
	 * 
	 ***********************************************/
	bool SetObjectProperty( std::string prop, std::string Aelem, std::string Belem );
	
	
	/********************************************//**
	 *  
	 * \brief get the values of a property related to a gven individual
	 * 
	 * Given  a set of properties like (indivname, value):prop, the
	 * method retrieves all the fields 'value'. 
	 * 
	 * @param prop      the property
	 * @param indivname the name of the individual
	 * 
	 * @returns the values of the property for the given individual
	 * 
	 ***********************************************/
	std::vector<std::string> GetValuedOfIndiv( std::string prop, std::string indivname );
	
	
	
	// ======== hypotheses
	//    methods for formulating and working with hypotheses
	
	/********************************************//**
	 *  
	 * \brief find all the complete hypotheses
	 * 
	 * The methos finds all the COMPLETE hypotheses, except for the 
	 * discarded ones. 
	 * 
	 * @returns a vector containing the tags of all the COMPLETE hyp.
	 * 
	 * @see ServiceFindConsistentHypotheses an example of usage
	 * 
	 ***********************************************/
	std::vector<std::string> FindCompleteHypotheses( );
	
	
	/********************************************//**
	 *  
	 * \brief find all the inconsistent hypotheses
	 * 
	 * @returns a vector containing the tags of all the INCONSISTENT hyp.
	 * 
	 * @note This method doesn't perform any filtering on the tags, so 
	 * the output is exactly the individuals belonging to INCONSISTENT
	 * without deleting the discarded hypotheses. I recommend to not
	 * use this method, except for some cases. 
	 * 
	 * @see ServiceFindConsistentHypotheses an example of usage
	 * 
	 ***********************************************/
	std::vector<std::string> FindInconsistentHypotheses( );
	
	
	/********************************************//**
	 *  
	 * \brief discard one hypothesis
	 * 
	 * Due to an issue of the implementation of aRMOR, the command "REMOVE 
	 * IND CLASS" has no effect on the ontology. So, a workaround has been 
	 * implemented. The class tracks the hypothesis as <i>discarded</i>; 
	 * all the other methods are implemented in a way such that a discarded 
	 * hypothesis doesn't appear anymore as <i>complete</i> or <i>existing</i>. 
	 * the discarded hypothesis is still in the ontology, but will be 
	 * discarded with a post filtering, or will become inconsistent. <br>
	 * 
	 * In RCL there's no need to reconsider hypotheses, so, in this specific
	 * context, this approach works well. 
	 * 
	 * @param hypTag the hypothesis tag to discard
	 * 
	 * @returns if the hypothesis has been discarded or not; false if the 
	 *    hypothesis wasn't defined before. 
	 * 
	 ***********************************************/
	bool RemoveHypothesis( std::string hypTag );
	
	
	
	// ======== utilities
	//    general-purpose methods
	
	/********************************************//**
	 *  
	 * \brief rewrite a string like '<uri\#value>' into 'value'
	 * 
	 * @param raw the string to be filtered
	 * 
	 * @returns the value inside the string
	 * 
	 ***********************************************/
	std::string FilterValue( std::string raw );
	
	
	/********************************************//**
	 *  
	 * \brief filter all the strings inside the array
	 * 
	 * It simply extends the method ArmorCluedo::FilterValue to an array
	 * of strings to be filtered. 
	 * 
	 * @param itemList the array of strings to be filtered
	 * 
	 * @returns the array containing the filtered strings
	 * 
	 ***********************************************/
	std::vector<std::string> FilterVector( std::vector<std::string>& itemlist );
	
private:
	// all the entities added
	std::vector<std::string> individuals;
	
	// removed hypotheses
	std::vector<std::string> DiscardHypotheses;
	
	// check if a string exists in one array
	bool ExistsItem( std::string item, const std::vector<std::string>& container );
	
	// track individual
	bool TrackIndiv( std::string indivname );
	
	// disjoint all the individuals
	void DisjointAllIndiv( std::string from );
	
	// search for a specific iterator on a vector
	//   it return vector::end() if the element is not contained
	std::vector<std::string>::iterator GetPositionOf( std::string tag, std::vector<std::string>& list );
};

#endif
