
CC            = g++ -D __STDC_FORMAT_MACROS -D __STDC_LIMIT_MACROS
LIB_DIR       = -L/usr/lib64 -L/usr/lib
LIB           = -ligraph
INC           = -I./include -I./cb_minisat
MINISAT_OBJS  = cb_minisat/build/release/minisat/core/Solver.o  cb_minisat/build/dynamic/minisat/utils/System.o

all: minisat libsubisosat.so tests

install: libsubisosat.so
	cp ./libsubisosat.so $(HOME)/lib

minisat:
	cd cb_minisat &&\
	make

obj/formula.o: include/formula.hpp src/formula.cpp
	$(CC) $(CFLAGS) -fPIC $(INC) $(LIB_DIR) -c src/formula.cpp -o obj/formula.o

libsubisosat.so: minisat obj/formula.o include/subisosat.hpp src/subisosat.cpp 
	$(CC) $(CFLAGS) -fPIC $(INC) $(LIB_DIR) $(MINISAT_OBJS) obj/formula.o -c src/subisosat.cpp -o libsubisosat.so

tests: bin/formula_tests bin/subisosat_tests

bin/formula_tests: obj/formula.o obj/formula_tests.o
	$(CC) $(LDFLAGS) $(LIB_DIR) $(MINISAT_OBJS) obj/formula_tests.o obj/formula.o -o bin/formula_tests

obj/formula_tests.o: include/formula.hpp src/formula_tests.cpp
	$(CC) $(CFLAGS) $(INC) $(LIB_DIR) -c src/formula_tests.cpp -o obj/formula_tests.o

bin/subisosat_tests: libsubisosat.so obj/subisosat_tests.o
	$(CC) $(LDFLAGS) $(LIB_DIR) $(MINISAT_OBJS) obj/formula.o obj/subisosat_tests.o libsubisosat.so -o bin/subisosat_tests $(LIB)

obj/subisosat_tests.o: include/subisosat.hpp src/subisosat_tests.cpp
	$(CC) $(CFLAGS) $(INC) $(LIB_DIR) -c src/subisosat_tests.cpp -o obj/subisosat_tests.o

clean:
	cd cb_minisat && make clean
	rm -rf *~
	rm -rf */*~
	rm -rf */*/*~
	rm -f obj/*
	rm -f bin/*
	rm -rf core_dump/*
	rm -f *.so
	rm -f formula_tests subisosat_tests

