#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include <stdlib.h>

struct node
{
    void *value;
    struct node *left;
    struct node *right;
};

typedef struct node node;

typedef struct
{
    node *root;
} binary_tree;

void init_binary_tree(binary_tree *tree);
unsigned int get_height(node *n);
node *new_node(void *value);
node *rotate_right(node *y);
node *rotate_left(node *x);
int get_balance(node *n);
node *insert_node(node *root, void *value, int (*compare)(const void *c1, const void *c2), int *inserted);
int insert_into_tree(binary_tree *tree, void *value, int (*compare)(const void *c1, const void *c2));
node *delete_node(node *root, void *value, int (*compare)(const void *c1, const void *c2), void (*delete_value)(void *c), int should_be_freed, int *deleted);
int delete_from_tree(binary_tree *tree, void *value, int (*compare)(const void *c1, const void *c2), void (*delete_value)(void *c), int should_be_freed);
void free_value(node *node, void (*delete_value)(void *c));
void delete_tree(binary_tree *tree, void (*delete_value)(void *c));
void delete_node_inorder(node *current, void (*delete_value)(void *c));
node *find_node(binary_tree *tree, void *value, int (*compare)(const void *c1, const void *c2));
void *find_value(binary_tree *tree, void *value, int (*compare)(const void *c1, const void *c2));

#endif