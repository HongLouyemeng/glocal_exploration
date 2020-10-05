#include "glocal_exploration/mapping/map_base.h"

#include <algorithm>

namespace glocal_exploration {

bool MapBase::findNearbyTraversablePoint(
    const FloatingPoint traversability_radius, Point* position) const {
  CHECK_NOTNULL(position);
  const Point initial_position = *position;

  constexpr int kMaxNumSteps = 20;

  FloatingPoint distance;
  Point gradient;
  int step_idx = 1;
  for (; step_idx < kMaxNumSteps; ++step_idx) {
    // Determine if we found a solution
    if (this->isTraversableInActiveSubmap(*position, traversability_radius)) {
      LOG(INFO) << "Skeleton planner: Succesfully moved point from "
                   "intraversable initial position ("
                << initial_position.transpose()
                << ") to traversable start point (" << position->transpose()
                << "), after " << step_idx << " gradient ascent steps.";
      return true;
    }
    // Get the distance
    if (!this->getDistanceAndGradientInActiveSubmap(*position, &distance,
                                                    &gradient)) {
      LOG(WARNING) << "Failed to look up distance and gradient "
                      "information at: "
                   << position->transpose();
      return false;
    }
    // Take a step in the direction that maximizes the distance
    const FloatingPoint step_size =
        std::max(this->getVoxelSize(), traversability_radius - distance);
    *position += step_size * gradient;
  }
  return false;
}

bool MapBase::findSafestNearbyPoint(const FloatingPoint minimum_distance,
                                    Point* position) const {
  CHECK_NOTNULL(position);
  const Point initial_position = *position;

  constexpr int kMaxNumSteps = 80;
  const FloatingPoint voxel_size = this->getVoxelSize();

  Point current_position = initial_position;
  FloatingPoint best_distance_so_far = 0.f;
  Point best_position_so_far = initial_position;
  int step_idx = 1;
  Point gradient;
  for (; step_idx < kMaxNumSteps; ++step_idx) {
    // Get the distance.
    FloatingPoint distance = 0.f;
    if (!this->getDistanceAndGradientInActiveSubmap(current_position, &distance,
                                                    &gradient) &&
        step_idx == 1) {
      LOG(WARNING) << "Failed to look up distance and gradient "
                      "information at: "
                   << current_position.transpose();
      return false;
    }

    // Save the new position if it is better, or see if it's time to terminate.
    if (best_distance_so_far < distance) {
      best_distance_so_far = distance;
      best_position_so_far = current_position;
    } else if (distance + voxel_size / 2 < best_distance_so_far ||
               step_idx == kMaxNumSteps - 1) {
      if (this->isTraversableInActiveSubmap(current_position,
                                            minimum_distance)) {
        LOG(INFO) << "Found a safe point near initial position ("
                  << initial_position.transpose() << "), at ("
                  << best_position_so_far.transpose() << ") with distance "
                  << best_distance_so_far << " after " << step_idx
                  << " gradient ascent steps.";
        *position = best_position_so_far;
        return true;
      }
    }

    // Take a tiny step in the direction that maximizes the distance.
    const FloatingPoint step_size = voxel_size;
    current_position += step_size * gradient;
  }

  LOG(INFO) << "Could not find a safer point near initial position ("
            << initial_position.transpose() << "), attempted " << step_idx
            << " gradient ascent steps.";
  return false;
}

}  // namespace glocal_exploration
