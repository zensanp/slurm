/*****************************************************************************\
 *  mysql_common.h - common functions for the mysql storage plugin.
 *****************************************************************************
 *
 *  Copyright (C) 2004-2007 The Regents of the University of California.
 *  Produced at Lawrence Livermore National Laboratory (cf, DISCLAIMER).
 *  Written by Danny Auble <da@llnl.gov>
 *
 *  This file is part of Slurm, a resource management program.
 *  For details, see <https://slurm.schedmd.com/>.
 *  Please also read the included file: DISCLAIMER.
 *
 *  Slurm is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your option)
 *  any later version.
 *
 *  In addition, as a special exception, the copyright holders give permission
 *  to link the code of portions of this program with the OpenSSL library under
 *  certain conditions as described in each individual source file, and
 *  distribute linked combinations including the two. You must obey the GNU
 *  General Public License in all respects for all of the code used other than
 *  OpenSSL. If you modify file(s) with this exception, you may extend this
 *  exception to your version of the file(s), but you are not obligated to do
 *  so. If you do not wish to do so, delete this exception statement from your
 *  version.  If you delete this exception statement from all source files in
 *  the program, then also delete it here.
 *
 *  Slurm is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with Slurm; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA.
 *
 *  This file is patterned after jobcomp_linux.c, written by Morris Jette and
 *  Copyright (C) 2002 The Regents of the University of California.
\*****************************************************************************/
#ifndef _MYSQL_COMMON_H
#define _MYSQL_COMMON_H

#include <inttypes.h>
#include <pthread.h>
#include <stdio.h>

#include "slurm/slurm_errno.h"
#include "src/common/list.h"
#include "src/common/xstring.h"

#include <mysql.h>
#include <mysqld_error.h>

typedef enum {
	SLURM_MYSQL_PLUGIN_NOTSET,
	SLURM_MYSQL_PLUGIN_AS, /* accounting_storage */
	SLURM_MYSQL_PLUGIN_JC, /* jobcomp */
} slurm_mysql_plugin_type_t;

typedef struct {
	char *cluster_name;
	MYSQL *db_conn;
	uint32_t flags;
	pthread_mutex_t lock;
	char *pre_commit_query;
	List update_list;
	int conn;
	uint64_t wsrep_trx_fragment_size_orig;
	char *wsrep_trx_fragment_unit_orig;
} mysql_conn_t;

typedef struct {
	char *backup;
	uint32_t port;
	char *host;
	char *user;
	char *params;
	char *pass;
} mysql_db_info_t;

typedef struct {
	char *name;
	char *options;
} storage_field_t;

extern mysql_conn_t *create_mysql_conn(int conn_num, bool rollback,
				       char *cluster_name);
extern int destroy_mysql_conn(mysql_conn_t *mysql_conn);
extern mysql_db_info_t *create_mysql_db_info(slurm_mysql_plugin_type_t type);
extern int destroy_mysql_db_info(mysql_db_info_t *db_info);

extern int mysql_db_get_db_connection(mysql_conn_t *mysql_conn, char *db_name,
				   mysql_db_info_t *db_info);
extern int mysql_db_close_db_connection(mysql_conn_t *mysql_conn);
extern int mysql_db_cleanup();
extern int mysql_db_query(mysql_conn_t *mysql_conn, char *query);
extern int mysql_db_delete_affected_rows(mysql_conn_t *mysql_conn, char *query);
extern int mysql_db_ping(mysql_conn_t *mysql_conn);
extern int mysql_db_commit(mysql_conn_t *mysql_conn);
extern int mysql_db_rollback(mysql_conn_t *mysql_conn);

extern MYSQL_RES *mysql_db_query_ret(mysql_conn_t *mysql_conn,
				     char *query, bool last);
extern int mysql_db_query_check_after(mysql_conn_t *mysql_conn, char *query);

extern uint64_t mysql_db_insert_ret_id(mysql_conn_t *mysql_conn, char *query);

extern int mysql_db_create_table(mysql_conn_t *mysql_conn, char *table_name,
				 storage_field_t *fields, char *ending);
extern int mysql_db_get_var_str(mysql_conn_t *mysql_conn,
				const char *variable_name,
				char **value);
extern int mysql_db_get_var_u64(mysql_conn_t *mysql_conn,
	    			 const char *variable_name,
			    	 uint64_t *value);
extern void mysql_db_enable_streaming_replication(mysql_conn_t *mysql_conn);
extern void mysql_db_restore_streaming_replication(mysql_conn_t *mysql_conn);
#endif
