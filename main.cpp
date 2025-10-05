#include <iostream>
#include "gauss_solver.h"

int main() {
    Eigen::MatrixXd A;
    Eigen::VectorXd B;
    
    std::string filename;
    std::cout << "Insert .csv filename" << std::endl;
    std::cin >> filename;

    read(filename, A, B);
    Eigen::VectorXd X = solve(A, B);
    write("result.csv", X);

    return 0;
}