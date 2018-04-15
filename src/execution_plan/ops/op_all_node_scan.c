#include "op_all_node_scan.h"

OpBase* NewAllNodeScanOp(RedisModuleCtx *ctx, Graph *g, Node **n, const char *graph_name) {
    return (OpBase*)NewAllNodeScan(ctx, g, n, graph_name);
}

AllNodeScan* NewAllNodeScan(RedisModuleCtx *ctx, Graph *g, Node **n, const char *graph_name) {
    // Get graph store
    LabelStore *store = LabelStore_Get(ctx, STORE_NODE, graph_name, NULL);
    if(store == NULL) {
        return NULL;
    }

    AllNodeScan *allNodeScan = malloc(sizeof(AllNodeScan));
    allNodeScan->ctx = ctx;
    allNodeScan->node = n;
    allNodeScan->_node = *n;
    allNodeScan->store = store;
    allNodeScan->graph = graph_name;
    LabelStore_Scan(store, &allNodeScan->iter);

    // Set our Op operations
    allNodeScan->op.name = "All Node Scan";
    allNodeScan->op.type = OPType_ALL_NODE_SCAN;
    allNodeScan->op.consume = AllNodeScanConsume;
    allNodeScan->op.reset = AllNodeScanReset;
    allNodeScan->op.free = AllNodeScanFree;
    allNodeScan->op.modifies = NewVector(char*, 1);

    Vector_Push(allNodeScan->op.modifies, Graph_GetNodeAlias(g, *n));

    return allNodeScan;
}

OpResult AllNodeScanConsume(OpBase *opBase, Graph* graph) {
    AllNodeScan *op = (AllNodeScan*)opBase;
    
    if(raxEOF(&op->iter)) {
        return OP_DEPLETED;
    }

    char *id;
    uint16_t idLen;
    Node *node;
    int res = LabelStoreIterator_Next(&op->iter, &id, &idLen, (void**)&node);
    
    if(res == 0) {
        return OP_DEPLETED;
    }
    
    /* Update node */
    *op->node = node;

    return OP_OK;
}

OpResult AllNodeScanReset(OpBase *op) {
    AllNodeScan *allNodeScan = (AllNodeScan*)op;
    
    *allNodeScan->node = allNodeScan->_node;
    LabelStoreIterator_Free(&allNodeScan->iter);

    LabelStore_Scan(allNodeScan->store, &allNodeScan->iter);
    return OP_OK;
}

void AllNodeScanFree(OpBase *ctx) {
    AllNodeScan *allNodeScan = (AllNodeScan *)ctx;
    LabelStoreIterator_Free(&allNodeScan->iter);
    free(allNodeScan);
}