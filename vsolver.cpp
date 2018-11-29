//
// Created by wu on 11/17/18.
//

#include "vsover.h"

#include <iostream>
#include <vector>


namespace {

    Minisat::Var toVar(int n, int k, int nmax, int kmax) {
        assert(n >= 0 && n < nmax && "Attempt to get var for nonexistant vertices");
        assert(k >= 0 && k < kmax && "Attempt to get var for nonexistant vertex cover");
        return n * kmax + k;
    }

    void log_var(Minisat::Lit lit) {
        if (sign(lit)) {
            std::clog << '-';
        }
        std::clog << var(lit) + 1 << ' ';
    }

    void log_clause(Minisat::vec<Minisat::Lit> const& clause) {
        for (int i = 0; i < clause.size(); ++i) {
            log_var(clause[i]);
        }
        std::clog << "0\n";
    }

    void log_clause(Minisat::Lit lhs, Minisat::Lit rhs) {
        log_var(lhs); log_var(rhs);
        std::clog << "0\n";
    }
}


void Solver::init_variables() {
    if (m_write_dimacs) {
        std::clog << "c (row, column) = variable\n";
    }
    for (int r = 0; r < nmax; ++r) {
        for (int c = 0; c < kmax; ++c) {
                auto var = solver.newVar();
                if (m_write_dimacs) {
                    std::clog << "c (" << r << ", " << c << ") = " << '\n';
            }
        }
    }
    std::clog << std::flush;
}

void Solver::exactly_one_true(Minisat::vec<Minisat::Lit> const& literals) {
    if (m_write_dimacs) {
        log_clause(literals);
    }
    for (size_t i = 0; i < literals.size(); ++i) {
        for (size_t j = i + 1; j < literals.size(); ++j) {
            if (m_write_dimacs) {
                log_clause(~literals[i], ~literals[j]);
            }
            solver.addClause(~literals[i], ~literals[j]);
        }
    }
}

void Solver::At_Least_One_In_Vertex_Cover() {
    for (int k = 0; k < this->kmax; ++k) {
        Minisat::vec<Minisat::Lit> literals;
        for (int n = 0; n < this->nmax; ++n) {
            literals.push(Minisat::mkLit(toVar(n,k, this->nmax, this->kmax)));
        }
        solver.addClause(literals);
    }
}

void Solver::Non_Vertex_Twice_in_Vertex_Cover() {
    for (int n = 0; n < this->nmax; ++n) {
        Minisat::vec<Minisat::Lit> literals;
        for (int k = 0; k < this->kmax; ++k) {
            literals.push(Minisat::mkLit(toVar(n,k, this->nmax, this->kmax)));
        }
        exactly_one_true(literals);
    }
}

void Solver::One_Vertex_Cover_is_One_Vertex() {
    for (int k = 0; k < this->kmax; ++k) {
        Minisat::vec<Minisat::Lit> literals;
        for (int n = 0; n < this->nmax; ++n) {
            literals.push(Minisat::mkLit(toVar(n,k, this->nmax, this->kmax)));
        }
        exactly_one_true(literals);
    }
}

void Solver::Edge_Incident_Vertex_Cover(std::vector<std::vector<int>> E_List) {
    for (int e= 0; e < E_List.size(); ++e) {
        Minisat::vec<Minisat::Lit> literals;
        for (int k = 0; k < this->kmax; ++k) {
            literals.push(Minisat::mkLit(toVar(E_List[e][0],k, this->nmax, this->kmax)));
            literals.push(Minisat::mkLit(toVar(E_List[e][1],k, this->nmax, this->kmax)));
        }
        solver.addClause(literals);
    }
}

Solver::Solver(int n, int k, std::vector<std::vector<int>> E_List, bool write_dimacs):
    m_write_dimacs(write_dimacs) {
    this->nmax = n;
    this->kmax = k;
    // Initialize the board
    init_variables();
    At_Least_One_In_Vertex_Cover();

    Non_Vertex_Twice_in_Vertex_Cover();
    One_Vertex_Cover_is_One_Vertex();
    Edge_Incident_Vertex_Cover(E_List);
}

bool Solver::solve() {
    return solver.solve();
}

vertex_cover Solver::get_solution() const {
    vertex_cover v(this->nmax);
    for (int n = 0; n < this->nmax; ++n) {
        for (int k = 0; k < this->kmax; ++k) {
                if (solver.modelValue(toVar(n, k,this->nmax,this->kmax))== Minisat::l_True) {
                    v[n] = 1;
                    break;
            }
        }
    }
    return v;
}