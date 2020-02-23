#include "tcmatch.h"
#include "match.h"
#include "utils.h"
#include "typecheck.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int expr_match_guard_item_check_type(symtab * tab, match_guard * match_value,
                                     int * result)
{
    symtab_entry * entry = NULL;

    entry = symtab_lookup(tab, match_value->guard_item.enum_id, SYMTAB_LOOKUP_GLOBAL);
    if (entry != NULL)
    {
        if (entry->type == SYMTAB_FUNC && entry->func_value != NULL)
        {
            *result = TYPECHECK_FAIL;
            print_error_msg(match_value->line_no, "found function %s at line %d instead of enum\n",
                            entry->func_value->decl->id, entry->func_value->line_no);
        }
        else if (entry->type == SYMTAB_PARAM && entry->param_value != NULL)
        {
            *result = TYPECHECK_FAIL;
            print_error_msg(match_value->line_no, "found symtab %s at line %d instead of enum\n",
                            entry->param_value->id, entry->param_value->line_no);
        }
        else if (entry->type == SYMTAB_BIND && entry->bind_value != NULL)
        {
            *result = TYPECHECK_FAIL;
            print_error_msg(match_value->line_no, "found binding %s at line %d instead of enum\n",
                            entry->bind_value->id, entry->bind_value->line_no);
        }
        else if (entry->type == SYMTAB_QUALIFIER && entry->qualifier_value != NULL)
        {
            *result = TYPECHECK_FAIL;
            print_error_msg(match_value->line_no, "found qualifier %s at line %d instead of enum\n",
                            entry->qualifier_value->id, entry->qualifier_value->line_no);
        }
        else if (entry->type == SYMTAB_RECORD && entry->record_value != NULL)
        {
            *result = TYPECHECK_FAIL;
            print_error_msg(match_value->line_no, "found record %s at line %d instead of enum\n",
                            entry->record_value->id, entry->record_value->line_no);
        }
        else if (entry->type == SYMTAB_ENUMTYPE && entry->enumtype_value != NULL)
        {
            enumerator * enumerator_value = enumtype_find_enumerator(entry->enumtype_value, match_value->guard_item.item_id);
            if (enumerator_value != NULL)
            {
                match_value->guard_item.enumtype_value = entry->enumtype_value;
                match_value->guard_item.enumerator_value = enumerator_value;
            }
            else
            {
                *result = TYPECHECK_FAIL;
                print_error_msg(match_value->line_no, "cannot find enum item %s.%s\n",
                                match_value->guard_item.enum_id, match_value->guard_item.item_id);
            }
        }
        else
        {
            assert(0);
        }
    }
    else
    {
        *result = TYPECHECK_FAIL;
        print_error_msg(match_value->line_no, "cannot find enum %s\n",
                        match_value->guard_item.enum_id);
    }

    return 0;
}                                     

int expr_match_guard_check_type(symtab * tab, match_guard * match_value,
                                func * func_value, unsigned int syn_level,
                                int * result)
{
    switch (match_value->type)
    {
    case MATCH_GUARD_ITEM:
        expr_match_guard_item_check_type(tab, match_value, result);
        expr_check_type(tab, match_value->guard_item.expr_value, func_value, syn_level, result);
    break;
    case MATCH_GUARD_ELSE:
        expr_check_type(tab, match_value->guard_else.expr_value, func_value, syn_level, result);
    break;
    }

    return 0;
}

int expr_match_guard_list_check_type(symtab * tab, match_guard_list * list,
                                     func * func_value,
                                     unsigned int syn_level, int * result)
{
    match_guard_list_node * node = list->tail;

    while (node != NULL)
    {
        match_guard * match_value = node->value;
        if (match_value != NULL)
        {
            expr_match_guard_check_type(tab, match_value, func_value, syn_level, result);
        }
        node = node->next;
    }

    return 0;
}                                     

