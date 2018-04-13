#ifndef __SYMTAB_H__
#define __SYMTAB_H__

#include "arg.h"
#include "func.h"

#define SYMTAB_ARG  1
#define SYMTAB_FUNC 2

typedef struct symtab_entry
{
    int type;
    const char * id;
    void * arg_func_value;    
} symtab_entry;

typedef struct symtab
{
    unsigned int size;
    unsigned int count;
    struct symtab * parent;
    symtab_entry * entries;
} symtab;

symtab_entry * symtab_entry_new(unsigned int size);
void symtab_entry_delete(symtab_entry * entries);
void symtab_entry_add_arg_func(symtab_entry * entries, unsigned int size,
                               int type, const char * id, void * arg_func_value);
void * symtab_entry_lookup_arg_func(symtab_entry * entries, unsigned int size, int type, const char * id);
void symtab_entry_resize(symtab_entry * entries, int size, symtab_entry * entries_new, int size_new);

symtab * symtab_new(unsigned int size, symtab * parent);
void symtab_delete(symtab * tab);

void symtab_add_arg(symtab * tab, arg * arg_value);
void symtab_add_func(symtab * tab, func * func_value);

arg * symtab_lookup_arg(symtab * tab, const char * id);
func * symtab_lookup_func(symtab * tab, const char * id);

void symtab_resize(symtab * tab);

#endif /* __SYMTAB_H__ */


