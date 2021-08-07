#ifndef __HI_LIST_H__
#define __HI_LIST_H__

#ifndef _LINUX_LIST_H

#define INIT_LIST_HEAD(ptr) \
do {                        \
    (ptr)->next = (ptr);    \
    (ptr)->prev = (ptr);    \
} while (0)

#define LIST_HEAD_INIT(name) { &(name), &(name) }

struct list_head {
    struct list_head *next, *prev;
};

static inline void __list_add(struct list_head * _new, struct list_head * prev, struct list_head * next)
{
    next->prev = _new;
    _new->next = next;
    _new->prev = prev;
    prev->next = _new;
}

static inline void list_add(struct list_head *_new, struct list_head *head)
{
    __list_add(_new, head, head->next);
}

static inline void list_add_tail(struct list_head *_new, struct list_head *head)
{
    __list_add(_new, head->prev, head);
}

static inline void __list_del(struct list_head * prev, struct list_head * next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
}

static inline void list_del_init(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    INIT_LIST_HEAD(entry);
}

static inline void list_move(struct list_head *list, struct list_head *head)
{
    __list_del(list->prev, list->next);
    list_add(list, head);
}

static inline void list_move_tail(struct list_head *list,
                  struct list_head *head)
{
    __list_del(list->prev, list->next);
    list_add_tail(list, head);
}

static inline int list_empty(struct list_head *head)
{
    return head->next == head;
}

static inline void __list_splice(struct list_head *list,
                                 struct list_head *head)
{
    struct list_head *first = list->next;
    struct list_head *last = list->prev;
    struct list_head *at = head->next;

    first->prev = head;
    head->next = first;

    last->next = at;
    at->prev = last;
}

static inline void list_splice(struct list_head *list, struct list_head *head)
{
    if (!list_empty(list))
    {
        __list_splice(list, head);
    }
}

static inline void list_splice_init(struct list_head *list, struct list_head *head)
{
    if (!list_empty(list))
    {
        __list_splice(list, head);
        INIT_LIST_HEAD(list);
    }
}

#define list_entry(ptr, type, member) \
    ((type *)((uintptr_t)(ptr)-((unsigned long)(&((type *)1)->member) - 1)))

#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)

#define get_first_item(attached, type, member) \
    ((type *)((char *)((attached)->next)-(unsigned long)(&((type *)0)->member)))

#endif

#endif
