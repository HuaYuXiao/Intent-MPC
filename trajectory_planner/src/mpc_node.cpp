/*
*	File: mpc_node.cpp
*	---------------
*   MPC trajectory planning node
*/

#include <ros/ros.h>
#include <trajectory_planner/mpcPlanner.h>
#include <trajectory_planner/polyTrajOccMap.h>
#include <trajectory_planner/obstacle_test/dynamicObstacleGenerator.h>
#include <visualization_msgs/MarkerArray.h>
ros::Publisher waypointsVisPub;
ros::Publisher inputTrajPub;
nav_msgs::Path inputTraj;
std::vector<std::vector<double>> waypoints;
bool newWaypoints = true;
bool newMsg = false;
std::vector<double> newPoint {0, 0, 1.0, 0};
void clickedPointCB(const geometry_msgs::PoseStamped::ConstPtr& cp){
	newPoint[0] = cp->pose.position.x;
	newPoint[1] = cp->pose.position.y;
	newPoint[2] = 1.0; // set height to be 1.0 m
	newPoint[3] = trajPlanner::rpy_from_quaternion(cp->pose.orientation);
	newMsg = true;
}

visualization_msgs::MarkerArray waypoints2vis(const std::vector<std::vector<double>>& waypoints){
	visualization_msgs::MarkerArray wpMsg;
	int id = 0;
	for (int i=0; i<int(waypoints.size()); ++i){
		visualization_msgs::Marker wp;
		wp.header.frame_id = "map";
		wp.header.stamp = ros::Time();
		wp.ns = "waypoints";
		wp.id = id;
		wp.type = visualization_msgs::Marker::SPHERE;
		wp.action = visualization_msgs::Marker::ADD;
		wp.pose.position.x = waypoints[i][0];
		wp.pose.position.y = waypoints[i][1];
		wp.pose.position.z = waypoints[i][2];
		wp.lifetime = ros::Duration(0.5);
		wp.scale.x = 0.4;
		wp.scale.y = 0.4;
		wp.scale.z = 0.4;
		wp.color.a = 0.7;
		wp.color.r = 1.0;
		wp.color.g = 0.5;
		wp.color.b = 1.0;		
		++id;
		wpMsg.markers.push_back(wp);
	}
	return wpMsg;
}

void visPub(){
	ros::Rate r(10);
	while (ros::ok()){
		if (waypoints.size() != 0){
			visualization_msgs::MarkerArray wpMsg = waypoints2vis(waypoints);
			waypointsVisPub.publish(wpMsg);
		}
		if (inputTraj.poses.size() != 0){
			inputTrajPub.publish(inputTraj);
		}
		r.sleep();
	}	
}

bool goalHasCollision(std::shared_ptr<mapManager::occMap> map, const std::vector<double>& goal){
	Eigen::Vector3d p;
	double r = 0.5;//radius for goal collision check
	for (double i=-r; i<=r;i+=0.1){
		for(double j=-r;j<=r;j+=0.1){
			for (double k = -r; k<=r; k+=0.1){
				p(0) = goal[0]+i;
				p(1) = goal[1]+j;
				p(2) = goal[2]+k;
				if (map->isInflatedOccupied(p)){
					return true;
				}
			}
		}
	}
	return false;
}

bool hasCollision(std::shared_ptr<trajPlanner::mpcPlanner> mp,std::shared_ptr<mapManager::occMap> map, ros::Time trajStartTime){
	double dt = mp->getTs();
	ros::Time currTime = ros::Time::now();
	double startTime = std::min((currTime-trajStartTime).toSec(),mp->getHorizon()*dt-2.0);
	double endTime = std::min(startTime+2.0, mp->getHorizon()*dt);
	// cout<<"collision check: start time  "<<startTime<<"end time: "<< endTime<<endl;
	for (double t = startTime; t<=endTime; t+=dt){
		Eigen::Vector3d p = mp->getPos(t);
		bool hasCollision = map->isInflatedOccupied(p);
		if (hasCollision){
			cout<<"Collision detected !"<<endl;
			return true;
		}
	}
	return false;
}

