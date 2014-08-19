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
#include "../src/pair.h"
#include "../src/pubsub.h"
#include "../src/ipc.h"

#include "testutil.h"

#define SOCKET_ADDRESS "ipc://test_nopeers.ipc"

#define BATCH_SEND 10

int main ()
{
    const char * msg = "01234567890123456789";
    int pub;
    int ret;
    unsigned int timeout;

    /* Open a pub socket, no peers listening. */
    pub = test_socket (AF_SP, NN_PUB);
    test_bind (pub, SOCKET_ADDRESS);

    /* Non blocking send will return right away with EAGAIN. */
    ret = nn_send (pub, msg, sizeof( msg ), NN_DONTWAIT);
    nn_assert (ret == -1 && errno == EAGAIN);

    /* Blocking, with a non infinite timeout (>=0) will return with ENOTCONN. */
    timeout = 0;
    ret = nn_setsockopt (pub, NN_SOL_SOCKET, NN_SNDTIMEO, &timeout, sizeof( timeout ) );
    nn_assert (ret == 0);
    ret = nn_send (pub, msg, sizeof( msg ), 0);
    nn_assert (ret == -1 && errno == ENOTCONN);

    test_close (pub);
    return 0;
}
