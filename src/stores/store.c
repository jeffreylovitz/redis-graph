#include "store.h"
#include "../rmutil/util.h"
#include "../rmutil/strings.h"
#include "../dep/rax/rax_type.h"

/* Creates a new LabelStore. */
LabelStore *__new_Store(const char *label) {
    LabelStore *store = calloc(1, sizeof(LabelStore));
    store->items = raxNew();
    store->stats.properties = raxNew();
    if(label) store->label = strdup(label);

    return store;
}

void LabelStore_Free(LabelStore *store) {
    raxFree(store->items);
    raxFree(store->stats.properties);
    if(store->label) free(store->label);
    free(store);
}

int LabelStore_Id(char **id, LabelStoreType type, const char *graph, const char *label) {
    if(label == NULL) {
        label = "ALL";
    }

    const char* storeType = NULL;

    switch(type) {
        case STORE_NODE:
            storeType = "NODE";
            break;
        case STORE_EDGE:
            storeType = "EDGE";
            break;
        default:
            // Unexpected store type.
            break;
    }
    
    return asprintf(id, "%s_%s_%s_%s", LABELSTORE_PREFIX, graph, storeType, label);
}

LabelStore *LabelStore_Get(RedisModuleCtx *ctx, LabelStoreType type, const char *graph, const char* label) {
	LabelStore *store = NULL;
    char *strKey;
    LabelStore_Id(&strKey, type, graph, label);

    RedisModuleString *rmStoreId = RedisModule_CreateString(ctx, strKey, strlen(strKey));
    free(strKey);
    
	RedisModuleKey *key = RedisModule_OpenKey(ctx, rmStoreId, REDISMODULE_WRITE);
    RedisModule_FreeString(ctx, rmStoreId);

	if (RedisModule_KeyType(key) == REDISMODULE_KEYTYPE_EMPTY) {
		store = __new_Store(label);
		RedisModule_ModuleTypeSetValue(key, RaxRedisModuleType, store);
	}

	store = RedisModule_ModuleTypeGetValue(key);
    RedisModule_CloseKey(key);
	return store;
}

/* Get all stores of given type. */
void LabelStore_Get_ALL(RedisModuleCtx *ctx, LabelStoreType type, const char *graph, LabelStore **stores, size_t *stores_len) {
    char *pattern;
    LabelStore_Id(&pattern, type, graph, "*");

    size_t key_count = 128;             /* Maximum number of keys we're willing to process. */
    RedisModuleString *str_keys[128];   /* Keys returned by SCAN. */

    RMUtil_SCAN(ctx, pattern, str_keys, &key_count);
    free(pattern);

    /* Consume SCAN */
    for(int i = 0; i < key_count; i++) {
        RedisModuleString *store_key = str_keys[i];
        if(i < *stores_len) {
            RedisModuleKey *key = RedisModule_OpenKey(ctx, store_key, REDISMODULE_WRITE);
            stores[i] = RedisModule_ModuleTypeGetValue(key);
            RedisModule_CloseKey(key);
        }
        RedisModule_FreeString(ctx, store_key);
    }

    /* Update number of stores fetched. */
    *stores_len = key_count;
}

int LabelStore_Cardinality(LabelStore *store) {
    return raxSize(store->items);
}

void LabelStore_Insert(LabelStore *store, char *id, GraphEntity *entity) {
    if (raxInsert(store->items, (unsigned char *)id, strlen(id), entity, NULL)) {
        /* Entity is new to the store,
         * update store's entity schema. */
        if(store->label) {
            /* Store has a label, not an 'ALL' store, where there are
             * multiple entities with different labels.
             * Add each of the entity's attribute names to store's stats,
             * We'll be using this information whenever we're required to
             * expand a collapsed entity. */
            int prop_count = entity->prop_count;
            for(int idx = 0; idx < prop_count; idx++) {
                char *prop_name = entity->properties[idx].name;
                raxInsert(store->stats.properties, (unsigned char *)prop_name, strlen(prop_name), NULL, NULL);
            }
        }
    }
}

int LabelStore_Remove(LabelStore *store, char *id) {
    return raxRemove(store->items, (unsigned char *)id, strlen(id), NULL);
}

void LabelStore_Scan(LabelStore *store, LabelStoreIterator *it) {
    raxStart(it, store->items);
    raxSeek(it, "^", NULL, 0);
}

int LabelStoreIterator_Next(LabelStoreIterator *it, char **key, uint16_t *len, void **value) {
    int res = raxNext(it);
    *key = (char*)it->key;
    *len = it->key_len;
    *value = it->data;
    return res;
}

void LabelStoreIterator_Free(LabelStoreIterator* it) {
	raxStop(it);
}