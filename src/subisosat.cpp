/********************************************************************************
  Copyright 2017 Frank Imeson, Siddharth Garg, and Mahesh V. Tripunitara

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*********************************************************************************/


#include "subisosat.hpp"
using namespace isosat;

//#define DEBUG
//#define DEBUG_SAT
//#define MINISAT_VERBOSE


/*****************************************************************************
 *****************************************************************************
 * 
 * iGraph Functions
 *         
 *****************************************************************************
 *****************************************************************************/


/************************************************************//**
 * @brief	
 * @return            string representation of connective	
 * @version						v0.01b
 ****************************************************************/
igraph_bool_t igraph_compare_transitives(
          const igraph_t *graph1,
          const igraph_t *graph2, 
			    const igraph_integer_t vid1,
			    const igraph_integer_t vid2,
			    void *arg)
{
    igraph_vector_t degree1, degree2;
    igraph_vector_init(&degree1, 1);
    igraph_vector_init(&degree2, 1);
    igraph_vs_t vs1, vs2;
    igraph_vs_1(&vs1, vid1);
    igraph_vs_1(&vs2, vid2);
    bool result(true);
    
    if (igraph_degree(graph1, &degree1, vs1, IGRAPH_IN, false) != IGRAPH_SUCCESS)
        return false;

    if (igraph_degree(graph2, &degree2, vs2, IGRAPH_IN, false) != IGRAPH_SUCCESS)
        return false;

    if (VECTOR(degree2)[0] > VECTOR(degree1)[0])
        result = false;

    if (igraph_degree(graph1, &degree1, vs1, IGRAPH_OUT, false) != IGRAPH_SUCCESS)
        return false;

    if (igraph_degree(graph2, &degree2, vs2, IGRAPH_OUT, false) != IGRAPH_SUCCESS)
        return false;

    if (VECTOR(degree2)[0] > VECTOR(degree1)[0])
        result = false;

    igraph_vector_destroy(&degree1);
    igraph_vector_destroy(&degree2);
    igraph_vs_destroy(&vs1);
    igraph_vs_destroy(&vs2);
    return result;
}



/************************************************************//**
 * @brief	
 * @return            string representation of connective	
 * @version						v0.01b
 ****************************************************************/
int igraph_vertex_degree (const igraph_t *graph1, const igraph_integer_t vid)
{
    igraph_vector_t degree;
    igraph_vector_init(&degree, 1);
    igraph_vs_t vs;
    igraph_vs_1(&vs, vid);
    
    igraph_degree(graph1, &degree, vs, IGRAPH_ALL, false);
    return VECTOR(degree)[0];
}


/************************************************************//**
 * @brief                           
      Decides wheater a mapping map12 or map21 subgraph of graph1 is isomorphic to graph2
      
 * @param	graph1
      The first input graph, may be directed or undirected.
      This is supposed to be the larger graph.
                                    
 * @param	graph2
      The second input graph, it must have the same directedness as graph1.
      This is supposed to be the smaller graph.
                                  
 * @param	vertex_colour1
      An optional colour vector for the first graph.
      If colour vectors are given for both graphs,
      then the subgraph isomorphism is calculated on the coloured graphs;
      i.e. two vertices can match only if their colour also matches.
      Supply a null pointer here if your graphs are not coloured.
                                  
 * @param	vertex_colour2
      An optional colour vector for the second graph.
      See the previous argument for explanation.

 * @param	edge_colour1
      An optional edge colour vector for the first graph.
      The matching edges in the two graphs must have matching colours as well.
      Supply a null pointer here if your graphs are not edge-coloured.
                                  
 * @param edge_colour2
      The edge colour vector for the second graph.
 
 * @param iso
      Pointer to a boolean.
      The result of the decision problem is stored here.
                                    
 * @param map12
      Pointer to a vector or NULL (either map12 or map21 has to be non NULL).
      If not NULL, then an isomorphic mapping from graph1 to graph2 is tested.
                                 
 * @param map21
      Pointer to a vector ot NULL (either map12 or map21 has to be non NULL).
      If not NULL, then an isomorphic mapping from graph2 to graph1 is tested.
                                    
 * @param node_compat_fn
      A pointer to a function of type igraph_isocompat_t.
      This function will be called by the algorithm to determine whether two nodes are compatible.
                                    
 * @param edge_compat_fn
      A pointer to a function of type igraph_isocompat_t.
      This function will be called by the algorithm to determine whether two edges are compatible.
                                    
 * @param  arg
      Extra argument to supply to functions none_compat_fn and edge_compat_fn               

 * @return                          result
 * @version						              v0.01b
 ****************************************************************/
