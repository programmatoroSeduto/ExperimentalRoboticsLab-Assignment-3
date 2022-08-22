
#define NODE_NAME "aruco_detection"

/********************************************//**
*  
* @file aruco_detection.cpp
* @brief arUco markers detection on 4 cameras and images display
* 
* @authors Francesco Ganci
* @version v1.0
* 
* This node implements two important functionalities: it displays what
* the cameras are seeing in a window, changing the video stream every
* 8 seconds; and, second, the most important thing, the node applies the
* detection of the ArUco markers distributed on the four cameras of the 
* robot. 
* 
* @todo it could be a good idea to expose the update time of the image 
* 	to the parameter server
* 
* @todo the program should decide (via parameter server) if to visualize 
* 	or not the echo from the cameras. 
* 
* @todo the user could also decide how many windows to open. Let's think 
* 	for instance on a scenario in which the user wants to see two outputs
* 	at the same time. 
*  
* @todo enable the support for the develop mode (it is missing so far)
* 
***********************************************/

#include "ros/ros.h"

#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include <aruco/cvdrawingutils.h>
#include <image_transport/image_transport.h>
#include <aruco/aruco.h>

#include "sensor_msgs/Image.h"
#include "sensor_msgs/image_encodings.h"
#include "std_msgs/Int32.h"

#include <string>

/// \private
#define SS(chr) std::string( chr )

/// \private
#define SSS(el) std::to_string( el )

#include <vector>
#include <algorithm>
#include <set>
#include <mutex>
#include <signal.h>

/// the video stream changes every UPDATE_TIME seconds
#define UPDATE_TIME 8.0

/// the maximum rate for the arUco detection
#define MAX_SUB_RATE 8

/// 'echo' windows title
static const std::string OPENCV_WINDOW = "RoboCLuedo Cameras";

/// arm camera identifier
#define ID_CAMERA_ARM 0

/// arm camera source topic
#define TOPIC_CAMERA_ARM "/robocluedo_robot/camera_arm/image_raw"

/// front camera identifier
#define ID_CAMERA_FRONT 1

/// front camera source topic
#define TOPIC_CAMERA_FRONT "/robocluedo_robot/camera_front_low/image_raw"

/// left camera identifier
#define ID_CAMERA_LEFT 2

/// left camera source topic
#define TOPIC_CAMERA_LEFT "/robocluedo_robot/camera_left/image_raw"

/// right camera identifier
#define ID_CAMERA_RIGHT 3

/// right camera source topic
#define TOPIC_CAMERA_RIGHT "/robocluedo_robot/camera_right/image_raw"

/// topic used for communicating the IDs when detected
#define TOPIC_DETECTED_IDS "/aruco_detected_ids"

/// node implementation (as class)
class node_aruco_detection
{
public:
	
	/********************************************//**
	 *  
	 * \brief class constructor
	 * 
	 * the constructor opens all the subscriptions with the cameras, and
	 * instanciates the publisher that  will transmit the new IDs when 
	 * identified. 
	 * 
	 * done that, the constructor opens the window. 
	 * 
	 * @note the class uses the ImageTransport which enables the class
	 * 	to use a compressed way to read the images.
	 * 
	 * @todo let the user decide whether to open the window and when to 
	 * 	close it. It could be a good idea to create a server for switching
	 * 	the window. 
	 * 
	 ***********************************************/
	node_aruco_detection( ) : 
		nh("~"), it(nh), useCamInfo(true), max_sub_rate( MAX_SUB_RATE )
	{
		// image inputs
		image_sub_arm = it.subscribe(TOPIC_CAMERA_ARM, 1, &node_aruco_detection::cbk_cam_arm, this);
		image_sub_front = it.subscribe(TOPIC_CAMERA_FRONT, 1, &node_aruco_detection::cbk_cam_front, this);
		image_sub_left = it.subscribe(TOPIC_CAMERA_LEFT, 1, &node_aruco_detection::cbk_cam_left, this);
		image_sub_right = it.subscribe(TOPIC_CAMERA_RIGHT, 1, &node_aruco_detection::cbk_cam_right, this);
		
		// setup ArUco
		nh.param<bool>("use_camera_info", useCamInfo, false);
		camParam = aruco::CameraParameters( );
		
		// keys publisher
		pub_marker_ids = nh.advertise<std_msgs::Int32>( TOPIC_DETECTED_IDS, 10 );
		
		// open the window
		cv::namedWindow( OPENCV_WINDOW );
	}
	
