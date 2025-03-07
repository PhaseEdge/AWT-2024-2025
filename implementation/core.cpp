/*
 * core.cpp version 1.0
 * Copyright (c) 2013 KAUST - InfoCloud Group (All Rights Reserved)
 * Author: Amin Allam
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

 #include "../include/core.h"
 #include <cstring>
 #include <cstdlib>
 #include <cstdio>
 #include <vector>
 #include <string>
 #include <climits>
 #include <iostream>
 #include <sstream>
 #include <set>
 #include <algorithm>
 #include <queue>
 #include <thread>
 #include <mutex>
 #include <condition_variable>
 #include <functional>
 
 #include "queries.h"
 #include <cache.h>
 #include <distance.h>
 
 using namespace std;
 ///////////////////////////////////////////////////////////////////////////////////////////////
 
 // Keeps all query ID results associated with a dcoument
 struct Document
 {
     DocID doc_id;
     unsigned int num_res;
     QueryID* query_ids;
 };
 
 ///////////////////////////////////////////////////////////////////////////////////////////////
 
 // Keeps all currently active queries
 Queries queries;
 
 // Keeps all currently available results that has not been returned yet
 queue<Document> docs;
 
 mutex docs_mutex; // Mutex to protect access to the docs queue
 
 ///////////////////////////////////////////////////////////////////////////////////////////////
 
 ErrorCode InitializeIndex(){return EC_SUCCESS;}
 
 ///////////////////////////////////////////////////////////////////////////////////////////////
 
 ErrorCode DestroyIndex(){return EC_SUCCESS;}
 
 ///////////////////////////////////////////////////////////////////////////////////////////////
 
 /**
  * @brief Converts a string consisting of multiple words separated by spaces into a list of strings,
  * where each element is a single word.
  */
 void string_to_words(const string& str, set<string>& words) {
     istringstream stream(str);
     string word;
 
     while (stream >> word) {
         words.insert(word);
     }
 }
 
 ErrorCode StartQuery(QueryID query_id, const char* query_str, MatchType match_type, unsigned int match_dist)
 {
     string str(query_str);
     Query query(match_type, match_dist, str);
     queries.add(query_id, query);
 
     return EC_SUCCESS;
 }
 
 ///////////////////////////////////////////////////////////////////////////////////////////////
 
 // Remove this query from the active query set
 ErrorCode EndQuery(QueryID query_id)
 {
     queries.remove(query_id);
     return EC_SUCCESS;
 }
 
 ///////////////////////////////////////////////////////////////////////////////////////////////
 
 /**
  * @brief checks if every word in the query does match any word in the document
  * 
  * @return true if all words in query match any word in document,
  * @return false otherwise
  */
 bool matchQuery(const Query& query, const set<string>& document_words){
     unsigned int distance;
     switch (query.match_type){
         case MT_EXACT_MATCH:
             for(const string& query_word: query.words){
                 if (document_words.find(query_word) == document_words.end()) {
                     return false;
                 }
             }
             return true;
         case MT_HAMMING_DIST:
             for(const string& query_word: query.words){
                 for(const string& doc_word: document_words) {
                     distance = hammingDistance(query_word, doc_word);
                     if(distance <= query.match_dist) break;
                 }
                 if(distance > query.match_dist) return false;
             } 
             return true;
         case MT_EDIT_DIST:
             for(const string& query_word: query.words){
                 for(const string& doc_word: document_words) {
                     distance = editDistance(query_word, doc_word);
                     if(distance <= query.match_dist) break;
                 }
                 if(distance > query.match_dist) return false;
             }
             return true;
     }
     perror("query match type not allowed! ");
     return false;
 }
 
 ErrorCode MatchDocument(DocID doc_id, const char* doc_str)
 {
     //transform one big document string to set of word-strings
     string cur_doc_str(doc_str);
     set<string> document_words;
     string_to_words(cur_doc_str, document_words);
 
     //match all queries
     set<QueryID> ids;
     for(const Query &query: queries.getAllQuerys()){
         //match query (true for match and false for no match)
         if(matchQuery(query, document_words)){
             //store query_ids of matching queries
             set<QueryID> &set_ids = queries.getIDs(query);
             ids.insert(set_ids.begin(), set_ids.end());
         }
     }
 
     //create document object
     Document document;
     document.doc_id=doc_id;
     document.num_res=ids.size();
     document.query_ids = (QueryID*) malloc(document.num_res * sizeof(QueryID));
     unsigned int i = 0;
     for (const QueryID& id : ids) {
         document.query_ids[i++] = id;
     }
 
     //add document
     {
         lock_guard<mutex> lock(docs_mutex);
         docs.push(document);
     }
 
     return EC_SUCCESS;
 }
 
 class ThreadPool {
 public:
     ThreadPool(size_t num_threads);
     ~ThreadPool();
     void enqueue(function<void()> task);
 
 private:
     vector<thread> workers;
     queue<function<void()>> tasks;
     mutex queue_mutex;
     condition_variable condition;
     bool stop;
 };
 
 ThreadPool::ThreadPool(size_t num_threads) : stop(false) { // Thread Pool Constructor itself pretty basic and simple however it is very useful
     for (size_t i = 0; i < num_threads; ++i) {
         workers.emplace_back([this] {
             for (;;) {
                 function<void()> task;
                 {
                     unique_lock<mutex> lock(this->queue_mutex);
                     this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                     if (this->stop && this->tasks.empty()) return;
                     task = move(this->tasks.front());
                     this->tasks.pop();
                 }
                 task();
             }
         });
     }
 }
 
 ThreadPool::~ThreadPool() {
     {
         unique_lock<mutex> lock(queue_mutex);
         stop = true;
     }
     condition.notify_all();
     for (thread &worker : workers) worker.join();
 }
 
 void ThreadPool::enqueue(function<void()> task) {
     {
         unique_lock<mutex> lock(queue_mutex);
         tasks.push(move(task));
     }
     condition.notify_one();
 }
 
 ThreadPool thread_pool(thread::hardware_concurrency()); // This one uses all the available cores of the machine
 
 ErrorCode MatchDocumentAsync(DocID doc_id, const char* doc_str) {
     thread_pool.enqueue([doc_id, doc_str] {
         MatchDocument(doc_id, doc_str);
     });
     return EC_SUCCESS;
 }
 
 ///////////////////////////////////////////////////////////////////////////////////////////////
 
 // Get the first undeliverd resuilt from "docs" and return it
 ErrorCode GetNextAvailRes(DocID* p_doc_id, unsigned int* p_num_res, QueryID** p_query_ids)
 {	
     *p_doc_id=0; *p_num_res=0; *p_query_ids=0;
     if(docs.empty()) return EC_NO_AVAIL_RES;
     
     Document document = docs.front();
     *p_doc_id=document.doc_id;
     *p_num_res= document.num_res;
     *p_query_ids=document.query_ids;
     
     docs.pop();
     return EC_SUCCESS;
 }
 
 ///////////////////////////////////////////////////////////////////////////////////////////////