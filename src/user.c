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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>

#include "patts.h"
#include "user.h"
#include "get.h"

#define MAX_ID_LEN 10 /* maximum length for a 32-bit INT in characters */

int
patts_get_active_task (char **out)
{
  int rc;
  const char *fmt = "SELECT * FROM TaskItem "
    "WHERE state=1 AND onClock=1 AND userID='%s' "
    "ORDER BY id DESC LIMIT 1";
  char *query;
  size_t qlen = 1;

  qlen += strlen (fmt) - 2;
  qlen += strlen (patts_get_user ());

  query = sqon_malloc (qlen * sizeof (char));
  if (NULL == query)
    return PATTS_MEMORYERROR;

  snprintf (query, qlen, fmt, patts_get_user ());

  rc = sqon_query (patts_get_db (), query, out, NULL);
  sqon_free (query);

  return rc;
}

int
patts_get_tree (char **out)
{
  int rc;
  const char *fmt = "SELECT id,typeID,userID,startTime FROM TaskItem "
    "WHERE state=1 AND onClock=1 AND userID='%s'";
  char *query;
  size_t qlen = 1;

  qlen += strlen (fmt) - 2;
  qlen += strlen (patts_get_user ());

  query = sqon_malloc (qlen * sizeof (char));
  if (NULL == query)
    return PATTS_MEMORYERROR;

  snprintf (query, qlen, fmt, patts_get_user ());

  rc = sqon_query (patts_get_db (), query, out, "id");
  sqon_free (query);

  return rc;
}

int
patts_clockin (const char *type)
{
  int rc;
  const char *fmt = "INSERT INTO "
    "TaskItem(state,onClock,startTime,typeID,userID) "
    "VALUES(1,1,NOW(),%s,'%s')";
  char *query;
  size_t qlen = 1;

  qlen += strlen (fmt) - 4;
  qlen += strlen (type);
  qlen += strlen (patts_get_user ());

  query = sqon_malloc (qlen * sizeof (char));
  if (NULL == query)
    return PATTS_MEMORYERROR;

  snprintf (query, qlen, fmt, type, patts_get_user ());

  rc = sqon_query (patts_get_db (), query, NULL, NULL);
  sqon_free (query);

  return rc;
}

int
patts_clockout (const char *item)
{
  int rc;
  const char *fmt = "UPDATE TaskItem SET onClock=0,stopTime=NOW() WHERE id=%s";
  char *query, *result;
  json_t *result_obj, *value;
  const char *key;
  size_t qlen = 1;

  qlen += strlen (fmt) - 2;
  qlen += MAX_ID_LEN;

  query = sqon_malloc (qlen * sizeof (char));
  if (NULL == query)
    return PATTS_MEMORYERROR;

  rc = patts_get_child_items (&result, item);

  if (rc)
    return rc;

  result_obj = json_loads (result, 0, NULL);
  if (NULL == result_obj)
    {
      sqon_free (query);
      return PATTS_MEMORYERROR;
    }

  sqon_free (result);

  rc = sqon_connect (patts_get_db ());
  if (rc)
    {
      sqon_free (query);
      return rc;
    }

  json_object_foreach (result_obj, key, value)
    {
      if (!strcmp (json_string_value (json_object_get (value, "onClock")), ""))
	continue; // ignore if already clocked out

      snprintf (query, qlen, fmt, key);

      rc = sqon_query (patts_get_db (), query, NULL, NULL);
      if (rc)
	break;
    }

  json_decref (result_obj);

  if (rc)
    {
      sqon_close (patts_get_db ());
      sqon_free (query);
      return rc;
    }

  snprintf (query, qlen, fmt, item);

  rc = sqon_query (patts_get_db (), query, NULL, NULL);
  sqon_close (patts_get_db ());
  sqon_free (query);

  return rc;
}
