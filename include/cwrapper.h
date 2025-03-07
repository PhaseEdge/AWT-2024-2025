#include <string>
#include <set>
#include <sstream>
#include <cache.h>
#include <queries.h>

extern "C" {
void* doc_str_to_doc_words(const char* doc_str);

void* init_cache(void *document_words);

void* init_ids();

unsigned int queries_size(void *queries);

void* query_by_index(void *queries, unsigned int index);

bool match_query(void* queries, unsigned int index, void* document_words, void* cache);

void add_ids(void* queries, unsigned int index, void* new_ids);

unsigned int ids_to_array(void* ids, unsigned int** result_array);

void start_query(void* queries, QueryID query_id, const char* query_str, MatchType match_type, unsigned int match_dist);

void end_query(void* queries, QueryID query_id);

void* init_queries();
}