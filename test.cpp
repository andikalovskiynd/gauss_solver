#include <gtest/gtest.h>
#include "gauss_solver.h"
#include <fstream>
#include <cstdio>
#include <string>
#include <random>

// --- SOLVER TESTS
// simple system
TEST(GaussSolver, SimpleSystem) {
    Eigen::MatrixXd A(2, 2);
    Eigen::VectorXd B(2);

    A << 2, 1,
        3, 4;
    B << 5, 6;

    Eigen::VectorXd X = solve(A, B);

    EXPECT_NEAR(X(0), 2.8, 1e-10);
    EXPECT_NEAR(X(1), -0.6, 1e-10);
}

// singular matrix
TEST(GaussSolver, SingularMatrix) {
    Eigen::MatrixXd A(2, 2);
    Eigen::VectorXd B(2);

    A << 2, 1,
        2, 1;
    B << 5, 6;

    EXPECT_THROW(Eigen::VectorXd X = solve(A, B), std::runtime_error);
}

// large matrix gives correct result
TEST(GaussSolver, LargeIsCorrect) {
    constexpr int N = 100;

    std::mt19937 gen(50);
    std::uniform_real_distribution<double> dist(-100.0, 100.0);

    Eigen::MatrixXd A(N, N);
    Eigen::VectorXd B(N);

    for (int i = 0; i < N; ++i) {
        B(i) = dist(gen);
        for (int j = 0; j < N; ++j) {
            A(i, j) = dist(gen);
        }
    }

    Eigen::VectorXd X = solve(A, B);
    Eigen::VectorXd check = A * X;

    for (int i = 0; i < N; ++i) EXPECT_NEAR(B(i), check(i), 1e-10);
}

// large matrix is reproducable and stable
TEST(GaussSolver, LargeIsReproducable) {
    constexpr int N = 100;

    std::mt19937 gen(50);
    std::uniform_real_distribution<double> dist(-100.0, 100.0);

    Eigen::MatrixXd A(N, N);
    Eigen::VectorXd B(N);

    for (int i = 0; i < N; ++i) {
        B(i) = dist(gen);
        for (int j = 0; j < N; ++j) {
            A(i, j) = dist(gen);
        }
    }

    Eigen::VectorXd X1 = solve(A, B);

    gen.seed(50);
    for (int i = 0; i < N; ++i) {
        B(i) = dist(gen);
        for (int j = 0; j < N; ++j) {
            A(i, j) = dist(gen);
        }
    }

    Eigen::VectorXd X2 = solve(A, B);

    ASSERT_EQ(X1.size(), X2.size());
    for (int i = 0; i < N; ++i) EXPECT_NEAR(X1(i), X2(i), 1e-10);
}

// --- READER TESTS
// valid csv file
TEST(Read, ValidCvsIsRead) {
    const std::string filename = "test_input.csv";
    std::ofstream file(filename);
    file << "2,1,5\n";
    file << "3,4,6\n";
    file.close();

    Eigen::MatrixXd A;
    Eigen::VectorXd B;
    read(filename, A, B);

    EXPECT_EQ(A.rows(), 2);
    EXPECT_EQ(A.cols(), 2);
    EXPECT_EQ(B.size(), 2);
    EXPECT_DOUBLE_EQ(A(0, 0), 2);
    EXPECT_DOUBLE_EQ(B(1), 6);

    std::remove(filename.c_str());
}

// non-square system
TEST(Read, NonSquareThrows) {
    const std::string filename = "test_invalid_input.csv";
    std::ofstream file(filename);
    file << "2,1,5\n"; 
    file << "3,4,6\n";
    file << "1,2,3\n";  
    file.close();

    Eigen::MatrixXd A;
    Eigen::VectorXd B;
    EXPECT_THROW(read(filename, A, B), std::runtime_error);

    std::remove(filename.c_str());
}

// --- WRITER TESTS
// write creates valid file
TEST(Write, CreatesValidFile) {
    const std::string filename = "test_output.csv";
    Eigen::VectorXd X(2);
    X << 2.8, -0.6;
    write(filename, X);

    std::ifstream file(filename);
    std::vector<double> values;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        double v = std::stod(line);
        values.push_back(v);
    }
    
    file.close();
    std::remove(filename.c_str());

    ASSERT_EQ(values.size(), 2);
    EXPECT_NEAR(values[0], 2.8, 1e-10);
    EXPECT_NEAR(values[1], -0.6, 1e-10);
}

