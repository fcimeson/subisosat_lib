

/****************************************************************************//**
 ********************************************************************************
 * @file        tests.cpp
 * @brief       
 * @author      Frank Imeson
 * @date        
 ********************************************************************************
 ********************************************************************************/
 
 
#include "formula.hpp"
using namespace formula;



/****************************************************************
 * Main
 ****************************************************************/
int main(int argc, char **argv) {

    Formula test01;

    Formula *test02 = new Formula(F_OR);
    test02->add(-1);
    test02->add(2);
    test02->add(3);

    Formula *test03 = new Formula(F_OR);
    test03->add(1);
    test03->add(2);
    test03->add(-3);

    test01.add(test02);
    test01.add(test03);
    cout << test01.str();

    Lit cnf_out_01;
    Formula formula01;
    test01.export_cnf(cnf_out_01, &formula01, NULL);
    formula01.add(cnf_out_01);
    cout << formula01.str();
    
    Solver solver01;
    test01.export_cnf(cnf_out_01, NULL, &solver01);
    solver01.addClause(cnf_out_01);
    while (solver01.solve()) {
        vec<Lit> soln;
        get_solution(solver01, soln, test01.maxVar());
        cout << "SAT: " << str(soln) << endl;
        negate_solution(soln, solver01);
    }
    





    cout << endl;

    test01.negate();
    cout << test01.str();

    Lit cnf_out_02;
    Formula formula02;
    test01.export_cnf(cnf_out_02, &formula02, NULL);
    formula02.add(cnf_out_02);
    cout << formula02.str();    

    Solver solver02;
    test01.export_cnf(cnf_out_02, NULL, &solver02);
    solver02.addClause(cnf_out_02);
    
    while (solver02.solve()) {
        vec<Lit> soln;
        get_solution(solver02, soln, test01.maxVar());
        cout << "SAT: " << str(soln) << endl;
        negate_solution(soln, solver02);
    }


 
}