int expr_match_guard_left_cmp(expr * value, match_guard * match_value, int * result)
{
    switch (match_value->type)
    {
    case MATCH_GUARD_ITEM:
        if (value->comb.comb != COMB_TYPE_ENUMTYPE)
        {
            *result = TYPECHECK_FAIL;
            print_error_msg(value->line_no,
                            "expression is %s not enum id\n",
                            comb_type_str(value->comb.comb));
            return 0;
        }
        if (value->comb.comb_enumtype != match_value->guard_item.enumtype_value)
        {
            *result = TYPECHECK_FAIL;
            print_error_msg(match_value->line_no,
                            "enums are different %s and %s\n",
                            value->comb.comb_enumtype->id,
                            match_value->guard_item.enumtype_value->id);
            return 0;
        }
    break;
    case MATCH_GUARD_ELSE:
    break;
    }
    
    return 0;
}

int expr_match_guard_list_left_cmp(expr * value, match_guard_list * list, int * result)
{
    match_guard_list_node * node = list->tail;

    while (node != NULL)
    {
        match_guard * match_value = node->value;
        if (match_value != NULL)
        {
            expr_match_guard_left_cmp(value, match_value, result);
        }
        node = node->next;
    }

    return 0;
}

unsigned int expr_match_gaurd_list_last_cnt(match_guard_list * list)
{
    unsigned int cnt = 0;
    match_guard_list_node * node = list->tail;
    
    while (node != NULL)
    {
        match_guard * match_value = node->value;
        if (match_value != NULL && match_value->type == MATCH_GUARD_ELSE)
        {
            cnt++; 
        }
        node = node->next;
    }
    
    return cnt;
}

int expr_match_guard_list_last_else(match_guard_list * list)
{
    match_guard_list_node * node = list->head;

    if (node != NULL)
    {
        match_guard * match_value = node->value;
        if (match_value != NULL && match_value->type == MATCH_GUARD_ELSE)
        {
            return TYPECHECK_SUCC;
        }
    }

    return TYPECHECK_FAIL;
}

int expr_match_guard_mark_item(match_guard * match_value)
{
    switch (match_value->type)
    {
        case MATCH_GUARD_ITEM:
        {
            enumerator * enumerator_value = match_value->guard_item.enumerator_value;
            if (enumerator_value != NULL)
            {
                /* not repeated values (warning) */
                if (enumerator_value->mark == 1)
                {
                    print_warning_msg(match_value->line_no,
                                      "repeated enum name %s.%s in match expression\n",
                                      match_value->guard_item.enum_id,
                                      match_value->guard_item.item_id);
                }
                enumerator_value->mark = 1;
            }
        }
        break;
        case MATCH_GUARD_ELSE:
        break;
    }

    return 0;
}

int expr_match_guard_list_mark_items(expr * value, match_guard_list * list)
{
    match_guard_list_node * node = list->tail;

    while (node != NULL)
    {
        match_guard * match_value = node->value;
        if (match_value != NULL)
        {
            expr_match_guard_mark_item(match_value);
        }
        node = node->next;
    }

    return 0;
}

int expr_match_guard_unmark_items(expr * value)
{
    expr * match_expr = value->match.expr_value;

    if (match_expr != NULL && match_expr->comb.comb == COMB_TYPE_ENUMTYPE)
    {
        enumtype * enum_value = match_expr->comb.comb_enumtype;
        if (enum_value != NULL && enum_value->enums != NULL)
        {
            enumerator_list_unmark(enum_value->enums);
        }
    }

    return 0;
}

