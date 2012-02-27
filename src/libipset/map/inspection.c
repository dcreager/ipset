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

bool
ipmap_is_empty(const struct ip_map *map)
{
    /* Since BDDs are unique, any map that maps all addresses to the
     * default value is “empty”. */
    return (map->map_bdd == map->default_bdd);
}

bool
ipmap_is_equal(const struct ip_map *map1, const struct ip_map *map2)
{
    /* Since BDDs are unique, maps can only be equal if their BDDs are
     * equal. */
    return (map1->map_bdd == map2->map_bdd);
}

size_t
ipmap_memory_size(const struct ip_map *map)
{
    return ipset_node_memory_size(map->map_bdd);
}


void
ipmap_ip_set(struct ip_map *map, struct cork_ip *addr, int value)
{
    if (addr->version == 4) {
        ipmap_ipv4_set(map, &addr->ip.v4, value);
    } else {
        ipmap_ipv6_set(map, &addr->ip.v6, value);
    }
}


void
ipmap_ip_set_network(struct ip_map *map, struct cork_ip *addr,
                     unsigned int netmask, int value)
{
    if (addr->version == 4) {
        ipmap_ipv4_set_network(map, &addr->ip.v4, netmask, value);
    } else {
        ipmap_ipv6_set_network(map, &addr->ip.v6, netmask, value);
    }
}


int
ipmap_ip_get(struct ip_map *map, struct cork_ip *addr)
{
    if (addr->version == 4) {
        return ipmap_ipv4_get(map, &addr->ip.v4);
    } else {
        return ipmap_ipv6_get(map, &addr->ip.v6);
    }
}
