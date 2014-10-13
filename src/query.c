/*
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "query.h"

struct drow *patts_new_drow(size_t fieldc)
{
    struct drow *row = malloc(sizeof(struct drow));
    if (row == NULL)
        return NULL;
    row->values = calloc(fieldc, sizeof(char *));

    if (row->values == NULL) {
        free(row);
        return NULL;
    }

    row->prev = NULL;
    row->next = NULL;
    return row;
}

void patts_free_drow(struct drow *row)
{
    if (row == NULL)
        return;
    free(row->values);
    free(row);
}

void patts_drow_set(struct drow *row, char **values, size_t fieldc)
{
    for (size_t i = 0; i < fieldc; ++i) {
        row->values[i] = values[i];
    }
}

struct dlist *patts_new_dlist(size_t fieldc, const char **fieldnames)
{
    struct dlist *list = malloc(sizeof(struct dlist));
    if (list == NULL)
        return NULL;
    list->fieldc = fieldc;
    list->fieldnames = calloc(fieldc, sizeof(char *));

    if (list->fieldnames == NULL) {
        free(list);
        return NULL;
    }

    for (size_t i = 0; i < fieldc; ++i)
        list->fieldnames[i] = fieldnames[i];

    list->first = NULL;
    list->last = NULL;
    return list;
}

size_t patts_dlist_size(const struct dlist *list)
{
    size_t count = 0;
    struct drow *row = list->first;

    while (row != NULL) {
        ++count;
        row = row->next;
    }

    return count;
}

void patts_free_dlist(struct dlist *list)
{
    if (list == NULL)
        return;
    free(list->fieldnames);
    struct drow *row = list->first;
    struct drow *next;
    do {
        next = row->next;
        patts_free_drow(row);
    } while ((row = next) != NULL);
    free(list);
}

void patts_dlist_add(struct dlist *list, struct drow *row)
{
    if (list->last == NULL) {
        list->first = row;
        list->last = row;
        row->prev = NULL;
        row->next = NULL;
    } else {
        list->last->next = row;
        row->prev = list->last;
        list->last = row;
    }
}

void patts_dlist_remove(struct dlist *list, struct drow *row)
{
    if (row == NULL)
        return;

    if (row == list->first && row == list->last) {
        list->first = NULL;
        list->last = NULL;
    } else if (row == list->first) {
        list->first = row->next;
        list->first->prev = NULL;
    } else if (row == list->last) {
        list->last = row->prev;
        list->last->next = NULL;
    } else {
        struct drow *after = row->next;
        struct drow *before = row->prev;
        after->prev = before;
        before->next = after;
    }

    patts_free_drow(row);
}

struct drow *patts_dlist_at(struct dlist *list, size_t index)
{
    size_t count = 0;
    struct drow *row = list->first;
    while (row != NULL) {
        if (count == index)
            return row;
        row = row->next;
    }
    return NULL;
}
