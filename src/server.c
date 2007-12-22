/*
 * uoproxy
 *
 * (c) 2005-2007 Max Kellermann <max@duempel.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "log.h"

#include <sys/types.h>
#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "server.h"
#include "sockbuff.h"
#include "buffer.h"
#include "compression.h"
#include "packets.h"
#include "dump.h"

struct uo_server {
    struct sock_buff *sock;
    uint32_t seed;
    int compression_enabled;

    const struct uo_server_handler *handler;
    void *handler_ctx;
};

static void
uo_server_invoke_free(struct uo_server *server)
{
    const struct uo_server_handler *handler;

    assert(server->handler != NULL);

    handler = server->handler;
    server->handler = NULL;

    handler->free(server->handler_ctx);
}

static void *
uo_server_peek(struct uo_server *server, size_t *lengthp);

static void
uo_server_shift(struct uo_server *server, size_t nbytes);

static int
server_sock_buff_data(void *ctx)
{
    struct uo_server *server = ctx;
    void *data;
    size_t length;
    int ret;

    while ((data = uo_server_peek(server, &length)) != NULL) {
        uo_server_shift(server, length);

        ret = server->handler->packet(data, length, server->handler_ctx);
        if (ret <= 0)
            return ret;
    }

    return 0;
}

static void
server_sock_buff_free(int error, void *ctx)
{
    struct uo_server *server = ctx;

    if (error == 0)
        log(2, "client closed the connection\n");
    else
        log_error("error during communication with client", error);

    sock_buff_dispose(server->sock);
    server->sock = NULL;

    uo_server_invoke_free(server);
}

struct sock_buff_handler server_sock_buff_handler = {
    .data = server_sock_buff_data,
    .free = server_sock_buff_free,
};

int uo_server_create(int sockfd,
                     const struct uo_server_handler *handler,
                     void *handler_ctx,
                     struct uo_server **serverp) {
    int ret;
    struct uo_server *server;

    assert(handler != NULL);
    assert(handler->packet != NULL);
    assert(handler->free != NULL);

    server = (struct uo_server*)calloc(1, sizeof(*server));
    if (server == NULL)
        return -ENOMEM;

    ret = sock_buff_create(sockfd, 8192, 65536,
                           &server_sock_buff_handler, server,
                           &server->sock);
    if (ret < 0) {
        free(server);
        return -ENOMEM;
    }

    server->handler = handler;
    server->handler_ctx = handler_ctx;

    *serverp = server;

    return 0;
}

void uo_server_dispose(struct uo_server *server) {
    if (server->sock != NULL)
        sock_buff_dispose(server->sock);
    free(server);
}

int uo_server_fileno(const struct uo_server *server) {
    return server->sock->fd;
}

u_int32_t uo_server_seed(const struct uo_server *server) {
    return server->seed;
}

static void *
uo_server_peek(struct uo_server *server, size_t *lengthp)
{
    unsigned char *p;
    size_t length, packet_length;

    if (server->sock == NULL)
        return NULL;

    p = buffer_peek(server->sock->input, &length);
    if (p == NULL)
        return NULL;

#ifdef DUMP_SERVER_PEEK
    printf("peek from client, length=%zu:\n", length);
    fhexdump(stdout, "  ", p, length);
    fflush(stdout);
#endif

    if (server->seed == 0) {
        /* the first packet from a client is the seed, 4 bytes without
           header */
        if (length < 4)
            return NULL;

        server->seed = *(uint32_t*)p;
        if (server->seed == 0) {
            fprintf(stderr, "zero seed from client\n");
            sock_buff_dispose(server->sock);
            server->sock = NULL;
            return NULL;
        }

        buffer_shift(server->sock->input, 4);
        sock_buff_event_setup(server->sock);

        p = buffer_peek(server->sock->input, &length);
        if (p == NULL)
            return NULL;
    }

    packet_length = get_packet_length(p, length);
    if (packet_length == PACKET_LENGTH_INVALID) {
        fprintf(stderr, "malformed packet from client:\n");
        fhexdump(stderr, "  ", p, length);
        fflush(stderr);
        sock_buff_dispose(server->sock);
        server->sock = NULL;
        return NULL;
    }

#ifdef DUMP_HEADERS
    printf("from client: 0x%02x length=%zu\n", p[0], packet_length);
#endif
    if (packet_length == 0 || packet_length > length)
        return NULL;
#ifdef DUMP_SERVER_RECEIVE
    fhexdump(stdout, "  ", p, packet_length);
    fflush(stdout);
#endif

    if (p[0] == PCK_GameLogin)
        server->compression_enabled = 1;

    *lengthp = packet_length;
    return p;
}

static void
uo_server_shift(struct uo_server *server, size_t nbytes)
{
    if (server->sock == NULL)
        return;

    buffer_shift(server->sock->input, nbytes);
    sock_buff_event_setup(server->sock);
}

void uo_server_send(struct uo_server *server,
                    const void *src, size_t length) {
    assert(length > 0);
    assert(get_packet_length(src, length) == length);

    if (server->sock == NULL)
        return;

#ifdef DUMP_SERVER_SEND
    printf("sending to packet to client, length=%zu:\n", length);
    fhexdump(stdout, "  ", src, length);
    fflush(stdout);
#endif

    if (server->compression_enabled) {
        ssize_t nbytes;

        nbytes = uo_compress(buffer_tail(server->sock->output),
                             buffer_free(server->sock->output),
                             src, length);
        if (nbytes < 0) {
            fprintf(stderr, "uo_compress() failed\n");
            sock_buff_dispose(server->sock);
            server->sock = NULL;
            return;
        }

        buffer_expand(server->sock->output, (size_t)nbytes);
    } else {
        if (length > buffer_free(server->sock->output)) {
            fprintf(stderr, "output buffer full in uo_server_send()\n");
            sock_buff_dispose(server->sock);
            server->sock = NULL;
            return;
        }

        buffer_append(server->sock->output, src, length);
    }

    sock_buff_event_setup(server->sock);
}
