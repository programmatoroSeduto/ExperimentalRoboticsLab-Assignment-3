
#define NODE_NAME "aruco_detection"

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
#define SS(chr) std::string( chr )
#define SSS(el) std::to_string( el )

#include <vector>
#include <algorithm>
#include <set>
#include <mutex>
#include <signal.h>

#define UPDATE_TIME 8.0
#define MAX_SUB_RATE 8

/// window name
static const std::string OPENCV_WINDOW = "RoboCLuedo Cameras";

#define ID_CAMERA_ARM 0
#define TOPIC_CAMERA_ARM "/robocluedo_robot/camera_arm/image_raw"
#define ID_CAMERA_FRONT 1
#define TOPIC_CAMERA_FRONT "/robocluedo_robot/camera_front_low/image_raw"
#define ID_CAMERA_LEFT 2
#define TOPIC_CAMERA_LEFT "/robocluedo_robot/camera_left/image_raw"
#define ID_CAMERA_RIGHT 3
#define TOPIC_CAMERA_RIGHT "/robocluedo_robot/camera_right/image_raw"

#define TOPIC_DETECTED_IDS "/aruco_detected_ids"

class node_aruco_detection
{
public:
	
	/** node class constructor */
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
	
	/** node class destructor */
	~node_aruco_detection( ) 
	{
		// destroy the window
		cv::destroyWindow( OPENCV_WINDOW );
	}
	
	/** working cycle of the node */
	void spin( )
	{
		// time at the beginning of the working cycle
		ros::WallTime t_update = ros::WallTime::now( ) + ros::WallDuration( UPDATE_TIME );
		
		ros::Rate r( 10 );
		while( ros::ok( ) )
		{
			r.sleep( );
			
			// update camera
			// if( ros::Time::now( ) >= t_update )
			if( t0_greater_than_t1( ros::WallTime::now( ), t_update ) )
			{
				current_camera_id = ( current_camera_id + 1 ) % 4;
				t_update = t_update + ros::WallDuration( UPDATE_TIME );
				
				ROS_INFO_STREAM( "(aruco_detection ) switching to camera with ID=" << current_camera_id );
			}
			
			// publish a new marker id if any
			if( ids_to_publish.size( ) - 1 > last_id_published )
			{
				std_msgs::Int32 msg;
				
				mtx_detect.lock( );
				while( ids_to_publish.size( ) - 1 > last_id_published )
				{
					msg.data = ids_to_publish[ ++last_id_published ];
					pub_marker_ids.publish( msg );
					
					ROS_INFO_STREAM( "(aruco_detection ) published ID=" << msg.data << "with idx=" << last_id_published );
				}
				mtx_detect.unlock( );
			}
		}
	}
	
	/** sub arm camera */
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
	
	/** sub front camera */
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
	
	/** sub left camera */
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
	
	/** sub right camera */
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
	
	/** perform the ArUco detection */
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
	
	/** show the image on the screen */
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
