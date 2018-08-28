/*
 * Copyright 2018 ARP Network
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

void print_usage_and_exit(char *progname);

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        print_usage_and_exit(argv[0]);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;

    if (argc == 3) {
        if (inet_pton(AF_INET, argv[1], &addr.sin_addr) != 1) {
            print_usage_and_exit(argv[0]);
        }
    }
    int port = atoi(argv[argc - 1]);
    if (port <= 0 || port > 65535) {
        print_usage_and_exit(argv[0]);
    }
    addr.sin_port = htons(port);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0); assert(sockfd >= 0);
    fcntl(sockfd, F_SETFL, O_NONBLOCK);
    connect(sockfd, (struct sockaddr *) &addr, sizeof (addr));

    fd_set fdset;
    struct timeval tv;
    FD_ZERO(&fdset);
    FD_SET(sockfd, &fdset);
    tv.tv_sec = 1; /* 1 second timeout */
    tv.tv_usec = 0;

    int rc = -1;
    if (select(sockfd + 1, NULL, &fdset, NULL, &tv) == 1) {
        socklen_t len = sizeof rc;
        getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &rc, &len);
    }

    close(sockfd);

    if (rc == 0) {
        printf("ok\n");
    } else {
        printf("failed\n");
    }

    return rc;
}

void print_usage_and_exit(char *progname) {
    fprintf(stderr, "Usage: %s [IP] PORT\n", progname);
    exit(-1);
}
