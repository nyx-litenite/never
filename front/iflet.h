/**
 * Copyright 2020 Slawomir Maludzinski
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
#ifndef __IFLET_H__
#define __IFLET_H__

typedef struct match_guard_item match_guard_item;
typedef struct match_guard_record match_guard_record;
typedef struct expr expr;

typedef enum iflet_type
{
    IFLET_TYPE_ITEM = 1,
    IFLET_TYPE_RECORD = 2
} iflet_type;

typedef struct iflet
{
    iflet_type type;
    union
    {
        match_guard_item * guard_item;
        match_guard_record * guard_record;
    };
    expr * expr_value;
    expr * then_value;
    expr * else_value;
    unsigned int line_no;
} iflet;

iflet * iflet_item_new(match_guard_item * guard_item,
                       expr * expr_value, expr * then_value, expr * else_value);
iflet * iflet_record_new(match_guard_record * guard_record,
                         expr * expr_value, expr * then_value, expr * else_value);

void iflet_delete(iflet * value);

#endif /* __IFLET_H__ */

