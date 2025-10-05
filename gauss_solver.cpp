#include "gauss_solver.h"
#include <iostream>
#include <fstream> 
#include <sstream>
#include <vector>
#include <stdexcept>

void read(const std::string& filename, Eigen::MatrixXd& A, Eigen::VectorXd& B) {
    std::ifstream file(filename);
    if (!file.is_open()) throw std::runtime_error("COULD NOT OPEN A FILE!");
    
    std::vector<std::vector<double>> rows;
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::vector<double> row;
        std::stringstream ss(line);
        std::string item;

        while (std::getline(ss, item, ',')) {
            try {
                double value = std::stod(item);
                row.push_back(value);
            }
            catch (...) {
                throw std::runtime_error("INVALID CSV NUMBER: " + item);
            }
        }

        if (row.size() < 2) throw std::runtime_error("INVALID ROW!");

        rows.push_back(row);
    }

    if (rows.empty()) throw std::runtime_error("EMPTY CSV FILE!");

    size_t n_cols = rows[0].size();
    for (const auto& row : rows) {
        if (row.size() != n_cols) throw std::runtime_error("INCONSISTENT NUMBER OF COLUMNS!");
    }

    size_t n = rows.size();
    size_t m = n_cols - 1;

    if (n != m) throw std::runtime_error("NON-SQUARE SYSTEM!");

    A = Eigen::MatrixXd(n, m);
    B = Eigen::VectorXd(n);

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            A(i, j) = rows[i][j];
        }
        B[i] = rows[i][m];
    }
}

void write(const std::string& filename, const Eigen::VectorXd& X) {
    std::ofstream file(filename);
    if (!file.is_open()) throw std::runtime_error("COULD NOT OPEN WRITING FILE!");

    for (size_t i = 0; i < X.size(); ++i) {
        file << X(i) << "\n";
    }

    std::cout << "Result written to " << filename << std::endl;
}

Eigen::VectorXd solve(Eigen::MatrixXd A, Eigen::VectorXd B) {
    int n = A.rows();

    // --- (1) Convert a matrix to an upper triangular form
    for (int k = 0; k < n; ++k) {

        // 1) search for key element in current column
        int key = k;
        for (int i = k + 1; i < n; ++i) {
            if (std::abs(A(i, k)) > std::abs(A(key, k))) key = i;
        }

        /* 2) due to the determinant decomposition theorem, every key element is a part of this decomposition
        which is represented as product of these keys, so if any key is == 0, then determinant is == 0 too */ 
        if (A(key, k) == 0) throw std::runtime_error("MATRIX IS SINGULAR!");

        /* 3) swap rows if needed
        also we get rid of division by zero in step 4 */
        if (key != k) {
            A.row(k).swap(A.row(key));
            std::swap(B(k), B(key));
        }

        // 4) zero elements under current key element A(k,k)
        for (int i = k + 1; i < n; ++i) {
            double factor =  A(i, k) / A(k, k);

            // A(i,k) → 0, and other are being updated
            A.row(i).segment(k, n - k) -= factor * A.row(k).segment(k, n-k);
            B(i) -= factor * B(k); // don't forget a B vector
        }
    }

    // --- (2) Going backwards: solving the Ax = B from last row and going up
    Eigen::VectorXd X(n);

    for (int i = n - 1; i >= 0; --i) {
        double sum = A.row(i).segment(i + 1, n - i -1).dot(X.segment(i + 1, n - i - 1));
        X(i) = (B(i) - sum) / A(i, i);
    }

    return X;
}