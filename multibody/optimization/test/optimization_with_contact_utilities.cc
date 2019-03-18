#include "drake/multibody/optimization/test/optimization_with_contact_utilities.h"

#include <utility>

namespace drake {
namespace multibody {
template <typename T>
FreeSpheresAndBoxes<T>::FreeSpheresAndBoxes(
    std::vector<SphereSpecification> spheres,
    std::vector<BoxSpecification> boxes,
    CoulombFriction<double> ground_friction)
    : spheres_{std::move(spheres)},
      boxes_{std::move(boxes)},
      ground_friction_{std::move(ground_friction)} {
  const int num_spheres = static_cast<int>(spheres_.size());
  const int num_boxes = static_cast<int>(boxes_.size());
  systems::DiagramBuilder<T> builder;
  std::tie(plant_, scene_graph_) = AddMultibodyPlantSceneGraph(&builder);
  for (int i = 0; i < num_spheres; ++i) {
    const auto& sphere =
        plant_->AddRigidBody("sphere" + std::to_string(i), spheres_[i].inertia);
    sphere_geometry_ids_.push_back(plant_->RegisterCollisionGeometry(
        sphere, Eigen::Isometry3d::Identity(),
        geometry::Sphere(spheres_[i].radius),
        "sphere" + std::to_string(i) + "_collision", spheres_[i].friction,
        scene_graph_));
  }
  for (int i = 0; i < num_boxes; ++i) {
    const auto& box =
        plant_->AddRigidBody("box" + std::to_string(i), boxes_[i].inertia);
    box_geometry_ids_.push_back(plant_->RegisterCollisionGeometry(
        box, Eigen::Isometry3d::Identity(),
        geometry::Box(boxes_[i].size(0), boxes_[i].size(1), boxes_[i].size(2)),
        "box" + std::to_string(i) + "_collision", boxes_[i].friction,
        scene_graph_));
  }
  // Add the ground
  const auto& ground = plant_->AddRigidBody(
      "ground", SpatialInertia<double>(1, Eigen::Vector3d::Zero(),
                                       UnitInertia<double>(1, 1, 1)));
  const Eigen::Vector3d ground_box_size(100, 100, 100);
  ground_geometry_id_ = plant_->RegisterCollisionGeometry(
      ground, Eigen::Isometry3d::Identity(),
      geometry::Box(ground_box_size(0), ground_box_size(1), ground_box_size(2)),
      "ground", ground_friction_, scene_graph_);
  Eigen::Isometry3d X_WG = Eigen::Isometry3d::Identity();
  X_WG.translation()(2) = -ground_box_size(2) / 2;
  plant_->WeldFrames(plant_->world_frame(), ground.body_frame(), X_WG);

  plant_->Finalize();
  diagram_ = builder.Build();

  diagram_context_ = diagram_->CreateDefaultContext();
  plant_context_ =
      &(diagram_->GetMutableSubsystemContext(*plant_, diagram_context_.get()));
}

template class FreeSpheresAndBoxes<double>;
template class FreeSpheresAndBoxes<AutoDiffXd>;
}  // namespace multibody
}  // namespace drake
