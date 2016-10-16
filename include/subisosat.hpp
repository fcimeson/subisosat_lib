
/****************************************************************************//**
 ********************************************************************************
 * @file        subisosat.hpp
 * @brief       
 * @author      Frank Imeson
 * @date        
 ********************************************************************************
 ********************************************************************************/

#ifndef SUBISO_H		// guard
#define SUBISO_H

/********************************************************************************
 * INCLUDE
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <igraph/igraph.h>

#include "formula.hpp"


/********************************************************************************
 * Defs
 ********************************************************************************/




/********************************************************************************
 * Prototypes
 ********************************************************************************/

int igraph_vertex_degree (const igraph_t *graph1, const igraph_integer_t vid);

igraph_bool_t igraph_compare_transitives (
          const igraph_t *graph1,
          const igraph_t *graph2, 
			    const igraph_integer_t vid1,
			    const igraph_integer_t vid2,
			    void *arg);


//void igraph_vector_print (const igraph_vector_t *vector) {
//    for (unsigned int i = 0; i < igraph_vector_size(vector); i++)
//        std::cout << VECTOR(*vector)[i] << " ";
//    std::cout << std::endl;
//};


// see cpp file for documentation
int igraph_test_isomorphic_map (const igraph_t *graph1, const igraph_t *graph2, 
          const igraph_vector_int_t *vertex_colour1,
          const igraph_vector_int_t *vertex_colour2,
          const igraph_vector_int_t *edge_colour1,
          const igraph_vector_int_t *edge_colour2,
          igraph_bool_t *iso,
          const igraph_vector_t *map12, 
          const igraph_vector_t *map21,
          igraph_isocompat_t *node_compat_fn,
          igraph_isocompat_t *edge_compat_fn,
          void *arg);


// see cpp file for documentation
int igraph_subisomorphic_sat (const igraph_t *graph1, const igraph_t *graph2, 
          const igraph_vector_int_t *vertex_colour1,
          const igraph_vector_int_t *vertex_colour2,
          const igraph_vector_int_t *edge_colour1,
          const igraph_vector_int_t *edge_colour2,
          igraph_bool_t *iso,
          igraph_vector_t *map12, 
          igraph_vector_t *map21,
          igraph_isocompat_t *node_compat_fn,
          igraph_isocompat_t *edge_compat_fn,
          void *arg);


// see cpp file for documentation
int igraph_get_subisomorphisms_sat (const igraph_t *graph1, const igraph_t *graph2, 
          const igraph_vector_int_t *vertex_colour1,
          const igraph_vector_int_t *vertex_colour2,
          const igraph_vector_int_t *edge_colour1,
          const igraph_vector_int_t *edge_colour2,
          igraph_vector_ptr_t *maps,
          igraph_vector_t *map12, 
          igraph_vector_t *map21,
          igraph_isocompat_t *node_compat_fn,
          igraph_isocompat_t *edge_compat_fn,
          void *arg);


// see cpp file for documentation
int igraph_count_subisomorphisms_sat (const igraph_t *graph1, const igraph_t *graph2, 
          const igraph_vector_int_t *vertex_colour1,
          const igraph_vector_int_t *vertex_colour2,
          const igraph_vector_int_t *edge_colour1,
          const igraph_vector_int_t *edge_colour2,
          igraph_integer_t *count,
          igraph_isocompat_t *node_compat_fn,
          igraph_isocompat_t *edge_compat_fn,
          void *arg);


// see cpp file for documentation
int igraph_subisomorphic_function_sat (const igraph_t *graph1, const igraph_t *graph2, 
          const igraph_vector_int_t *vertex_colour1,
          const igraph_vector_int_t *vertex_colour2,
          const igraph_vector_int_t *edge_colour1,
          const igraph_vector_int_t *edge_colour2,
          igraph_vector_t *map12, 
          igraph_vector_t *map21,
          igraph_isohandler_t *isohandler_fn,
          igraph_isocompat_t *node_compat_fn,
          igraph_isocompat_t *edge_compat_fn,
          void *arg);



/********************************************************************************
 * isosat
 ********************************************************************************/


namespace isosat {
using namespace Minisat;
using namespace std;

struct M21;
class Isosat;

string str (const M21 &lit);
string str (const igraph_vector_t &vector);



struct M21 {
    bool sign;
    unsigned int vid2, vid1;
    M21 () { vid1=0; vid2=0; sign=false;};
    M21 (int _vid2, int _vid1, bool _sign=false) { vid1=_vid1; vid2=_vid2; sign=_sign; };
};


class Isosat {
    private:
        
        int error;
        int v1_size, v2_size;
        int conflict_budget, propagation_budget;
        Solver solver;

        void minisat_cb (
            const VMap<lbool> &assigns, 
            const vec<Lit>& trail, 
            vec<Lit>& infer_list);

        static void minisat_cb_wrapper (
            void* object_pointer, 
            const VMap<lbool> &assigns, 
            const vec<Lit>& trail, 
            vec<Lit>& infer_list);

        int set_size();
        string str (const vec<Lit> &vector);

    public:

        Isosat (const igraph_t *graph1, const igraph_t *graph2, 
                const igraph_vector_int_t *vertex_colour1,
                const igraph_vector_int_t *vertex_colour2,
                const igraph_vector_int_t *edge_colour1,
                const igraph_vector_int_t *edge_colour2,
                igraph_isocompat_t *node_compat_fn,
                igraph_isocompat_t *edge_compat_fn,
                void *arg);

        int add_edge (const igraph_t *graph1, const igraph_t *graph2,
                const igraph_integer_t eid,
                const igraph_vector_int_t *vertex_colour1,
                const igraph_vector_int_t *vertex_colour2,
                const igraph_vector_int_t *edge_colour1,
                const igraph_vector_int_t *edge_colour2,
                igraph_isocompat_t *node_compat_fn,
                igraph_isocompat_t *edge_compat_fn,
                void *arg);

        int solve (
                igraph_bool_t *iso,
                igraph_vector_t *map12, 
                igraph_vector_t *map21,
                const vec<Lit> *assumptions = NULL);

        void setConfBudget(int budget) { conflict_budget    = budget; };
        void setPropBudget(int budget) { propagation_budget = budget; };

        int negate (const igraph_vector_t *map12, igraph_vector_t *map21);
        int negate (const M21 v21_map);
        int get_error () {return error;}

        Lit translate (const M21 &lit);
        M21 translate (const Lit &lit);
};


} // end namespace

#endif
