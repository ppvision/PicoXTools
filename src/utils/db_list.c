
#include "db_list.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void list_init(list_t *list) {
  list->head = NULL;
  list->tail = NULL;
}

void list_append(list_t *list, list_node_t *node) {
  if (list->tail == NULL && list->head == NULL) {
    list->head = node;
    list->tail = node;
    node->next = NULL;
    node->prev = NULL;
  } else {
    list->tail->next = node;
    node->prev = list->tail;
    node->next = NULL;
    list->tail = node;
  }
}

void list_remove(list_t *list, list_node_t *node) {
  if (list->head == node) {
    list->head = node->next;
  }
  if (list->tail == node) {
    list->tail = node->prev;
  }
  if (node->prev != NULL) {
    node->prev->next = node->next;
  }
  if (node->next != NULL) {
    node->next->prev = node->prev;
  }
}

