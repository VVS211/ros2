#include"rclcpp/rclcpp.hpp"
#include<chrono>
#include<vector>
#include<queue>
#include"std_msgs/msg/int32.hpp"
using namespace std::chrono;
//此为发布者
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
class Robot_Speed : public rclcpp::Node
{
private:
    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr speed_publisher;
    rclcpp::TimerBase::SharedPtr timer_;
    //定义一个索引，记录发到第几个
    size_t index_;
    //定义vector数组
    std::vector<int> data_;
    //定义queue容器储存速度
    std::queue<int> speed;
    //用来做previous speed
    int memory = 0;
    void robot_speed()
    {
        //把当前索引对应的数字放进消息里
        auto msg = std_msgs::msg::Int32();
        msg.data = data_[index_];
        //发布消息
        speed_publisher->publish(msg);
        //索引递增并取模
        index_ = (index_ + 1) % data_.size();
        //判断queue容器是否为空
        if(speed.empty())
        {
            //向queue容器中添加数据
            speed.push(msg.data);
            // std::thread check(checkSpeedChange,speed.back(),0);
            // check.join();
            std::cout << "Receive Speed: " << msg.data << std::endl;
            std::cout << "Waiting for next command..." << std::endl;
            return;
        }else
        {
            speed.push(msg.data);
            //用新的变量储存数据，防止线程的bug
            int current_speed = speed.front();
            int previous_speed = memory;
            //打印日志
            RCLCPP_INFO(this->get_logger(),"\nReceive Speed: %d\n Processing Speed: %d\n Previous Speed: %d",speed.back(),speed.front(),memory);
            //开启线程
            std::thread check(checkSpeedChange,current_speed,previous_speed);
            check.join();
            memory = speed.front();
            //删去旧数据
            speed.pop();
        }
    }
public:
    Robot_Speed(const std::string node_name) : Node(node_name)
    {
        data_ = {2,5,4,8,3,6};
        speed_publisher = this->create_publisher<std_msgs::msg::Int32>("/robot_speed",10);
        timer_ = this->create_wall_timer(1s,std::bind(&Robot_Speed::robot_speed,this));
    }
};

int main(int argc,char* argv[])
{
    rclcpp::init(argc,argv);
    auto node = std::make_shared<Robot_Speed>("robot");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}