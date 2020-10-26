#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if(!client.call(srv)) {
        ROS_ERROR("Fail to call service drive_bot");
    }
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    int left = int(img.step/3);
    int right = left*2;

    for(int i=0; i<img.height; i++) {
        for(int j=0; j<img.step; j+=3) {
            int pixel = i*img.step + j;
            if ((img.data[pixel]==white_pixel) 
                && (img.data[pixel+1]==white_pixel) 
                && (img.data[pixel+2]==white_pixel)) {
                
                if(j < left) {
                    drive_robot(0.0, 0.5);
                    return;
                } else if(j > right) {
                    drive_robot(0.0, -0.5);
                    return;
                } else {
                    drive_robot(0.5, 0.0);
                    return;
                }
            }
        }
    }
    drive_robot(0.0, 0.0);
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);
    ROS_INFO("Ready to read Image");

    // Handle ROS communication events
    ros::spin();

    return 0;
}
