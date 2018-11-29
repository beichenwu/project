//
// Created by wu on 11/17/18.
//

#ifndef A4_ECE650_VSOVER_H
#define A4_ECE650_VSOVER_H

#endif //A4_ECE650_VSOVER_H
#pragma once

#include <minisat/core/Solver.h>

#include <vector>

using vertex_cover = std::vector<int>;

class Solver {
private:
    const bool m_write_dimacs = false;
    Minisat::Solver solver;
    int nmax;
    int kmax;

public:
    Solver(int n, int k, std::vector<std::vector<int>> e_list, bool write_dimacs = false);
    // Returns true if applying the board does not lead to UNSAT result
    // Returns true if the vertex problem has a solution
    bool solve();
    vertex_cover get_solution() const;

private:
    void At_Least_One_In_Vertex_Cover();
    void Non_Vertex_Twice_in_Vertex_Cover();
    void One_Vertex_Cover_is_One_Vertex();
    void Edge_Incident_Vertex_Cover(std::vector<std::vector<int>> E_list);
    void exactly_one_true(Minisat::vec<Minisat::Lit> const& literals);
    void init_variables();
};