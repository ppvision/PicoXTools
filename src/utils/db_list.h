#pragma once

#include <stdint.h>

typedef struct list_node_s list_node_t;
typedef struct list_s list_t;

struct list_node_s {
  list_node_t *prev;
  list_node_t *next;
};

struct list_s {
  list_node_t *head;
  list_node_t *tail;
};

void list_init(list_t *list);
void list_append(list_t *list, list_node_t *node);
void list_remove(list_t *list, list_node_t *node);