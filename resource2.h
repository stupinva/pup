#ifndef __RESOURCE2__
#define __RESOURCE2__

#include <stdio.h>
#include "restable.h"

bool_t is_resource2(restable_t * rt);

#define RESOURCE2_SUBDIRS FALSE
#define RESOURCE2_MERGE FALSE
#define RESOURCE2_META FALSE
#define RESOURCE2_TIME FALSE
#define RESOURCE2_PAGE 1

bool_t resource2_read_dir(restable_t * rt);
bool_t resource2_fill_filename(resentry_t * re);

#define resource2_extract_resource rt_extract_resource

#define resource2_save_meta rt_not_save_meta
#define resource2_load_meta rt_not_load_meta

bool_t resource2_fill_name(resentry_t * re);
bool_t resource2_prepare_dir(restable_t * rt);
bool_t resource2_add_resource(restable_t * rt, size_t i);
bool_t resource2_write_dir(restable_t * rt);

#endif
