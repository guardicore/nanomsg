/*
    Copyright (c) 2012 Martin Sustrik  All rights reserved.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom
    the Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
*/

#ifndef NN_GLOCK_INCLUDED
#define NN_GLOCK_INCLUDED

/*  Implementation of a global lock (critical section). The lock is meant to
    be used to synchronise the initialisation/termination of the library. */


#ifdef GLOBCAL_LOCK_DBG
#define nn_glock_lock()	do {															        	\
        DWORD pid = GetCurrentProcessId();                                                          \
		FILE *fp = fopen("nanomsg-lock.log", "a");											        \
		fprintf(fp, "pid=%d:%s:%d[%s]: nn_glock_lock() IN\n", pid, __FILE__, __LINE__, __FUNCTION__);	\
		_nn_glock_lock();																	\
		fprintf(fp, "pid=%d:%s:%d[%s]: nn_glock_lock() OUT\n", pid, __FILE__, __LINE__, __FUNCTION__);	\
		fclose(fp);																			\
	} while (0)


#define nn_glock_unlock()	do {	                                                                \
		FILE *fp = fopen("nanomsg-lock.log", "a");												    \
        DWORD pid = GetCurrentProcessId();                                                          \
		fprintf(fp, "pid=%d:%s:%d[%s]: nn_glock_unlock() IN\n", pid, __FILE__, __LINE__, __FUNCTION__);	\
		_nn_glock_unlock();																		    \
		fprintf(fp, "pid=%d:%s:%d[%s]: nn_glock_unlock() OUT\n", pid, __FILE__, __LINE__, __FUNCTION__);	\
		fclose(fp);																				    \
	} while (0)

#else

#define nn_glock_lock()	_nn_glock_lock()
#define nn_glock_unlock()	_nn_glock_unlock()

#endif

void _nn_glock_lock (void);
void _nn_glock_unlock (void);

#endif

