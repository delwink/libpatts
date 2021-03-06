/*
 *  libpatts - Backend library for PATTS Ain't Time Tracking Software
 *  Copyright (C) 2014-2016 Delwink, LLC
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, version 3 only.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <jansson.h>

#include "get.h"
#include "internal.h"
#include "patts.h"
#include "user.h"

static int
get_all (char **out, const char *table, const char *pk)
{
  const char *fmt = "SELECT * FROM %s WHERE state=1";
  size_t qlen = 1;

  qlen += strlen (fmt) - 2;
  qlen += strlen (table);
  char query[qlen];

  snprintf (query, qlen, fmt, table);
  return patts_query (query, out, pk);
}

static int
get_where (char **out, const char *table, const char *where, const char *pk)
{
  const char *fmt = "SELECT * FROM %s WHERE state=1 AND %s";
  size_t qlen = 1;

  qlen += strlen (fmt) - 4;
  qlen += strlen (table);
  qlen += strlen (where);
  char query[qlen];

  snprintf (query, qlen, fmt, table, where);
  return patts_query (query, out, pk);
}

static int
get_by_id (char **out, const char *table, const char *id, const char *pk,
	   bool quote)
{
  int rc;
  const char *wherefmt = "%s=%s";
  char *esc_id;
  size_t wlen = 1;

  rc = patts_escape (id, &esc_id, quote);
  if (rc)
    return rc;

  wlen += strlen (wherefmt) - 2;
  wlen += strlen (pk);
  wlen += MAX_ID_LEN * 2;
  char where[wlen];

  snprintf (where, wlen, wherefmt, pk, esc_id);
  patts_free (esc_id);

  return get_where (out, table, where, pk);
}

static int
get_children (char **out, const char *table, const char *parent_id)
{
  int rc;
  const char *fmt = "SELECT * FROM %s WHERE state=1 AND parentID=%s";
  char *esc_parent;
  size_t qlen = 1;

  rc = patts_escape (parent_id, &esc_parent, false);
  if (rc)
    return rc;

  qlen += strlen (fmt) - 4;
  qlen += strlen (table);
  qlen += MAX_ID_LEN * 2;
  char query[qlen];

  snprintf (query, qlen, fmt, table, esc_parent);
  patts_free (esc_parent);

  return patts_query (query, out, "id");
}

int
patts_get_users (char **out)
{
  return get_all (out, "User", "dbUser");
}

int
patts_get_user_byid (char **out, const char *id)
{
  if (strlen (id) > USERNAME_LEN)
    return PATTS_OVERFLOW;

  return get_by_id (out, "User", id, "dbUser", true);
}

int
patts_get_types (char **out)
{
  return get_all (out, "TaskType", "id");
}

int
patts_get_type_byid (char **out, const char *id)
{
  if (strlen (id) > MAX_ID_LEN)
    return PATTS_OVERFLOW;

  return get_by_id (out, "TaskType", id, "id", false);
}

int
patts_get_child_types (char **out, const char *parent_id)
{
  if (strlen (parent_id) > MAX_ID_LEN)
    return PATTS_OVERFLOW;

  return get_children (out, "TaskType", parent_id);
}

int
patts_get_items (char **out)
{
  return get_all (out, "TaskItem", "id");
}

int
patts_get_item_byid (char **out, const char *id)
{
  if (strlen (id) > MAX_ID_LEN)
    return PATTS_OVERFLOW;

  return get_by_id (out, "TaskItem", id, "id", false);
}

int
patts_get_last_item (char **out, const char *user_id)
{
  int rc;
  const char *fmt = "SELECT id FROM TaskItem WHERE state=1 AND userID='%s' "
    "ORDER BY id DESC LIMIT 1";
  char *result, *esc_user;
  size_t qlen = 1;

  if (strlen (user_id) > USERNAME_LEN)
    return PATTS_OVERFLOW;

  rc = patts_escape (user_id, &esc_user, false);
  if (rc)
    return rc;

  qlen += strlen (fmt) - 2;
  qlen += strlen (esc_user);
  char query[qlen];

  snprintf (query, qlen, fmt, esc_user);
  patts_free (esc_user);

  rc = patts_query (query, &result, NULL);
  if (rc)
    return rc;

  if (!strcmp (result, "[]"))
    {
      *out = NULL;
      patts_free (result);
      return 0;
    }

  uint32_t id;
  rc = sscanf (result, "[{\"id\": \"%" SCNu32 "\"}]", &id);
  patts_free (result);

  if (rc != 1)
    return PATTS_UNEXPECTED;

  *out = patts_malloc ((MAX_ID_LEN + 1) * sizeof (char));
  if (NULL == *out)
    return PATTS_MEMORYERROR;

  snprintf (*out, MAX_ID_LEN + 1, "%" PRIu32, id);
  return 0;
}

static int
items_byuser (char **out, const char *user_id, const char *fmt)
{
  int rc;
  char *esc_user;
  size_t qlen = 1;

  if (strlen (user_id) > USERNAME_LEN)
    return PATTS_OVERFLOW;

  rc = patts_escape (user_id, &esc_user, false);
  if (rc)
    return rc;

  qlen += strlen (fmt) - 2;
  qlen += USERNAME_LEN;
  char query[qlen];

  snprintf (query, qlen, fmt, esc_user);
  patts_free (esc_user);

  return patts_query (query, out, "id");
}

int
patts_get_items_byuser (char **out, const char *user_id)
{
  if (strlen (user_id) > USERNAME_LEN)
    return PATTS_OVERFLOW;

  return items_byuser (out, user_id,
		       "SELECT * FROM TaskItem WHERE state=1 AND userID='%s'");
}

int
patts_get_items_byuser_onclock (char **out, const char *user_id)
{
  if (strlen (user_id) > USERNAME_LEN)
    return PATTS_OVERFLOW;

  return items_byuser (out, user_id, "SELECT * FROM TaskItem "
		       "WHERE state=1 AND stopTime IS NULL AND userID='%s'");
}

int
patts_get_child_items (char **out, const char *id)
{
  int rc;
  const char *fmt = "SELECT startTime,stopTime FROM TaskItem WHERE "
    "state=1 AND id=%s";
  char old_start[DATETIME_LEN + 1], old_stop[DATETIME_LEN + 1];
  char *query, *result, *esc_id;
  json_t *result_arr, *result_obj;
  size_t qlen = 1;

  if (strlen (id) > MAX_ID_LEN)
    return PATTS_OVERFLOW;

  rc = patts_escape (id, &esc_id, false);
  if (rc)
    return rc;

  qlen += strlen (fmt) - 2;
  qlen += strlen (esc_id);

  query = patts_malloc (qlen * sizeof (char));
  if (NULL == query)
    {
      patts_free (esc_id);
      return PATTS_MEMORYERROR;
    }

  snprintf (query, qlen, fmt, esc_id);
  patts_free (esc_id);

  rc = patts_query (query, &result, NULL);
  patts_free (query);

  if (rc)
    {
      return rc;
    }
  else if (strcmp (result, "[]") == 0)
    {
      patts_free (result);
      return PATTS_NOSUCHITEM;
    }

  result_arr = json_loads (result, 0, NULL);
  patts_free (result);

  result_obj = json_array_get (result_arr, 0);

  json_t *stop = json_object_get (result_obj, "stopTime");
  bool stopped = json_typeof (stop) != JSON_NULL;

  strcpy (old_start,
	  json_string_value (json_object_get (result_obj, "startTime")));
  if (stopped)
    strcpy (old_stop, json_string_value (stop));

  json_decref (result_arr);

  fmt = "SELECT * FROM TaskItem "
    "WHERE state=1 AND startTime>='%s' %s%s%s AND userID='%s' "
    "AND id<>%s";
  const char *stop_check = "AND stopTime<='";

  qlen = 1 + strlen (fmt) - 12;
  qlen += DATETIME_LEN * 2;
  qlen += strlen (stop_check) + 1;
  qlen += USERNAME_LEN * 2;

  query = patts_malloc (qlen * sizeof (char));
  if (NULL == query)
    return PATTS_MEMORYERROR;

  snprintf (query, qlen, fmt, old_start, stopped ? stop_check : "",
	    stopped ? old_stop : "", stopped ? "'" : "", patts_get_user_esc (),
	    id);

  rc = patts_query (query, out, "id");
  patts_free (query);

  return rc;
}
