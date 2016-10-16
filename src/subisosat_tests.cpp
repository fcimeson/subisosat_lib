

/****************************************************************************//**
 ********************************************************************************
 * @file        tests.cpp
 * @brief       
 * @author      Frank Imeson
 * @date        
 ********************************************************************************
 ********************************************************************************/
 
#include "subisosat.hpp"
using namespace isosat;


/************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************/
double urand () {
	return (double)rand()/(double)RAND_MAX;
}


/************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************/
int rand_int (int low, int high) {
    assert(high >= low);
	  return rand() % (high-low + 1) + low;
}



/************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************/
//void test(bool result, int count) {
//	if (result) {
//		printf("   Test %2d..............................................%c[%d;%d;%dm [ok] %c[%dm \n", 
//		       count, 0x1B, BRIGHT,GREEN,BG_BLACK,0x1B, 0);
//	} else {
//		printf("   Test %2d..............................................%c[%d;%d;%dm [fail] %c[%dm \n",
//		       count, 0x1B, BRIGHT,RED,BG_BLACK,0x1B, 0);
//	}
//}


/****************************************************************
 * Main
 ****************************************************************/
int main(int argc, char **argv) {
    srand ( time(NULL) );
    int v1_size(10), e1_size(20);
    int v2_size(rand_int(0,v1_size)), e2_size(rand_int(0,e1_size));

    /******************************
     * Create graph1 (G) and graph2 (H)
     ******************************/
    igraph_t graph1, graph2;
    igraph_erdos_renyi_game(&graph1, 
                            IGRAPH_ERDOS_RENYI_GNM,
                            v1_size,
                            e1_size,
                            IGRAPH_DIRECTED,
                            IGRAPH_NO_LOOPS);

    igraph_copy(&graph2, &graph1);
    
    while (igraph_vcount(&graph2) > v2_size) {
        int vid2 = rand_int(0, igraph_vcount(&graph2)-1);
        igraph_vs_t vertex_set;
        igraph_vs_1(&vertex_set, vid2);
        igraph_delete_vertices(&graph2, vertex_set);
    }

    while (igraph_ecount(&graph2) > e2_size) {
        int eid2 = rand_int(0, igraph_ecount(&graph2)-1);
        igraph_delete_edges(&graph2, igraph_ess_1(eid2));
    }

    FILE *file;
    file = fopen("graph1.gml", "w");
    igraph_write_graph_gml(&graph1, file, NULL, NULL);
    fclose(file);

    file = fopen("graph2.gml", "w");
    igraph_write_graph_gml(&graph2, file, NULL, NULL);
    fclose(file);

    igraph_empty(&graph1, 0, 1);
    file = fopen("graph1.gml", "r");
    igraph_read_graph_gml(&graph1, file);
    fclose(file);

    igraph_empty(&graph2, 0, 1);
    file = fopen("graph2.gml", "r");
    igraph_read_graph_gml(&graph2, file);
    fclose(file);

    cout << "|V(G)| = " 
         << (int) igraph_vcount(&graph1) 
         << ", |E(G)| = " 
         << (int) igraph_ecount(&graph1) 
         << endl;
    cout << "|V(H)| = " 
         << (int) igraph_vcount(&graph2) 
         << ", |E(H)| = " 
         << (int) igraph_ecount(&graph2) 
         << endl;

    /******************************
     * Tests
     ******************************/
    igraph_bool_t iso;
    int count;

    if (igraph_isomorphic(&graph1, &graph1, &iso) == IGRAPH_SUCCESS)
        cout << "        iso(G,G): " << string( (iso) ? "True":"False" ) << endl;

    if (igraph_subisomorphic_vf2(&graph1, &graph2,0,0,0,0,&iso,0,0,0,0,0) == IGRAPH_SUCCESS)
        cout << "     subiso(G,H): " << string( (iso) ? "True":"False" ) << endl;

    if (igraph_subisomorphic_sat(&graph1, &graph2,0,0,0,0,&iso,0,0,0,0,0) == IGRAPH_SUCCESS)
        cout << "  subisosat(G,H): " << string( (iso) ? "True":"False" ) << endl;

    if (igraph_count_subisomorphisms_vf2(&graph1, &graph2,0,0,0,0,&count,0,0,0) == IGRAPH_SUCCESS)
        cout << "    #subiso(G,H): " << count << endl;

    if (igraph_count_subisomorphisms_sat(&graph1, &graph2,0,0,0,0,&count,
                                         &igraph_compare_transitives,0,0) == IGRAPH_SUCCESS)
        cout << " #subisosat(G,H): " << count << endl;

}






