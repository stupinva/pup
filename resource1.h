#ifndef __RESOURCE1__
#define __RESOURCE1__

#include <stdio.h>
#include "restable.h"

bool_t is_resource1(restable_t * rt);

#define RESOURCE1_SUBDIRS FALSE
#define RESOURCE1_MERGE FALSE
#define RESOURCE1_META FALSE
#define RESOURCE1_TIME FALSE
#define RESOURCE1_PAGE 1

bool_t resource1_read_dir(restable_t * rt);
bool_t resource1_fill_filename(resentry_t * re);

#define resource1_extract_resource rt_extract_resource

#define resource1_save_meta rt_not_save_meta
#define resource1_load_meta rt_not_load_meta

bool_t resource1_fill_name(resentry_t * re);
bool_t resource1_prepare_dir(restable_t * rt);
#define resource1_add_resource rt_add_resource
bool_t resource1_write_dir(restable_t * rt);

#endif
