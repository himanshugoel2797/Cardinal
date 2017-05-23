// Copyright (c) 2017 Himanshu Goel
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#ifndef CARDINAL_TAG_DB_H
#define CARDINAL_TAG_DB_H

#include "types.h"

typedef enum {
  TagDatabaseErrors_None,
  TagDatabaseErrors_EntryFull,
  TagDatabaseErrors_InvalidTag,
} TagDatabaseErrors;

TagDatabaseErrors TagDB_Initialize(void);

TagDatabaseErrors TagDB_AddObjectToTag(const char *tag, uint64_t obj);

TagDatabaseErrors TagDB_RemoveObjectFromTag(const char *tag, uint64_t obj);

TagDatabaseErrors TagDB_RemoveObject(uint64_t obj);

TagDatabaseErrors TagDB_EvaluateQuery(const char *path, uint64_t *cnt,
                                      uint64_t *objs);

#endif