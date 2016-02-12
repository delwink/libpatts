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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>

#include "get.h"
#include "internal.h"
#include "patts.h"
#include "user.h"

int
patts_get_active_task (char **out)
{
  const char *fmt = "SELECT * FROM TaskItem "
    "WHERE state=1 AND stopTime IS NULL AND userID='%s' "
    "ORDER BY id DESC LIMIT 1";
  size_t qlen = 1;

  qlen += strlen (fmt) - 2;
  qlen += USERNAME_LEN * 2;
  char query[qlen];

  snprintf (query, qlen, fmt, patts_get_user_esc ());
  return patts_query (query, out, "id");
}

int
patts_get_tree (char **out)
{
  const char *fmt = "SELECT id,typeID,userID,startTime FROM TaskItem "
    "WHERE state=1 AND stopTime IS NULL AND userID='%s'";
  size_t qlen = 1;

  qlen += strlen (fmt) - 2;
  qlen += USERNAME_LEN * 2;
  char query[qlen];

  snprintf (query, qlen, fmt, patts_get_user_esc ());
  return patts_query (query, out, "id");
}

static int
parent_type (char **out, const char *type)
{
  const char *fmt = "SELECT parentID FROM TaskType WHERE id=%s";
  size_t qlen = strlen (fmt) + (MAX_ID_LEN * 2) - 1;
  char query[qlen];

  snprintf (query, qlen, fmt, type);
  return patts_query (query, out, NULL);
}

int
patts_clockin (const char *type)
{
  int rc;
  const char *fmt = "%s,'%s'";
  char *active_task, *esc_type;
  size_t len = 1;

  rc = patts_get_active_task (&active_task);
  if (rc)
    return rc;

  rc = patts_escape (type, &esc_type, false);
  if (rc)
    {
      patts_free (active_task);
      return rc;
    }

  if (strcmp (active_task, "[]") != 0 && strcmp (active_task, "{}") != 0)
    {
      json_t *json_active_task = json_loads (active_task, 0, NULL);
      patts_free (active_task);
      if (NULL == json_active_task)
	{
	  patts_free (esc_type);
	  return PATTS_MEMORYERROR;
	}

      const char *key;
      json_t *value;
      const char *active_task_type = NULL;
      json_object_foreach (json_active_task, key, value)
	{
	  json_t *type = json_object_get (value, "typeID");
	  active_task_type = json_string_value (type);
	  break;
	}

      char *child_tasks;
      rc = patts_get_child_types (&child_tasks, active_task_type);
      json_decref (json_active_task);
      if (rc)
	{
	  patts_free (esc_type);
	  return rc;
	}

      json_t *json_child_tasks = json_loads (child_tasks, 0, NULL);
      patts_free (child_tasks);
      if (NULL == json_child_tasks)
	{
	  patts_free (esc_type);
	  return PATTS_MEMORYERROR;
	}

      if (!json_is_object (json_child_tasks))
	{
	  json_decref (json_child_tasks);
	  patts_free (esc_type);
	  return PATTS_UNEXPECTED;
	}

      bool found = false;
      json_object_foreach (json_child_tasks, key, value)
	{
	  if (strcmp (key, type) == 0)
	    {
	      found = true;
	      break;
	    }
	}
      json_decref (json_child_tasks);

      if (!found)
	{
	  patts_free (esc_type);
	  return PATTS_UNAVAILABLE;
	}
    }
  else
    {
      patts_free (active_task);

      char *this;
      rc = parent_type (&this, esc_type);
      if (rc)
	{
	  patts_free (esc_type);
	  return rc;
	}

      if (strcmp (this, "[]") == 0)
	{
	  patts_free (this);
	  patts_free (esc_type);
	  return PATTS_UNAVAILABLE;
	}

      uint32_t parent;
      rc = sscanf (this, "[{\"parentID\": \"%" SCNu32 "\"}]", &parent);
      patts_free (this);
      if (rc != 1)
	{
	  patts_free (esc_type);
	  return PATTS_UNEXPECTED;
	}

      if (parent != 0)
	{
	  patts_free (esc_type);
	  return PATTS_UNAVAILABLE;
	}

      rc = 0;
    }

  len += strlen (fmt) - 4;
  len += MAX_ID_LEN * 2;
  len += USERNAME_LEN * 2;
  char args[len];

  snprintf (args, len, fmt, esc_type, patts_get_user_esc ());
  patts_free (esc_type);

  return call_procedure ("clockIn", args);
}

int
patts_clockout (const char *item)
{
  int rc;
  char *esc_item;

  rc = patts_escape (item, &esc_item, false);
  if (rc)
    return rc;

  rc = call_procedure ("clockOut", esc_item);
  patts_free (esc_item);

  return rc;
}
