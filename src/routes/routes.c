#include "./routes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct RouteNode *initializeRoute(char *key, char *value) {
  struct RouteNode *node = (struct RouteNode *)malloc(sizeof(struct RouteNode));
  node->key = key;
  node->value = value;

  node->left = node->right = NULL;
  return node;
}

struct RouteNode *addRoute(struct RouteNode *root, char *key, char *value) {
  if (root == NULL) {
    return initializeRoute(key, value);
  }

  if (strcmp(key, root->key) < 0) {
    root->left = addRoute(root->left, key, value);
  } else if (strcmp(key, root->key) > 0) {
    root->right = addRoute(root->right, key, value);
  }

  return root;
}

struct RouteNode *searchRoute(struct RouteNode *root, char *key) {
  if (root == NULL) {
    return NULL;
  }
  if (strcmp(root->key, key) == 0) {
    return root;
  } else if (strcmp(root->key, key) < 0) {
    return searchRoute(root->right, key);
  } else if (strcmp(root->key, key) > 0) {
    return searchRoute(root->left, key);
  }
}

void in_order_navigation(struct RouteNode *root) {
  if (root != NULL) {
    in_order_navigation(root->left);
    printf("\n%s ==> %s\n", root->key, root->value);
    in_order_navigation(root->right);
  }
}

void dealloc_tree(struct RouteNode *node) {
  if (node == NULL) {
    return;
    dealloc_tree(node->left);
    dealloc_tree(node->right);
    free(node);
  }
}
