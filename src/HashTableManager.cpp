//
// Created by Ali Allahyar on 07.01.23.
//

#include <cstring>
#include <iostream>
#include "HashTableManager.h"

using namespace ClassProject;

HashTableManager::HashTableManager() {
    computed_nodes = create_table(CAPACITY);
};

static LinkedList* allocate_list () {
    auto* list = (LinkedList*) malloc (sizeof(LinkedList));
    return list;
}

static LinkedList* linkedlist_insert(LinkedList* list, Ht_item* item) {
    // Inserts the item onto the Linked List
    if (list->next == nullptr) {
        LinkedList *node = allocate_list();
        node->item = item;
        node->next = nullptr;
        list->next = node;
        return list;
    }

    LinkedList* temp = list;
    while (temp->next->next) {
        temp = temp->next;
    }

    LinkedList* node = allocate_list();
    node->item = item;
    node->next = nullptr;
    temp->next = node;

    return list;
}

static Ht_item* linkedlist_remove(LinkedList* list) {
    // Removes the head from the linked list and returns the item of the popped element
    if (!list)
        return nullptr;
    if (!list->next)
        return nullptr;
    LinkedList* node = list->next;
    LinkedList* temp = list;
    temp->next = nullptr;
    list = node;
    Ht_item* it = nullptr;
    memcpy(temp->item, it, sizeof(Ht_item));
    free(temp->item);
    free(temp);
    return it;
}

static void free_linkedlist(LinkedList* list) {
    LinkedList* temp;
    while (list) {
        temp = list;
        list = list->next;
        free(temp->item);
        free(temp);
    }
}

static LinkedList** create_overflow_buckets(HashTable* table) {
    // Create the overflow buckets; an array of linkedlists
    auto** buckets = (LinkedList**) calloc (table->size, sizeof(LinkedList*));
    for (int i=0; i<table->size; i++)
        buckets[i] = nullptr;
    return buckets;
}

static void free_overflow_buckets(HashTable* table) {
    // Free all the overflow bucket lists
    LinkedList** buckets = table->overflow_buckets;
    for (int i=0; i<table->size; i++)
        free_linkedlist(buckets[i]);
    free(buckets);
}

unsigned long HashTableManager::hash_function(ClassProject::BDD_ID key) {
    return key % CAPACITY;
}

Ht_item* HashTableManager::create_item(ClassProject::BDD_ID key, ClassProject::Node* value) {
    // Creates a pointer to a new hash table item
    auto* item = (Ht_item*) malloc (sizeof(Ht_item));
    item->key = key;
    item->value = value;

    return item;
}

HashTable* HashTableManager::create_table(int size) {
    // Creates a new HashTable
    auto* table = (HashTable*) malloc (sizeof(HashTable));
    table->size = size;
    table->count = 0;
    // An array of items
    table->items = (Ht_item**) calloc (table->size, sizeof(Ht_item*));
    for (int i=0; i<table->size; i++)
        table->items[i] = nullptr;
    table->overflow_buckets = create_overflow_buckets(table);

    return table;
}

void HashTableManager::free_item(Ht_item* item) {
    free(item);
}

void HashTableManager::free_table(HashTable* table) {
    // Frees the table
    for (int i=0; i<table->size; i++) {
        Ht_item* item = table->items[i];
        if (item != nullptr)
            free_item(item);
    }

    free_overflow_buckets(table);
    free(table->items);
    free(table);
}

void HashTableManager::handle_collision(HashTable* table, unsigned long index, Ht_item* item) {
    LinkedList* head = table->overflow_buckets[index];

    if (head == nullptr) {
        // We need to create the list
        head = allocate_list();
        head->item = item;
        table->overflow_buckets[index] = head;
        return;
    }
    else {
        // Insert to the list
        table->overflow_buckets[index] = linkedlist_insert(head, item);
        return;
    }
}