int igraph_test_isomorphic_map (
    const igraph_t *graph1,
    const igraph_t *graph2, 
    const igraph_vector_int_t *vertex_colour1,
    const igraph_vector_int_t *vertex_colour2,
    const igraph_vector_int_t *edge_colour1,
    const igraph_vector_int_t *edge_colour2,
    igraph_bool_t *iso,
    const igraph_vector_t *map12, 
    const igraph_vector_t *map21,
    igraph_isocompat_t *node_compat_fn,
    igraph_isocompat_t *edge_compat_fn,
    void *arg)
{
    *iso = false;

    /******************************
     * Reject bad input
     ******************************/
    if ( (map12 == NULL && map21 == NULL) || (map12 != NULL && map21 != NULL) ) {
        cout << "test_isomorphic_map: no map to test" << endl;
        return IGRAPH_ARPACK_MODEINV;
    }

    /******************************
     * Setup isomorphism map from 2->1
     ******************************/
    igraph_vector_t map;
    if (map21 == NULL) {

        if (igraph_vector_size(map12) != igraph_vcount(graph1)) {
            cout << "test_isomorphic_map: map is incorrect size" << endl;
            return IGRAPH_EINVVID;
        }

        igraph_vector_init(&map, igraph_vcount(graph2));
        for (unsigned int vid1=0; vid1 < igraph_vector_size(map12); vid1++) {
            if ( VECTOR(*map12)[vid1] > 0 && VECTOR(*map12)[vid1] < igraph_vcount(graph2) ) {
                VECTOR(map)[ (int)VECTOR(*map12)[vid1] ] = vid1;
            }
        }

    } else {
        if (igraph_vector_size(map21) != igraph_vcount(graph2)) {
            cout << "test_isomorphic_map: map is incorrect size" << endl;
            return IGRAPH_EINVVID;
        }
        igraph_vector_copy(&map, map21);
    }


    /******************************
     * Check vertex properties
     ******************************/
    bool verticies_ok = true;
    bool seen[igraph_vcount(graph1)];
    for (unsigned int v=0; v<igraph_vcount(graph1); v++)
        seen[v] = false;

    for (unsigned int vid2 = 0; vid2 < igraph_vector_size(&map); vid2++) {

        int vid1 = (int)VECTOR(map)[vid2];
        if (vid1 < 0 || vid1 >= igraph_vcount(graph1) ) {
//            cout << "test_isomorphic_map: map out of range" << endl;
            verticies_ok = false;
            break;
        }

        // check bijective property
        if (seen[vid1]) {
//            cout << "test_isomorphic_map: duplicate map: " << vid1 << endl;
            verticies_ok = false;
            break;
        } else {
            seen[vid1] = true;
        }

        // check colour
        if ( vertex_colour1 != NULL && vertex_colour2 != NULL) {
            if ( VECTOR(*vertex_colour1)[vid1] != VECTOR(*vertex_colour2)[vid2] ) {
//                cout << "test_isomorphic_map: incorrect colour" << endl;
                verticies_ok = false;
                break;
            }
        }

        // check node_compat_fn
        if ( node_compat_fn != NULL) {
            if ( !(*node_compat_fn)(graph1, graph2, vid1, vid2, arg) ) {
//                cout << "test_isomorphic_map: node compare" << endl;
                verticies_ok = false;
                break;
            }
        }

    }

    if (!verticies_ok) {
        *iso = false;
        igraph_vector_destroy(&map);
        return IGRAPH_SUCCESS;
    }


    /******************************
     * Check edge properties
     * compare each edge in graph1 to graph2
     ******************************/
    bool edges_ok = true;
    for (unsigned int eid2 = 0; eid2 < igraph_ecount(graph2); eid2++) {

        // get vertices for edge i in E(graph2)
        int from2, to2;
        igraph_edge(graph2, eid2, &from2, &to2);

        // Get edge id for graph1
        int eid1(-1);
        if ( igraph_get_eid(
                graph1, 
                &eid1, 
                VECTOR(map)[from2], 
                VECTOR(map)[to2], 
                IGRAPH_DIRECTED, 
                0 /* error */)
             != IGRAPH_SUCCESS)
        {
//            cout << "test_isomorphic_map: invalid eid" << endl;
            edges_ok = false;
            break;        
        }
        
        if (eid1 == -1) {
//            cout << "test_isomorphic_map: invalid eid" << endl;
            edges_ok = false;
            break; 
        }

        // check edge_colour_fn
        if (edge_colour1 != NULL && edge_colour1 != NULL) {
            if ( VECTOR(*edge_colour1)[eid1] != VECTOR(*edge_colour2)[eid2] ) {
//                cout << "test_isomorphic_map: incorrect edge colour" << endl;
                edges_ok = false;
                break;
            }
        }

        // check edge_compat_fn
        if ( edge_compat_fn != NULL) {
            if ( !(*edge_compat_fn)(graph1, graph2, eid1, eid2, arg) ) {
//                cout << "test_isomorphic_map: incorrect edge compare" << endl;
                edges_ok = false;
                break;
            }
        }
        
    }
    igraph_vector_destroy(&map);

    if (!edges_ok) {
        *iso = false;
        return IGRAPH_SUCCESS;
    }

    *iso = true;
    return IGRAPH_SUCCESS;
}



