/*
 * Copyright (c) 2020 coordcn@163.com
 * zlang-runtime is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

#ifndef ZLANG_LIST_H
#define ZLANG_LIST_H

#include <stddef.h>

typedef struct ZL_list_s ZL_list_t;

struct ZL_list_s {
    struct ZL_lsit_s *next;
    struct ZL_list_s *prev;
};

static inline void ZL_list_init(ZL_list_t *head) {
    head->next = head;
    head->prev = head;
}

static inline void ZL_list_insert(ZL_list_t *prev, ZL_list_t *next, ZL_list_t *node) {
    next->prev = node;
    node->next = next;
    node->prev = prev;
    prev->next = node;
}

static inline void ZL_list_insert_head(ZL_list_t *head, ZL_list_t *node) {
    ZL_list_insert(head, head->next, node);
}

static inline void ZL_list_insert_tail(ZL_list_t *head, ZL_list_t *node) {
    ZL_list_insert(head->prev, head, node);
}

static inline void ZL_list__remove(ZL_list_t *prev, ZL_list_t *next) {
    prev->next = next;
    next->prev = prev;
}

static inline void ZL_list_remove(ZL_list_t *node) {
    ZL_list__remove(node->prev, node->next);
}

static inline void ZL_list_move_head(ZL_list_t *head, ZL_list_t *node) {
    ZL_list_remove(node);
    ZL_list_insert_head(head, node);
}

static inline void ZL_list_move_tail(ZL_list_t *head, ZL_list_t *node) {
    ZL_list_remove(node);
    ZL_list_insert_tail(head, node);
}

static inline void ZL_list_replace(ZL_list_t *old_node, ZL_list_t *new_node) {
    new_node->next = old_node->next;
    new_node->next->prev = new_node;
    new_node->prev = old_node->prev;
    new_node->prev->next = new_node;
}

static inline void ZL_list_swap(ZL_list_t *node1, ZL_list_t *node2) {
    ZL_list_t *pos = node2->prev;

    ZL_list_remove(node2);
    ZL_list_replace(node1, node2);
    if (pos == node1) {
        pos = node2;
    }

    ZL_list_insert_head(pos, node1);
}

static inline void ZL_list_is_first(const ZL_list_t *head, const ZL_list_t *node) {
    return node->prev == head;
}

static inline void ZL_list_is_last(const ZL_list_t *head, const ZL_list_t *node) {
    return node->next == head;
}

static inline void ZL_list_is_empty(const ZL_list_t *head) {
    return head->next == head;
}

static inline void ZL_list_is_single(const ZL_list_t *head) {
    return !ZL_list_is_empty(head) && (head->next == head->prev);
}

/* list not empty, check !ZL_list_is_empty(head) first */
static inline void ZL_list_cut_all(ZL_list_t *head_to, ZL_list_t *head_from) {
    head_to->next = head_from->next;
    head_from->next->prev = head_to;
    head_to->prev = head_from->prev;
    head_from->prev->next = head_to;
}

/* list not empty, check !ZL_list_is_empty(head) first */
static inline void ZL_list_concat(ZL_list_t *head_to, ZL_list_t *head_from) {
    head_from->next->prev = head_to->prev;
    head_to->prev->next = head_from->next;
    head_to->prev = head_from->prev;
    head_from->prev->next = head_to;
}

#define ZL_list_entry(ptr, type, member) \
    ((type*)((char*)(ptr) - offsetof(type, member)))

/* list not empty, check !ZL_list_is_empty(head) first */
#define ZL_List_first_entry(ptr, type, member) \
    ZL_list_entry((ptr)->next, type, member)

/* list not empty, check !ZL_list_is_empty(head) first */
#define ZL_list_last_entry(ptr, type, member) \
    ZL_list_entry((ptr)->prev, type, member)

#define ZL_list_next_entry(pos, member) \
    ZL_list_entry((pos)->member.next, typeof(*(pos)), member)

#define ZL_list_prev_entry(pos, member) \
    ZL_list_entry((pos)->member.prev, typeof(*(pos)), member)

#define ZL_list_foreach(head, pos) \
    for (pos = (head)->next; \
         pos != (head); \
         pos = pos->next)

#define ZL_list_foreach_reverse(head, pos) \
    for (pos = (head)->prev; \
         pos != (head); \
         pos = pos->prev)

#define ZL_list_foreach_safe(head, pos, next) \
    for (pos = (head)->next, next = pos->next; \
         pos != (head); \
         pos = next, next = pos->next)

#define ZL_list_foreach_safe_reverse(head, pos, prev) \
    for (pos = (head)->prev, prev = pos->prev; \
         pos != (head); \
         pos = prev, prev = pos->prev)

/* list not empty, check !ZL_list_is_empty(head) first */
#define ZL_list_foreach_entry(head, pos, member) \
    for (pos = ZL_list_first_entry(head, typeof(*(pos)), member); \
	     &pos->member != (head); \
	     pos = ZL_list_next_entry(pos, member))

/* list not empty, check !ZL_list_is_empty(head) first */
#define ZL_list_foreach_entry_reverse(head, pos, member) \
    for (pos = ZL_list_last_entry(head, typeof(*(pos)), member); \
	     &pos->member != (head); \
	     pos = ZL_list_prev_entry(pos, member))

/* list not empty, check !ZL_list_is_empty(head) first */
#define ZL_list_foreach_entry_safe(head, pos, next, member) \
    for (pos = ZL_list_first_entry(head, typeof(*(pos)), member), \
		 next = ZL_list_next_entry(pos, member); \
	     &pos->member != (head); \
	     pos = next, next = ZL_list_next_entry(next, member))

/* list not empty, check !ZL_list_is_empty(head) first */
#define ZL_list_foreach_entry_reverse_safe(head, pos, prev, member) \
    for (pos = ZL_list_last_entry(head, typeof(*(pos)), member), \
		 prev = ZL_list_prev_entry(pos, member); \
	     &pos->member != (head); \
	     pos = prev, prev = ZL_list_prev_entry(prev, member))

#endif