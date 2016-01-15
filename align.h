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
#ifndef DIncluded_align
#define DIncluded_align 1
#include <stddef.h>
/* Determine largest power-of-two factor for Number */
extern size_t LargestPowerOfTwoFactor(size_t Number);
/*
 * Returns the size of any padding that would be introduced by the
 * TailAlignedSize function.  If the arguments would result in the
 * TailAlignedSize function returning zero, this function returns
 * SIZE_MAX
 */
extern size_t PaddingSize(size_t HeadSize, size_t TailSize);
/*
 * Sort an array of elements into descending order.  The Array consists
 * of Count elements, each having Size bytes.  For each element, a size_t is
 * expected to be at offset 0 into that element.  It is the value of this
 * size_t object which determines the final ordering.  If Array is null or
 * Count is zero or Size < the size of a size_t, this function simply
 * returns without modifying anything
 */
extern void SortSizesDescending(size_t * Array, size_t Count, size_t Size);
/*
 * Returns the total size required to satisfy the alignment for two objects:
 *   The head: An object at an offset of zero
 *   The tail: An object whose final byte is the final byte of the total size
 * If either size is zero, this function returns zero.
 * If the sizes of the two objects are too large, this funtion returns zero.
 * Padding, if any, will begin at HeadSize bytes into the total size
 */
extern size_t TailAlignedSize(size_t HeadSize, size_t TailSize);
extern size_t TailOffset(size_t TotalSize, size_t TailSize);

#endif /* DIncluded_align */