	/********************************************//**
	 *  
	 * \brief class destructor
	 * 
	 * @todo see the constructor: it could be a good idea to provide the
	 * 	"user" the control on the window. 
	 * 
	 ***********************************************/
	~node_aruco_detection( ) 
	{
		// destroy the window
		cv::destroyWindow( OPENCV_WINDOW );
	}
	
	/********************************************//**
	 *  
	 * \brief working cycle of the node
	 * 
	 * This working cycle is executed in concurrency with the receiving
	 * and the detection from the cameras signals. Its main purpose is
	 * to detect when the set of IDs has changed size (i.e. when there
	 * are new IDs to send) and in that case, to publish them.
	 * 
	 * This cycle also changes the video stream each UPDATE_TIME seconds.
	 * 
	 * @attention Very important to point out that <i>the node sends only
	 * the new IDs without repetitions</i>. The node could detect the same 
	 * marker a number of times, but only one message is sent, corresponding
	 * to the first succeeded detection. This is another trick to not waste
	 * time: it is important to keep this node as much efficient as possible. 
	 * 
	 ***********************************************/
	void spin( )
	{
		// time at the beginning of the working cycle
		ros::WallTime t_update = ros::WallTime::now( ) + ros::WallDuration( UPDATE_TIME );
		
		ros::Rate r( 10 );
		while( ros::ok( ) )
		{
			r.sleep( );
			
			// ROS_INFO_STREAM( "(aruco_detection ) size-1=" << ((int)ids_to_publish.size( )) - 1 << " -- idx=" << last_id_published );
			
			// update camera
			// if( ros::Time::now( ) >= t_update )
			if( t0_greater_than_t1( ros::WallTime::now( ), t_update ) )
			{
				current_camera_id = ( current_camera_id + 1 ) % 4;
				t_update = t_update + ros::WallDuration( UPDATE_TIME );
				
				ROS_INFO_STREAM( "(aruco_detection ) switching to camera with ID=" << current_camera_id );
			}
			
			// publish a new marker id if any
			if( ((int)ids_to_publish.size( )) - 1 > last_id_published )
			{
				std_msgs::Int32 msg;
				
				// ROS_INFO_STREAM( "(aruco_detection ) time to send new IDs! size-1=" << ((int)ids_to_publish.size( )) - 1 << " -- idx=" << last_id_published );
				
				mtx_detect.lock( );
				while( ((int)ids_to_publish.size( )) - 1 > last_id_published )
				{
					ROS_INFO_STREAM( "(aruco_detection ) inside the while... size-1=" << ((int)ids_to_publish.size( )) - 1 << " -- idx=" << last_id_published );
					
					last_id_published++;
					msg.data = ids_to_publish[ last_id_published ];
					pub_marker_ids.publish( msg );
					
					ROS_INFO_STREAM( "(aruco_detection ) published ID=" << msg.data << " with idx=" << last_id_published );
				}
				mtx_detect.unlock( );
			}
		}
	}
	
	/********************************************//**
	 *  
	 * \brief subscription to the camera topic
	 * 
	 * The node has four subscription callbacks, one for each camera 
	 * image topic, each one with pretty much the same structure: read the
	 * image, try to perform the detection, try to send the output to
	 * the window, and finally wait a bit. 
	 * 
	 * In particular, the last waiting ensures that the update rate of 
	 * the camera isn't going to be not so high. See MAX_SUB_RATE, which 
	 * is the maximum frequency the subscriber can read the images. See 
	 * also the class constructor, which ggives queue size 1 to all the
	 * topics: this causes a image drop if the rate from the cameras is 
	 * too much high. 
	 * 
	 * There are other workarounds for overcome the performance issue, see 
	 * the method for the detection. The point is that <i>the arUco detection
	 * process is quite heavy to carry out</i>: if the cameras would run 
	 * the detection at each frame with a too much high frequency, the
	 * application would perform poorly. 
	 * 
	 * @param msg the message from the camera
	 * 
	 ***********************************************/
	void cbk_cam_arm( const sensor_msgs::ImageConstPtr& msg )
	{
		// get the image
		ros::Time curr_stamp = msg->header.stamp;
		cv_bridge::CvImagePtr cv_ptr;
		cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
		
		// detection
		aruco_detect( ID_CAMERA_ARM, cv_ptr->image );
		
		// output
		show_camera( ID_CAMERA_ARM, cv_ptr->image );
		
		max_sub_rate.sleep( );
	}
	