/************************************************************//**
 * @brief                           
      Decides wheater a subgraph of graph1 is isomorphic to graph2
      
 * @param	graph1
      The first input graph, may be directed or undirected.
      This is supposed to be the larger graph.
                                    
 * @param	graph2
      The second input graph, it must have the same directedness as graph1.
      This is supposed to be the smaller graph.
                                  
 * @param	vertex_colour1
      An optional colour vector for the first graph.
      If colour vectors are given for both graphs,
      then the subgraph isomorphism is calculated on the coloured graphs;
      i.e. two vertices can match only if their colour also matches.
      Supply a null pointer here if your graphs are not coloured.
                                  
 * @param	vertex_colour2
      An optional colour vector for the second graph.
      See the previous argument for explanation.

 * @param	edge_colour1
      An optional edge colour vector for the first graph.
      The matching edges in the two graphs must have matching colours as well.
      Supply a null pointer here if your graphs are not edge-coloured.
                                  
 * @param edge_colour2
      The edge colour vector for the second graph.
 
 * @param iso
      Pointer to a boolean.
      The result of the decision problem is stored here.
                                    
 * @param map12
      Pointer to a vector or NULL.
      If not NULL, then an isomorphic mapping from graph1 to graph2 is stored here.
                                 
 * @param map21
      Pointer to a vector ot NULL.
      If not NULL, then an isomorphic mapping from graph2 to graph1 is stored here.
                                    
 * @param node_compat_fn
      A pointer to a function of type igraph_isocompat_t.
      This function will be called by the algorithm to determine whether two nodes are compatible.
                                    
 * @param edge_compat_fn
      A pointer to a function of type igraph_isocompat_t.
      This function will be called by the algorithm to determine whether two edges are compatible.
                                    
 * @param  arg
      Extra argument to supply to functions none_compat_fn and edge_compat_fn               

 * @return                          Error code.
 * @version						              v0.01b
 ****************************************************************/
int igraph_subisomorphic_sat (
    const igraph_t *graph1,
    const igraph_t *graph2, 
    const igraph_vector_int_t *vertex_colour1,
    const igraph_vector_int_t *vertex_colour2,
    const igraph_vector_int_t *edge_colour1,
    const igraph_vector_int_t *edge_colour2,
    igraph_bool_t *iso,
    igraph_vector_t *map12, 
    igraph_vector_t *map21,
    igraph_isocompat_t *node_compat_fn,
    igraph_isocompat_t *edge_compat_fn,
    void *arg)
{
    Isosat isosat(graph1, graph2, vertex_colour1,vertex_colour2,
                  edge_colour1, edge_colour2, node_compat_fn,
                  edge_compat_fn, arg);

    return isosat.solve(iso, map12, map21);
}


/************************************************************//**
 * @brief	
 * @return            string representation of connective	
 * @version						v0.01b
 ****************************************************************/
