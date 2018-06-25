CFLAGS:=$(CFLAGS)
CC_OBJECTS:=$(CC_OBJECTS)
LIBRMUTIL=../../src/rmutil/librmutil.a
LIBTRIEMAP=../../src/util/triemap/libtriemap.a
LIBGRAPHBLAS=../../deps/GraphBLAS/build/libgraphblas.a

LIBS=$(LIBTRIEMAP) $(LIBRMUTIL) $(LIBGRAPHBLAS)
DEPS=$(CC_OBJECTS) $(LIBS)

SRCDIR := $(shell pwd)
LDFLAGS :=  -lc -lm -ldl -lpthread

CC=gcc
.SUFFIXES: .c .so .xo .o

graph: test_graph.o
	$(CC) $(CFLAGS) -o test_graph test_graph.o  $(DEPS) $(LDFLAGS)

algebraic_expression: test_algebraic_expression.o
	$(CC) $(CFLAGS) -o test_algebraic_expression test_algebraic_expression.o  $(DEPS) $(LDFLAGS)

tuples_iter: test_tuples_iter.o
	$(CC) $(CFLAGS) -o test_tuples_iter test_tuples_iter.o  $(DEPS) $(LDFLAGS)

filter_tree: test_filter_tree.o
	$(CC) $(CFLAGS) -o test_filter_tree test_filter_tree.o  $(DEPS) $(LDFLAGS)

agg_sum: test_agg_sum.o
	$(CC) $(CFLAGS)  -o test_agg_sum test_agg_sum.o $(DEPS) $(LDFLAGS)

aggregate_functions: test_aggregate_functions.o
	$(CC) $(CFLAGS)  -o test_aggregate_functions test_aggregate_functions.o $(DEPS) $(LDFLAGS)

resultset_record: test_resultset_record.o
	$(CC) $(CFLAGS)  -o test_resultset_record test_resultset_record.o  $(DEPS) $(LDFLAGS)

value: test_value.o
	$(CC) $(CFLAGS)  -o test_value test_value.o  $(DEPS) $(LDFLAGS)

ast: test_ast.o
	$(CC) $(CFLAGS)  -o test_ast test_ast.o  $(DEPS) $(LDFLAGS)

build: graph algebraic_expression tuples_iter filter_tree agg_sum value resultset_record aggregate_functions ast

test_graph:
	@(sh -c ./test_graph)
.PHONY: test_graph

test_algebraic_expression:
	@(sh -c ./test_algebraic_expression)
.PHONY: test_algebraic_expression

test_tuples_iter:
	@(sh -c ./test_tuples_iter)
.PHONY: test_tuples_iter

test_filter_tree:
	@(sh -c ./test_filter_tree)
.PHONY: test_filter_tree

test_agg_sum:
	@(sh -c ./test_agg_sum)
.PHONY: test_agg_sum

test_aggregate_functions:
	@(sh -c ./test_aggregate_functions)
.PHONY: test_aggregate_functions

test_resultset_record:
	@(sh -c ./test_resultset_record)
.PHONY: test_resultset_record

test_value:
	@(sh -c ./test_value)
.PHONY: test_value

test_ast:
	@(sh -c ./test_ast)
.PHONY: test_ast

test: test_graph test_algebraic_expression test_tuples_iter test_filter_tree test_agg_sum test_value test_resultset_record test_aggregate_functions test_ast

all: build test