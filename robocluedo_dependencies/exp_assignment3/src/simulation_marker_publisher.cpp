
#include "ros/ros.h"
#include "visualization_msgs/MarkerArray.h"
#include "geometry_msgs/Point.h"

#include <vector>

int main( int argc, char* argv[] )
{
	ros::init( argc, argv, "simulation_marker_publisher" );
	ros::NodeHandle nh;
	
	// ROS_INFO_STREAM( "(marker_publisher ) " );
	
	ROS_INFO_STREAM( "(marker_publisher ) starting..." );
	
	ROS_INFO_STREAM( "(marker_publisher ) publisher " << "/visualization_marker" << " ... " );
	ros::Publisher vis_pub = nh.advertise<visualization_msgs::MarkerArray>( "/visualization_marker", 0 );
	ROS_INFO_STREAM( "(marker_publisher ) publisher " << "/visualization_marker" << " ... OK" );
	
	ROS_INFO_STREAM( "(marker_publisher ) loading markers ... " );
	visualization_msgs::MarkerArray markers;
	
	// list of markers for the problem
	std::vector<geometry_msgs::Point> wps;
	{
		geometry_msgs::Point wp; 
			wp.z = 0.0;
		
		/*wp1*/ wp.x = -4; wp.y = -3;  wps.push_back(wp);
		/*wp2*/ wp.x = -4; wp.y = 2;  wps.push_back(wp);
		/*wp3*/ wp.x = -4; wp.y = 7;  wps.push_back(wp);
		/*wp4*/ wp.x = 5; wp.y = -7;  wps.push_back(wp);
		/*wp5*/ wp.x = 5; wp.y = -3;  wps.push_back(wp);
		/*wp6*/ wp.x = 5; wp.y = 1;  wps.push_back(wp);
	}
	
	// marker definition
	visualization_msgs::Marker marker;
	marker.header.frame_id = "odom";
	marker.header.stamp = ros::Time();
	marker.id = 0;
	marker.type = visualization_msgs::Marker::SPHERE;
	marker.action = visualization_msgs::Marker::ADD;
	marker.pose.orientation.x = 0.0;
	marker.pose.orientation.y = 0.0;
	marker.pose.orientation.z = 0.0;
	marker.pose.orientation.w = 1.0;
	marker.scale.x = 0.5;
	marker.scale.y = 0.5;
	marker.scale.z = 0.5;
	marker.color.a = 1.0; 
	marker.color.r = 0.0;
	marker.color.g = 1.0;
	marker.color.b = 0.0;
	
	int id = 0;
	for( geometry_msgs::Point& wp : wps )
	{
		marker.id = id++;
		ROS_INFO_STREAM( "(marker_publisher ) registering waypoint (x=" << wp.x << ", y=" << wp.y << ")" );
		marker.pose.position = wp;
		markers.markers.push_back( marker );
	}
	
	ROS_INFO_STREAM( "(marker_publisher ) ready -- publishing markers" );
	ros::Rate r( 5 );
	while( ros::ok() )
	{
		r.sleep( );
		vis_pub.publish( markers );
		
		ros::spinOnce( );
	}
	
	return 0;
}
