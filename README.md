# subisosat_lib
This library provides methods to solve the subgraph isomorphism problems using a SAT solver (minisat), which was used in "Securing Computer Hardware Using 3D Integrated Circuit (IC) Technology and Split Manufacturing for Obfuscation". The methods that the library provide are the same methods that the igraph library offers (with the same interface). Specifically
 - igraph_subisomorphic_sat
 - igraph_get_subisomorphisms_sat
 - igraph_count_subisomorphisms_sat

The documentation for the equivelent igraph metods can be found at: http://igraph.org/c/doc/igraph-Isomorphism.html#igraph_subisomorphic.


# Citing this work
      @inproceedings{imeson2013securing,
        title={Securing Computer Hardware Using 3D Integrated Circuit (IC) Technology and Split Manufacturing for Obfuscation.},
        author={Imeson, Frank and Emtenan, Ariq and Garg, Siddharth and Tripunitara, Mahesh V},
        booktitle={USENIX Security},
        volume={13},
        year={2013}
      }


# Installation
This software depends on the C++ igraph library and minisat (minisat is included). To compile type make.


# License
Copyright 2017 Frank Imeson, Siddharth Garg, and Mahesh V. Tripunitara
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.


# Contact information
Frank Imeson  
Department of Electrical and Computer Engineering  
University of Waterloo  
Waterloo, ON Canada  
web: https://ece.uwaterloo.ca/~fcimeson/  
email: fcimeson@uwaterloo.ca  
