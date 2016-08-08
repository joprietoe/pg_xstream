
#include "utils.h"
#include "types.h"
#include "postgres.h"
#include "catalog/pg_type.h"
#include "executor/spi.h"
#include "debug.h"

int64 get_Int64(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info) {
    Datum binval;
    bool isnull;
    int64 value = 0;
    binval = SPI_getbinval(*tuple, *tupdesc, info.colNumber, &isnull);
    if (isnull)
        elog(ERROR, "Unexpected Null value in column %s", info.name);
    switch (info.type) {
        case INT2OID:
            value = (int64_t) DatumGetInt16(binval);
            break;
        case INT4OID:
            value = (int64_t) DatumGetInt32(binval);
            break;
        case INT8OID:
            value = DatumGetInt64(binval);
            break;
        default:
            elog(ERROR,
                    "Unexpected Column type of %s. Expected ANY-INTEGER",
                    info.name);
    }
    PGR_DBG("Variable: %s Value: %lld", info.name, value);
    return value;
}