#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "types.h"

//extern void process(PG_FUNCTION_ARGS);

extern update_t* scatter(vertex_t *v, edge_t *e);
extern void gather(update_t *u);
extern void apply_updates(vertex_t *v, update_t *u);

#endif