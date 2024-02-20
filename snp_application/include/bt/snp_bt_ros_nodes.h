#pragma once

#include <behaviortree_ros2/bt_action_node.hpp>
#include <behaviortree_ros2/bt_service_node.hpp>
#include <behaviortree_ros2/bt_topic_pub_node.hpp>
#include <behaviortree_ros2/bt_topic_sub_node.hpp>

#include <control_msgs/action/follow_joint_trajectory.hpp>
#include <industrial_reconstruction_msgs/srv/start_reconstruction.hpp>
#include <industrial_reconstruction_msgs/srv/stop_reconstruction.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <snp_msgs/srv/execute_motion_plan.hpp>
#include <snp_msgs/srv/generate_motion_plan.hpp>
#include <snp_msgs/srv/generate_scan_motion_plan.hpp>
#include <snp_msgs/srv/generate_tool_paths.hpp>
#include <std_srvs/srv/trigger.hpp>

namespace snp_application
{
static const std::string MOTION_GROUP_PARAM = "motion_group";
static const std::string REF_FRAME_PARAM = "reference_frame";
static const std::string TCP_FRAME_PARAM = "tcp_frame";
static const std::string CAMERA_FRAME_PARAM = "camera_frame";
static const std::string MESH_FILE_PARAM = "mesh_file";
static const std::string START_STATE_REPLACEMENT_TOLERANCE_PARAM = "start_state_replacement_tolerance";

template <typename T>
T get_parameter(rclcpp::Node::SharedPtr node, const std::string& key)
{
  T val;
  if (!node->get_parameter(key, val))
    throw std::runtime_error("Failed to get '" + key + "' parameter");
  return val;
}

template <typename T>
T get_parameter_or(rclcpp::Node::SharedPtr node, const std::string& key, const T& default_val)
{
  T val;
  node->get_parameter_or(key, val, default_val);
  return val;
}

template <typename T>
class SnpRosServiceNode : public BT::RosServiceNode<T>
{
public:
  using BT::RosServiceNode<T>::RosServiceNode;
  BT::NodeStatus onFailure(BT::ServiceNodeErrorCode error) override;
};

template <typename T>
class SnpRosActionNode : public BT::RosActionNode<T>
{
public:
  using BT::RosActionNode<T>::RosActionNode;
  BT::NodeStatus onFailure(BT::ActionNodeErrorCode error) override;
};

class TriggerServiceNode : public SnpRosServiceNode<std_srvs::srv::Trigger>
{
public:
  using SnpRosServiceNode<std_srvs::srv::Trigger>::providedPorts;
  using SnpRosServiceNode<std_srvs::srv::Trigger>::SnpRosServiceNode;

  bool setRequest(typename Request::SharedPtr& request) override;
  BT::NodeStatus onResponseReceived(const typename Response::SharedPtr& response) override;
};

class ExecuteMotionPlanServiceNode : public SnpRosServiceNode<snp_msgs::srv::ExecuteMotionPlan>
{
public:
  inline static std::string MOTION_PLAN_INPUT_PORT_KEY = "motion_plan";
  inline static std::string USE_TOOL_INPUT_PORT_KEY = "use_tool";
  inline static BT::PortsList providedPorts()
  {
    return providedBasicPorts({ BT::InputPort<trajectory_msgs::msg::JointTrajectory>(MOTION_PLAN_INPUT_PORT_KEY),
                                BT::InputPort<bool>(USE_TOOL_INPUT_PORT_KEY) });
  }

  using SnpRosServiceNode<snp_msgs::srv::ExecuteMotionPlan>::SnpRosServiceNode;