int igraph_count_subisomorphisms_sat (
    const igraph_t *graph1,
    const igraph_t *graph2, 
    const igraph_vector_int_t *vertex_colour1,
    const igraph_vector_int_t *vertex_colour2,
    const igraph_vector_int_t *edge_colour1,
    const igraph_vector_int_t *edge_colour2,
    igraph_integer_t *count,
    igraph_isocompat_t *node_compat_fn,
    igraph_isocompat_t *edge_compat_fn,
    void *arg)
{
    Isosat isosat(graph1, graph2, vertex_colour1,vertex_colour2,
                  edge_colour1, edge_colour2, node_compat_fn,
                  edge_compat_fn, arg);


    igraph_bool_t iso(true);
    *count = 0;
    igraph_vector_t map21;
    igraph_vector_init(&map21, igraph_vcount(graph2));
    while (iso) {
        isosat.solve(&iso, NULL, &map21);
        if (iso) {
            #ifndef NDEBUG
                igraph_bool_t iso_test(false);
                igraph_test_isomorphic_map (graph1, graph2, vertex_colour1, vertex_colour2,
                                            edge_colour1, edge_colour2, &iso_test,
                                            NULL, &map21, node_compat_fn, edge_compat_fn, arg);
                assert(iso_test);
            #endif
            isosat.negate(NULL, &map21);
            (*count)++;
        }
    }
	igraph_vector_destroy(&map21);

    return IGRAPH_SUCCESS;
}














/*****************************************************************************
 *****************************************************************************
 * 
 * isosat functions
 *         
 *****************************************************************************
 *****************************************************************************/
 
 


/************************************************************//**
 * @brief	
 * @return            string representation of connective	
 * @version						v0.01b
 ****************************************************************/
string isosat::str (const M21 &lit) {
    stringstream out;
    out << string(lit.sign ? "-":" ")
        << "M_"
        << lit.vid2
        << ","
        << lit.vid1;
    return out.str();
}




/************************************************************//**
 * @brief	
 * @return            string representation of connective	
 * @version						v0.01b
 ****************************************************************/
string isosat::str (const igraph_vector_t &vector) {
    stringstream out;
    for (unsigned int i = 0; i < igraph_vector_size(&vector); i++) {
        out << (int) VECTOR(vector)[i] << " ";
    }
    return out.str();
}




/*****************************************************************************
 *****************************************************************************
 * 
 * Isosat Class
 *         
 *****************************************************************************
 *****************************************************************************/




/************************************************************//**
 * @brief	
 * @return            string representation of connective	
 * @version						v0.01b
 ****************************************************************/
string Isosat::str (const vec<Lit> &vector) {
    stringstream out;
    for (unsigned int i = 0; i < vector.size(); i++)
        out << isosat::str( translate(vector[i]) ) << " ";
    return out.str();
}



/************************************************************//**
 * @brief	
 * @return            string representation of connective	
 * @version						v0.01b
 ****************************************************************/
int Isosat::set_size () {
    return v1_size*v2_size;
}



/************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************/
Isosat::Isosat (
    const igraph_t *graph1,
    const igraph_t *graph2, 
    const igraph_vector_int_t *vertex_colour1,
    const igraph_vector_int_t *vertex_colour2,
    const igraph_vector_int_t *edge_colour1,
    const igraph_vector_int_t *edge_colour2,
    igraph_isocompat_t *node_compat_fn,
    igraph_isocompat_t *edge_compat_fn,
    void *arg)
    : error(IGRAPH_FAILURE)
    , conflict_budget(-1)
    , propagation_budget(-1)
{
    /******************************
     * Setup Solver
     ******************************/
    v1_size = igraph_vcount(graph1);
    v2_size = igraph_vcount(graph2);
    solver.callback_obj_pt  = this;
    solver.callback         = &Isosat::minisat_cb_wrapper;

    while ( solver.nVars() < set_size())
        solver.newVar();

//    igraph_set_error_handler(igraph_error_handler_ignore);

    /******************************
     * Add G to solver (most of formual is done in callbacks)
     * 1 - x_jk must have at least 1 entry per j
     * 2 - x_jk must be false if incorrect vertex match
     * O(n^2)using namespace formula;
     ******************************/
    for (unsigned int vid2 = 0; vid2 < igraph_vcount(graph2); vid2++) {
        vec<Lit> clause;
        for (unsigned int vid1 = 0; vid1 < igraph_vcount(graph1); vid1++) {
            
            bool match(true);
            if (vertex_colour1 != NULL && vertex_colour2 != NULL)
                if (VECTOR(*vertex_colour1)[vid1] != VECTOR(*vertex_colour2)[vid2])
                    match = false;

            if (node_compat_fn != NULL)
                if (!(*node_compat_fn)(graph1, graph2, vid1, vid2, arg))
                    match = false;

           if (match) {
                clause.push( translate(M21(vid2, vid1, false)) );
            } else {
                // restrictions
                if ( !solver.addClause( translate(M21(vid2, vid1, true)) ) ) {
                    #ifdef DEBUG
                        cerr << "Error: addClause() " << formula::str(translate(M21(vid2, vid1, true))) << endl;
                    #endif 
                    error = IGRAPH_FAILURE;
                    return;
                }
            }

        }

        // no possible mapping return false
        if (clause.size() == 0) {
            #ifdef DEBUG
              cerr << "Error in Setup: No possible vertex Mappings (row)" << endl;
            #endif
            error = IGRAPH_FAILURE;
            return; 
        }
        #ifdef DEBUG_SAT
            cout << "row: " << str(clause) << endl;
        #endif
        if ( !solver.addClause(clause) ) {
              #ifdef DEBUG
                  cerr << "Error: addClause() " << formula::str(clause) << endl;
              #endif 
              error = IGRAPH_FAILURE;
              return;
        }

    }


    /******************************
     * Add H to solver
     * 3 - Each edge e in H geusing namespace formula;t's mapped to each edge in G
     * O(n^2)
     ******************************/
    for (unsigned int eid2=0; eid2<igraph_ecount(graph2); eid2++) {
        if (add_edge(graph1, graph2,
                     eid2,
                     vertex_colour1,
                     vertex_colour2,
                     edge_colour1,
                     edge_colour2,
                     node_compat_fn,
                     edge_compat_fn,
                     arg) != IGRAPH_SUCCESS) {
            error = IGRAPH_FAILURE;
            return;
        }
    }

    error = IGRAPH_SUCCESS;
}



