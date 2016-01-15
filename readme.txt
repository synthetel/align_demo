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

Compile with something like:

  gcc -ansi -pedantic -Wall -Wextra -Werror -o align align.c test.c

(Unless SIZE_MAX gives you trouble, in which case you might require C99 mode.)

What you might find from the output is that sometimes changing the order
of members makes a difference and sometimes it doesn't!
