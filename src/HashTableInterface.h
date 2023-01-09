//
// Created by Ali Allahyar on 07.01.23.
//

#ifndef VDSPROJECT_HASHTABLEINTERFACE_H
#define VDSPROJECT_HASHTABLEINTERFACE_H

#include <string>
#include <set>
#include "Manager.h"

namespace ClassProject {

    typedef struct Ht_item Ht_item;
    typedef struct LinkedList LinkedList;
    typedef struct HashTable HashTable;

    class HashTableInterface {
    public:
        virtual unsigned long hash_function(BDD_ID key) = 0;

        virtual Ht_item* create_item(BDD_ID key, Node* value) = 0;

        virtual HashTable* create_table(int size) = 0;

        virtual void free_item(Ht_item* item) = 0;

        virtual void free_table(HashTable* table) = 0;

        virtual void handle_collision(HashTable* table, unsigned long index, Ht_item* item) = 0;

        virtual void ht_insert(HashTable* table, Ht_item* item, unsigned long index) = 0;

        virtual ClassProject::Node* ht_find(HashTable* table, unsigned long key) = 0;

        virtual void ht_remove(HashTable* table, BDD_ID key) = 0;

        virtual std::string node_to_string(Node* node) = 0;

        virtual void print_search(HashTable* table, BDD_ID key) = 0;

        virtual void print_table()  = 0;

        virtual void add_computed_table(BDD_ID key, Node* node) = 0;
    };
}

#endif
