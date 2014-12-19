#ifndef __radixtree_h__
#define __radixtree_h__

/*
 * https://github.com/j0sh/radixtree.git
 *
 * Copyright 2010 Josh Allmann <joshua.allmann@gmail.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided with
 *       the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY Josh Allmann ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL Josh Allmann OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * )SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#define RADIXTREE_KEYSIZE 128

typedef struct rxt_node {
    int   color;
    char *key;
    void *value;
    int   pos; // bit index of the key to compare at (critical position)
    char  keycache[RADIXTREE_KEYSIZE];
    int   level; // tree level; for debug only
    int   parent_id; //for debug only

    struct rxt_node *parent;
    struct rxt_node *left;
    struct rxt_node *right;
}rxt_node;

int       rxt_put(char*, void *, rxt_node*);
void     *rxt_get(char*, rxt_node*);
void     *rxt_delete(char*, rxt_node*);

void      rxt_free(rxt_node *);
rxt_node *rxt_init();

#endif

