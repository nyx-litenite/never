/**
 * Copyright 2018 Slawomir Maludzinski
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "object.h"

object_arr_dim * object_arr_dim_new(unsigned int dims)
{
    object_arr_dim * dv = (object_arr_dim *)malloc(dims * sizeof(object_arr_dim));    
    return dv;
}

void object_arr_dim_delete(object_arr_dim * dv)
{
    free(dv);
}

object * object_new()
{
    object * obj = (object *)malloc(sizeof(object));
    
    return obj;
}

object * object_new_int(int value)
{
    object * obj = (object *)malloc(sizeof(object));

    obj->type = OBJECT_INT;
    obj->int_value = value;
    
    return obj;
}

object * object_new_float(float value)
{
    object * obj = (object *)malloc(sizeof(object));

    obj->type = OBJECT_FLOAT;
    obj->float_value = value;
    
    return obj;
}

object * object_new_vec(unsigned int size)
{
    object * obj = (object *)malloc(sizeof(object));
    object_vec * vec_value = (object_vec *)malloc(sizeof(object_vec));

    vec_value->size = size;
    if (vec_value->size == 0)
    {
        vec_value->value = NULL;
    }
    else
    {
        vec_value->value = (mem_ptr *)malloc(size * sizeof(mem_ptr));
    }

    obj->type = OBJECT_VEC;
    obj->vec_value = vec_value;
    
    return obj;
}

void object_arr_dim_mult(unsigned int dims, object_arr_dim * dv, unsigned int * elems)
{
    unsigned int d = 0;
    unsigned int e = 1;
    
    for (d = 0; d < dims; d++)
    {
        e *= dv[d].elems;
    }
    *elems = e;
    for (d = 0; d < dims; d++)
    {
        e /= dv[d].elems;
        dv[d].mult = e;
    }
}

unsigned int object_arr_dim_addr(unsigned int dims, object_arr_dim * dv, object_arr_dim * addr, int * oobounds)
{
    unsigned int m;
    unsigned int addr_int = 0;
    
    for (m = 0; m < dims; m++)
    {
        if (dv[m].elems <= addr[m].mult)
        {
            *oobounds = m;
            return 0;
        }
        addr_int += dv[m].mult * addr[m].mult;
    }
    
    *oobounds = -1;
    return addr_int;
}

object * object_new_arr(unsigned int dims, object_arr_dim * dv)
{
    object * obj = (object *)malloc(sizeof(object));
    object_arr * arr_value = (object_arr *)malloc(sizeof(object_arr));
    
    arr_value->dims = dims;
    arr_value->dv = dv;
    object_arr_dim_mult(dims, arr_value->dv, &arr_value->elems);
    
    if (arr_value->elems != 0)
    {
        arr_value->value = (mem_ptr *)malloc(arr_value->elems * sizeof(mem_ptr));
    }
    else
    {
        arr_value->value = NULL;
    }

    obj->type = OBJECT_ARRAY;
    obj->arr_value = arr_value;
    
    return obj;
}

object * object_new_func(mem_ptr vec, ip_ptr addr)
{
    object * obj = (object *)malloc(sizeof(object));
    object_func * func_value = (object_func *)malloc(sizeof(object_func));

    func_value->vec = vec;
    func_value->addr = addr;

    obj->type = OBJECT_FUNC;
    obj->func_value = func_value;

    return obj;
}

void object_delete(object * obj)
{
    switch (obj->type)
    {
        case OBJECT_UNKNOWN:
            assert(0);
        break;
        case OBJECT_INT:
        case OBJECT_FLOAT:
        break;
        case OBJECT_VEC:
            if (obj->vec_value != NULL && obj->vec_value->value != NULL)
            {
                free(obj->vec_value->value);
            }
            if (obj->vec_value != NULL)
            {
                free(obj->vec_value);
            }
        break;
        case OBJECT_ARRAY:
            if (obj->arr_value != NULL)
            {
                if (obj->arr_value->value != NULL)
                {
                    free(obj->arr_value->value);
                }
                if (obj->arr_value->dv != NULL)
                {
                    object_arr_dim_delete(obj->arr_value->dv);
                }
                free(obj->arr_value);
            }
        break;
        case OBJECT_FUNC:
            free(obj->func_value);
        break;
    }
    free(obj);
}

void object_print(object * obj)
{
    switch (obj->type)
    {
        case OBJECT_UNKNOWN:
            printf("object_unknown\n");
        break;
        case OBJECT_INT:
            printf("object_int\n");
        break;
        case OBJECT_FLOAT:
            printf("object_float\n");
        break;
        case OBJECT_VEC:
            printf("object_vec\n");
        break;
        case OBJECT_ARRAY:
            printf("object_array\n");
        break;
        case OBJECT_FUNC:
            printf("object_func\n");
        break;
    }
}
