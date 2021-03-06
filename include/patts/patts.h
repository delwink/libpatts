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

/**
 * @file patts.h
 * @version 1.0
 * @date 02/16/2016
 * @author David McMackins II
 * @brief Functions global to PATTS.
 */

#ifndef DELWINK_PATTS_H
#define DELWINK_PATTS_H

#include <stdint.h>
#include <sqon.h>

#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS
# define __END_DECLS
#endif

__BEGIN_DECLS

/**
 * @brief Error codes returned on failure.
 */
enum patts_error
{
  PATTS_LOADERROR   = -60,
  PATTS_MEMORYERROR = -62,
  PATTS_OVERFLOW    = -63,
  PATTS_UNEXPECTED  = -64,
  PATTS_UNAVAILABLE = -65,
  PATTS_NOSUCHUSER  = -73,
  PATTS_NOSUCHITEM  = -74
};

/**
 * @brief Internally-used implementation of malloc(); defaults to
 * sqon_malloc().
 * @param n Number of bytes to allocate on the heap.
 * @return Pointer to n bytes of available memory.
 */
void *
patts_malloc (size_t n);

/**
 * @brief Frees memory allocated with patts_malloc().
 * @param v Pointer returned by earlier call to patts_malloc().
 */
void
patts_free (void *v);

/**
 * @brief Changes the memory management functions used internally.
 * @param new_malloc The new malloc() function to be used.
 * @param new_free The new free() function to be used.
 */
void
patts_set_alloc_funcs (void *(*new_malloc) (size_t n),
		       void (*new_free) (void *v));

/**
 * @brief Alias for libsqon's MySQL database connection type.
 */
#define PATTS_DBCONN_MYSQL SQON_DBCONN_MYSQL

/**
 * @brief Initializes connection to the PATTS database.
 * @param db_type Database connection type.
 * @param host Hostname or IP address of the database server.
 * @param user Database server username.
 * @param passwd Database server authentication password.
 * @param database Database to be used.
 * @param port String representation of the port number.
 * @return Nonzero on error.
 */
int
patts_init (uint8_t db_type, const char *host, const char *user,
	    const char *passwd, const char *database, const char *port);

/**
 * @brief Free memory used internally by libpatts; call after finished using
 * the library or before a second call to patts_init().
 */
void
patts_cleanup (void);

/**
 * @brief Explicitly establish connection with the PATTS database server.
 * @return Nonzero if error connecting.
 */
int
patts_connect (void);

/**
 * @brief Closes a connection to the PATTS database server.
 */
void
patts_close (void);

/**
 * @brief Query the PATTS database.
 * @param query UTF-8 encoded SQL statement.
 * @param out Pointer to string which will be allocated and populated with
 * JSON-formatted response from the database; must free with patts_free();
 * populated as object if table has a primary key, else an array; can be NULL
 * if no result is expected.
 * @param primary_key Primary key expected in return value, if any (else NULL).
 * @return Negative if input or IO error; positive if error from server.
 */
int
patts_query (const char *query, char **out, const char *primary_key);

/**
 * @brief Gets the primary key of a table.
 * @param table Name of the database table.
 * @param out Pointer to string which will be allocated and populated with the
 * table's primary key.
 * @return Negative if input or IO error; positive if error from server.
 */
int
patts_get_primary_key (const char *table, char **out);

/**
 * @brief Properly escapes a string for the database engine.
 * @param in String to be escaped.
 * @param out Pointer to string which will be allocated and populated with the
 * escaped value.
 * @param quote Whether to surround the output string in apostrophes.
 * @return Nonzero on error.
 */
int
patts_escape (const char *in, char **out, bool quote);

/**
 * @brief Gets the active username.
 * @return String representation of the active username.
 */
const char *
patts_get_user (void);

/**
 * @brief Gets an escaped version of the active username.
 * @return String containing the escaped username.
 */
const char *
patts_get_user_esc (void);

/**
 * @brief Checks user's admin rights.
 * @return true if user is a PATTS administrator, else false.
 */
bool
patts_have_admin (void);

/**
 * @brief Checks the database schema version on the remote server.
 * @param out Pointer to the output location.
 * @return Nonzero on error.
 */
int
patts_get_db_version (uint32_t *out);

/**
 * @brief Checks the version of this library.
 * @return String representation of this library version.
 */
const char *
patts_get_library_version (void);

__END_DECLS

#endif
