/*
 * Simple alignment demonstration
 *
 * Copyright (C) 2016 Synthetel Corporation. All rights reserved.
 * Web-site: https://www.synthetel.com
 * Author: Shao Miller <github@synthetel.com>
 *
 * License:
 *   You are permitted to download, modify, compile and use this
 * code, but you may not re-distribute it in either source-code nor compiled
 * form unless this entire C comment-block is reproduced and distributed intact
 * with your re-distribution, or unless you have obtained explicit permission
 * from Synthetel Corporation.
 * (This simple license will be reviewed, at some point.)
 *
 * Details:
 *   Ordering the members of a structure from largest to smallest tends to
 * maximize storage efficiency.  This is a small demonstration.
 *
 *   It is sometimes the case that you are using a library that uses an
 * opaque structure which you might like to allocate, track, and deallocate
 * on your own, perhaps combining the structure with other data to save the
 * overhead of allocations or simply to keep things together for easy
 * debugging.  If you know the size of the opaque structure, you can use
 * the TailAlignedSize function in this code to portably determine a
 * storage-size which is safely aligned for your own data as well as for the
 * opaque structure, with the latter occupying the end of that storage-size.
 *
 *   For example, libevent2 provides the 'event_get_struct_event_size'
 * function, which allows your program to know the size of a 'struct event'
 * at run-time instead of at translation-time.  With this knowledge, you
 * can determine the size you'd need to have your own contextual data with
 * a trailing 'struct event'.  If the common case for you is to have an
 * allocation for contextual data and an allocation for the libevent2 event,
 * this strategy reduces your allocations by half.
 *
 * Simple math reveals where a trailing object begins, given the head, or
 * where a header object begins, given the tail.
 */
#include <stdlib.h>
#include <stdio.h>
#include "align.h"

#define DCountOf(arr) (sizeof (arr) / sizeof *(arr))
#define DQuote(x) # x
#define DSizeDesc(type) { sizeof (type), DQuote(type) }

struct size_desc
  {
    size_t sz;
    const char * name;
  };

static size_t max_factor(const struct size_desc * sizes, const size_t count);
static void print_order(const struct size_desc * sizes, const size_t count);
static void show_padding1(const struct size_desc * sizes, const size_t count);
static void show_padding2(const struct size_desc * sizes, const size_t count);

int main(void)
  {
    unsigned int i;
    static struct size_desc test1[] =
      {
        DSizeDesc(char[2]),
        DSizeDesc(double),
        DSizeDesc(int),
        DSizeDesc(void *),
        DSizeDesc(short[3])
      };
    static struct size_desc test2[] =
      {
        DSizeDesc(char[2]),
        DSizeDesc(double),
        DSizeDesc(int),
        DSizeDesc(void *)
      };
    struct test
      {
        struct size_desc * first;
        size_t cnt;
      }
      * test,
      tests[] =
      {
        { test1, DCountOf(test1) },
        { test2, DCountOf(test2) }
      };


    for (i = 0; i < DCountOf(tests); ++i)
      {
        printf("--- Test #%u ---\n\n", (unsigned int) i + 1);
        test = tests + i;

        printf("Original order:\n");
        print_order(test->first, test->cnt);
        puts("");

        show_padding1(test->first, test->cnt);
        show_padding2(test->first, test->cnt);

        SortSizesDescending(&test->first->sz, test->cnt, sizeof *test->first);
        printf("Sorted largest to smallest:\n");
        print_order(test->first, test->cnt);
        puts("");

        show_padding1(test->first, test->cnt);
        show_padding2(test->first, test->cnt);
      }

    return EXIT_SUCCESS;
  }

static size_t max_factor(const struct size_desc * sizes, const size_t count)
  {
    size_t factor;
    size_t i;
    size_t max;

    for ((max = 0), (i = 0); i < count; ++i)
      {
        factor = LargestPowerOfTwoFactor(sizes[i].sz);
        if (factor > max)
          max = factor;
      }
    return max;
  }

static void print_order(const struct size_desc * sizes, const size_t count)
  {
    typedef unsigned int ui;
    size_t i;

    for (i = 0; i < count; ++i)
      printf("%u: %u : %s\n", (ui) i, (ui) sizes[i].sz, sizes[i].name);
  }

static void show_padding1(const struct size_desc * sizes, const size_t count)
  {
    size_t i;
    unsigned int padding;
    size_t sum;

    /* Compute padding, given this order */
    printf("Assuming we build the structure member-wise-incrementally...\n");
    for ((i = 1), (sum = sizes->sz); i < count; ++i)
      {
        padding = (unsigned int) PaddingSize(sum, sizes[i].sz);
        if (padding)
          printf("%u bytes of padding before %u\n", padding, (unsigned int) i);
        sum = TailAlignedSize(sum, sizes[i].sz);
      }
    printf("Total size: %u bytes\n\n", (unsigned int) sum);
  }

static void show_padding2(const struct size_desc * sizes, const size_t count)
  {
    typedef unsigned int ui;
    size_t factor;
    size_t i;
    size_t max;
    unsigned int padding;
    size_t r;
    size_t sum;

    /* Compute padding, given this order */
    printf("Assuming we build the structure with full knowledge...\n");
    max = max_factor(sizes, count);
    printf("Alignment requirement: %u\n", (ui) max);
    for ((i = 1), (sum = sizes->sz); i < count; ++i)
      {
        factor = LargestPowerOfTwoFactor(sizes[i].sz);
        sum += sizes[i].sz;
        r = sum % factor;
        if (r)
          {
            padding = factor - r;
            printf("%u bytes of padding before %u\n", padding, (ui) i);
            sum += padding;
          }
      }
    factor = max;
    r = sum % factor;
    if (r)
      {
        padding = factor - r;
        printf("%u bytes of padding at the end\n", padding);
        sum += padding;
      }
    printf("Total size: %u bytes\n\n", (ui) sum);
  }
