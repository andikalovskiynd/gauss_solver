#pragma once

#include <Eigen/Dense>
#include <string>

void read(const std::string& filename, Eigen::MatrixXd& A, Eigen::VectorXd& B);
void write(const std::string& filename, const Eigen::VectorXd& X);
Eigen::VectorXd solve(Eigen::MatrixXd A, Eigen::VectorXd B);