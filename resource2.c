#include <stdio.h>
#include <string.h>
#include "bin.h"
#include "resource2.h"

#define RESOURCE2_IDENT "RESOURCE2xxx"
#define RESOURCE2_HEADER_IDENT_SIZE 12
#define RESOURCE2_HEADER_NUMBER_SIZE 4
#define RESOURCE2_HEADER_PSEUDONAME_SIZE 12
#define RESOURCE2_HEADER_FULLSIZE_SIZE 4
#define RESOURCE2_HEADER_ZEROS_SIZE 4
#define RESOURCE2_HEADER_SIZE (RESOURCE2_HEADER_IDENT_SIZE + RESOURCE2_HEADER_NUMBER_SIZE +\
	RESOURCE2_HEADER_PSEUDONAME_SIZE + RESOURCE2_HEADER_FULLSIZE_SIZE + \
	RESOURCE2_HEADER_ZEROS_SIZE)

#define RESOURCE2_ENTRY_NAME_SIZE 12
#define RESOURCE2_ENTRY_OFFSET_SIZE 4
#define RESOURCE2_ENTRY_SIZE_SIZE 4
#define RESOURCE2_ENTRY_SIZE (RESOURCE2_ENTRY_NAME_SIZE +  RESOURCE2_ENTRY_OFFSET_SIZE + \
	RESOURCE2_ENTRY_SIZE_SIZE)

bool_t is_resource2(restable_t * rt)
{
  char ident[RESOURCE2_HEADER_IDENT_SIZE];

  if (readf(rt->file, "c12", ident) != OK)
    return FALSE;
  if (strncmp(ident, RESOURCE2_IDENT, RESOURCE2_HEADER_IDENT_SIZE) != 0)
    return FALSE;
  return TRUE;
}

bool_t resource2_read_entry(FILE * file, resentry_t * re)
{
  char name[RESOURCE2_ENTRY_NAME_SIZE];

  if (readf(file, "c12l4l4", name, &(re->offset), &(re->size)) != OK)
  {
    fprintf(stderr, "resource2_read_entry: Can't read entry.\n");
    return FALSE;
  }
  re->compressed = re->size;

  re->offset += RESOURCE2_ENTRY_SIZE;

  name[0] ^= 0xAD;
  name[1] ^= 0xDE;
  name[2] ^= 0xED;
  name[3] ^= 0xAC;

  name[4] ^= 0xAD;
  name[5] ^= 0xDE;
  name[6] ^= 0xED;
  name[7] ^= 0xAC;

  name[8] ^= 0xAD;
  name[9] ^= 0xDE;
  name[10] ^= 0xED;
  name[11] ^= 0xAC;

  s_strncpy(&(re->name), name, RESOURCE2_ENTRY_NAME_SIZE);
  return TRUE;
}

bool_t resource2_write_entry(FILE * file, resentry_t * re)
{
  char name[RESOURCE2_ENTRY_NAME_SIZE];

  if (strlen(re->name) > RESOURCE2_ENTRY_NAME_SIZE)
  {
    fprintf(stderr, "resource2_write_entry: Name too long for entry.\n");
    return FALSE;
  }
  strncpy(name, re->name, RESOURCE2_ENTRY_NAME_SIZE);

  name[0] ^= 0xAD;
  name[1] ^= 0xDE;
  name[2] ^= 0xED;
  name[3] ^= 0xAC;

  name[4] ^= 0xAD;
  name[5] ^= 0xDE;
  name[6] ^= 0xED;
  name[7] ^= 0xAC;

  name[8] ^= 0xAD;
  name[9] ^= 0xDE;
  name[10] ^= 0xED;
  name[11] ^= 0xAC;

  if (writef(file, "c12l4l4",
             name, re->offset - RESOURCE2_ENTRY_SIZE, re->size) != OK)
  {
    fprintf(stderr, "resource2_write_entry: Can't write entry.\n");
    return FALSE;
  }
  return TRUE;
}

