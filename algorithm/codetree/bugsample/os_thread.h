/*========================================================================

*//** @file os_thread.h  

This is the os-generic portion of the OS abstraction layer regarding 
threading. 

This OS abstraction layer for threading is intended to be used only
by the JPEG library and is not exported.

The API abstracts the OS implementation of primitives such as threads,
mutexes and conditional variables. The symmantics is extremely similar
to the posix thread API. However only a subset of feationality is
available. Broadcasting of conditional variables is for example omitted
intentionally from this API for ease of implementation on different
OS'es.

par EXTERNALIZED FUNCTIONS
  (none)

@par INITIALIZATION AND SEQUENCING REQUIREMENTS
  (none)

Copyright (C) 2009 Qualcomm Technologies, Inc.
All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
*//*====================================================================== */

/*========================================================================
                             Edit History

$Header:

when       who     what, where, why
--------   ---     -------------------------------------------------------
05/07/08   vma     Created file.

========================================================================== */