void HashTableManager::ht_insert(HashTable* table, Ht_item* item, unsigned long index) {

    Ht_item* current_item = table->items[index];

    if (current_item == nullptr) {
        // Key does not exist.
        if (table->count == table->size) {
            // Hash Table Full
            printf("Insert Error: Hash Table is full\n");
            // Remove the created item
            free_item(item);
            return;
        }

        // Insert directly
        table->items[index] = item;
        table->count++;
    }

    else {
        // We only need to update value
        if (current_item->key == item->key) {
            table->items[index]->value = item->value;
            return;
        }

        else {
            // We have Collision
            handle_collision(table, index, item);
            return;
        }
    }
}

ClassProject::Node* HashTableManager::ht_find(HashTable* table, unsigned long key) {
    // Searches the key in the hashtable
    // and returns NULL if it doesn't exist
    unsigned long index = hash_function(key);
    Ht_item* item = table->items[index];
    LinkedList* head = table->overflow_buckets[index];

    // Ensure that we move to items which are not NULL
    while (item != nullptr) {
        if (item->key == key)
            return item->value;
        if (head == nullptr)
            return nullptr;
        item = head->item;
        head = head->next;
    }
    return nullptr;
}

void HashTableManager::ht_remove(HashTable* table, ClassProject::BDD_ID key) {
    // Removes an item from the table
    unsigned long index = hash_function(key);
    Ht_item* item = table->items[index];
    LinkedList* head = table->overflow_buckets[index];

    if (item == nullptr) {
        // Does not exist. Return
        return;
    }
    else {
        if (head == nullptr && item->key == key) {
            // No collision chain. Remove the item
            // and set table index to NULL
            table->items[index] = nullptr;
            free_item(item);
            table->count--;
            return;
        }
        else if (head != nullptr) {
            // Collision Chain exists
            if (item->key == key) {
                // Remove this item and set the head of the list as the new item
                free_item(item);
                LinkedList* node = head;
                head = head->next;
                node->next = nullptr;
                table->items[index] = create_item(node->item->key, node->item->value);
                free_linkedlist(node);
                table->overflow_buckets[index] = head;
                return;
            }

            LinkedList* curr = head;
            LinkedList* prev = nullptr;

            while (curr) {
                if (curr->item->key == key) {
                    if (prev == nullptr) {
                        // First element of the chain. Remove the chain
                        free_linkedlist(head);
                        table->overflow_buckets[index] = nullptr;
                        return;
                    }
                    else {
                        // This is somewhere in the chain
                        prev->next = curr->next;
                        curr->next = nullptr;
                        free_linkedlist(curr);
                        table->overflow_buckets[index] = head;
                        return;
                    }
                }
                curr = curr->next;
                prev = curr;
            }

        }
    }
}

std::string HashTableManager::node_to_string(ClassProject::Node* node) {
    return "Node[ " +
        std::to_string(node->high) + ", " +
        std::to_string(node->low) + ", " +
        std::to_string(node->top_var) + ", " +
        node->label +
        "]";
}

void HashTableManager::print_search(HashTable* table, ClassProject::BDD_ID key) {
    ClassProject::Node* val;
    if ((val = ht_find(table, key)) == nullptr) {
        printf("%s does not exist\n", std::to_string(key).c_str());
        return;
    }
    else {
        printf("Key:%s, Value:%s\n", std::to_string(key).c_str(), node_to_string(val).c_str());
    }
}

void HashTableManager::print_table() {
    printf("\n-------------------\n");
    for (int i = 0; i < computed_nodes->size; i++) {
        if (computed_nodes->items[i]) {
            std::cout << "Index: "<< i << ", Key: " << std::to_string(computed_nodes->items[i]->key) << ", Value: " << node_to_string(computed_nodes->items[i]->value);

            if (computed_nodes->overflow_buckets[i]) {
                std::cout << " => Overflow Bucket => ";
                LinkedList *head = computed_nodes->overflow_buckets[i];
                while (head) {
                    std::cout << "Key: "<< std::to_string(head->item->key)<< ", Value: " << node_to_string(head->item->value);
                    head = head->next;
                }
            }
            printf("\n");
        }
    }
    printf("-------------------\n");

}

void HashTableManager::add_computed_table(BDD_ID key, Node* value) {
    Ht_item* item = HashTableManager::create_item(key, value);
    unsigned long index = hash_function(key);
    HashTableManager::ht_insert(computed_nodes, item, index);
}






