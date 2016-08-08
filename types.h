/*PGR-GNU*****************************************************************
File: pgr_types.h

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
#ifndef TYPES_H
#define TYPES_H

#pragma once

//#include <stdint.h>
#include "postgres.h"

typedef struct edge
{
  int64 from;
  int64 to;
  int64 cost;
} edge_t;

typedef struct vertex
{
  int64 id;
  int64 value;
  int64 parent;
  int64 *accum;
  int64  accum_size;
} vertex_t;

typedef struct update
{
  int64 id;
  int64 parent;
  int64 accum;

 } update_t;

typedef struct {
    int colNumber;
    uint64_t type;
    bool strict;
    char *name;
    expectType eType;

} Column_info_t;

#endif