  bool setRequest(typename Request::SharedPtr& request) override;
  BT::NodeStatus onResponseReceived(const typename Response::SharedPtr& response) override;
};

class GenerateMotionPlanServiceNode : public SnpRosServiceNode<snp_msgs::srv::GenerateMotionPlan>
{
public:
  inline static std::string TOOL_PATHS_INPUT_PORT_KEY = "tool_paths";
  inline static std::string APPROACH_OUTPUT_PORT_KEY = "approach";
  inline static std::string PROCESS_OUTPUT_PORT_KEY = "process";
  inline static std::string DEPARTURE_OUTPUT_PORT_KEY = "departure";
  inline static BT::PortsList providedPorts()
  {
    return providedBasicPorts({ BT::InputPort<std::vector<snp_msgs::msg::ToolPath>>(TOOL_PATHS_INPUT_PORT_KEY),
                                BT::OutputPort<trajectory_msgs::msg::JointTrajectory>(APPROACH_OUTPUT_PORT_KEY),
                                BT::OutputPort<trajectory_msgs::msg::JointTrajectory>(PROCESS_OUTPUT_PORT_KEY),
                                BT::OutputPort<trajectory_msgs::msg::JointTrajectory>(DEPARTURE_OUTPUT_PORT_KEY) });
  }

  using SnpRosServiceNode<snp_msgs::srv::GenerateMotionPlan>::SnpRosServiceNode;

  bool setRequest(typename Request::SharedPtr& request) override;
  BT::NodeStatus onResponseReceived(const typename Response::SharedPtr& response) override;
};

class GenerateScanMotionPlanServiceNode : public SnpRosServiceNode<snp_msgs::srv::GenerateScanMotionPlan>
{
public:
  inline static std::string APPROACH_OUTPUT_PORT_KEY = "approach";
  inline static std::string PROCESS_OUTPUT_PORT_KEY = "process";
  inline static std::string DEPARTURE_OUTPUT_PORT_KEY = "departure";
  inline static BT::PortsList providedPorts()
  {
    return providedBasicPorts({ BT::OutputPort<trajectory_msgs::msg::JointTrajectory>(APPROACH_OUTPUT_PORT_KEY),
                                BT::OutputPort<trajectory_msgs::msg::JointTrajectory>(PROCESS_OUTPUT_PORT_KEY),
                                BT::OutputPort<trajectory_msgs::msg::JointTrajectory>(DEPARTURE_OUTPUT_PORT_KEY) });
  }

  using SnpRosServiceNode<snp_msgs::srv::GenerateScanMotionPlan>::SnpRosServiceNode;

  bool setRequest(typename Request::SharedPtr& request) override;
  BT::NodeStatus onResponseReceived(const typename Response::SharedPtr& response) override;
};

class GenerateToolPathsServiceNode : public SnpRosServiceNode<snp_msgs::srv::GenerateToolPaths>
{
public:
  inline static std::string TOOL_PATHS_OUTPUT_PORT_KEY = "tool_paths";
  inline static BT::PortsList providedPorts()
  {
    return providedBasicPorts({ BT::OutputPort<std::vector<snp_msgs::msg::ToolPath>>(TOOL_PATHS_OUTPUT_PORT_KEY) });
  }

  using SnpRosServiceNode<snp_msgs::srv::GenerateToolPaths>::SnpRosServiceNode;

  bool setRequest(typename Request::SharedPtr& request) override;
  BT::NodeStatus onResponseReceived(const typename Response::SharedPtr& response) override;
};

class StartReconstructionServiceNode
  : public SnpRosServiceNode<industrial_reconstruction_msgs::srv::StartReconstruction>
{
public:
  using SnpRosServiceNode<industrial_reconstruction_msgs::srv::StartReconstruction>::providedPorts;
  using SnpRosServiceNode<industrial_reconstruction_msgs::srv::StartReconstruction>::SnpRosServiceNode;

  bool setRequest(typename Request::SharedPtr& request) override;
  BT::NodeStatus onResponseReceived(const typename Response::SharedPtr& response) override;
};

class StopReconstructionServiceNode : public SnpRosServiceNode<industrial_reconstruction_msgs::srv::StopReconstruction>
{
public:
  using SnpRosServiceNode<industrial_reconstruction_msgs::srv::StopReconstruction>::providedPorts;
  using SnpRosServiceNode<industrial_reconstruction_msgs::srv::StopReconstruction>::SnpRosServiceNode;

