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
/* C >= C99 required for SIZE_MAX */
#include <stdint.h>
#include <stdlib.h>
#include "align.h"

static int size_sorter(const void * a, const void * b);

size_t LargestPowerOfTwoFactor(size_t Number)
  {
    size_t npow;
    size_t pow;

    npow = pow = 1;
    while (Number >= npow && !(Number % npow) && SIZE_MAX - npow >= npow)
      {
        pow = npow;
        npow *= 2;
      }
    return pow;
  }

size_t PaddingSize(size_t HeadSize, size_t TailSize)
  {
    size_t total_size;

    total_size = TailAlignedSize(HeadSize, TailSize);
    if (!total_size)
      return SIZE_MAX;
    return total_size - TailSize - HeadSize;
  }

/* Used by SortSizesDescending */
static int size_sorter(const void * a, const void * b)
  {
    size_t sa;
    size_t sb;

    /* The naive test doesn't account for overflow */
    sa = *(size_t *) a;
    sb = *(size_t *) b;
    if (sa < sb)
      return 1;
    if (sa > sb)
      return -1;
    return 0;
  }

void SortSizesDescending(size_t * Array, size_t Count, size_t Size)
  {
    /* Nothing to do? */
    if (!Array || !Count || Size < sizeof (size_t))
      return;
    qsort(Array, Count, Size, size_sorter);
  }

size_t TailAlignedSize(size_t HeadSize, size_t TailSize)
  {
    size_t factor;
    size_t hpow;
    size_t padding;
    size_t remainder;
    size_t sum;
    size_t tpow;

    /* Check for proper parameters */
    if (!HeadSize || !TailSize)
      return 0;

    /* Determine largest power-of-two factor for head */
    hpow = LargestPowerOfTwoFactor(HeadSize);

    /* Determine largest power-of-two factor for tail */
    tpow = LargestPowerOfTwoFactor(TailSize);

    /* Choose the strictest alignment */
    if (hpow > tpow)
      factor = hpow;
      else
      factor = tpow;

    /* Check for overflow */
    if (SIZE_MAX - HeadSize < TailSize)
      return 0;

    /* Tentative, optimistic result */
    sum = HeadSize + TailSize;

    /* Avoid unexpected problems */
    remainder = sum % factor;
    if (remainder)
      {
        padding = factor - remainder;
        /* Check for overflow */
        if (SIZE_MAX - sum < padding)
          return 0;
        sum += padding;
      }

    return sum;
  }

size_t TailOffset(size_t TotalSize, size_t TailSize)
  {
    return TotalSize - TailSize;
  }