int main(int argc, char** argv){
	ros::init(argc, argv, "mpc_navigation_node");
	ros::NodeHandle nh;
	ros::Subscriber clickedPointSub = nh.subscribe("/move_base_simple/goal", 1000, clickedPointCB);
	waypointsVisPub = nh.advertise<visualization_msgs::MarkerArray>("/waypoints", 1000);
	inputTrajPub = nh.advertise<nav_msgs::Path>("/input_traj", 1000);

	double desiredVel = 1.0;
	double desiredAcc = 1.0;

	std::shared_ptr<mapManager::occMap> map;
	map.reset(new mapManager::occMap (nh));

	std::shared_ptr<trajPlanner::polyTrajOccMap> pt;
	pt.reset(new trajPlanner::polyTrajOccMap (nh));
	pt->setMap(map);
	pt->updateDesiredVel(desiredVel);
	pt->updateDesiredAcc(desiredAcc);

	std::shared_ptr<trajPlanner::mpcPlanner> mp;
	mp.reset(new trajPlanner::mpcPlanner (nh));
	mp->updateMaxVel(desiredVel);
	mp->updateMaxAcc(desiredAcc);
	mp->setMap(map);

	std::shared_ptr<trajPlanner::obstacleGenerator> obg;
	obg.reset(new trajPlanner::obstacleGenerator (nh));

	std::thread visWorker = std::thread(visPub);

	int countLoop = 0;
	ros::Rate r(10);
	while (ros::ok()){
		cout << "----------------------------------------------------" << endl;
		cout << "[Test MPC Node]: Request No. " << countLoop+1 << endl;
		cout << "[Test MPC Node]: Wait for waypoints..." << endl;
		int countPoints = 0;
		bool lastPoint = false;
		while (ros::ok() and not lastPoint){
			std::vector<double> currPose;
			while (ros::ok() ){
				if (newMsg){
					if (newWaypoints){
						waypoints.clear();
						newWaypoints = false;
					}
					currPose = newPoint;
					newMsg = false;
					if (currPose[3] != 0){
						if (waypoints.size() > 1){
							lastPoint = true;
							newWaypoints = true;
							cout << "[Test MPC Node]: received last point. Waypoints size: " << waypoints.size() << endl;
							break;
						}
					}
					else{
						cout << "[Test MPC Node]: current point" << " i=" << countPoints << " (" << currPose[0] << " " << currPose[1] << " " << currPose[2] << " " << currPose[3] << ")" << endl;
					}
					if (not goalHasCollision(map,currPose)){
						waypoints.push_back(currPose);
						++countPoints;
					}
					else{
						cout<<" [Test MPC Node]: invalid, please select new waypoint."<<endl;
					}

					break;
				}
				ros::spinOnce();
				r.sleep();
			}
		}

		nav_msgs::Path waypointInputMsg;
		for (int i=0; i<int(waypoints.size()); ++i){
			geometry_msgs::PoseStamped ps;
			ps.pose.position.x = waypoints[i][0];
			ps.pose.position.y = waypoints[i][1];
			ps.pose.position.z = waypoints[i][2];
			waypointInputMsg.poses.push_back(ps);
		}

		Eigen::Vector3d startVel (0, 0, 0);
		Eigen::Vector3d startAcc (0, 0, 0);
		Eigen::Vector3d endVel (0, 0, 0);
		Eigen::Vector3d endAcc (0, 0, 0);
		std::vector<Eigen::Vector3d> startEndConditions {startVel, startAcc, endVel, endAcc};

		
		pt->updatePath(waypointInputMsg, startEndConditions);
		inputTraj.poses.clear();
		pt->makePlan(inputTraj);

		double dt = 0.1;
		nav_msgs::Path mpcInputTraj = pt->getTrajectory(dt);

		// MPC part
		mp->updatePath(mpcInputTraj, dt);
		Eigen::Vector3d currPos (waypoints[0][0], waypoints[0][1], waypoints[0][2]);
		Eigen::Vector3d currVel (0.0, 0.0, 0.0);
		Eigen::Vector3d goalPos (waypoints.back()[0], waypoints.back()[1], waypoints.back()[2]);

		double t = 0;
		bool firstTime = true;
		ros::Time trajStartTime;
		while (ros::ok() and ((currPos - goalPos).norm() >= 0.2 or t <= 1.0)){
			ros::Time mpcStartTime = ros::Time::now();
			mp->updateDynamicObstacles(obg->getObstaclePos(), obg->getObstacleVel(), obg->getObstacleSize());
			mp->updateCurrStates(currPos, currVel);
			
			// std::mutex m;
			// std::condition_variable cv;
			bool planSuccess;
			// planSuccess = mp->makePlanCG();
			planSuccess = mp->makePlan();
			ros::Time mpcEndTime = ros::Time::now();
			cout << "[Test MPC Node]: MPC runtime [s]: " << (mpcEndTime - mpcStartTime).toSec() << "\t\r" << std::flush;;
			if (planSuccess){
				trajStartTime = mpcStartTime;
				currPos = mp->getPos(dt);
				currVel = mp->getVel(dt);	
				firstTime = false;
			}
			// else if (not firstTime and not hasCollision(mp,map,trajStartTime)){
			// 	double time = (ros::Time::now()-trajStartTime).toSec();
			// 	currPos = mp->getPos(time);
			// 	currVel = mp->getVel(time);	
			// }
			t += dt;
			
			ros::spinOnce();
			r.sleep();
		}
		cout << "[Test MPC Node]: Complete Current Trajectory." << endl;
		++countLoop;
	}




	return 0;
}