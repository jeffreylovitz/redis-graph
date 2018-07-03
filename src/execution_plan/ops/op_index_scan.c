#include "op_index_scan.h"

OpBase *NewIndexScanOp(QueryGraph *qg, Graph *g, Node **node, IndexCreateIter *iter) {
  return (OpBase*)NewIndexScan(qg, g, node, iter);
}

IndexScan* NewIndexScan(QueryGraph *qg, Graph *g, Node **node, IndexCreateIter *iter) {

  IndexScan *indexScan = malloc(sizeof(IndexScan));
  /* TODO I think these are getting set to dummy nodes rather than proper
   * graph nodes. It is set before getting accessed by Consume, but an assertion
   * in the FilterTree _applyPredicateFilters fails if they are not set here.
   * Determine appropriate settings. */
  indexScan->node = node;
  indexScan->_node = *node;
  indexScan->g = g;

  // Set our Op operations
  indexScan->op.name = "Index Scan";
  indexScan->op.type = OPType_INDEX_SCAN;
  indexScan->op.consume = IndexScanConsume;
  indexScan->op.reset = IndexScanReset;
  indexScan->op.free = IndexScanFree;
  indexScan->op.modifies = NewVector(char*, 1);

  Vector_Push(indexScan->op.modifies, QueryGraph_GetNodeAlias(qg, *node));

  indexScan->operand = IndexCreateIter_BuildMatrix(iter, g->node_count);
  // Should be masked in IndexIterator
  indexScan->iter = TuplesIter_new(indexScan->operand);

  return indexScan;
}

  /*
     TODO deletes probably make the basic assumption that IDs are immutable invalid
   */
OpResult IndexScanConsume(OpBase *opBase, QueryGraph* graph) {
  IndexScan *op = (IndexScan*)opBase;

  GrB_Index node_id;
  if (TuplesIter_next(op->iter, NULL, &node_id) == TuplesIter_DEPLETED) {
    return OP_DEPLETED;
  }

  *op->node = Graph_GetNode(op->g, node_id);
  (*op->node)->id = node_id;

  return OP_OK;
}

OpResult IndexScanReset(OpBase *ctx) {
  IndexScan *indexScan = (IndexScan*)ctx;

  /* Restore original node. */
  *indexScan->node = indexScan->_node;
  // Verify that this call does what we want
  TuplesIter_reset(indexScan->iter);

  return OP_OK;
}

void IndexScanFree(OpBase *op) {
  IndexScan *indexScan = (IndexScan *)op;
  TuplesIter_free(indexScan->iter);
  GrB_Matrix_free(&indexScan->operand);
  free(indexScan);
}