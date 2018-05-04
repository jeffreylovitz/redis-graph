#include "./match.h"
#include "../ast_common.h"

AST_MatchNode* New_AST_MatchNode(Vector *elements) {
	AST_MatchNode *matchNode = (AST_MatchNode*)malloc(sizeof(AST_MatchNode));
	matchNode->graphEntities = elements;
	return matchNode;
}

void Free_AST_MatchNode(AST_MatchNode *matchNode) {
	for(int i = 0; i < Vector_Size(matchNode->graphEntities); i++) {
		AST_GraphEntity *ge;
		Vector_Get(matchNode->graphEntities, i, &ge);
		Free_AST_GraphEntity(ge);
	}

	Vector_Free(matchNode->graphEntities);
	free(matchNode);
}
