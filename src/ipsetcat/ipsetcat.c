/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010-2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */


#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libcork/core.h>

#include "ipset/ipset.h"


static char  *input_filename = "-";
static char  *output_filename = "-";
static bool  want_networks = false;


static struct option longopts[] = {
    { "output", required_argument, NULL, 'o' },
    { "networks", no_argument, NULL, 'n' },
    { NULL, 0, NULL, 0 }
};

static void
usage(void)
{
    fprintf(stderr,
            "Usage: ipsetcat [--output=<output file>]\n"
            "                [--networks]\n"
            "                <IP file>\n");
}


int
main(int argc, char **argv)
{
    ipset_init_library();

    /* Parse the command-line options. */

    int  ch;
    while ((ch = getopt_long(argc, argv, "o:n", longopts, NULL)) != -1) {
        switch (ch) {
            case 'o':
                output_filename = optarg;
                break;

            case 'n':
                want_networks = true;
                break;

            default:
                usage();
                exit(1);
        }
    }

    argc -= optind;
    argv += optind;

    if (argc > 1) {
        fprintf(stderr, "ERROR: You cannot specify multiple input files.\n");
        usage();
        exit(1);
    }

    input_filename = argv[0];

    /* Read in the IP set files specified on the command line. */
    struct ip_set  *set = NULL;
    FILE  *stream;
    bool  close_stream;

    /* Create a FILE object for the file. */
    if (strcmp(input_filename, "-") == 0) {
        fprintf(stderr, "Opening stdin...\n");
        input_filename = "stdin";
        stream = stdin;
        close_stream = false;
    } else {
        fprintf(stderr, "Opening file %s...\n", input_filename);
        stream = fopen(input_filename, "rb");
        if (stream == NULL) {
            fprintf(stderr, "Cannot open file %s:\n  %s\n",
                    input_filename, strerror(errno));
            exit(1);
        }
        close_stream = true;
    }

    /* Read in the IP set from the specified file. */
    set = ipset_load(stream);
    if (set == NULL) {
        fprintf(stderr, "Error reading %s:\n  %s\n",
                input_filename, cork_error_message());
        exit(1);
    }

    if (close_stream) {
        fclose(stream);
    }

    /* Print out the IP addresses in the set. */
    FILE  *ostream;
    bool  close_ostream;
    if ((output_filename == NULL) || (strcmp(output_filename, "-") == 0)) {
        fprintf(stderr, "Writing to stdout...\n");
        ostream = stdout;
        output_filename = "stdout";
        close_ostream = false;
    } else {
        fprintf(stderr, "Writing to file %s...\n", output_filename);
        ostream = fopen(output_filename, "wb");
        if (ostream == NULL) {
            fprintf(stderr, "Cannot open file %s:\n  %s\n",
                    output_filename, strerror(errno));
            exit(1);
        }
        close_ostream = true;
    }

    char  ip_buf[CORK_IP_STRING_LENGTH];
    struct cork_buffer  buf = CORK_BUFFER_INIT();

    struct ipset_iterator  *it;
    if (want_networks) {
        /* If requested, iterate through network blocks instead of
         * individual IP addresses. */
        it = ipset_iterate_networks(set, true);
    } else {
        /* The user wants individual IP addresses.  Hope they know what
         * they're doing! */
        it = ipset_iterate(set, true);
    }

    for (/* nothing */; !it->finished; ipset_iterator_advance(it)) {
        cork_ip_to_raw_string(&it->addr, ip_buf);
        if ((it->addr.version == 4 && it->cidr_prefix == 32) ||
            (it->addr.version == 6 && it->cidr_prefix == 128)) {
            cork_buffer_printf(&buf, "%s\n", ip_buf);
        } else {
            cork_buffer_printf(&buf, "%s/%u\n", ip_buf, it->cidr_prefix);
        }

        if (fputs(buf.buf, ostream) == EOF) {
            fprintf(stderr, "Cannot write to file %s:\n  %s\n",
                    output_filename, strerror(errno));
            exit(1);
        }
    }

    cork_buffer_done(&buf);
    ipset_free(set);

    /* Close the output stream for exiting. */
    if (close_ostream) {
        fclose(ostream);
    }

    return 0;
}