int expr_match_guard_are_all_mark_items(expr * value, int * result)
{
    expr * match_expr = value->match.expr_value;

    if (match_expr != NULL && match_expr->comb.comb == COMB_TYPE_ENUMTYPE)
    {
        enumtype * enum_value = match_expr->comb.comb_enumtype;
        if (enum_value != NULL && enum_value->enums != NULL)
        {
            enumerator_list * list = enum_value->enums;
            if (list == NULL)
            {
                *result = TYPECHECK_SUCC;
                return 0;
            }

            enumerator_list_node * node = list->tail;
            while (node != NULL)
            {
                enumerator * enumerator_value = node->value;
                if (enumerator_value != NULL && enumerator_value->mark == 0)
                {
                    print_error_msg(value->line_no,
                                    "match expression does not cover %s.%s enum\n",
                                    enum_value->id, enumerator_value->id);
                    *result = TYPECHECK_FAIL;
                }
                node = node->next;
            }
        }
        else
        {
            *result = TYPECHECK_SUCC;
        }
    }
    else if (match_expr != NULL)
    {
        print_error_msg(value->line_no,
                        "match expression wrong type %s\n",
                        comb_type_str(match_expr->comb.comb));
        *result = TYPECHECK_FAIL;
    }

    return 0;
}

int expr_match_guard_list_exhaustive(expr * value, match_guard_list * list, int * result)
{
    unsigned int match_else_cnt = 0;
    
    match_else_cnt = expr_match_gaurd_list_last_cnt(list);
    if (match_else_cnt > 0)
    {
        /* not double else (warning) */
        if (match_else_cnt > 1)
        {
            print_warning_msg(value->line_no,
                              "match expression contains more than one else guard\n");
        }
        
        /* else as last guard present (warning) */
        if (expr_match_guard_list_last_else(list) == TYPECHECK_FAIL)
        {
            print_warning_msg(value->line_no,
                              "match expression else guard is not last\n");
        }
    }
    else
    {
        expr_match_guard_unmark_items(value);
    
        expr_match_guard_list_mark_items(value, list);
        
        expr_match_guard_are_all_mark_items(value, result);
    }

    return 0;
}

int expr_match_guard_list_right_cmp(expr * value, match_guard_list * list, int * result)
{
    match_guard_list_node * node = list->tail;

    if (node != NULL)
    {
        match_guard * first_guard = node->value;
        expr * first_expr = NULL;

        if (first_guard != NULL)
        {
            first_expr = match_guard_get_expr(first_guard);
        }
        
        node = node->next;

        while (node != NULL && first_expr != NULL)
        {
            match_guard * second_guard = node->value;
            expr * second_expr = NULL;
            
            if (second_guard != NULL)
            {
                second_expr = match_guard_get_expr(second_guard);
            }
        
            if (second_expr != NULL)
            {
                expr_comb_cmp_and_set(first_expr, second_expr, value, result);
            }
        
            node = node->next;
        }
    }

    return 0;
}                                    

int expr_comb_is_enum(expr * value, int * result)
{
    if (value->comb.comb == COMB_TYPE_ENUMTYPE)
    {
        return TYPECHECK_SUCC;
    }

    *result = TYPECHECK_FAIL;
    print_error_msg(value->line_no,
                    "expression is %s not enum name\n",
                    comb_type_str(value->comb.comb));

    return TYPECHECK_FAIL;
}

int expr_match_check_type(symtab * tab, expr * value, func * func_value,
                          unsigned int syn_level, int * result)
{
    expr_check_type(tab, value->match.expr_value, func_value, syn_level, result);

    /* expr_check is enum */
    expr_comb_is_enum(value->match.expr_value, result);

    /* initially expression type is of check expression */
    value->comb = value->match.expr_value->comb;
    
    if (value->match.match_guards != NULL)
    {
        expr_match_guard_list_check_type(tab, value->match.match_guards, func_value, syn_level, result);

        /* expr_check matches expr_match_guard_list type */
        expr_match_guard_list_left_cmp(value->match.expr_value, value->match.match_guards, result);

        /* exhaustive guards */
        expr_match_guard_list_exhaustive(value, value->match.match_guards, result);

        /* check if guard right sides are the same and set match type */
        expr_match_guard_list_right_cmp(value, value->match.match_guards, result);
    }

    if (*result == TYPECHECK_FAIL)
    {
        value->comb.comb = COMB_TYPE_ERR;
    }

    return 0;
}                              

