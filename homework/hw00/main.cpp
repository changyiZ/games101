#include<cmath>
#include<eigen3/Eigen/Core>
#include<eigen3/Eigen/Dense>
#include<iostream>

int main() {
    /* 
    * PA 0
    */
    Eigen::Vector2f p(2, 1);
    Eigen::Matrix2f rotation = Eigen::Rotation2Df(45 * M_PI / 180).toRotationMatrix();
    Eigen::Vector2f move(1, 2);
    std::cout << rotation * p + move << std::endl;
    return 0;
}