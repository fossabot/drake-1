#include "drake/multibody/optimization/static_equilibrium_constraint.h"

#include "drake/multibody/inverse_kinematics/kinematic_constraint_utilities.h"

namespace drake {
namespace multibody {
namespace {
int GetLambdaSize(
    const std::map<std::pair<geometry::GeometryId, geometry::GeometryId>,
                   GeometryPairContactWrenchEvaluatorBinding>&
        contact_pair_to_wrench_evaluator) {
  int num_lambda = 0;
  for (const auto& term : contact_pair_to_wrench_evaluator) {
    num_lambda += term.second.contact_wrench_evaluator->num_lambda();
  }
  return num_lambda;
}
}  // namespace
StaticEquilibriumConstraint::StaticEquilibriumConstraint(
    const MultibodyPlant<AutoDiffXd>* plant,
    systems::Context<AutoDiffXd>* context,
    const std::map<std::pair<geometry::GeometryId, geometry::GeometryId>,
                   GeometryPairContactWrenchEvaluatorBinding>&
        contact_pair_to_wrench_evaluator)
    : solvers::Constraint(plant->num_velocities(),
                          plant->num_positions() + plant->num_actuated_dofs() +
                              GetLambdaSize(contact_pair_to_wrench_evaluator),
                          Eigen::VectorXd::Zero(plant->num_velocities()),
                          Eigen::VectorXd::Zero(plant->num_velocities())),
      plant_{plant},
      context_{context},
      contact_pair_to_wrench_evaluator_(contact_pair_to_wrench_evaluator),
      B_actuation_{plant_->MakeActuationMatrix()} {}

void StaticEquilibriumConstraint::DoEval(
    const Eigen::Ref<const Eigen::VectorXd>& x, Eigen::VectorXd* y) const {
  AutoDiffVecXd y_autodiff(num_constraints());
  DoEval(x.cast<AutoDiffXd>(), &y_autodiff);
  *y = math::autoDiffToValueMatrix(y_autodiff);
}

void StaticEquilibriumConstraint::DoEval(
    const Eigen::Ref<const AutoDiffVecXd>& x, AutoDiffVecXd* y) const {
  const auto& q = x.head(plant_->num_positions());
  const auto& u =
      x.segment(plant_->num_positions(), plant_->num_actuated_dofs());
  *y = B_actuation_ * u;
  if (internal::AreAutoDiffVecXdEqual(q, plant_->GetPositions(*context_))) {
    plant_->SetPositions(context_, q);
  }
  *y += plant_->CalcGravityGeneralizedForces(*context_);
  const auto& query_port = plant_->get_geometry_query_input_port();
  if (!query_port.HasValue(*context_)) {
    throw std::invalid_argument(
        "StaticEquilibriumConstraint: Cannot get a valid "
        "geometry::QueryObject. Please refer to AddMultibodyPlantSceneGraph "
        "on connecting MultibodyPlant to SceneGraph.");
  }
  const auto& query_object =
      query_port.Eval<geometry::QueryObject<AutoDiffXd>>(*context_);
  const std::vector<geometry::SignedDistancePair<double>>
      signed_distance_pairs =
          query_object.ComputeSignedDistancePairwiseClosestPoints();
  const geometry::SceneGraphInspector<AutoDiffXd>& inspector =
      query_object.inspector();
  const int lambda_start_index_in_x =
      plant_->num_positions() + plant_->num_actuated_dofs();
  for (const auto& signed_distance_pair : signed_distance_pairs) {
    const geometry::FrameId frame_A_id =
        inspector.GetFrameId(signed_distance_pair.id_A);
    const geometry::FrameId frame_B_id =
        inspector.GetFrameId(signed_distance_pair.id_B);
    const Frame<AutoDiffXd>& frameA =
        plant_->GetBodyFromFrameId(frame_A_id)->body_frame();
    const Frame<AutoDiffXd>& frameB =
        plant_->GetBodyFromFrameId(frame_B_id)->body_frame();

    // Compute the Jacobian.
    const Vector3<double> p_ACa =
        inspector.X_FG(signed_distance_pair.id_A) * signed_distance_pair.p_ACa;
    const Vector3<double> p_BCb =
        inspector.X_FG(signed_distance_pair.id_B) * signed_distance_pair.p_BCb;
    Eigen::Matrix<AutoDiffXd, 6, Eigen::Dynamic> Jv_V_WCa(
        6, plant_->num_velocities());
    Eigen::Matrix<AutoDiffXd, 6, Eigen::Dynamic> Jv_V_WCb(
        6, plant_->num_velocities());
    plant_->CalcJacobianSpatialVelocity(
        *context_, JacobianWrtVariable::kV, frameA, p_ACa.cast<AutoDiffXd>(),
        plant_->world_frame(), plant_->world_frame(), &Jv_V_WCa);
    plant_->CalcJacobianSpatialVelocity(
        *context_, JacobianWrtVariable::kV, frameB, p_BCb.cast<AutoDiffXd>(),
        plant_->world_frame(), plant_->world_frame(), &Jv_V_WCb);

    // Find the lambda corresponding to the geometry pair (id_A, id_B).
    const auto it = contact_pair_to_wrench_evaluator_.find(
        std::make_pair(signed_distance_pair.id_A, signed_distance_pair.id_B));
    if (it == contact_pair_to_wrench_evaluator_.end()) {
      throw std::runtime_error(
          "The input argument contact_pair_to_wrench_evaluator in the "
          "StaticEquilibriumConstraint constructor doesn't include all "
          "possible contact pairs.");
    }

    VectorX<AutoDiffXd> lambda(
        it->second.contact_wrench_evaluator->num_lambda());

    for (int i = 0; i < lambda.rows(); ++i) {
      lambda(i) = x(lambda_start_index_in_x +
                    it->second.lambda_indices_in_all_lambda[i]);
    }

    AutoDiffVecXd F_AB_W;
    it->second.contact_wrench_evaluator->Eval(
        it->second.contact_wrench_evaluator->SetVariableValues(*context_,
                                                               lambda),
        &F_AB_W);

    // By definition, F_AB_W is the contact wrench applied to id_B from id_A,
    // at the contact point. By Newton's third law, the contact wrench applied
    // to id_A from id_B at the contact point is -lambda.
    *y += Jv_V_WCa.transpose() * -F_AB_W + Jv_V_WCb.transpose() * F_AB_W;
  }
}
void StaticEquilibriumConstraint::DoEval(
    const Eigen::Ref<const VectorX<symbolic::Variable>>&,
    VectorX<symbolic::Expression>*) const {
  throw std::runtime_error(
      "StaticEquilibriumConstraint: do not support Eval with symbolic variable "
      "and expressions.");
}

solvers::Binding<StaticEquilibriumConstraint> CreateStaticEquilibriumConstraint(
    const MultibodyPlant<AutoDiffXd>* plant,
    systems::Context<AutoDiffXd>* context,
    const std::vector<std::pair<std::shared_ptr<ContactWrenchEvaluator>,
                                VectorX<symbolic::Variable>>>&
        contact_wrench_evaluators_and_lambda,
    const Eigen::Ref<const VectorX<symbolic::Variable>>& q_vars,
    const Eigen::Ref<const VectorX<symbolic::Variable>>& u_vars) {
  std::map<std::pair<geometry::GeometryId, geometry::GeometryId>,
           GeometryPairContactWrenchEvaluatorBinding>
      contact_pair_to_wrench_evaluator;
  const int num_lambda = std::accumulate(
      contact_wrench_evaluators_and_lambda.begin(),
      contact_wrench_evaluators_and_lambda.end(), 0,
      [](int a, const std::pair<std::shared_ptr<ContactWrenchEvaluator>,
                                VectorX<symbolic::Variable>>&
                    contact_wrench_evaluator_and_lambda) {
        return a + contact_wrench_evaluator_and_lambda.second.rows();
      });
  VectorX<symbolic::Variable> all_lambda(num_lambda);
  int lambda_count = 0;
  for (const auto& contact_wrench_evaluator_and_lambda :
       contact_wrench_evaluators_and_lambda) {
    const auto& contact_wrench_evaluator =
        contact_wrench_evaluator_and_lambda.first;
    const auto& lambda_i = contact_wrench_evaluator_and_lambda.second;
    DRAKE_DEMAND(contact_wrench_evaluator->num_lambda() == lambda_i.rows());
    std::vector<int> lambda_indices_in_all_lambda(lambda_i.rows());
    for (int j = 0; j < lambda_i.rows(); ++j) {
      lambda_indices_in_all_lambda[j] = lambda_count + j;
      all_lambda(lambda_count + j) = lambda_i(j);
    }
    contact_pair_to_wrench_evaluator.emplace(
        contact_wrench_evaluator->geometry_id_pair(),
        GeometryPairContactWrenchEvaluatorBinding{lambda_indices_in_all_lambda,
                                                  contact_wrench_evaluator});
    lambda_count += lambda_i.rows();
  }
  DRAKE_DEMAND(q_vars.rows() == plant->num_positions());
  DRAKE_DEMAND(u_vars.rows() == plant->num_actuated_dofs());
  VectorX<symbolic::Variable> q_u_lambda(
      plant->num_positions() + plant->num_actuated_dofs() + num_lambda);
  q_u_lambda << q_vars, u_vars, all_lambda;
  auto static_equilibrium_constraint =
      std::make_shared<StaticEquilibriumConstraint>(
          plant, context, contact_pair_to_wrench_evaluator);
  return solvers::Binding<StaticEquilibriumConstraint>(
      static_equilibrium_constraint, q_u_lambda);
}
}  // namespace multibody
}  // namespace drake
