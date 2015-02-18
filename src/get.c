/*
 *  libpatts - Backend library for PATTS Ain't Time Tracking Software
 *  Copyright (C) 2014-2015 Delwink, LLC
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "patts.h"
#include "get.h"

static int
get_all (char **out, const char *table)
{
  int rc;
  const char *fmt = "SELECT * FROM %s";
  char *query;
  size_t qlen = 1;

  qlen += strlen (fmt) - 2;
  qlen += strlen (table);

  query = sqon_malloc (qlen * sizeof (char));
  if (NULL == query)
    return PATTS_MEMORYERROR;

  snprintf (query, qlen, fmt, table);

  rc = sqon_query (patts_get_db (), query, out, "id");
  sqon_free (query);

  return rc;
}

static int
get_where (char **out, const char *table, const char *where)
{
  int rc;
  const char *fmt = "SELECT * FROM %s WHERE %s";
  char *query;
  size_t qlen = 1;

  qlen += strlen (fmt) - 4;
  qlen += strlen (table);
  qlen += strlen (where);

  query = sqon_malloc (qlen * sizeof (char));
  if (NULL == query)
    return PATTS_MEMORYERROR;

  snprintf (query, qlen, fmt, table, where);

  rc = sqon_query (patts_get_db (), query, out, "id");
  sqon_free (query);

  return rc;
}

static int
get_by_id (char **out, const char *table, const char *id)
{
  int rc;
  const char *wherefmt = "id=%s";
  char *where;
  size_t wlen = 1;

  wlen += strlen (wherefmt) - 2;
  wlen += strlen (id);

  where = sqon_malloc (wlen * sizeof (char));
  if (NULL == where)
    return PATTS_MEMORYERROR;

  snprintf (where, wlen, wherefmt, id);

  rc = get_where (out, table, where);
  sqon_free (where);

  return rc;
}

static int
get_children (char **out, const char *table, const char *parent_id)
{
  int rc;
  const char *fmt = "SELECT * FROM %s WHERE parentID=%s";
  char *query;
  size_t qlen = 1;

  qlen += strlen (fmt) - 4;
  qlen += strlen (table);
  qlen += strlen (parent_id);

  query = sqon_malloc (qlen * sizeof (char));
  if (NULL == query)
    return PATTS_MEMORYERROR;

  snprintf (query, qlen, fmt, table, parent_id);

  rc = sqon_query (patts_get_db (), query, out, "id");
  sqon_free (query);

  return rc;
}

int
patts_get_users (char **out)
{
  return get_all (out, "User");
}

int
patts_get_user_byid (char **out, const char *id)
{
  return get_by_id (out, "User", id);
}

int
patts_get_types (char **out)
{
  return get_all (out, "TaskType");
}

int
patts_get_type_byid (char **out, const char *id)
{
  return get_by_id (out, "TaskType", id);
}

int
patts_get_child_types (char **out, const char *parent_id)
{
  return get_children (out, "TaskType", parent_id);
}

int
patts_get_items (char **out)
{
  return get_all (out, "TaskItem");
}

int
patts_get_item_byid (char **out, const char *id)
{
  return get_by_id (out, "TaskItem", id);
}

int
patts_get_last_item (size_t *out, const char *user_id)
{
  int rc;
  const char *fmt = "SELECT id FROM TaskItem WHERE userID=%s "
    "ORDER BY id DESC LIMIT 1";
  char *result, *query;
  size_t qlen = 1;

  qlen += strlen (fmt) - 2;
  qlen += strlen (user_id);

  query = sqon_malloc (qlen * sizeof (char));
  if (NULL == query)
    return PATTS_MEMORYERROR;

  snprintf (query, qlen, fmt, user_id);

  rc = sqon_query (patts_get_db (), query, &result, NULL);
  sqon_free (query);

  if (rc)
    return rc;

  rc = sscanf (result, "[{\"id\": \"%u\"}]", out);
  sqon_free (result);

  if (!rc)
    return PATTS_UNEXPECTED;

  return 0;
}

int
patts_get_items_byuser (char **out, const char *user_id)
{
  int rc;
  const char *fmt = "SELECT * FROM TaskItem WHERE userID=%s";
  char *query;
  size_t qlen = 1;

  qlen += strlen (fmt) - 2;
  qlen += strlen (user_id);

  query = sqon_malloc (qlen * sizeof (char));
  if (NULL == query)
    return PATTS_MEMORYERROR;

  snprintf (query, qlen, fmt, user_id);

  rc = sqon_query (patts_get_db (), query, out, "id");
  sqon_free (query);

  return rc;
}

int
patts_get_items_byuser_onclock (char **out, const char *user_id)
{
  int rc;
  const char *fmt = "SELECT * FROM TaskItem WHERE userID=%s AND onClock=1";
  char *query;
  size_t qlen = 1;

  qlen += strlen (fmt) - 2;
  qlen += strlen (user_id);

  query = sqon_malloc (qlen * sizeof (char));
  if (NULL == query)
    return PATTS_MEMORYERROR;

  snprintf (query, qlen, fmt, user_id);

  rc = sqon_query (patts_get_db (), query, out, "id");
  sqon_free (query);

  return rc;
}

int
patts_get_child_items (struct dlist **out, const char *id)
{
  if (id == NULL)
    return 1;

  char *s = calloc (patts_qlen (), sizeof (char));
  if (s == NULL)
    return -1;

  int rc = sprintf (s, "%s%s%s",
		    u8
		    "typeID,userID,startTime,stopTime FROM TaskItem WHERE ",
		    u8 "id=", id);
  if ((size_t) rc >= patts_qlen ())
    {
      free (s);
      return 2;
    }

  struct dlist *parent;
  rc = cq_select_query (patts_get_db (), &parent, s);
  if (rc)
    {
      free (s);
      return rc;
    }

  const char *typeID = parent->first->values[0],
    *user_id = parent->first->values[1],
    *startTime = parent->first->values[2],
    *stopTime = parent->first->values[3];

  struct dlist *child_types;
  rc = patts_get_child_types (&child_types, typeID);
  if (rc)
    {
      cq_free_dlist (parent);
      free (s);
      return rc;
    }

  size_t tid_index;
  rc = cq_field_to_index (child_types, u8 "id", &tid_index);
  if (rc)
    {
      cq_free_dlist (parent);
      cq_free_dlist (child_types);
      free (s);
      return 100;
    }

  char *tids = calloc ((patts_fmaxlen () + strlen (u8 "'',"))
		       * cq_dlist_size (child_types), sizeof (char));
  if (tids == NULL)
    {
      cq_free_dlist (parent);
      cq_free_dlist (child_types);
      free (s);
      return -3;
    }

  for (struct drow * row = child_types->first; row != NULL; row = row->next)
    {
      strcat (tids, u8 "'");
      strcat (tids, row->values[tid_index]);
      strcat (tids, u8 "'");
      if (row->next != NULL)
	strcat (tids, u8 ",");
    }

  rc = sprintf (s, "%s%s%s%s%s%s%s%s%s%s%s",
		u8 "startTime>='", startTime, u8 "',",
		u8 "stopTime<='", stopTime, u8 "',",
		u8 "typeID IN (", tids, u8 "),", u8 "userID=", user_id);
  cq_free_dlist (parent);
  cq_free_dlist (child_types);
  free (tids);
  if ((size_t) rc >= patts_qlen ())
    {
      free (s);
      return 101;
    }

  rc = cq_select_all (patts_get_db (), u8 "TaskItem", out, s);
  free (s);

  return rc;
}
