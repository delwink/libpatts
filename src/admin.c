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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqon.h>

#include "admin.h"
#include "internal.h"
#include "patts.h"

int
patts_create_user (const char *id, const char *host, const char *passwd)
{
  int rc;
  const char *fmt = "'%s','%s','%s'";
  char *esc_id, *esc_host, *esc_passwd;
  size_t len = 1;

  if (strlen (id) > USERNAME_LEN)
    return PATTS_OVERFLOW;

  rc = patts_escape (id, &esc_id, false);
  if (rc)
    return rc;

  rc = patts_escape (host, &esc_host, false);
  if (rc)
    {
      patts_free (esc_id);
      return rc;
    }

  rc = patts_escape (passwd, &esc_passwd, false);
  if (rc)
    {
      patts_free (esc_id);
      patts_free (esc_host);
      return rc;
    }

  len += strlen (fmt) - 6;
  len += MAX_ID_LEN * 2;
  len += strlen (esc_host);
  len += strlen (esc_passwd);
  char args[len];

  snprintf (args, len, fmt, esc_id, esc_host, esc_passwd);
  patts_free (esc_id);
  patts_free (esc_host);
  patts_free (esc_passwd);

  return call_procedure ("createUser", args);
}

int
patts_create_task (const char *parent_id, const char *display_name)
{
  int rc;
  const char *fmt = "INSERT INTO TaskType(parentID,displayName,state) "
    "VALUES(%s,'%s',1)";
  char *esc_parent, *esc_dispname;
  size_t qlen = 1;

  if (strlen (display_name) > DISPNAME_LEN / 2)
    return PATTS_OVERFLOW;

  rc = patts_escape (parent_id, &esc_parent, false);
  if (rc)
    return rc;

  rc = patts_escape (display_name, &esc_dispname, false);
  if (rc)
    {
      patts_free (esc_parent);
      return rc;
    }

  qlen += strlen (fmt) - 4;
  qlen += MAX_ID_LEN * 2;
  qlen += DISPNAME_LEN;
  char query[qlen];

  snprintf (query, qlen, fmt, esc_parent, esc_dispname);
  patts_free (esc_parent);
  patts_free (esc_dispname);

  return patts_query (query, NULL, NULL);
}

static int
set_state (const char *id, const char *table, const char *state,
	   const char *idcol, bool quote_id)
{
  int rc;
  const char *fmt = "UPDATE %s SET state=%s WHERE %s=%s";
  char *esc_id, *esc_table, *esc_state;
  size_t qlen = 1;

  rc = patts_escape (id, &esc_id, quote_id);
  if (rc)
    return rc;

  rc = patts_escape (table, &esc_table, false);
  if (rc)
    {
      patts_free (esc_id);
      return rc;
    }

  rc = patts_escape (state, &esc_state, false);
  if (rc)
    {
      patts_free (esc_id);
      patts_free (esc_table);
      return rc;
    }

  qlen += strlen (fmt) - 8;
  qlen += strlen (idcol);
  qlen += MAX_ID_LEN * 2;
  qlen += strlen (esc_table);
  qlen += strlen (esc_state);
  char query[qlen];

  snprintf (query, qlen, fmt, esc_table, esc_state, idcol, esc_id);
  patts_free (esc_id);
  patts_free (esc_table);
  patts_free (esc_state);

  return patts_query (query, NULL, NULL);
}

int
patts_delete_user (const char *id)
{
  int rc;
  const char *fmt = "'%s'";
  size_t len = 1;

  if (strlen (id) > USERNAME_LEN)
    return PATTS_OVERFLOW;

  char *esc_id;
  rc = patts_escape (id, &esc_id, false);
  if (rc)
    return rc;

  len += strlen (fmt) - 2;
  len += USERNAME_LEN * 2;
  char args[len];

  snprintf (args, len, fmt, esc_id);
  patts_free (esc_id);

  return call_procedure ("deleteUser", args);
}

int
patts_delete_task (const char *id)
{
  return set_state (id, "TaskType", "0", "id", false);
}

static int
proc_with_id (const char *proc, const char *id, const char *host)
{
  int rc;
  const char *fmt = "'%s','%s'";
  char *esc_id, *esc_host;
  size_t len = 1;

  if (strlen (id) > USERNAME_LEN)
    return PATTS_OVERFLOW;

  rc = patts_escape (id, &esc_id, false);
  if (rc)
    return rc;

  rc = patts_escape (host, &esc_host, false);
  if (rc)
    {
      patts_free (esc_id);
      return rc;
    }

  len += strlen (fmt) - 4;
  len += USERNAME_LEN * 2;
  len += strlen (esc_host);
  char args[len];

  snprintf (args, len, fmt, esc_id, esc_host);
  patts_free (esc_id);
  patts_free (esc_host);

  return call_procedure (proc, args);
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
