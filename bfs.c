/*PGR-GNU*****************************************************************
File: floydWarshall.c

Generated with Template by:
Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developer: 
Copyright (c) 2015 Celia Virginia Vergara Castillo
Mail: vicky_vergara@hotmail.com

------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

********************************************************************PGR-GNU*/

#include <unistd.h>

#include "postgres.h"
#include "funcapi.h"
#if PGSQL_VERSION > 92
#include "access/htup_details.h"
#endif
#include "fmgr.h"

// #define DEBUG

#include "debug.h"
#include "types.h"
#include "postgres_connection.h"
#include "edge_input.h"
#include "utils.h"
//#include "algorithm.h"


PG_FUNCTION_INFO_V1(process);

static void process(PG_FUNCTION_ARGS) {

    
    int64 n_total = PG_GETARG_INT64(0); // total numbers of nodes
    int64 n_intervals = PG_GETARG_INT64(1); // numbers of the node's intervals according to total numbers of nodes
    
    int64 min, max;


    //Create partitions

    /*
    Create vertex subset
    select v.* from V where v.id >= V1 and v.id <= v2  
    */
    char * sql_command;

     /*
    Create edge subset
    select v.id, v.parent, v.value, e.from, e.cost from E e, V v where v.id = e.to and v.id >= v1 and v.id <= v2
                                                                        and  e.from >= v1 and e.from <= v2  
    */
    pgr_SPI_connect();

    PGR_DBG("Create partition");

    void *SPIplan = pgr_SPI_cursor_open_with_arg(sql_command, min, max);
    
    //char *sql =  pgr_text2char(PG_GETARG_TEXT_P(0); // sql command
    
    bool moredata = TRUE;
    int64 total_tuples = 0;
    const int tuple_limit = 1000000;
    edge_t *edges;
    Column_info_t info[5];

    while (moredata == TRUE) {
        SPI_cursor_fetch(SPIportal, TRUE, tuple_limit);
        ntuples = SPI_processed;
        total_tuples += ntuples;

        if (ntuples > 0) {
            if ((*edges) == NULL)
                (*edges) = (pgr_edge_t *)palloc0(ntuples * sizeof(edge_t));
            else
                (*edges) = (pgr_edge_t *)repalloc((*edges), ntuples * sizeof(edge_t));

            if ((*edges) == NULL) {
                elog(ERROR, "Out of memory");
            }
            
            size_t t;
            SPITupleTable *tuptable = SPI_tuptable;
            TupleDesc tupdesc = SPI_tuptable->tupdesc;

            PGR_DBG("processing %d edge tuples", ntuples);

            for (t = 0; t < ntuples; t++) {
                HeapTuple tuple = tuptable->vals[t];
                info[0]
                pgr_fetch_edge(&tuple, &tupdesc, info,
                        &default_id, -1,
                        &(*edges)[total_tuples - ntuples + t],
                        &valid_edges);
            }
            SPI_freetuptable(tuptable);
        } else {
            moredata = FALSE;
        }
    }


    if (total_tuples == 0 || valid_edges == 0) {
        (*totalTuples) = 0;
        PGR_DBG("NO edges");
        return;
    }


    (*totalTuples) = total_tuples;
    PGR_DBG("Finish reading %ld edges, %ld", total_tuples, (*totalTuples));
    time_msg(" reading Edges", start_t, clock());

    pgr_edge_t *edges = NULL;
    size_t total_tuples = 0;
    
    pgr_get_data_4_columns(edges_sql, &edges, &total_tuples);

    if (total_tuples == 0) {
        PGR_DBG("No edges found");
        (*result_count) = 0;
        (*result_tuples) = NULL;
        pgr_SPI_finish();
        return;
    }
    PGR_DBG("Total %ld tuples in query:", total_tuples);

    clock_t start_t = clock();
    PGR_DBG("Starting processing");
    char *err_msg = (char *)"";
    do_pgr_floydWarshall(
            edges,
            total_tuples,
            directed,
            result_tuples,
            result_count,
            &err_msg);
    time_msg(" processing FloydWarshall", start_t, clock());

    PGR_DBG("Returning %ld tuples\n", *result_count);
    PGR_DBG("Returned message = %s\n", err_msg);

    pfree(edges);
    pgr_SPI_finish();
}
/*
typedef struct edge
{
  int64 from;
  int64 to;
  int64 cost;
} edge_t;
v.id, v.parent, v.value, e.from, e.cost
*/
static void pgr_fetch_edge(HeapTuple *tuple, TupleDesc *tupdesc, edge_t *edge) {
    
    edge->from = 
    edge->source = pgr_SPI_getBigInt(tuple, tupdesc,  info[1]);
    edge->target = pgr_SPI_getBigInt(tuple, tupdesc, info[2]);
    edge->cost = pgr_SPI_getFloat8(tuple, tupdesc, info[3]);

    if (column_found(info[4].colNumber)) {
        edge->reverse_cost = pgr_SPI_getFloat8(tuple, tupdesc, info[4]);
    } else {
        edge->reverse_cost = default_rcost;
    }

    *valid_edges = edge->cost < 0? *valid_edges: *valid_edges + 1;
    *valid_edges = edge->reverse_cost < 0? *valid_edges: *valid_edges + 1;
}

//int64 id;
//  int64 accum;
update_t* scatter(int64 value, edge_t *e){
    update_t *u = (update_t *)palloc(sizeof(update_t));
     if ((*u) == NULL) {
                elog(ERROR, "Out of memory for update struct");
     }
    u->id = e->to;
    u->accum = value + e->cost;
    u->parent = e->from;
    return u;
    
}
//void gather(update_t *u);
//void apply_updates(vertex_t *v, update_t *u);


