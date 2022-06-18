#include <stdio.h>
#include <string.h>
#include "bin.h"
#include "resource1.h"

#define RESOURCE1_HEADER_IDENT "RESOURCE1\r\n\x1A"
#define RESOURCE1_HEADER_IDENT_SIZE 12
#define RESOURCE1_HEADER_NUMBER_SIZE 4
#define RESOURCE1_HEADER_SIZE (RESOURCE1_HEADER_IDENT_SIZE + RESOURCE1_HEADER_NUMBER_SIZE)

#define RESOURCE1_FOOTER_IDENT "KYLEKYLEKYLE"
#define RESOURCE1_FOOTER_IDENT_SIZE 12
#define RESOURCE1_FOOTER_OFFSET_SIZE 4
#define RESOURCE1_FOOTER_SIZE (RESOURCE1_FOOTER_IDENT_SIZE + RESOURCE1_FOOTER_OFFSET_SIZE)

#define RESOURCE1_ENTRY_NAME_SIZE 12
#define RESOURCE1_ENTRY_OFFSET_SIZE 4
#define RESOURCE1_ENTRY_SIZE (RESOURCE1_ENTRY_NAME_SIZE +  RESOURCE1_ENTRY_OFFSET_SIZE)

bool_t is_resource1(restable_t * rt)
{
  char ident[RESOURCE1_HEADER_IDENT_SIZE];

  if (readf(rt->file, "c12", ident) != OK)
    return FALSE;
  if (strncmp(ident, RESOURCE1_HEADER_IDENT, RESOURCE1_HEADER_IDENT_SIZE) != 0)
    return FALSE;
  return TRUE;
}

bool_t resource1_read_entry(FILE * file, resentry_t * re)
{
  char name[RESOURCE1_ENTRY_NAME_SIZE];

  if (readf(file, "c12l4", name, &(re->offset)) != OK)
  {
    fprintf(stderr, "resource1_read_entry: Can't read entry.\n");
    return FALSE;
  }
  re->size = 0;
  re->compressed = 0;

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

  s_strncpy(&(re->name), name, RESOURCE1_ENTRY_NAME_SIZE);
  return TRUE;
}

bool_t resource1_write_entry(FILE * file, resentry_t * re)
{
  char name[RESOURCE1_ENTRY_NAME_SIZE];

  if (strlen(re->name) > RESOURCE1_ENTRY_NAME_SIZE)
  {
    fprintf(stderr, "resource1_write_entry: Name too long for entry.\n");
    return FALSE;
  }
  strncpy(name, re->name, RESOURCE1_ENTRY_NAME_SIZE);

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

  if (writef(file, "c12l4", name, re->offset) != OK)
  {
    fprintf(stderr, "resource1_write_entry: Can't write entry.\n");
    return FALSE;
  }
  return TRUE;
}

bool_t resource1_read_dir(restable_t * rt)
{
  char ident[RESOURCE1_HEADER_IDENT_SIZE];
  size_t number;
  size_t size;
  size_t i;

  if (readf(rt->file, "c12l4", ident, &number, NULL) != OK)
  {
    fprintf(stderr, "resource1_read_dir: Can't read header.\n");
    return FALSE;
  }
  if (strncmp(ident, RESOURCE1_HEADER_IDENT, RESOURCE1_HEADER_IDENT_SIZE) != 0)
  {
    fprintf(stderr, "resource1_read_dir: Wrong header ident.\n");
    return FALSE;
  }
  if (rt_set_number(rt, number) == FALSE)
  {
    fprintf(stderr, "resource1_read_dir: Can't resize entries.\n");
    return FALSE;
  }
  for(i = 0; i < rt->number; i++)
  {
    if (resource1_read_entry(rt->file, &(rt->entries[i])) == FALSE)
      return FALSE;
  }
  if (readf(rt->file, "c12l4", ident, &size) != OK)
  {
    fprintf(stderr, "resource1_read_dir: Can't read footer.\n");
    return FALSE;
  }
  if (strncmp(ident, RESOURCE1_FOOTER_IDENT, RESOURCE1_FOOTER_IDENT_SIZE) != 0)
  {
    fprintf(stderr, "resource1_read_dir: Wrong footer ident.\n");
    return FALSE;
  }
  if (size != fsize(rt->file))
  {
    fprintf(stderr, "resource1_read_dir: Wrong size of file in footer.\n");
    return FALSE;
  }

  for(i = 0; i < rt->number - 1; i++)
  {
    if (rt->entries[i].offset > rt->entries[i + 1].offset)
    {
      fprintf(stderr, "resource1_read_dir: Wrong offsets in entries.\n");
      return FALSE;
    }
    rt->entries[i].size = rt->entries[i + 1].offset - rt->entries[i].offset;
    rt->entries[i].compressed = rt->entries[i].size;
  }
  if (rt->entries[rt->number - 1].offset > size) {
    fprintf(stderr, "resource1_read_dir: Wrong offset of last entry.\n");
    return FALSE;
  }
  rt->entries[rt->number - 1].size = size - rt->entries[rt->number - 1].offset;
  rt->entries[rt->number - 1].compressed = rt->entries[rt->number - 1].size;
  return TRUE;
}

bool_t resource1_fill_filename(resentry_t * re)
{
  s_strcpy(&(re->filename), re->name);
  s_strlower(re->filename);
  if (re->filename == NULL)
    return FALSE;
  return TRUE;
}

bool_t resource1_fill_name(resentry_t * re)
{
  s_strcpy(&(re->name), re->filename);
  s_strupper(re->name);
  if (is_dos_filename(re->name) == FALSE)
  {
    fprintf(stderr, "resource1_fill_name: \"%s\" is not DOS filename.\n", re->name);
    return FALSE;
  }
  return TRUE;
}

bool_t resource1_prepare_dir(restable_t * rt)
{
  fseek(rt->file, RESOURCE1_HEADER_SIZE + RESOURCE1_ENTRY_SIZE * rt->number + RESOURCE1_FOOTER_SIZE, SEEK_SET);
  return TRUE;
}

bool_t resource1_write_dir(restable_t * rt)
{
  size_t i;
  size_t offset;

  offset = ftell(rt->file);
  fseek(rt->file, 0, SEEK_SET);
  if (writef(rt->file, "c12l4", RESOURCE1_HEADER_IDENT, rt->number) != OK)
  {
    fprintf(stderr, "resource1_write_dir: Can't write header.\n");
    return FALSE;
  }
  for(i = 0; i < rt->number; i++)
  {
    if (resource1_write_entry(rt->file, &(rt->entries[i])) == FALSE)
      return FALSE;
  }
  if (writef(rt->file, "c12l4", RESOURCE1_FOOTER_IDENT, offset) != OK)
  {
    fprintf(stderr, "resource1_write_dir: Can't write footer.\n");
    return FALSE;
  }
  fseek(rt->file, offset, SEEK_SET);
  return TRUE;
}
