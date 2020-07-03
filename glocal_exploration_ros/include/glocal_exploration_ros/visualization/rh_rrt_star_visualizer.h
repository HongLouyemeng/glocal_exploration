#ifndef GLOCAL_EXPLORATION_VISUALIZATION_RH_RRT_STAR_VISUALIZER_
#define GLOCAL_EXPLORATION_VISUALIZATION_RH_RRT_STAR_VISUALIZER_

#include "glocal_exploration_ros/visualization/local_planner_visualizer_base.h"
#include "glocal_exploration/planning/local_planner/rh_rrt_star.h"

namespace glocal_exploration {

class RHRRTStarVisualizer : public LocalPlannerVisualizerBase {
 public:

  RHRRTStarVisualizer(const ros::NodeHandle& nh, const std::shared_ptr<LocalPlannerBase> &planner);

  void visualize() override;

 protected:
  std::shared_ptr<RHRRTStar> planner_;
  ros::Publisher pub_;

  // params
  bool visualize_gain_;
  bool visualize_value_;

  // variables
  int num_previous_msgs_;
};

} // namespace glocal_exploration

#endif // GLOCAL_EXPLORATION_VISUALIZATION_RH_RRT_STAR_VISUALIZER_