  bool setRequest(typename Request::SharedPtr& request) override;
  BT::NodeStatus onResponseReceived(const typename Response::SharedPtr& response) override;
};

class ToolPathsPubNode : public BT::RosTopicPubNode<geometry_msgs::msg::PoseArray>
{
public:
  inline static std::string TOOL_PATHS_INPUT_PORT_KEY = "tool_paths";
  inline static BT::PortsList providedPorts()
  {
    return providedBasicPorts({ BT::InputPort<std::vector<snp_msgs::msg::ToolPath>>(TOOL_PATHS_INPUT_PORT_KEY) });
  }
  using BT::RosTopicPubNode<geometry_msgs::msg::PoseArray>::RosTopicPubNode;

  bool setMessage(geometry_msgs::msg::PoseArray& msg) override;
};

class MotionPlanPubNode : public BT::RosTopicPubNode<trajectory_msgs::msg::JointTrajectory>
{
public:
  inline static std::string APPROACH_INPUT_PORT_KEY = "approach";
  inline static std::string PROCESS_INPUT_PORT_KEY = "process";
  inline static std::string DEPARTURE_INPUT_PORT_KEY = "departure";
  inline static BT::PortsList providedPorts()
  {
    return providedBasicPorts({
        BT::InputPort<trajectory_msgs::msg::JointTrajectory>(APPROACH_INPUT_PORT_KEY),
        BT::InputPort<trajectory_msgs::msg::JointTrajectory>(PROCESS_INPUT_PORT_KEY),
        BT::InputPort<trajectory_msgs::msg::JointTrajectory>(DEPARTURE_INPUT_PORT_KEY),
    });
  }
  using BT::RosTopicPubNode<trajectory_msgs::msg::JointTrajectory>::RosTopicPubNode;

  bool setMessage(trajectory_msgs::msg::JointTrajectory& msg) override;
};

class FollowJointTrajectoryActionNode : public SnpRosActionNode<control_msgs::action::FollowJointTrajectory>
{
public:
  inline static std::string TRAJECTORY_INPUT_PORT_KEY = "trajectory";
  inline static BT::PortsList providedPorts()
  {
    return providedBasicPorts({ BT::InputPort<trajectory_msgs::msg::JointTrajectory>(TRAJECTORY_INPUT_PORT_KEY) });
  }
  using SnpRosActionNode<control_msgs::action::FollowJointTrajectory>::SnpRosActionNode;

  bool setGoal(Goal& goal) override;
  BT::NodeStatus onResultReceived(const WrappedResult& result) override;
};

class UpdateTrajectoryStartStateNode : public BT::RosTopicSubNode<sensor_msgs::msg::JointState>
{
public:
  inline static std::string TRAJECTORY_INPUT_PORT_KEY = "input";
  inline static std::string TRAJECTORY_OUTPUT_PORT_KEY = "output";
  inline static BT::PortsList providedPorts()
  {
    return providedBasicPorts({ BT::InputPort<trajectory_msgs::msg::JointTrajectory>(TRAJECTORY_INPUT_PORT_KEY),
                                BT::OutputPort<trajectory_msgs::msg::JointTrajectory>(TRAJECTORY_OUTPUT_PORT_KEY) });
  }
  using BT::RosTopicSubNode<sensor_msgs::msg::JointState>::RosTopicSubNode;

  BT::NodeStatus onTick(const typename sensor_msgs::msg::JointState::SharedPtr& last_msg) override;
};

/**
 * @brief Condition node for spinning the BT ROS node to keep it accessible for parameter updates, service calls, etc.
 */
class RosSpinnerNode : public BT::ConditionNode
{
public:
  inline static BT::PortsList providedPorts()
  {
    return {};
  }

  explicit RosSpinnerNode(const std::string& instance_name, const BT::NodeConfig& config, rclcpp::Node::SharedPtr node);

protected:
  BT::NodeStatus tick() override;

  rclcpp::Node::SharedPtr node_;
};

}  // namespace snp_application