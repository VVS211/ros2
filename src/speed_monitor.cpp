#include"rclcpp/rclcpp.hpp"
#include<chrono>
#include"std_msgs/msg/int32.hpp"
using namespace std::chrono;
//此为订阅者
//子线程
void checkSpeedChange(int currentSpeed,int previousSpeed)
{
    //vector容器储存接受的数据
    static std::vector<int> change;
    //统计WARNING的次数
    static int count = 0;
    int diff = std::abs(currentSpeed - previousSpeed);//前一次速度与后一次速度的差的绝对值
    std::cout << "Speed Difference: " << diff << std::endl;
        if(diff >= 3)
        {
            std::cout << "State: WARNING" << std::endl;
            count++;
        }else
        {
            std::cout << "State: NORMAL" << std::endl;
        }
        //把这次的速度差储存到vector容器中(vector容器只储存最近4次的纪录)
        change.push_back(diff);
        if(change.size()>4)
        {
            //把最早的数据删去
            change.erase(change.begin());
        }
         if(count>=2)
        {
            std::cout << "System State: UNSTABLE" << std::endl;
        }else
        {
            std::cout << "System State: STABLE" << std::endl;
        }
        std::cout << std::endl;
}

class listener : public rclcpp::Node
{
private:
    rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr speed_subscription_;
    int memory = 0;
    bool speed_check = true;
    int previous = 0;
    void  robot_speed(const std_msgs::msg::Int32 msg)
    {
        if(speed_check)     //if只进一次
        {
            RCLCPP_INFO(this->get_logger(),"\nReceive Speed: %d\n Waiting for next command...",msg.data);
            speed_check = false;
            previous = memory;
            memory = msg.data;
            return;
        }
        int current_speed = memory;
        int previous_speed = previous;
        RCLCPP_INFO(this->get_logger(),"\nReceive Speed: %d\n Processing Speed: %d\n Previous Speed: %d",msg.data,memory,previous);
        //开启线程
        std::thread check(checkSpeedChange,current_speed,previous_speed);
        check.join();
        //更新值
        previous = memory;
        memory = msg.data;
    }
public:
    listener(const std::string node_name) : Node(node_name)
    {
        speed_subscription_ = this->create_subscription<std_msgs::msg::Int32>
        ("/robot_speed",10,std::bind(&listener::robot_speed,this,std::placeholders::_1));
    }
};
int main(int argc,char* argv[])
{
    rclcpp::init(argc,argv);
    auto node = std::make_shared<listener>("robot");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}