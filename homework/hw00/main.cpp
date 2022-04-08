#include<cmath>
#include<eigen3/Eigen/Core>
#include<eigen3/Eigen/Dense>
#include<iostream>

int main() {
    /* 
    * PA 0
    */
    Eigen::Vector2f p(2, 1);
    Eigen::Matrix2f rotation = Eigen::Rotation2Df(45.0 / 180 * M_PI).toRotationMatrix();
    std::cout << rotation * p << std::endl;
    return 0;
}