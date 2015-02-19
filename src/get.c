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

#define DATETIME_LEN 20 /* Enough space to hold a DATETIME plus a NUL byte */

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
patts_get_child_items (char **out, const char *id)
{
  int rc;
  const char *fmt = "SELECT startTime,stopTime FROM TaskItem WHERE id=%s"
  char *query, *result, *old_start, *old_stop;
  json_t *result_arr, *result_obj;
  size_t qlen = 1;

  qlen += strlen (fmt) - 2;
  qlen += strlen (id);

  query = sqon_malloc (qlen * sizeof (char));
  if (NULL == query)
    return PATTS_MEMORYERROR;

  old_start = sqon_malloc (DATETIME_LEN * sizeof (char));
  if (NULL == old_start)
    {
      sqon_free (query);
      return PATTS_MEMORYERROR;
    }

  old_stop = sqon_malloc (DATETIME_LEN * sizeof (char));
  if (NULL == old_stop)
    {
      sqon_free (query);
      sqon_free (old_start);
      return PATTS_MEMORYERROR;
    }

  snprintf (query, qlen, fmt, id);

  rc = sqon_query (patts_get_db (), query, &result, NULL);
  sqon_free (query);

  if (rc)
    {
      sqon_free (old_start);
      sqon_free (old_stop);
      return rc;
    }

  result_arr = json_loads (result);
  sqon_free (result);

  result_obj = json_array_get (result_arr, 0);

  strcpy (old_start,
	  json_string_value (json_object_get (result_obj, "startTime")));
  strcpy (old_stop,
	  json_string_value (json_object_get (result_obj, "stopTime")));

  json_decref (result_arr);

  fmt = "SELECT * FROM TaskType WHERE startTime>='%s' AND stopTime<='%s' "
    "AND userID=%s";

  qlen = 1 + strlen (fmt) - 6;
  qlen += DATETIME_LEN * 2;
  qlen += strlen (patts_get_user ());

  query = sqon_malloc (qlen * sizeof (char));
  if (NULL == query)
    {
      sqon_free (old_start);
      sqon_free (old_stop);
      return PATTS_MEMORYERROR;
    }

  snprintf (query, qlen, fmt, old_start, old_stop, patts_get_user ());
  sqon_free (old_start);
  sqon_free (old_stop);

  rc = sqon_query (patts_get_db (), query, &result, "id");
  sqon_free (query);

  if (rc)
    return rc;

  result_obj = json_loads (result);
  sqon_free (result);

  rc = json_object_del (result_obj, id); // remove parent from child list

  if (!rc)
    {
      *out = json_dumps (result_obj, JSON_PRESERVE_ORDER);
      if (NULL == *out)
	rc = PATTS_MEMORYERROR;
    }

  json_decref (result_obj);
  return rc;
}
