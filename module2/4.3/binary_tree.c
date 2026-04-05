#include "binary_tree.h"
#include <stdlib.h>
#include <stdio.h>

#define BALANCE_THRESHOLD 10

void init_binary_tree(binary_tree *tree)
{
    tree->root = NULL;
}

unsigned int get_height(node *n)
{
    if (n == NULL)
    {
        return 0;
    }
    int left_height = get_height(n->left);
    int right_height = get_height(n->right);
    return 1 + (left_height > right_height ? left_height : right_height);
}

node *new_node(void *value)
{
    node *n = (node *)malloc(sizeof(node));
    if (n != NULL)
    {
        n->value = value;
        n->left = NULL;
        n->right = NULL;
    }
    return n;
}

node *rotate_right(node *y)
{
    node *x = y->left;
    node *z = x->right;

    x->right = y;
    y->left = z;

    return x;
}

node *rotate_left(node *x)
{
    node *y = x->right;
    node *z = y->left;

    y->left = x;
    x->right = z;

    return y;
}

int get_balance(node *n)
{
    if (n == NULL)
    {
        return 0;
    }
    return get_height(n->left) - get_height(n->right);
}

node *insert_node(node *root, void *value, int (*compare)(const void *c1, const void *c2))
{
    if (root == NULL)
    {
        return new_node(value);
    }

    int cmp = compare(value, root->value);
    if (cmp < 0)
    {
        root->left = insert_node(root->left, value, compare);
    }
    else if (cmp > 0)
    {
        root->right = insert_node(root->right, value, compare);
    }
    else
    {
        return root;
    }

    int balance = get_balance(root);

    if (balance > 1 && compare(value, root->left->value) < 0)
    {
        return rotate_right(root);
    }

    if (balance < -1 && compare(value, root->right->value) > 0)
    {
        return rotate_left(root);
    }

    if (balance > 1 && compare(value, root->left->value) > 0)
    {
        root->left = rotate_left(root->left);
        return rotate_right(root);
    }

    if (balance < -1 && compare(value, root->right->value) < 0)
    {
        root->right = rotate_right(root->right);
        return rotate_left(root);
    }

    return root;
}

int insert_into_tree(binary_tree *tree, void *value, int (*compare)(const void *c1, const void *c2))
{
    tree->root = insert_node(tree->root, value, compare);
    return 0;
}

node *delete_node(node *root, void *value, int (*compare)(const void *c1, const void *c2), void (*delete_value)(void *c), int should_be_freed)
{
    if (root == NULL)
    {
        return root;
    }

    int cmp = compare(value, root->value);
    if (cmp < 0)
    {
        root->left = delete_node(root->left, value, compare, delete_value, should_be_freed);
    }
    else if (cmp > 0)
    {
        root->right = delete_node(root->right, value, compare, delete_value, should_be_freed);
    }
    else
    {
        if (root->left == NULL)
        {
            node *temp = root->right;

            if (should_be_freed)
            {
                free_value(root, delete_value);
            }
            else
            {
                free(root);
            }

            root = temp;
        }
        else if (root->right == NULL)
        {
            node *temp = root->left;

            if (should_be_freed)
            {
                free_value(root, delete_value);
            }
            else
            {
                free(root);
            }

            root = temp;
        }
        else
        {
            node *successor = root->right;

            while (successor->left != NULL)
            {
                successor = successor->left;
            }

            if (should_be_freed)
            {
                (*delete_value)(root->value);
            }
            root->value = successor->value;

            root->right = delete_node(root->right, successor->value, compare, delete_value, 0);
        }
    }

    if (root == NULL)
    {
        return root;
    }

    int balance = get_balance(root);

    if (balance > 1 && get_balance(root->left) >= 0)
    {
        return rotate_right(root);
    }

    if (balance < -1 && get_balance(root->right) <= 0)
    {
        return rotate_left(root);
    }

    if (balance > 1 && get_balance(root->left) < 0)
    {
        root->left = rotate_left(root->left);
        return rotate_right(root);
    }

    if (balance < -1 && get_balance(root->right) > 0)
    {
        root->right = rotate_right(root->right);
        return rotate_left(root);
    }

    return root;
}

int delete_from_tree(binary_tree *tree, void *value, int (*compare)(const void *c1, const void *c2), void (*delete_value)(void *c), int should_be_freed)
{
    tree->root = delete_node(tree->root, value, compare, delete_value, should_be_freed);
    return 0;
}

void free_value(node *node, void (*delete_value)(void *c))
{
    (*delete_value)(node->value);
    free(node->value);
    free(node);
}

void delete_tree(binary_tree *tree, void (*delete_value)(void *c))
{
    delete_node_inorder(tree->root, delete_value);
}

void delete_node_inorder(node *current, void (*delete_value)(void *c))
{
    if (current == NULL)
    {
        return;
    }

    delete_node_inorder(current->left, delete_value);
    delete_node_inorder(current->right, delete_value);

    free_value(current, delete_value);
}

node *find_node(binary_tree *tree, void *value, int (*compare)(const void *c1, const void *c2))
{
    node *current = tree->root;
    while (current != NULL)
    {
        int cmp = compare(value, current->value);
        if (cmp == 0)
        {
            return current;
        }
        else if (cmp < 0)
        {
            current = current->left;
        }
        else
        {
            current = current->right;
        }
    }
    return NULL;
}

void *find_value(binary_tree *tree, void *value, int (*compare)(const void *c1, const void *c2))
{
    node *current = find_node(tree, value, compare);
    return current ? current->value : NULL;
}