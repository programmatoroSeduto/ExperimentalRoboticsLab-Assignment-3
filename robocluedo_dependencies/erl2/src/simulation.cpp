#include <ros/ros.h>
#include <gazebo_msgs/LinkStates.h>
#include <visualization_msgs/MarkerArray.h>
#include <erl2/ErlOracle.h>
#include <erl2/Oracle.h>

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>

#define NOT_TESTING true
#define DIST_THRESH 0.25

/*
#if NOT_TESTING
	#define DIST_THRESH 0.35
#else
	#define DIST_THRESH 0.5
#endif
*/

ros::Publisher oracle_pub;

double markx[4];
double marky[4];
double markz[4];

double lastmarkx = 0.0;
double lastmarky = 0.0;

const std::string key[3] = {"who", "what", "where"};
const std::string person[6] = {"missScarlett", "colonelMustard", "mrsWhite", "mrGreen", "mrsPeacock", "profPlum"};
const std::string object[6] = {"candlestick", "dagger", "leadPipe", "revolver", "rope", "spanner"};
const std::string place[9] = {"conservatory", "lounge", "kitchen", "library", "hall", "study", "bathroom", "diningRoom", "billiardRoom"}; 

int uIDs[3]={-1,-1,-1};
int winID = -1;
 
std::vector<erl2::ErlOracle> oracle_msgs;

double distfromtarget (double x, double y, double z, double x1, double y1, double z1){
	double dist = sqrt((x-x1)*(x-x1)+(y-y1)*(y-y1)+(z-z1)*(z-z1));
	return dist;
	
}

bool oracleService(erl2::Oracle::Request &req, erl2::Oracle::Response &res)
	{
		res.ID = winID;
		return true;
	}

void oracleCallback(const gazebo_msgs::LinkStates::ConstPtr& msg)
{
   for(int i=0; i< msg->name.size(); i++){
	   if (msg->name[i].find("cluedo_link")!= std::string::npos){
		   for(int j=0; j<4;j++){
				if ((distfromtarget(msg->pose[i].position.x, msg->pose[i].position.y, msg->pose[i].position.z, markx[j],marky[j],markz[j])<DIST_THRESH) && ((lastmarkx !=markx[j]) || (lastmarky != marky[j]))){
				erl2::ErlOracle oracle_msg;
				if( NOT_TESTING )
					oracle_msg.ID = rand() % 6;
				else
					oracle_msg.ID = winID;
				if((rand()%4==1) && NOT_TESTING){
					int a = rand()%5;
					if(a==0){
						oracle_msg.key = "";
						oracle_msg.value = "";
					}
					if (a==1){
						oracle_msg.key="";
						oracle_msg.value=person[rand()%6];
					}
					if (a==2){
						oracle_msg.key="";
						oracle_msg.value=object[rand()%6];
					}
					if (a==3){
						oracle_msg.key="when";
						oracle_msg.value="-1";
					}
					if (a==4){
						oracle_msg.key="who";
						oracle_msg.value="-1";
					}
				}
				else {
					if( NOT_TESTING )
					{
						oracle_msg.key = key[rand()%3];
						bool existing = false;
						for(int k=0; k<oracle_msgs.size();k++){
							if((oracle_msg.ID == oracle_msgs[k].ID)&&(oracle_msg.key == oracle_msgs[k].key)){
								oracle_msg.value = oracle_msgs[k].value;
								existing = true;	
							}
						}
						if ((!existing) || (std::find(std::begin(uIDs), std::end(uIDs), oracle_msg.ID) != std::end(uIDs))){	
							if (oracle_msg.key == "who")
								oracle_msg.value = person[rand()%6];
							if (oracle_msg.key == "what")
								oracle_msg.value = object[rand()%6];
							if (oracle_msg.key == "where")
								oracle_msg.value = place[rand()%9];
							
							oracle_msgs.push_back(oracle_msg);
						}
					}
					else
					{
						oracle_msg.key = key[rand()%3];
						
						if (oracle_msg.key == "who")
							oracle_msg.value = person[winID];
						if (oracle_msg.key == "what")
							oracle_msg.value = object[winID];
						if (oracle_msg.key == "where")
							oracle_msg.value = place[winID];
						
						oracle_msgs.push_back(oracle_msg);
					}
				}
				oracle_pub.publish(oracle_msg);
				lastmarkx = markx[j];
				lastmarky = marky[j];
		   }
		}
	  }
	}
} 

int main(int argc, char **argv)
{

ros::init(argc, argv, "assignment2");
ros::NodeHandle nh;
ros::Publisher vis_pub = nh.advertise<visualization_msgs::MarkerArray>( "/visualization_marker", 0 );
oracle_pub = nh.advertise<erl2::ErlOracle>( "/oracle_hint", 0 );
ros::ServiceServer service= nh.advertiseService("/oracle_solution", oracleService);
ros::Subscriber sub = nh.subscribe("/gazebo/link_states", 10, oracleCallback);
visualization_msgs::MarkerArray markers;
srand (time(NULL));
const double zpos[2] = {0.75, 1.25};
int RandIndex;


visualization_msgs::Marker marker;
marker.header.frame_id = "odom";
marker.header.stamp = ros::Time();
marker.id = 0;
marker.type = visualization_msgs::Marker::SPHERE;
marker.action = visualization_msgs::Marker::ADD;
marker.pose.position.x = -3.0;
markx[0]=-3.0;
marker.pose.position.y = 0.0;
marky[0]=0.0;
RandIndex = rand() % 2;
marker.pose.position.z = zpos[RandIndex];
markz[0]=marker.pose.position.z;
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
markers.markers.push_back(marker);

marker.id = 1;
marker.pose.position.x = 3.0;
markx[1]=3.0;
marker.pose.position.y = 0.0;
marky[1]=0.0;
RandIndex = rand() % 2;
marker.pose.position.z = zpos[RandIndex];
markz[1]=marker.pose.position.z;
markers.markers.push_back(marker);

marker.id = 2;
marker.pose.position.x = 0.0;
markx[2]=0.0;
marker.pose.position.y = -3.0;
marky[2]=-3.0;
RandIndex = rand() % 2;
marker.pose.position.z = zpos[RandIndex];
markz[2]=marker.pose.position.z;
markers.markers.push_back(marker);

marker.id = 3;
marker.pose.position.x = 0.0;
markx[3]=0.0;
marker.pose.position.y = 3.0;
marky[3]=3.0;
RandIndex = rand() % 2;
marker.pose.position.z = zpos[RandIndex];
markz[3]=marker.pose.position.z;
markers.markers.push_back(marker);

int uid;
for (int i = 0; i < 4; i++){	
	do{
		uid = rand()%6;
		for( int i = 0; i < 3; i++ ){
			if(uid == uIDs[i] ){
					uid = -1;
					break;
				}
			}
		}while(uid == -1);
		
	if(i==3){
		winID = uid;
	}
	else{
    uIDs[i] = uid;
	}
}




while (ros::ok()){
	vis_pub.publish(markers);
	ros::spinOnce();
}

ros::shutdown();

return 0;
}
