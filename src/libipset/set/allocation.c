/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009-2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <libcork/core.h>

#include "ipset/bdd/nodes.h"
#include "ipset/ipset.h"
#include "../internal.h"


void
ipset_init(struct ip_set *set)
{
    /* The set starts empty, so every value assignment should yield
     * false. */
    set->set_bdd = ipset_node_cache_terminal(ipset_cache, false);
}


struct ip_set *
ipset_new(void)
{
    struct ip_set  *result = cork_new(struct ip_set);
    ipset_init(result);
    return result;
}


void
ipset_done(struct ip_set *set)
{
    /* nothing to do */
}


void
ipset_free(struct ip_set *set)
{
    ipset_done(set);
    free(set);
}
