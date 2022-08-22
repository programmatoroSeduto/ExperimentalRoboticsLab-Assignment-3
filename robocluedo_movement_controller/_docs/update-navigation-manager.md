
# HOW TO update the navigation manager

---

```{toctree}
---
caption: Contents
---
./update-navigation-manager.md
```

---

The current navigation manager applies the C++ plymorphism in order to make a flexible and helpful abstraction layer between the nodes requiring the navigation services and how the navigation service is really implemented. After all, each node which wants to use the navigation should just tell the node in charge to manage the navigation the final position and, in case, the final orientation of the robot, and nothing more (at least in a very simple and common scenario).

## A description of the navigation Manager Node

The navigation manager uses the *controllers*, i.e. modules (objects) using the low-level implementation of the navigation in order to reach the objective provided. In particular, here are some characteristics of this particular system:

- the navigation manager is a *list of controllers* with one controller active set apart from the array
- each controller can be enabled or disabled
- all the services and topics needed to make the low level work are hidden in the controller, whereas the navigation manager sees alway the same interface
- a controller can be replaced *at runtime* depending on the situation, if needed, which means that the robot can use many combinations of the low level behaviours in order to carry out the objective

The current navigation manager (for the third part of the RoboCLuedo project) contains two controllers:

- the controller with ID=0 controls: bug_m, and indirectly head_orientation and go_to_point
- the controller with ID=1 controls move_base and indirectly head_orientation

bu for instance I could think on a controller which makes the big part of the trave with move_base, and arrived at a certain distance from the objective, uses the bug_m to perform the final steps of the navigation. This could be implemented in a controller using almost all the behaviours seen above. This is what this model can do. 

## More techincally speaking

each controller is implemented as a class. Here is its "blueprint":

```c++
class nav_??? : public navigation_controller
{
public:
	
	/** class constructor */
	nav_???( std::string name = "", int code = -1 ) :
		navigation_controller( name, code )
	{ 
		// ...
	}
	
	/** class destructor */
	~nav_???( )
	{ 
		// ...
	}
	
	/** turn on the algorithm */
	bool enable( )
	{
		if( !channels_enabled )
		{
			// ... try opening the channels ... 
			
			(ros::Duration(2)).sleep();
			channels_enabled = true;
		}
		
		// ...
		
		this->enabled = true;
		return true;
	}
	
	/** use the algorithm for the 2D navigation */
	bool perform_navigation( 
		robocluedo_movement_controller_msgs::NavigationService::Request& req,
		robocluedo_movement_controller_msgs::NavigationService::Response& res )
	{
		if( !enabled )
		{
			res.success = false;
			res.details = "the controller isn't active";
			
			return false;
		}
		
		// ... perform the navigation
		
		res.success = true;
		res.details = "";
		
		return true;
	};
	
	/** turn off the algorithm */
	bool disable( )
	{
		this->enabled = false;
		
		if( channels_enabled )
		{
			// ...
			
			this->channels_enabled = false;
		}
		
		// ...
		
		return true;
	}

private:

	// ... services and topics for the navigation ... 
	
	/// channels opened?
	bool channels_enabled = false;
};
```

### the class Navigation Controller

as you can noticed in the code above, all the controllers must inherit from a class called `navigation_controller` which is rewritten here for your convenience:

```c++
class navigation_controller
{
public:
	
	/// navigation identfier
	int nav_code = -1;
	
	/// navigation algorithm name
	std::string nav_name = "";
	
	/** class constructor */
	navigation_controller( std::string name = "", int code = -1 ) :
		nav_code( code )
	{ 
		if( name != "" )
			this->nav_name = name;
	}
	
	/** class destructor */
	~navigation_controller( ) { }
	
	/** (virtual) turn on the algorithm */
	virtual bool enable( )
	{
		this->enabled = true;
		return true;
	}
	
	/** (virtual) use the algorithm for the 2D navigation */
	virtual bool perform_navigation( 
		robocluedo_movement_controller_msgs::NavigationService::Request& req,
		robocluedo_movement_controller_msgs::NavigationService::Response& res )
	{
		return this->enabled;
	};
	
	/** (virtual) turn off the algorithm */
	virtual bool disable( )
	{
		this->enabled = false;
		return true;
	}
	
	/** check wether the algorithm is enabled or not */
	bool is_enabled( ) { return this->enabled; }

protected:
	
	/// ROS Node handle 
	ros::NodeHandle nh;
	
	/// activity state of the algorithm
	bool enabled = false;
};
```

you can notice here that each controller has some very important methods in its interface:

- `bool enable( )` : called for turning on the controller when selected; here the controller opens the channels with the low level, if they are yet not activated.
	
	here I suggest to open up the interfaces once, and then to check for their validity at each calling. Another approach is to use the `enable()` to open up the interfaces, and then use `disable()` to turn them off, each time in the lifecycle of the component. 
	
- `bool perform_navigation(req, res)` is function called by the general service of the navigation manager. It should return true when the navigation succeeded, and false when there's something wrong; it also returns a explaination (if possible) of the error.
	
	Notice that the service request is handled by the navigation manager, but it is job of this function to manipulate the data inside the message according to the proceeding of the navigation, exactly as this would be the real service call. 
	
- `bool disable( )` turns off the component
	
	see the comment under the `bool enable( )` call. 

## register a controller

Implementing the controller is needed, but not sufficient. In order to make the controller available inside the navigation manager, you need to register it. Here's a example you can use for this task:

```{attention}
the service for changin the algorithm assumes to use a ID for selecting the algorithm. The function `.register_algorithm()` assigns a ID to the algorithm *according to the order of all the registrations*.. Hence, be sure that the ID is always the same for that algorithm: choose the ID at the beginning of the project, and keep this forever (unless you don't want to modify or refactor your code). 
```

```c++
class_navigation_manager nav;

// controller bug_m
nav_bug_m bug_m;
bug_m.nav_name = "bug_m";
nav.register_algorithm( &bug_m );

// controller move_base_nav
nav_move_base moveb;
moveb.nav_name = "move_base_nav";
nav.register_algorithm( &moveb );
```

In this example, `bug_m` will have ID=0 whereas `move_base_nav` will have ID=1 . 

## (in the end) how to create a new component

here are the steps:

1. define the new controller exploiting the inheritance 
2. implement it
3. register the controller in the main function

very simple. 

## Observations

- one controller could always interact with the parameter server, and this enables to overcome the simplicity of the interface for communicating with the controller
- the naigation manager could implement many versions of the same controller, even using the inheritance if it is a good idea, in order to make a more detailed control 