bool_t resource2_read_dir(restable_t * rt)
{
  char ident[RESOURCE2_HEADER_IDENT_SIZE];
  size_t number;
  size_t i;

  if (readf(rt->file, "c12l4c20", ident, &number, NULL) != OK)
  {
    fprintf(stderr, "resource2_read_dir: Can't read header.\n");
    return FALSE;
  }
  if (strncmp(ident, RESOURCE2_IDENT, RESOURCE2_HEADER_IDENT_SIZE) != 0)
  {
    fprintf(stderr, "resource2_read_dir: Wrong ident.\n");
    return FALSE;
  }
  if (number == 0)
  {
    fprintf(stderr, "resource2_read_dir: Wrong number of entries.\n");
    return FALSE;
  }
  if (rt_set_number(rt, number - 1) == FALSE)
  {
    fprintf(stderr, "resource2_read_dir: Can't resize entries.\n");
    return FALSE;
  }
  for(i = 0; i < rt->number; i++)
  {
    if (resource2_read_entry(rt->file, &(rt->entries[i])) == FALSE)
      return FALSE;
  }
  return TRUE;
}

bool_t resource2_fill_filename(resentry_t * re)
{
  s_strcpy(&(re->filename), re->name);
  s_strlower(re->filename);
  if (re->filename == NULL)
    return FALSE;
  return TRUE;
}

bool_t resource2_fill_name(resentry_t * re)
{
  s_strcpy(&(re->name), re->filename);
  s_strupper(re->name);
  if (is_dos_filename(re->name) == FALSE)
  {
    fprintf(stderr, "resource2_fill_name: \"%s\" is not DOS filename.\n", re->name);
    return FALSE;
  }
  return TRUE;
}

bool_t resource2_prepare_dir(restable_t * rt)
{
  fseek(rt->file, RESOURCE2_HEADER_SIZE + RESOURCE2_ENTRY_SIZE * rt->number, SEEK_SET);
  return TRUE;
}

bool_t resource2_write_dir(restable_t * rt)
{
  size_t i;
  size_t offset;

  offset = ftell(rt->file);
  fseek(rt->file, 0, SEEK_SET);
  if (writef(rt->file, "c12l4b4b4b4l4z4",
             RESOURCE2_IDENT,
             rt->number + 1,
             0x7F91A2F8,
             0xADDEEDAC,
             0xADDEEDAC, rt->entries[0].offset - RESOURCE2_ENTRY_SIZE) != OK)
  {
    fprintf(stderr, "resource2_write_dir: Can't write header.\n");
    return FALSE;
  }
  for(i = 0; i < rt->number; i++)
  {
    if (resource2_write_entry(rt->file, &(rt->entries[i])) == FALSE)
      return FALSE;
  }
  for(i = 0; i < rt->number - 1; i++)
  {
    fseek(rt->file, rt->entries[i].offset - RESOURCE2_ENTRY_SIZE, SEEK_SET);
    if (resource2_write_entry(rt->file, &(rt->entries[i + 1])) == FALSE)
      return FALSE;
  }
  fseek(rt->file, rt->entries[rt->number - 1].offset - RESOURCE2_ENTRY_SIZE,
        SEEK_SET);
  if (resource2_write_entry(rt->file, &(rt->entries[0])) == FALSE)
    return FALSE;
  fseek(rt->file, offset, SEEK_SET);
  return TRUE;
}

bool_t resource2_add_resource(restable_t * rt, size_t i)
{
  char *filename;

  if (writec(rt->file, NULL, RESOURCE2_ENTRY_SIZE) != OK)
  {
    fprintf(stderr, "resource2_add_resource: Can't write header of entry #%zu.\n", i);
    s_free(&filename);
    return FALSE;
  }

  filename = NULL;
  rt->entries[i].offset = ftell(rt->file);
  s_strcpy(&filename, rt->basepath);
  s_strcat(&filename, rt->entries[i].filename);
  if (fadd(rt->file, filename, &(rt->entries[i].size)) == FALSE)
  {
    fprintf(stderr, "resource2_add_resource: Can't open or read file \"%s\".\n",
            filename);
    s_free(&filename);
    return FALSE;
  }
  s_free(&filename);
  return TRUE;
}
