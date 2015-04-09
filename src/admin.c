/*
 *  libpatts - Backend library for PATTS Ain't Time Tracking Software
 *  Copyright (C) 2014-2015 Delwink, LLC
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqon.h>

#include "admin.h"
#include "patts.h"
#include "internal.h"

int
patts_create_user (const char *id, const char *host, const char *passwd)
{
  int rc;
  const char *fmt = "'%s','%s','%s'";
  char *args, *esc_id, *esc_host, *esc_passwd;
  size_t len = 1;
  size_t lens[] = {
    strlen (id) * 2 + 1,
    strlen (host) * 2 + 1,
    strlen (passwd) * 2 + 1
  };

  esc_id = sqon_malloc (lens[0] * sizeof (char));
  if (NULL == esc_id)
    return PATTS_MEMORYERROR;

  esc_host = sqon_malloc (lens[1] * sizeof (char));
  if (NULL == esc_host)
    {
      sqon_free (esc_id);
      return PATTS_MEMORYERROR;
    }

  esc_passwd = sqon_malloc (lens[2] * sizeof (char));
  if (NULL == esc_passwd)
    {
      sqon_free (esc_id);
      sqon_free (esc_host);
      return PATTS_MEMORYERROR;
    }

  for (size_t i = 0; i < 3; ++i)
    {
      switch (i)
	{
	case 0:
	  rc = sqon_escape (patts_get_db (), id, esc_id, lens[i], false);
	  break;

	case 1:
	  rc = sqon_escape (patts_get_db (), host, esc_host, lens[i], false);
	  break;

	case 2:
	  rc = sqon_escape (patts_get_db (), passwd, esc_passwd, lens[i],
			    false);
	  break;
	}

      if (rc)
	break;
    }

  if (rc)
    {
      sqon_free (esc_id);
      sqon_free (esc_host);
      sqon_free (esc_passwd);
      return rc;
    }

  len += strlen (fmt) - 6;
  len += strlen (esc_id);
  len += strlen (esc_host);
  len += strlen (esc_passwd);

  args = sqon_malloc (len * sizeof (char));
  if (NULL == args)
    {
      sqon_free (esc_id);
      sqon_free (esc_host);
      sqon_free (esc_passwd);
      return PATTS_MEMORYERROR;
    }

  snprintf (args, len, fmt, esc_id, esc_host, esc_passwd);
  sqon_free (esc_id);
  sqon_free (esc_host);
  sqon_free (esc_passwd);

  rc = call_procedure ("createUser", args);
  sqon_free (args);

  return rc;
}

int
patts_create_task (const char *parent_id, const char *display_name)
{
  int rc;
  const char *fmt = "INSERT INTO TaskType(parentID,displayName) "
    "VALUES(%s,'%s')";
  char *query, *esc_parent, *esc_dispname;
  size_t qlen = 1;
  size_t lens[] = {
    strlen (parent_id) * 2 + 1,
    strlen (display_name) * 2 + 1
  };

  esc_parent = sqon_malloc (lens[0] * sizeof (char));
  if (NULL == esc_parent)
    return PATTS_MEMORYERROR;

  esc_dispname = sqon_malloc (lens[1] * sizeof (char));
  if (NULL == esc_dispname)
    {
      sqon_free (esc_parent);
      return PATTS_MEMORYERROR;
    }

  for (size_t i = 0; i < 2; ++i)
    {
      switch (i)
	{
	case 0:
	  rc = sqon_escape (patts_get_db (), parent_id, esc_parent, lens[i],
			    false);
	  break;

	case 1:
	  rc = sqon_escape (patts_get_db (), display_name, esc_dispname,
			    lens[i], false);
	  break;
	}

      if (rc)
	break;
    }

  if (rc)
    {
      sqon_free (esc_parent);
      sqon_free (esc_dispname);
      return rc;
    }

  qlen += strlen (fmt) - 4;
  qlen += strlen (esc_parent);
  qlen += strlen (esc_dispname);

  query = sqon_malloc (qlen * sizeof (char));
  if (NULL == query)
    {
      sqon_free (esc_parent);
      sqon_free (esc_dispname);
      return PATTS_MEMORYERROR;
    }

  snprintf (query, qlen, fmt, esc_parent, esc_dispname);
  sqon_free (esc_parent);
  sqon_free (esc_dispname);

  rc = sqon_query (patts_get_db (), query, NULL, NULL);
  sqon_free (query);

  return rc;
}

static int
set_state (const char *id, const char *table, const char *state)
{
  int rc;
  const char *fmt = "UPDATE %s SET state=%s WHERE id=%s";
  char *query, *esc_id, *esc_table, *esc_state;
  size_t qlen = 1;
  size_t lens[] = {
    strlen (id) * 2 + 1,
    strlen (table) * 2 + 1,
    strlen (state) * 2 + 1
  };

  esc_id = sqon_malloc (lens[0] * sizeof (char));
  if (NULL == esc_id)
    return PATTS_MEMORYERROR;

  esc_table = sqon_malloc (lens[1] * sizeof (char));
  if (NULL == esc_table)
    {
      sqon_free (esc_id);
      return PATTS_MEMORYERROR;
    }

  esc_state = sqon_malloc (lens[2] * sizeof (char));
  if (NULL == esc_state)
    {
      sqon_free (esc_id);
      sqon_free (esc_table);
      return PATTS_MEMORYERROR;
    }

  for (size_t i = 0; i < 3; ++i)
    {
      switch (i)
	{
	case 0:
	  rc = sqon_escape (patts_get_db (), id, esc_id, lens[i], false);
	  break;

	case 1:
	  rc = sqon_escape (patts_get_db (), table, esc_table, lens[i], false);
	  break;

	case 2:
	  rc = sqon_escape (patts_get_db (), state, esc_state, lens[i], false);
	  break;
	}

      if (rc)
	break;
    }

  if (rc)
    {
      sqon_free (esc_id);
      sqon_free (esc_table);
      sqon_free (esc_state);
      return rc;
    }

  qlen += strlen (fmt) - 6;
  qlen += strlen (esc_id);
  qlen += strlen (esc_table);
  qlen += strlen (esc_state);

  query = sqon_malloc (qlen * sizeof (char));
  if (NULL == query)
    {
      sqon_free (esc_id);
      sqon_free (esc_table);
      sqon_free (esc_state);
      return PATTS_MEMORYERROR;
    }

  snprintf (query, qlen, fmt, esc_table, esc_state, esc_id);
  sqon_free (esc_id);
  sqon_free (esc_table);
  sqon_free (esc_state);

  rc = sqon_query (patts_get_db (), query, NULL, NULL);
  sqon_free (query);

  return rc;
}

int
patts_delete_user (const char *id)
{
  return set_state (id, "User", "0");
}

int
patts_delete_task (const char *id)
{
  return set_state (id, "TaskType", "0");
}

static int
proc_with_id (const char *proc, const char *id, const char *host)
{
  int rc;
  const char *fmt = "'%s','%s'";
  char *args, *esc_id, *esc_host;
  size_t len = 1;
  size_t lens[] = {
    strlen (id) * 2 + 1,
    strlen (host) * 2 + 1
  };

  esc_id = sqon_malloc (lens[0] * sizeof (char));
  if (NULL == esc_id)
    return PATTS_MEMORYERROR;

  esc_host = sqon_malloc (lens[1] * sizeof (char));
  if (NULL == esc_host)
    {
      sqon_free (esc_id);
      return PATTS_MEMORYERROR;
    }

  for (size_t i = 0; i < 2; ++i)
    {
      switch (i)
	{
	case 0:
	  rc = sqon_escape (patts_get_db (), id, esc_id, lens[i], false);
	  break;

	case 1:
	  rc = sqon_escape (patts_get_db (), host, esc_host, lens[i], false);
	  break;
	}

      if (rc)
	break;
    }

  if (rc)
    {
      sqon_free (esc_id);
      sqon_free (esc_host);
      return rc;
    }

  len += strlen (fmt) - 4;
  len += strlen (esc_id);
  len += strlen (esc_host);

  args = sqon_malloc (len * sizeof (char));
  if (NULL == args)
    {
      sqon_free (esc_id);
      sqon_free (esc_host);
      return PATTS_MEMORYERROR;
    }

  snprintf (args, len, fmt, esc_id, esc_host);
  sqon_free (esc_id);
  sqon_free (esc_host);

  rc = call_procedure (proc, args);
  sqon_free (args);

  return rc;
}

int
patts_grant_admin (const char *id, const char *host)
{
  return proc_with_id ("grantAdmin", id, host);
}

int
patts_revoke_admin (const char *id, const char *host)
{
  return proc_with_id ("revokeAdmin", id, host);
}
