/*
 * uoproxy
 * $Id$
 *
 * (c) 2005 Max Kellermann <max@duempel.org>
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

#ifndef __INSTANCE_H
#define __INSTANCE_H

struct selectx;

struct instance {
    /* configuration */
    struct config *config;

    /* state */
    struct connection *connections_head;
    struct relay_list *relays;

    struct timeval tv;
};

void instance_pre_select(struct instance *instance,
                         struct selectx *sx);

void instance_post_select(struct instance *instance,
                          struct selectx *sx);

void instance_idle(struct instance *instance, time_t now);

void instance_schedule(struct instance *instance, time_t secs);

#endif