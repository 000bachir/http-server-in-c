#ifndef ROUTENODE_H
#define ROUTENODE_H

struct RouteNode {
  char *key;
  char *value;
  struct RouteNode *left, *right;
};

void dealloc_tree(struct RouteNode *node);
void in_order_trav(struct RouteNode *root);

struct RouteNode *searchRoutes(struct RouteNode *root, char *key);
struct RouteNode *addRoute(struct RouteNode *root, char *key, char *value);
struct RouteNode *initializeRoute(char *key, char *value);

#endif

// need to understand better the structs and the route node that i copied form
// this github repo