	/********************************************//**
	 *  
	 * \brief sub front camera callback
	 * 
	 * @param msg the message from the camera
	 * 
	 * @see cbk_cam_arm
	 * 
	 ***********************************************/
	void cbk_cam_front( const sensor_msgs::ImageConstPtr& msg )
	{
		// get the image
		ros::Time curr_stamp = msg->header.stamp;
		cv_bridge::CvImagePtr cv_ptr;
		cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
		
		// detection
		aruco_detect( ID_CAMERA_FRONT, cv_ptr->image );
		
		// output
		show_camera( ID_CAMERA_FRONT, cv_ptr->image );
		
		max_sub_rate.sleep( );
	}
	
	/********************************************//**
	 *  
	 * \brief sub left camera callback
	 * 
	 * @param msg the message from the camera
	 * 
	 * @see cbk_cam_arm
	 * 
	 ***********************************************/
	void cbk_cam_left( const sensor_msgs::ImageConstPtr& msg )
	{
		// get the image
		ros::Time curr_stamp = msg->header.stamp;
		cv_bridge::CvImagePtr cv_ptr;
		cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
		
		// detection
		aruco_detect( ID_CAMERA_LEFT, cv_ptr->image );
		
		// output
		show_camera( ID_CAMERA_LEFT, cv_ptr->image );
		
		max_sub_rate.sleep( );
	}
	
	/********************************************//**
	 *  
	 * \brief sub right camera callback
	 * 
	 * @param msg the message from the camera
	 * 
	 * @see cbk_cam_arm
	 * 
	 ***********************************************/
	void cbk_cam_right( const sensor_msgs::ImageConstPtr& msg )
	{
		// get the image
		ros::Time curr_stamp = msg->header.stamp;
		cv_bridge::CvImagePtr cv_ptr;
		cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
		
		// detection
		aruco_detect( ID_CAMERA_RIGHT, cv_ptr->image );
		
		// output
		show_camera( ID_CAMERA_RIGHT, cv_ptr->image );
		
		max_sub_rate.sleep( );
	}
	
	/********************************************//**
	 *  
	 * \brief perform the ArUco detection 
	 * 
	 * This function enables to use the arUco detector for detecting
	 * new markers in one camera frame. 
	 * 
	 * Since this is a very time-consuming operation, in order to not 
	 * call this function with a prohibitive frequency, a number of
	 * workarounds have been put in action. 
	 * 
	 * First of all, the function can be called by one thread each time. 
	 * There's a semaphore to be acquired when the function is called, and
	 * released after the detection has been completed. 
	 * 
	 * Second, the 'wait' of the semaphore is not a lock, but a try_lock, 
	 * meaning that if the thread can't use the callback, the functon 
	 * returns immediately and the frame dropped. The main assumption, 
	 * satisfied in most of the cases, is that the frame of the cameras 
	 * is high enough. Moreover, in this case the robot doesn't move at a
	 * high velocity, hence the approach works well. 
	 * 
	 * @param camera_id the ID of the camera requiring the detection
	 * @param image the image read from the camera stream (one frame)
	 * 
	 * @note the two assumptions are: the frequency the robot acquires
	 * 	new images is high enough; and the robot doesn't move too much 
	 * 	fast. 
	 * 
	 ***********************************************/
	void aruco_detect( int camera_id, cv::Mat& image )
	{
		if( !mtx_detect.try_lock( ) ) return;
		
		// markers detection
		std::vector<aruco::Marker> markers;
		double marker_size;
		mDetector.detect(image, markers, camParam, marker_size, false);
		
		if(markers.size() > 0)
		{
			// int prev_size = ids.size( );
			
			for(aruco::Marker& m : markers) 
				if( (ids.insert(m.id)).second )
				{
					ROS_INFO_STREAM( "(aruco_detection ) [camera id=" << camera_id << "] new ID detected : " << m.id );
					ids_to_publish.push_back( m.id );
				}
			
			/*
			if(prev_size != ids.size())
				ROS_INFO_STREAM( "(aruco_detection ) [camera id=" << camera_id << "] found markers : " << set2string(ids) );
			*/
		}
		
		mtx_detect.unlock( );
	}
	
