#ifdef UTILS_H
#define UTILS_H

#include "types.h"
#include "postgres.h"
#include "catalog/pg_type.h"


int64 get_Int64(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info);
#endif // !UTILS_H