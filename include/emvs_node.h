#ifndef _EMVS_NODE_H_
#define _EMVS_NODE_H_

#include <queue>
#include <thread>
#include <cmath>
#include <algorithm>
#include <mutex>
#include <thread>
#include <Eigen/Core>

#include "keyframe_dsi.h"
#include "filters.h"
#include "utilities.h"

// ROS
#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <sensor_msgs/CameraInfo.h>
#include <dvs_msgs/Event.h>
#include <dvs_msgs/EventArray.h>
#include <tf2_ros/transform_broadcaster.h>
#include <geometry_msgs/TransformStamped.h>

//OpenCV
#include <cv.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/core/eigen.hpp>

// PCL
#include <sensor_msgs/PointCloud2.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/common/transforms.h>


namespace emvs{

using cv::Mat;
using cv::Mat_;

// TODO read camera info from topic instead
const int sensor_rows = 180;
const int sensor_cols = 240;
const cv::Mat D_camera = (cv::Mat_<double>(5,1) << -0.3684363117977873, 0.1509472435566583, -0.0002961305343848646, -0.000759431726241032, 0.0);
const cv::Mat K_camera = (cv::Mat_<double>(3,3) << 199.0923665423112, 0.0, 132.1920713777002, 0.0, 198.8288204700886, 110.7126600112956, 0.0, 0.0, 1.0);

// TODO make these params 
const double min_depth = 0.3;
const double max_depth = 3.0;
const double N_planes = 50;
const double fx = 199.0923665423112;
const double fy = 198.8288204700886;

static const std::string OPENCV_WINDOW = "Set of events";


/*! Implementation of event-based multi-view stereo algorithm.
 	Inputs: events and pose estimates
 	Outputs: pointcloud
*/
class EmvsNode
{
public:
	EmvsNode();
	~EmvsNode();

private:
	ros::NodeHandle nh_;
	ros::Subscriber events_sub_;
	ros::Subscriber camera_info_sub_;
	ros::Subscriber ground_truth_sub_;
	ros::Subscriber imu_sub_;

	tf2_ros::TransformBroadcaster tf_br_;

	ros::Publisher pointcloud_pub_;
	PointCloud map_points_;

	// sensor_msgs::CameraInfo camera_info_;
	// bool camera_info_received_;

	bool first_;

	KeyframeDsi kf_dsi_;
	bool events_updated_;

	cv::Mat new_events_;
	std::queue<geometry_msgs::PoseStamped> received_poses_;

	static constexpr double new_kf_dist_thres_ = 0.1; // TODO make this dependent on average scene depth
	Eigen::Vector3d kf_pos_;
	Eigen::Vector4d kf_quat_;

	//filter params
	// double min_depth_, max_depth_;
	// int N_planes_;
	// int gf_size_, gf_sigma_, mf_r_, confidence_map_cushion_;

	// Worker threads
	std::mutex dsi_mutex_;
	std::thread events_to_dsi_th_;
	std::queue<std::pair<Mat, geometry_msgs::PoseStamped> > events_to_dsi_queue_;
	void process_events_to_dsi();

	std::thread dsi_to_map_th_;
	std::queue<std::pair<std::shared_ptr<KeyframeDsi>, geometry_msgs::PoseStamped > > dsi_to_map_queue_;
	void process_dsi_to_map();

	void eventCallback(const dvs_msgs::EventArray& msg);
	void poseCallback(const geometry_msgs::PoseStamped& msg);
	// void camerainfoCallback(const sensor_msgs::CameraInfo& msg);

	Mat undistortImage(const Mat input_image);

	bool checkForNewKeyframe(const geometry_msgs::PoseStamped& pose);
	void addEventsToDsi(const Mat& events, const geometry_msgs::PoseStamped& cam_pose);
	void addDsiToMap(KeyframeDsi& kf_dsi, geometry_msgs::PoseStamped& kf_pose);

};


} // end namespace emvs

#endif