/************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************/
int Isosat::add_edge (
    const igraph_t *graph1,
    const igraph_t *graph2,
    const igraph_integer_t eid2,
    const igraph_vector_int_t *vertex_colour1,
    const igraph_vector_int_t *vertex_colour2,
    const igraph_vector_int_t *edge_colour1,
    const igraph_vector_int_t *edge_colour2,
    igraph_isocompat_t *node_compat_fn,
    igraph_isocompat_t *edge_compat_fn,
    void *arg)
{
    assert(v1_size == igraph_vcount(graph1));
    assert(v2_size == igraph_vcount(graph2));
    
    int from2, to2;
    igraph_edge(graph2, eid2, &from2, &to2);

    formula::Formula phrase00(formula::F_OR);
    for (unsigned int eid1 = 0; eid1 < igraph_ecount(graph1); eid1++) {
        int from1, to1;
        igraph_edge(graph1, eid1, &from1, &to1);
        
        bool match(true);
        if (vertex_colour1 != NULL && vertex_colour2 != NULL)
            if ( VECTOR(*vertex_colour1)[from1] != VECTOR(*vertex_colour2)[from2] ||
                 VECTOR(*vertex_colour1)[to1]   != VECTOR(*vertex_colour2)[to2] )
                match = false;

        if (node_compat_fn != NULL)
            if ( !(*node_compat_fn)(graph1, graph2, from1, from2, arg) ||
                 !(*node_compat_fn)(graph1, graph2, to1, to2, arg) )
                match = false;

        if (edge_colour1 != NULL && edge_colour2 != NULL)
            if (VECTOR(*edge_colour1)[eid1] != VECTOR(*edge_colour2)[eid2])
                match = false;

        if (edge_compat_fn != NULL)
            if (!(*edge_compat_fn)(graph1, graph2, eid1, eid2, arg))
                match = false;

        if (match) {
            formula::Formula* phrase01 = new formula::Formula(formula::F_AND);
            phrase01->add( translate(M21(from2, from1)) );
            phrase01->add( translate(M21(to2, to1)) );
            phrase00.add(phrase01);
        }
    }

    // no edge to map to
    if (phrase00.size() == 0) {
        #ifdef DEBUG
          cerr << "Error in Setup: No possible edge Mappings" << endl;
        #endif
        return IGRAPH_FAILURE;
    }

    Lit cnf_out;
    if (int err = phrase00.export_cnf(cnf_out, NULL, &solver) < 0) {
        #ifdef DEBUG
          cerr << "Error in Setup: export_cnf failed" << endl;
        #endif
        return IGRAPH_FAILURE;
    }

    solver.addClause(cnf_out);
    #ifdef DEBUG_SAT
        cout << "edge sat: " << endl << phrase00.str();
    #endif  

    return IGRAPH_SUCCESS;
}



