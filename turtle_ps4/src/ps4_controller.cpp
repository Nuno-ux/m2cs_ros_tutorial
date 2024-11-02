#include "rclcpp/rclcpp.hpp"
#include "turtlesim/srv/set_pen.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "m2_interfaces/msg/joy_data.hpp"
#include "std_srvs/srv/empty.hpp"
// Hints: some includes for the service are missing

class Ps4ControllerNode : public rclcpp::Node {
public:
    Ps4ControllerNode()
        : Node("ps4_controller_node")
    {
        // 1. create a subscriber here for subscribing PS4 data...
        sub_ = this->create_subscription<m2_interfaces::msg::JoyData>("/input/joy_data", 10,
            [this](const m2_interfaces::msg::JoyData::SharedPtr msg) { ps4_callback(*msg); });

        // 2. create a publisher here for publishing turtle's velocity...
        pub_ = this->create_publisher<geometry_msgs::msg::Twist>("turtle1/cmd_vel", 10);

        // 3. create a service client here for clear background...
        client_clear_ = this->create_client<std_srvs::srv::Empty>("/clear");

        // 4. create a service client here for change pen color...
        client_set_pen_ =  this->create_client<turtlesim::srv::SetPen>("/turtle1/set_pen");
    }

private:
    void ps4_callback(const m2_interfaces::msg::JoyData& data)
    {
        // --- Requirement 1. Basic driving ---
        geometry_msgs::msg::Twist twist;
        
        // TODO: 1.1 change level of "aggressiveness"
        if ((data.dpad_y == 1) && (old_data_.dpad_y != 1) && (aggressiveness_ < 5)) {
            aggressiveness_ += 1;
        }
        if ((data.dpad_y == -1) && (old_data_.dpad_y != -1) && (aggressiveness_ > 1)){
            aggressiveness_ -= 1;
        }
        // TODO: 1.2 calculate output velocity from input ps4 data, and publish it
        twist.linear.x = aggressiveness * data.hat_ly;
        twist.angular.z = aggressiveness * data.hat_rx;
        pub_->publish(twist);
        // --- Requirement 2. Clear background ---
        // TODO: detect a 'ps' button being pressed, and call corresponding service
        // Hint: to detect a button being pressed, you can the following logic (this is not the real code):
        //
        //      if ((data.button is pressed) and (old_data_.button not pressed)),
        //          then do sth...
        if ((data.ps == 1) && (old_data_.ps == 0)) {
            auto request = std::make_shared<std_srvs::srv::Empty::Request>();
            client_clear_->async_send_request(request);
        }



        // --- Requirement 3. Change pen color ---
        // TODO: detect buttons being pressed, and call corresponding service
        // Hint: to detect a button being pressed, you can the following logic (this is not the real code):
        //
        //      if ((data.button is pressed) and (old_data_.button not pressed)),
        //          then do sth...
        if ((data.cross == 1) && (old_data_.cross == 0)) {
            auto request = std::make_shared<turtlesim::srv::SetPen::Request>();
            request->r = 0;
            request->g = 0;
            request->b = 255;
            request->width = 5;
            client_set_pen_->async_send_request(request);
        }
         if ((data.circle == 1) && (old_data_.circle == 0)) {
            auto request = std::make_shared<turtlesim::srv::SetPen::Request>();
            request->r = 255;
            request->g = 0;
            request->b = 0;
            request->width = 5;
            client_set_pen_->async_send_request(request);
        }
        if ((data.triangle == 1) && (old_data_.triangle == 0)) {
            auto request = std::make_shared<turtlesim::srv::SetPen::Request>();
            request->r = 0;
            request->g = 255;
            request->b = 0;
            request->width = 5;
            client_set_pen_->async_send_request(request);
        }
        if ((data.square == 1) && (old_data_.square == 0)) {
            auto request = std::make_shared<turtlesim::srv::SetPen::Request>();
            request->r = 255;
            request->g = 0;
            request->b = 255;
            request->width = 5;
            client_set_pen_->async_send_request(request);
        }


        // update old_data_ (You don't need to change this code)
        old_data_ = data;
    }

private:
    m2_interfaces::msg::JoyData old_data_;
    rclcpp::Subscription<m2_interfaces::msg::JoyData>::SharedPtr sub_;
    // TODO: add a publisher here for publishing turtle's velocity...
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;
    rclcpp::Client<std_srvs::srv::Empty>::SharedPtr client_clear_;
    // TODO: add a service client here for changing turtle's pen color...
    rclcpp::Client<turtlesim::srv::SetPen>::SharedPtr client_set_pen_;
    float aggressiveness_ = 1.0;
};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Ps4ControllerNode>());
    rclcpp::shutdown();
    return 0;
}
