# -*- python -*-

# Should include everything any consumer of Drake would ever need.
#
# Do not update this list by hand; instead, run build_components_refresh.py.
#
# When adding new components to the package, please also add the licenses for
# any new external dependencies to :external_licenses.
LIBDRAKE_COMPONENTS = [
    "//automotive/maliput/api:api",
    "//automotive/maliput/dragway:dragway",
    "//automotive/maliput/monolane:builder",
    "//automotive/maliput/monolane:lanes",
    "//automotive/maliput/monolane:loader",
    "//automotive/maliput/monolane:monolane",
    "//automotive/maliput/multilane:builder",
    "//automotive/maliput/multilane:lanes",
    "//automotive/maliput/multilane:loader",
    "//automotive/maliput/multilane:multilane",
    "//automotive/maliput/rndf:builder",
    "//automotive/maliput/rndf:lanes",
    "//automotive/maliput/rndf:loader",
    "//automotive/maliput/rndf:rndf",
    "//automotive/maliput/utility:utility",
    "//automotive:automotive_simulator",
    "//automotive:bicycle_car",
    "//automotive:box_car_vis",
    "//automotive:car_vis",
    "//automotive:car_vis_applicator",
    "//automotive:curve2",
    "//automotive:generated_translators",
    "//automotive:generated_vectors",
    "//automotive:idm_controller",
    "//automotive:idm_planner",
    "//automotive:lane_direction",
    "//automotive:maliput_railcar",
    "//automotive:mobil_planner",
    "//automotive:monolane_onramp_merge",
    "//automotive:prius_vis",
    "//automotive:pure_pursuit",
    "//automotive:pure_pursuit_controller",
    "//automotive:road_odometry",
    "//automotive:simple_car",
    "//automotive:simple_powertrain",
    "//automotive:trajectory_car",
    "//common/proto:call_matlab",
    "//common/proto:call_python",
    "//common/proto:matlab_rpc",
    "//common/proto:protobuf",
    "//common/trajectories/qp_spline:continuity_constraint",
    "//common/trajectories/qp_spline:spline_generation",
    "//common/trajectories/qp_spline:spline_information",
    "//common/trajectories/qp_spline:value_constraint",
    "//common/trajectories:piecewise_function",
    "//common/trajectories:piecewise_polynomial",
    "//common/trajectories:piecewise_polynomial_trajectory",
    "//common/trajectories:piecewise_quaternion",
    "//common/trajectories:trajectory",
    "//common:autodiff",
    "//common:autodiffxd_make_coherent",
    "//common:common",
    "//common:cond",
    "//common:copyable_unique_ptr",
    "//common:default_scalars",
    "//common:double",
    "//common:drake_path",
    "//common:dummy_value",
    "//common:essential",
    "//common:extract_double",
    "//common:find_resource",
    "//common:hash",
    "//common:is_approx_equal_abstol",
    "//common:is_cloneable",
    "//common:is_less_than_comparable",
    "//common:nice_type_name",
    "//common:number_traits",
    "//common:polynomial",
    "//common:reset_after_move",
    "//common:reset_on_copy",
    "//common:scoped_singleton",
    "//common:sorted_pair",
    "//common:sorted_vectors_have_intersection",
    "//common:symbolic",
    "//common:symbolic_decompose",
    "//common:text_logging_gflags_h",
    "//common:type_safe_index",
    "//common:unused",
    "//geometry/query_results:penetration_as_point_pair",
    "//geometry:frame_kinematics",
    "//geometry:geometry_context",
    "//geometry:geometry_frame",
    "//geometry:geometry_ids",
    "//geometry:geometry_index",
    "//geometry:geometry_instance",
    "//geometry:geometry_state",
    "//geometry:geometry_system",
    "//geometry:geometry_visualization",
    "//geometry:identifier",
    "//geometry:internal_frame",
    "//geometry:internal_geometry",
    "//geometry:proximity_engine",
    "//geometry:shape_specification",
    "//lcm:interface",
    "//lcm:lcm",
    "//lcm:lcm_log",
    "//lcm:mock",
    "//lcm:translator_base",
    "//manipulation/perception:optitrack_pose_extractor",
    "//manipulation/planner:constraint_relaxing_ik",
    "//manipulation/planner:differential_inverse_kinematics",
    "//manipulation/planner:robot_plan_interpolator",
    "//manipulation/schunk_wsg:schunk_wsg_constants",
    "//manipulation/schunk_wsg:schunk_wsg_controller",
    "//manipulation/schunk_wsg:schunk_wsg_lcm",
    "//manipulation/sensors:xtion",
    "//manipulation/util:frame_pose_tracker",
    "//manipulation/util:moving_average_filter",
    "//manipulation/util:robot_state_msg_translator",
    "//manipulation/util:sim_diagram_builder",
    "//manipulation/util:simple_tree_visualizer",
    "//manipulation/util:trajectory_utils",
    "//manipulation/util:world_sim_tree_builder",
    "//math:autodiff",
    "//math:barycentric",
    "//math:continuous_algebraic_riccati_equation",
    "//math:discrete_algebraic_riccati_equation",
    "//math:eigen_sparse_triplet",
    "//math:evenly_distributed_pts_on_sphere",
    "//math:expmap",
    "//math:geometric_transform",
    "//math:gradient",
    "//math:gray_code",
    "//math:jacobian",
    "//math:matrix_util",
    "//math:orthonormal_basis",
    "//math:quadratic_form",
    "//math:saturate",
    "//math:vector3_util",
    "//math:wrap_to",
    "//multibody/benchmarks/acrobot:acrobot",
    "//multibody/benchmarks/acrobot:make_acrobot_plant",
    "//multibody/benchmarks/free_body:free_body",
    "//multibody/benchmarks/kuka_iiwa_robot:make_kuka_iiwa_model",
    "//multibody/benchmarks/mass_damper_spring:mass_damper_spring_analytical_solution",  # noqa
    "//multibody/collision:bullet_collision",
    "//multibody/collision:collision",
    "//multibody/collision:collision_api",
    "//multibody/collision:fcl_collision",
    "//multibody/collision:model",
    "//multibody/constraint:constraint",
    "//multibody/constraint:constraint_solver",
    "//multibody/joints:joints",
    "//multibody/multibody_tree/math:rotation_matrix",
    "//multibody/multibody_tree/math:spatial_acceleration",
    "//multibody/multibody_tree/math:spatial_algebra",
    "//multibody/multibody_tree/math:spatial_force",
    "//multibody/multibody_tree/math:spatial_vector",
    "//multibody/multibody_tree/math:spatial_velocity",
    "//multibody/multibody_tree/math:transform",
    "//multibody/multibody_tree/multibody_plant:multibody_plant",
    "//multibody/multibody_tree:articulated_body_inertia",
    "//multibody/multibody_tree:multibody_tree",
    "//multibody/multibody_tree:multibody_tree_context",
    "//multibody/multibody_tree:multibody_tree_element",
    "//multibody/multibody_tree:multibody_tree_indexes",
    "//multibody/multibody_tree:multibody_tree_topology",
    "//multibody/multibody_tree:rotational_inertia",
    "//multibody/multibody_tree:spatial_inertia",
    "//multibody/multibody_tree:unit_inertia",
    "//multibody/parsers:parsers",
    "//multibody/parsing:frame_cache",
    "//multibody/parsing:sdf_parser",
    "//multibody/parsing:sdf_spec",
    "//multibody/rigid_body_plant:compliant_contact_model",
    "//multibody/rigid_body_plant:compliant_material",
    "//multibody/rigid_body_plant:contact_results",
    "//multibody/rigid_body_plant:contact_results_to_lcm",
    "//multibody/rigid_body_plant:create_load_robot_message",
    "//multibody/rigid_body_plant:drake_visualizer",
    "//multibody/rigid_body_plant:frame_visualizer",
    "//multibody/rigid_body_plant:rigid_body_plant",
    "//multibody/rigid_body_plant:rigid_body_plant_bridge",
    "//multibody/rigid_body_plant:rigid_body_plant_that_publishes_xdot",
    "//multibody/shapes:shapes",
    "//multibody:approximate_ik",
    "//multibody:global_inverse_kinematics",
    "//multibody:inverse_kinematics",
    "//multibody:kinematics_cache",
    "//multibody:kinematics_cache_helper",
    "//multibody:resolve_center_of_pressure",
    "//multibody:rigid_body",
    "//multibody:rigid_body_actuator",
    "//multibody:rigid_body_constraint",
    "//multibody:rigid_body_frame",
    "//multibody:rigid_body_loop",
    "//multibody:rigid_body_tree",
    "//multibody:rigid_body_tree_alias_groups",
    "//multibody:rigid_body_tree_alias_groups_proto",
    "//multibody:rigid_body_tree_construction",
    "//perception:point_cloud",
    "//perception:point_cloud_flags",
    "//solvers:bilinear_product_util",
    "//solvers:binding",
    "//solvers:branch_and_bound",
    "//solvers:constraint",
    "//solvers:cost",
    "//solvers:create_constraint",
    "//solvers:create_cost",
    "//solvers:decision_variable",
    "//solvers:dreal_solver",
    "//solvers:equality_constrained_qp_solver",
    "//solvers:evaluator_base",
    "//solvers:function",
    "//solvers:gurobi_qp",
    "//solvers:gurobi_solver",
    "//solvers:indeterminate",
    "//solvers:integer_optimization_util",
    "//solvers:ipopt_solver",
    "//solvers:linear_system_solver",
    "//solvers:mathematical_program",
    "//solvers:mathematical_program_api",
    "//solvers:mixed_integer_optimization_util",
    "//solvers:moby_lcp_solver",
    "//solvers:mosek_solver",
    "//solvers:nlopt_solver",
    "//solvers:non_convex_optimization_util",
    "//solvers:osqp_solver",
    "//solvers:rotation_constraint",
    "//solvers:scs_solver",
    "//solvers:snopt_solver",
    "//solvers:solver_id",
    "//solvers:solver_type",
    "//solvers:solver_type_converter",
    "//solvers:symbolic_extraction",
    "//solvers:system_identification",
    "//systems/analysis:analysis",
    "//systems/analysis:explicit_euler_integrator",
    "//systems/analysis:implicit_euler_integrator",
    "//systems/analysis:integrator_base",
    "//systems/analysis:runge_kutta2_integrator",
    "//systems/analysis:runge_kutta3_integrator",
    "//systems/analysis:semi_explicit_euler_integrator",
    "//systems/analysis:simulator",
    "//systems/controllers/plan_eval:generic_plan",
    "//systems/controllers/plan_eval:plan_eval_base_system",
    "//systems/controllers/qp_inverse_dynamics:id_controller_config",
    "//systems/controllers/qp_inverse_dynamics:lcm_utils",
    "//systems/controllers/qp_inverse_dynamics:param_parser",
    "//systems/controllers/qp_inverse_dynamics:qp_inverse_dynamics",
    "//systems/controllers/qp_inverse_dynamics:qp_inverse_dynamics_system",
    "//systems/controllers/qp_inverse_dynamics:qp_output_translator_system",
    "//systems/controllers/qp_inverse_dynamics:robot_kinematic_state",
    "//systems/controllers/qp_inverse_dynamics:robot_kinematic_state_translator_system",  # noqa
    "//systems/controllers:control_util",
    "//systems/controllers:dynamic_programming",
    "//systems/controllers:instantaneous_qp_controller",
    "//systems/controllers:inverse_dynamics",
    "//systems/controllers:inverse_dynamics_controller",
    "//systems/controllers:linear_model_predictive_controller",
    "//systems/controllers:linear_quadratic_regulator",
    "//systems/controllers:pid_controlled_system",
    "//systems/controllers:pid_controller",
    "//systems/controllers:polynomial_encode_decode",
    "//systems/controllers:qp_common",
    "//systems/controllers:setpoint",
    "//systems/controllers:side",
    "//systems/controllers:state_feedback_controller_interface",
    "//systems/controllers:zmp_planner",
    "//systems/controllers:zmp_util",
    "//systems/estimators:kalman_filter",
    "//systems/estimators:luenberger_observer",
    "//systems/framework:abstract_values",
    "//systems/framework:cache_and_dependency_tracker",
    "//systems/framework:context",
    "//systems/framework:context_base",
    "//systems/framework:continuous_state",
    "//systems/framework:diagram",
    "//systems/framework:diagram_builder",
    "//systems/framework:diagram_context",
    "//systems/framework:diagram_continuous_state",
    "//systems/framework:discrete_values",
    "//systems/framework:event_collection",
    "//systems/framework:framework",
    "//systems/framework:framework_common",
    "//systems/framework:input_port_descriptor",
    "//systems/framework:input_port_evaluator_interface",
    "//systems/framework:input_port_value",
    "//systems/framework:leaf_context",
    "//systems/framework:leaf_output_port",
    "//systems/framework:leaf_system",
    "//systems/framework:output_port_value",
    "//systems/framework:parameters",
    "//systems/framework:single_output_vector_source",
    "//systems/framework:state",
    "//systems/framework:system",
    "//systems/framework:system_constraint",
    "//systems/framework:system_scalar_converter",
    "//systems/framework:system_symbolic_inspector",
    "//systems/framework:value",
    "//systems/framework:vector",
    "//systems/framework:vector_system",
    "//systems/lcm:lcm",
    "//systems/lcm:lcm_driven_loop",
    "//systems/lcm:lcmt_drake_signal_translator",
    "//systems/lcm:translator",
    "//systems/lcm:translator_system",
    "//systems/plants/spring_mass_system:spring_mass_system",
    "//systems/primitives:adder",
    "//systems/primitives:affine_system",
    "//systems/primitives:barycentric_system",
    "//systems/primitives:constant_value_source",
    "//systems/primitives:constant_vector_source",
    "//systems/primitives:demultiplexer",
    "//systems/primitives:first_order_low_pass_filter",
    "//systems/primitives:gain",
    "//systems/primitives:integrator",
    "//systems/primitives:linear_system",
    "//systems/primitives:matrix_gain",
    "//systems/primitives:multiplexer",
    "//systems/primitives:pass_through",
    "//systems/primitives:piecewise_polynomial_affine_system",
    "//systems/primitives:piecewise_polynomial_linear_system",
    "//systems/primitives:primitives",
    "//systems/primitives:random_source",
    "//systems/primitives:saturation",
    "//systems/primitives:signal_log",
    "//systems/primitives:signal_logger",
    "//systems/primitives:sine",
    "//systems/primitives:time_varying_data",
    "//systems/primitives:trajectory_source",
    "//systems/primitives:wrap_to_system",
    "//systems/primitives:zero_order_hold",
    "//systems/rendering:drake_visualizer_client",
    "//systems/rendering:frame_velocity",
    "//systems/rendering:pose_aggregator",
    "//systems/rendering:pose_bundle",
    "//systems/rendering:pose_bundle_to_draw_message",
    "//systems/rendering:pose_stamped_t_pose_vector_translator",
    "//systems/rendering:pose_vector",
    "//systems/robotInterfaces:body_motion_data",
    "//systems/robotInterfaces:qp_locomotion_plan",
    "//systems/sensors:accelerometer",
    "//systems/sensors:beam_model",
    "//systems/sensors:beam_model_params",
    "//systems/sensors:camera_info",
    "//systems/sensors:depth_sensor",
    "//systems/sensors:depth_sensor_to_lcm_point_cloud_message",
    "//systems/sensors:gyroscope",
    "//systems/sensors:image",
    "//systems/sensors:image_to_lcm_image_array_t",
    "//systems/sensors:optitrack_encoder",
    "//systems/sensors:optitrack_sender",
    "//systems/sensors:rgbd_camera",
    "//systems/sensors:rgbd_renderer",
    "//systems/sensors:rotary_encoders",
    "//systems/sensors:sensors",
    "//systems/trajectory_optimization:direct_collocation",
    "//systems/trajectory_optimization:direct_transcription",
    "//systems/trajectory_optimization:generalized_constraint_force_evaluator",
    "//systems/trajectory_optimization:joint_limit_constraint_force_evaluator",
    "//systems/trajectory_optimization:multiple_shooting",
    "//systems/trajectory_optimization:position_constraint_force_evaluator",
    "//util:lcm_util",
    "//util:util",
]