/************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************/
int Isosat::solve (
    igraph_bool_t *iso,
    igraph_vector_t *map12, 
    igraph_vector_t *map21,
    const vec<Lit> *assumptions)
{

    *iso = false;
    if (error != IGRAPH_SUCCESS) {
        #ifdef DEBUG
          cerr << "Error in solve(): setup failed" << endl;
        #endif
        return error;
    }

    if (conflict_budget > 0)
        solver.setConfBudget(conflict_budget);

    if (propagation_budget > 0)
        solver.setPropBudget(propagation_budget);
    
    #ifdef MINISAT_VERBOSE
      solver.verbosity = 99;
    #endif

    if (assumptions == NULL)
        *iso = (solver.solveLimited(vec<Lit>()) == l_True);
    else
        *iso = (solver.solveLimited(*assumptions) == l_True);


    if (*iso == true && (map12 != NULL || map21 != NULL) ) {

        for (unsigned int vid1 = 0; vid1 < v1_size; vid1++) {

            if (map12 != NULL)
                VECTOR(*map12)[vid1] = -1;

            for (unsigned int vid2 = 0; vid2 < v2_size; vid2++) {
                if (solver.model[ var(translate(M21(vid2, vid1))) ] == l_True) {

                    if (map12 != NULL)
                          VECTOR(*map12)[vid1] = vid2;

                    if (map21 != NULL)
                          VECTOR(*map21)[vid2] = vid1;

                }
            }
        }
    }

    return IGRAPH_SUCCESS;
}



/************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************/
int Isosat::negate (const igraph_vector_t *map12, igraph_vector_t *map21) {

    if ( (map12 == NULL && map21 == NULL) || (map12 != NULL && map21 != NULL) )
        return IGRAPH_ARPACK_MODEINV;

    if (map21 != NULL) {
        assert ( igraph_vector_size(map21) == v2_size );
        vec<Lit> neg_list;
        for (unsigned int vid2 = 0; vid2 < v2_size; vid2++) {
            int vid1 = (int)VECTOR(*map21)[vid2];
            neg_list.push( translate(M21(vid2, vid1, true)) );
        }
        solver.addClause(neg_list);
        #ifdef DEBUG
            cout << "negate " << formula::str(neg_list) << endl;
        #endif
    } else {
        assert ( igraph_vector_size(map12) == v1_size );
        vec<Lit> neg_list;
        for (unsigned int vid1 = 0; vid1 < v1_size; vid1++) {
            int vid2 = (int)VECTOR(*map12)[vid1];
            if (vid2 >= 0 && vid2 < v2_size) 
                neg_list.push( translate(M21(vid2, vid1, true)) );
        }
        solver.addClause(neg_list);
        #ifdef DEBUG
            cout << "negate " << formula::str(neg_list) << endl;
        #endif
    }
    return 0;
}



/************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************/
M21 Isosat::translate (const Lit &lit) {
    assert ( var(lit) <= set_size() );
    return M21( var(lit)/v1_size, var(lit)%v1_size, sign(lit) );
}



/************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************/
Lit Isosat::translate (const M21 &lit) {
//cout << ::str(lit) << " = " << formula::str(mkLit( lit.vid2*v1_size + lit.vid1, lit.sign )) << endl;
    assert ( lit.vid1 < v1_size && lit.vid2 < v2_size );
    return mkLit( lit.vid2*v1_size + lit.vid1, lit.sign );
}




/************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************/
void Isosat::minisat_cb_wrapper (void* _object_pointer, const VMap<lbool> &assigns, const vec<Lit>& trail, vec<Lit>& infer_list) {
    Isosat* object_pointer = (Isosat*) _object_pointer;
    return object_pointer->minisat_cb(assigns, trail, infer_list);
}



/************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************/
void Isosat::minisat_cb (const VMap<lbool> &assigns, const vec<Lit>& trail, vec<Lit>& infer_list) {
    if ( assigns[ var(trail.last()) ] == l_True && var(trail.last()) < set_size() ) {
        M21 new_decide = translate(trail.last());

        // negate all other variables in column
        for (unsigned int vid1 = 0; vid1 < v1_size; vid1++) {
            if (vid1 != new_decide.vid1)
                infer_list.push( translate(M21(new_decide.vid2, vid1, true)) );
        }

        // negate all other variables in row
        for (unsigned int vid2 = 0; vid2 < v2_size; vid2++) {
            if (vid2 != new_decide.vid2)
                infer_list.push( translate(M21(vid2, new_decide.vid1, true)) );
        }
        
        #ifdef DEBUG
            cout << "new_decide: " << formula::str(trail.last()) << endl;
            cout << "infer_list: " << formula::str(infer_list) << endl;
        #endif
    }
}