	/********************************************//**
	 *  
	 * \brief update the image in the window
	 * 
	 * The function shows a new frame into the window. It also applies a
	 * writing pointing out which camera the user is looking currently. 
	 * 
	 * @param camera_id the ID of the camera requiring the detection
	 * @param image the image read from the camera stream (one frame)
	 * 
	 * @note if the camera_id doesn't coincide with the one set by the 
	 * 	working cycle, the function returns immediately without updating
	 * 	the window. 
	 * 
	 ***********************************************/
	void show_camera( int camera_id, cv::Mat& image )
	{
		if( camera_id != current_camera_id ) return;
		
		// name of the camera requiring the print
		std::string cam_name = "";
		switch( camera_id )
		{
		case ID_CAMERA_ARM:
			cam_name = "camera ARM";
		break;
		case ID_CAMERA_FRONT:
			cam_name = "camera FRONT";
		break;
		case ID_CAMERA_LEFT:
			cam_name = "camera LEFT";
		break;
		case ID_CAMERA_RIGHT:
			cam_name = "camera RIGHT";
		break;
		default:
			return;
		}
		
		cv::putText(image, cam_name, cv::Point(10, 25), cv::FONT_HERSHEY_TRIPLEX, 1.0, CV_RGB(0,0,0), 2);
		cv::imshow(OPENCV_WINDOW, image);
		cv::waitKey(30);
	}
	
private:
	
	/// ROS node handle
    ros::NodeHandle nh;
	
	/// control semaphore for detection
	std::mutex mtx_detect;
	
	/// control semaphore for image output
	std::mutex mtx_show;
	
	/// arUco camera parameters
	aruco::CameraParameters camParam;
	
	/// image transport object
	image_transport::ImageTransport it;
	
	/// input img subscriber : arm
	image_transport::Subscriber image_sub_arm;
	
	/// input img subscriber : front
	image_transport::Subscriber image_sub_front;
	
	/// input img subscriber : left
	image_transport::Subscriber image_sub_left;
	
	/// input img subscriber : right
	image_transport::Subscriber image_sub_right;
	
	/// arUco detector
	aruco::MarkerDetector mDetector;
	
	/// the set of the found IDs
	std::set<int> ids;
	
	/// the list of IDs to publish
	std::vector<int> ids_to_publish;
	
	/// the last index sent in the vector
	int last_id_published = -1;
	
	/// use the camera parameters
	bool useCamInfo = true;
	
	/// current camera showing the image on the window
	int current_camera_id = 0;
	
	/// marker ids publisher
	ros::Publisher pub_marker_ids;
	
	/// max rate of the subscribers
	ros::Rate max_sub_rate;
	
	/** convert a set into a string */
	std::string set2string(std::set<int>& st)
	{
		std::string str = "";
		
		for(auto& elem : st)
			str += "" + std::to_string(elem) + " ";
		
		return str;
	}
	
	/** convert a set into a dynamic array */
	std::vector<int> set2vector(std::set<int>& st)
	{
		std::vector<int> v;
		
		for(auto& elem : st) v.push_back(elem);
		
		return v;
	}
	
	/** comparison between two time instants: t0 >= t1? */
	bool t0_greater_than_t1( ros::WallTime t0, ros::WallTime t1 )
	{
		// ROS_INFO_STREAM( "(aruco_detection, DEBUG ) t0.toSec=" << t0.toSec() << " -- t1.toSec()=" << t1.toSec() );
		return ( t0.toSec() >= t1.toSec() );
	}
};



/** shudown message */
void shut_msg( int sig )
{
	ROS_INFO_STREAM( "(aruco_detection ) stopping... " );
	ros::shutdown( );
}


/** main function */
int main(int argc, char** argv)
{
	ros::init( argc, argv, NODE_NAME, ros::init_options::NoSigintHandler );
	signal( SIGINT, shut_msg );
	
	ros::AsyncSpinner spinner(6);
	spinner.start( );
	
	ROS_INFO_STREAM( "(aruco_detection ) starting... " );
	node_aruco_detection node;
	
	ROS_INFO_STREAM( "(aruco_detection ) ready " );
	node.spin( );
	
	return 0;
}
