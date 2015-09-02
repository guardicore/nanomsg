/*
    Copyright (c) 2012 250bpm s.r.o.  All rights reserved.

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

#include "../src/nn.h"
#include "../src/pubsub.h"
#include "../src/ipc.h"

#include "testutil.h"
#include "../src/utils/thread.c"

/* This tests two scenarios
 * 1. Initializing an ipc be it sub/pub in XP and closing the same socket on another thread
 * 2. Creating an ipc like a pub on one thread and creating the partner ipc like sub in another thread. 
 *
 * so as a rule for XP, well for using nanomsg in general since we still support XP that we have to be mindful which thread we create our sockets/ipc since it might cause 
 * unknown bugs in XP, for Vista+ you don't have to do this as it can support it.
 */

extern int nn_test_force_xp;

//tests an issue in XP where an ipc crashes when initialized in another thread and deleted on the main thread
#define MAIN_IPC "ipc://main.ipc"
int g_main_pub_s, g_main_pub_ep, g_main_sub_s, g_main_sub_ep;
int g_ThreadExit = 0, g_sub_created = 0;
struct nn_pollfd pfd [1];
void init_pub()
{
	int timeout = 0, ret = 0;
	//create a PUB socket that  no client will connect to
	g_main_pub_s = test_socket( AF_SP, NN_PUB );
	nn_assert(g_main_pub_s >= 0);

	ret = nn_setsockopt( g_main_pub_s, NN_SOL_SOCKET, NN_SNDTIMEO, &timeout, sizeof(timeout) );
	nn_assert( ret == 0 );
	g_main_pub_ep = test_bind( g_main_pub_s, MAIN_IPC );
}

static void routine_2 (NN_UNUSED void *arg)
{
	int timeout = 0, ret = 0;
	int rc = 0, poll_interval = 500; //poll every 500ms
	int ctr = 0;

	//create our sub in this thread
	g_main_sub_s = test_socket( AF_SP, NN_SUB );
	nn_assert(g_main_sub_s >= 0);

	ret = nn_setsockopt( g_main_sub_s, NN_SUB, NN_SUB_SUBSCRIBE, "", 0 );
	nn_assert( ret == 0 );

	g_main_sub_ep = test_connect( g_main_sub_s, MAIN_IPC ); 

	g_sub_created = 1; //we let our main thread know that the sub has been initialized and supposedly connected. No errors will occur on this side
}

static void routine_1 (NN_UNUSED void *arg)
{
	init_pub();
}

//NOTE: comment to do second test
#define TEST_1

int main ()
{
	int err = 0;
	struct nn_thread thread;
	nn_test_force_xp = 1; //force to use XP code

#ifdef TEST_1
	nn_thread_init (&thread, routine_1, NULL);

	nn_thread_term (&thread);
	
	/*
	 * Expected behavior
	   Vista+ : will wait infinitely at close
	   XP : will assert at close internally because of a bad state.
	*/
	test_close(g_main_pub_s);
#else
	init_pub();

	nn_thread_init (&thread, routine_2, NULL);
	while(g_sub_created == 0) {}; //wait for the thread to create our sub socket
	/*
	 * Expected behavior
	   XP and Vista+ : will assert at sending since no connection has been made.
	*/
	test_send (g_main_pub_s, "hello"); 
	nn_thread_term (&thread);

#endif

	return 0;
}

