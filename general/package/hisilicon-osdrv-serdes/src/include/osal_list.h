#ifndef _OSAL_LIST_H
#define _OSAL_LIST_H

#define OSAL_NULL            (0)

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

struct osal_list_head {
    struct osal_list_head *next, *prev;
};
#define OSAL_LIST_HEAD_INIT(name) \
    {                             \
        &(name), &(name)          \
    }

#define OSAL_LIST_HEAD(name) \
    struct osal_list_head name = OSAL_LIST_HEAD_INIT(name)

static inline void OSAL_INIT_LIST_HEAD(struct osal_list_head *list)
{
    list->next = list;
    list->prev = list;
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void osal___list_add(struct osal_list_head *new,
                                   struct osal_list_head *prev,
                                   struct osal_list_head *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void osal_list_add(struct osal_list_head *new, struct osal_list_head *head)
{
    osal___list_add(new, head, head->next);
}

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void osal_list_add_tail(struct osal_list_head *new, struct osal_list_head *head)
{
    osal___list_add(new, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void osal___list_del(struct osal_list_head *prev, struct osal_list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */
static inline void osal___list_del_entry(struct osal_list_head *entry)
{
    osal___list_del(entry->prev, entry->next);
}

#define OSAL_LIST_POISON1    ((void *)0x00100100)
#define OSAL_LIST_POISON2    ((void *)0x00200200)

static inline void osal_list_del(struct osal_list_head *entry)
{
    osal___list_del(entry->prev, entry->next);
    entry->next = OSAL_LIST_POISON1;
    entry->prev = OSAL_LIST_POISON2;
}

/**
 * list_replace - replace old entry by new one
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void osal_list_replace(struct osal_list_head *old,
                                     struct osal_list_head *new)
{
    new->next = old->next;
    new->next->prev = new;
    new->prev = old->prev;
    new->prev->next = new;
}

static inline void osal_list_replace_init(struct osal_list_head *old,
                                          struct osal_list_head *new)
{
    osal_list_replace(old, new);
    OSAL_INIT_LIST_HEAD(old);
}

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static inline void osal_list_del_init(struct osal_list_head *entry)
{
    osal___list_del_entry(entry);
    OSAL_INIT_LIST_HEAD(entry);
}

/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void osal_list_move(struct osal_list_head *list, struct osal_list_head *head)
{
    osal___list_del_entry(list);
    osal_list_add(list, head);
}

/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void osal_list_move_tail(struct osal_list_head *list,
                                       struct osal_list_head *head)
{
    osal___list_del_entry(list);
    osal_list_add_tail(list, head);
}

/**
 * list_is_last - tests whether @list is the last entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static inline int osal_list_is_last(const struct osal_list_head *list,
                                    const struct osal_list_head *head)
{
    return list->next == head;
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int osal_list_empty(const struct osal_list_head *head)
{
    return head->next == head;
}

/**
 * list_empty_careful - tests whether a list is empty and not being modified
 * @head: the list to test
 *
 * Description:
 * tests whether a list is empty _and_ checks that no other CPU might be
 * in the process of modifying either member (next or prev)
 *
 * NOTE: using list_empty_careful() without synchronization
 * can only be safe if the only activity that can happen
 * to the list entry is list_del_init(). Eg. it cannot be used
 * if another CPU could re-list_add() it.
 */
static inline int osal_list_empty_careful(const struct osal_list_head *head)
{
    struct osal_list_head *next = head->next;
    return (next == head) && (next == head->prev);
}

/**
 * list_rotate_left - rotate the list to the left
 * @head: the head of the list
 */
static inline void osal_list_rotate_left(struct osal_list_head *head)
{
    struct osal_list_head *first;

    if (!osal_list_empty(head)) {
        first = head->next;
        osal_list_move_tail(first, head);
    }
}

/**
 * list_is_singular - tests whether a list has just one entry.
 * @head: the list to test.
 */
static inline int osal_list_is_singular(const struct osal_list_head *head)
{
    return !osal_list_empty(head) && (head->next == head->prev);
}

static inline void osal___list_cut_position(struct osal_list_head *list,
                                            struct osal_list_head *head, struct osal_list_head *entry)
{
    struct osal_list_head *new_first = entry->next;
    list->next = head->next;
    list->next->prev = list;
    list->prev = entry;
    entry->next = list;
    head->next = new_first;
    new_first->prev = head;
}

/**
 * list_cut_position - cut a list into two
 * @list: a new list to add all removed entries
 * @head: a list with entries
 * @entry: an entry within head, could be the head itself
 *    and if so we won't cut the list
 *
 * This helper moves the initial part of @head, up to and
 * including @entry, from @head to @list. You should
 * pass on @entry an element you know is on @head. @list
 * should be an empty list or a list you do not care about
 * losing its data.
 *
 */
static inline void osal_list_cut_position(struct osal_list_head *list,
                                          struct osal_list_head *head, struct osal_list_head *entry)
{
    if (osal_list_empty(head)) {
        return;
    }
    if (osal_list_is_singular(head) &&
        ((head->next != entry) && (head != entry))) {
        return;
    }
    if (entry == head) {
        OSAL_INIT_LIST_HEAD(list);
    } else {
        osal___list_cut_position(list, head, entry);
    }
}

static inline void osal___list_splice(const struct osal_list_head *list,
                                      struct osal_list_head *prev,
                                      struct osal_list_head *next)
{
    struct osal_list_head *first = list->next;
    struct osal_list_head *last = list->prev;

    first->prev = prev;
    prev->next = first;

    last->next = next;
    next->prev = last;
}

/**
 * list_splice - join two lists, this is designed for stacks
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void osal_list_splice(const struct osal_list_head *list,
                                    struct osal_list_head *head)
{
    if (!osal_list_empty(list)) {
        osal___list_splice(list, head, head->next);
    }
}

/**
 * list_splice_tail - join two lists, each list being a queue
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void osal_list_splice_tail(struct osal_list_head *list,
                                         struct osal_list_head *head)
{
    if (!osal_list_empty(list)) {
        osal___list_splice(list, head->prev, head);
    }
}

/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
static inline void osal_list_splice_init(struct osal_list_head *list,
                                         struct osal_list_head *head)
{
    if (!osal_list_empty(list)) {
        osal___list_splice(list, head, head->next);
        OSAL_INIT_LIST_HEAD(list);
    }
}

/**
 * list_splice_tail_init - join two lists and reinitialise the emptied list
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * Each of the lists is a queue.
 * The list at @list is reinitialised
 */
static inline void osal_list_splice_tail_init(struct osal_list_head *list,
                                              struct osal_list_head *head)
{
    if (!osal_list_empty(list)) {
        osal___list_splice(list, head->prev, head);
        OSAL_INIT_LIST_HEAD(list);
    }
}

#undef osal_offsetof
#ifdef __compiler_offsetof
#define osal_offsetof(TYPE, MEMBER) __compiler_offsetof(TYPE, MEMBER)
#else
#define osal_offsetof(TYPE, MEMBER) ((int)(unsigned long)&((TYPE *)0)->MEMBER)
#endif

#define osal_container_of(ptr, type, member) ({          \
    const __typeof__( ((type *)0)->member ) *__mptr = (ptr);    \
   (type *)( (char *)__mptr - osal_offsetof(type,member) ); })

/**
 * list_entry - get the struct for this entry
 * @ptr:    the &struct list_head pointer.
 * @type:    the type of the struct this is embedded in.
 * @member:    the name of the list_struct within the struct.
 */
#define osal_list_entry(ptr, type, member) \
    osal_container_of(ptr, type, member)

/**
 * list_first_entry - get the first element from a list
 * @ptr:    the list head to take the element from.
 * @type:    the type of the struct this is embedded in.
 * @member:    the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define osal_list_first_entry(ptr, type, member) \
    osal_list_entry((ptr)->next, type, member)

/**
 * list_for_each    -    iterate over a list
 * @pos:    the &struct list_head to use as a loop cursor.
 * @head:    the head for your list.
 */
#define osal_list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * __list_for_each    -    iterate over a list
 * @pos:    the &struct list_head to use as a loop cursor.
 * @head:    the head for your list.
 *
 * This variant doesn't differ from list_for_each() any more.
 * We don't do prefetching in either case.
 */
#define osal___list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * list_for_each_prev    -    iterate over a list backwards
 * @pos:    the &struct list_head to use as a loop cursor.
 * @head:    the head for your list.
 */
#define osal_list_for_each_prev(pos, head) \
    for (pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:    the &struct list_head to use as a loop cursor.
 * @n:        another &struct list_head to use as temporary storage
 * @head:    the head for your list.
 */
#define osal_list_for_each_safe(pos, n, head)              \
    for (pos = (head)->next, n = pos->next; pos != (head); \
         pos = n, n = pos->next)

/**
 * list_for_each_prev_safe - iterate over a list backwards safe against removal of list entry
 * @pos:    the &struct list_head to use as a loop cursor.
 * @n:        another &struct list_head to use as temporary storage
 * @head:    the head for your list.
 */
#define osal_list_for_each_prev_safe(pos, n, head) \
    for (pos = (head)->prev, n = pos->prev;        \
         pos != (head);                            \
         pos = n, n = pos->prev)

/**
 * list_for_each_entry    -    iterate over list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:    the head for your list.
 * @member:    the name of the list_struct within the struct.
 */
#define osal_list_for_each_entry(pos, head, member)                     \
    for (pos = osal_list_entry((head)->next, __typeof__(*pos), member); \
         &pos->member != (head);                                        \
         pos = osal_list_entry(pos->member.next, __typeof__(*pos), member))

/**
 * list_for_each_entry_reverse - iterate backwards over list of given type.
 * @pos:    the type * to use as a loop cursor.
 * @head:    the head for your list.
 * @member:    the name of the list_struct within the struct.
 */
#define osal_list_for_each_entry_reverse(pos, head, member)             \
    for (pos = osal_list_entry((head)->prev, __typeof__(*pos), member); \
         &pos->member != (head);                                        \
         pos = osal_list_entry(pos->member.prev, __typeof__(*pos), member))

/**
 * list_prepare_entry - prepare a pos entry for use in list_for_each_entry_continue()
 * @pos:    the type * to use as a start point
 * @head:    the head of the list
 * @member:    the name of the list_struct within the struct.
 *
 * Prepares a pos entry for use as a start point in list_for_each_entry_continue().
 */
#define osal_list_prepare_entry(pos, head, member) \
    ((pos) ?: osal_list_entry(head, __typeof__(*pos), member))

/**
 * list_for_each_entry_continue - continue iteration over list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:    the head for your list.
 * @member:    the name of the list_struct within the struct.
 *
 * Continue to iterate over list of given type, continuing after
 * the current position.
 */
#define osal_list_for_each_entry_continue(pos, head, member)                \
    for (pos = osal_list_entry(pos->member.next, __typeof__(*pos), member); \
         &pos->member != (head);                                            \
         pos = osal_list_entry(pos->member.next, __typeof__(*pos), member))

/**
 * list_for_each_entry_continue_reverse - iterate backwards from the given point
 * @pos:    the type * to use as a loop cursor.
 * @head:    the head for your list.
 * @member:    the name of the list_struct within the struct.
 *
 * Start to iterate over list of given type backwards, continuing after
 * the current position.
 */
#define osal_list_for_each_entry_continue_reverse(pos, head, member)        \
    for (pos = osal_list_entry(pos->member.prev, __typeof__(*pos), member); \
         &pos->member != (head);                                            \
         pos = osal_list_entry(pos->member.prev, __typeof__(*pos), member))

/**
 * list_for_each_entry_from - iterate over list of given type from the current point
 * @pos:    the type * to use as a loop cursor.
 * @head:    the head for your list.
 * @member:    the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing from current position.
 */
#define osal_list_for_each_entry_from(pos, head, member) \
    for (; &pos->member != (head);                       \
         pos = osal_list_entry(pos->member.next, __typeof__(*pos), member))

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:    the type * to use as a loop cursor.
 * @n:        another type * to use as temporary storage
 * @head:    the head for your list.
 * @member:    the name of the list_struct within the struct.
 */
#define osal_list_for_each_entry_safe(pos, n, head, member)              \
    for (pos = osal_list_entry((head)->next, __typeof__(*pos), member),  \
        n = osal_list_entry(pos->member.next, __typeof__(*pos), member); \
         &pos->member != (head);                                         \
         pos = n, n = osal_list_entry(n->member.next, __typeof__(*n), member))

/**
 * list_for_each_entry_safe_continue - continue list iteration safe against removal
 * @pos:    the type * to use as a loop cursor.
 * @n:        another type * to use as temporary storage
 * @head:    the head for your list.
 * @member:    the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing after current point,
 * safe against removal of list entry.
 */
#define osal_list_for_each_entry_safe_continue(pos, n, head, member)        \
    for (pos = osal_list_entry(pos->member.next, __typeof__(*pos), member), \
        n = osal_list_entry(pos->member.next, __typeof__(*pos), member);    \
         &pos->member != (head);                                            \
         pos = n, n = osal_list_entry(n->member.next, __typeof__(*n), member))

/**
 * list_for_each_entry_safe_from - iterate over list from current point safe against removal
 * @pos:    the type * to use as a loop cursor.
 * @n:        another type * to use as temporary storage
 * @head:    the head for your list.
 * @member:    the name of the list_struct within the struct.
 *
 * Iterate over list of given type from current point, safe against
 * removal of list entry.
 */
#define osal_list_for_each_entry_safe_from(pos, n, head, member)          \
    for (n = osal_list_entry(pos->member.next, __typeof__(*pos), member); \
         &pos->member != (head);                                          \
         pos = n, n = osal_list_entry(n->member.next, __typeof__(*n), member))

/**
 * list_for_each_entry_safe_reverse - iterate backwards over list safe against removal
 * @pos:    the type * to use as a loop cursor.
 * @n:        another type * to use as temporary storage
 * @head:    the head for your list.
 * @member:    the name of the list_struct within the struct.
 *
 * Iterate backwards over list of given type, safe against removal
 * of list entry.
 */
#define osal_list_for_each_entry_safe_reverse(pos, n, head, member)      \
    for (pos = osal_list_entry((head)->prev, __typeof__(*pos), member),  \
        n = osal_list_entry(pos->member.prev, __typeof__(*pos), member); \
         &pos->member != (head);                                         \
         pos = n, n = osal_list_entry(n->member.prev, __typeof__(*n), member))

/**
 * list_safe_reset_next - reset a stale list_for_each_entry_safe loop
 * @pos:    the loop cursor used in the list_for_each_entry_safe loop
 * @n:        temporary storage used in list_for_each_entry_safe
 * @member:    the name of the list_struct within the struct.
 *
 * list_safe_reset_next is not safe to use in general if the list may be
 * modified concurrently (eg. the lock is dropped in the loop body). An
 * exception to this is if the cursor element (pos) is pinned in the list,
 * and list_safe_reset_next is called after re-taking the lock and before
 * completing the current iteration of the loop body.
 */
#define osal_list_safe_reset_next(pos, n, member) \
    n = osal_list_entry(pos->member.next, __typeof__(*pos), member)

/*
 * Double linked lists with a single pointer list head.
 * Mostly useful for hash tables where the two pointer list head is
 * too wasteful.
 * You lose the ability to access the tail in O(1).
 */
struct osal_hlist_node {
    struct osal_hlist_node *next, **pprev;
};
struct osal_hlist_head {
    struct osal_hlist_node *first;
};

#define OSAL_HLIST_HEAD_INIT \
    {                        \
        .first = OSAL_NULL   \
    }
#define OSAL_HLIST_HEAD(name) struct osal_hlist_head name = { .first = OSAL_NULL }
#define INIT_OSAL_HLIST_HEAD(ptr) ((ptr)->first = OSAL_NULL)
static inline void INIT_OSAL_HLIST_NODE(struct osal_hlist_node *h)
{
    h->next = OSAL_NULL;
    h->pprev = OSAL_NULL;
}

static inline int osal_hlist_unhashed(const struct osal_hlist_node *h)
{
    return !h->pprev;
}

static inline int osal_hlist_empty(const struct osal_hlist_head *h)
{
    return !h->first;
}

static inline void osal___hlist_del(struct osal_hlist_node *n)
{
    struct osal_hlist_node *next = n->next;
    struct osal_hlist_node **pprev = n->pprev;
    *pprev = next;
    if (next) {
        next->pprev = pprev;
    }
}

static inline void osal_hlist_del(struct osal_hlist_node *n)
{
    osal___hlist_del(n);
    n->next = OSAL_LIST_POISON1;
    n->pprev = OSAL_LIST_POISON2;
}

static inline void osal_hlist_del_init(struct osal_hlist_node *n)
{
    if (!osal_hlist_unhashed(n)) {
        osal___hlist_del(n);
        INIT_OSAL_HLIST_NODE(n);
    }
}

static inline void osal_hlist_add_head(struct osal_hlist_node *n, struct osal_hlist_head *h)
{
    struct osal_hlist_node *first = h->first;
    n->next = first;
    if (first) {
        first->pprev = &n->next;
    }
    h->first = n;
    n->pprev = &h->first;
}

/* next must be != NULL */
static inline void osal_hlist_add_before(struct osal_hlist_node *n,
                                         struct osal_hlist_node *next)
{
    n->pprev = next->pprev;
    n->next = next;
    next->pprev = &n->next;
    *(n->pprev) = n;
}

static inline void osal_hlist_add_after(struct osal_hlist_node *n,
                                        struct osal_hlist_node *next)
{
    next->next = n->next;
    n->next = next;
    next->pprev = &n->next;

    if (next->next) {
        next->next->pprev = &next->next;
    }
}

/* after that we'll appear to be on some hlist and hlist_del will work */
static inline void osal_hlist_add_fake(struct osal_hlist_node *n)
{
    n->pprev = &n->next;
}

/*
 * Move a list from one list head to another. Fixup the pprev
 * reference of the first entry if it exists.
 */
static inline void osal_hlist_move_list(struct osal_hlist_head *old,
                                        struct osal_hlist_head *new)
{
    new->first = old->first;
    if (new->first) {
        new->first->pprev = &new->first;
    }
    old->first = OSAL_NULL;
}

#define osal_hlist_entry(ptr, type, member) osal_container_of(ptr, type, member)

#define osal_hlist_for_each(pos, head) \
    for (pos = (head)->first; pos; pos = pos->next)

#define osal_hlist_for_each_safe(pos, n, head) \
    for (pos = (head)->first; pos && ({ n = pos->next; 1; });    \
         pos = n)

/**
 * hlist_for_each_entry    - iterate over list of given type
 * @tpos:    the type * to use as a loop cursor.
 * @pos:    the &struct hlist_node to use as a loop cursor.
 * @head:    the head for your list.
 * @member:    the name of the hlist_node within the struct.
 */
#define osal_hlist_for_each_entry(tpos, pos, head, member) \
    for (pos = (head)->first;                              \
         pos &&                                            \
         ({ tpos = osal_hlist_entry(pos, __typeof__(*tpos), member); 1; });                                            \
         pos = pos->next)

/**
 * hlist_for_each_entry_continue - iterate over a hlist continuing after current point
 * @tpos:    the type * to use as a loop cursor.
 * @pos:    the &struct hlist_node to use as a loop cursor.
 * @member:    the name of the hlist_node within the struct.
 */
#define osal_hlist_for_each_entry_continue(tpos, pos, member) \
    for (pos = (pos)->next;                                   \
         pos &&                                               \
         ({ tpos = osal_hlist_entry(pos, __typeof__(*tpos), member); 1; });                                               \
         pos = pos->next)

/**
 * hlist_for_each_entry_from - iterate over a hlist continuing from current point
 * @tpos:    the type * to use as a loop cursor.
 * @pos:    the &struct hlist_node to use as a loop cursor.
 * @member:    the name of the hlist_node within the struct.
 */
#define osal_hlist_for_each_entry_from(tpos, pos, member) \
    for (; pos &&                                         \
           ({ tpos = osal_hlist_entry(pos, __typeof__(*tpos), member); 1; });                                         \
         pos = pos->next)

/**
 * hlist_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @tpos:    the type * to use as a loop cursor.
 * @pos:    the &struct hlist_node to use as a loop cursor.
 * @n:        another &struct hlist_node to use as temporary storage
 * @head:    the head for your list.
 * @member:    the name of the hlist_node within the struct.
 */
#define osal_hlist_for_each_entry_safe(tpos, pos, n, head, member) \
    for (pos = (head)->first;                                      \
         pos && ({ n = pos->next; 1; }) &&                                           \
         ({ tpos = osal_hlist_entry(pos, __typeof__(*tpos), member); 1; });                                                    \
         pos = n)

#endif
