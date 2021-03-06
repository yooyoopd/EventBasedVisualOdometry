#ifndef _KF_DSI_H_
#define _KF_DSI_H_

#include <vector>
#include <math.h>
#include <iostream>

#include <cv.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "filters.h"
#include "utilities.h"

static const int confidence_map_cushion = 6;

namespace emvs{

// TODO make these params
const int gauss_filter_size = 5;
const int gauss_filter_sigma = 2;
const int median_filter_radius = 7;

static const std::string DEPTHMAP_WINDOW = "depthmap";

/*! Disparity space image for keyframe in EMVS algorithm.
	Keeps a discretized "voxel grid" of space in front of keyframe.
*/
class KeyframeDsi
{
public:
	KeyframeDsi(double im_height, double im_width, double min_depth, double max_depth,
				int N_planes, double fx, double fy);
	~KeyframeDsi();

	void resetDsi();
	PointCloud getFiltered3dPoints();

	int getPlaneDepth(const int layer);
	void addToDsi(const cv::Mat& events, const int layer);

	KeyframeDsi* clone() const;

	const int N_planes_;

private:
	double min_depth_, max_depth_; // [m]
	double im_height_, im_width_; // [pixels]
	double fx_, fy_; // [pixels]

	//filter parameters
	// int gf_size_, gf_sigma_, mf_r_, confidence_map_cushion_;

	// Mapping of size of each layer: pixels -> meters
	std::vector<std::vector<double> > planes_scaling_;
	// Depths of each layer [m]
	std::vector<double> planes_depths_;
	// Holds voxel grid values of disparity space image
	std::vector<cv::Mat> dsi_;

	// Intermediate steps for getting filtered 3d points in local frame
	cv::Mat getDepthmap();
	void projectDepthmapTo3d(cv::Mat& depthmap, PointCloud& points_camera_frame);

};

} // end namespace emvs

#endif
