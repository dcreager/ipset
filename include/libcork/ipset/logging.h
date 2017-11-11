/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, libcorkipset authors
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_IPSET_LOGGING_H
#define LIBCORK_IPSET_LOGGING_H


#if !defined(IPSET_DEBUG)
#define IPSET_DEBUG 0
#endif

#if IPSET_DEBUG
#include <stdio.h>
#define DEBUG(...) \
    do { \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n"); \
    } while (0)
#else
#define DEBUG(...) /* no debug messages */
#endif


#endif  /* LIBCORK_IPSET_LOGGING_H */
