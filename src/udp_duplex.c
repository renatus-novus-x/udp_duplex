/* udp_duplex.c - simple UDP ping-pong sample for
 * Windows / macOS / Linux / X68000 (elf2x68k + libsocket)
 *
 * Usage:
 *   udp_duplex <local_port> <peer_ip> <peer_port>
 *
 * Example:
 *   On host A (192.168.0.10):
 *     udp_duplex 50000 192.168.0.11 50001
 *   On host B (192.168.0.11):
 *     udp_duplex 50001 192.168.0.10 50000
 *
 * Every peer sends "hello <counter>" once per second and
 * prints all received packets.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(_WIN32)
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET sock_t;
#else
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
typedef int sock_t;
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET (-1)
#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR (-1)
#endif

static int set_nonblock(sock_t s)
{
#if defined(_WIN32)
  u_long mode = 1;
  if (ioctlsocket(s, FIONBIO, &mode) != 0) {
    return -1;
  }
  return 0;
#else
  int flags;

#if defined(O_NONBLOCK)
  flags = fcntl(s, F_GETFL, 0);
  if (flags < 0) {
    return -1;
  }
  if (fcntl(s, F_SETFL, flags | O_NONBLOCK) < 0) {
    return -1;
  }
  return 0;
#else
  /* If fcntl/O_NONBLOCK is not available, we just keep it blocking. */
  (void)s;
  return 0;
#endif

#endif
}

static void tiny_sleep(void)
{
#if defined(_WIN32)
  Sleep(10);          /* 10 ms */
#else
  /* Linux / macOS / X68000 (elf2x68k) should have usleep() */
  usleep(10 * 1000);  /* 10 ms */
#endif
}

int main(int argc, char **argv)
{
  if (argc != 4) {
    fprintf(stderr,
            "Usage: %s <local_port> <peer_ip> <peer_port>\n",
            argv[0]);
    return 1;
  }

  int local_port = atoi(argv[1]);
  const char *peer_ip = argv[2];
  int peer_port = atoi(argv[3]);

#if defined(_WIN32)
  WSADATA wsa;
  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
    fprintf(stderr, "WSAStartup failed\n");
    return 1;
  }
#endif

  sock_t sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock == INVALID_SOCKET) {
#if defined(_WIN32)
    fprintf(stderr, "socket() failed: %d\n", WSAGetLastError());
#else
    perror("socket");
#endif
    return 1;
  }

  struct sockaddr_in local_addr;
  memset(&local_addr, 0, sizeof(local_addr));
  local_addr.sin_family = AF_INET;
  local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  local_addr.sin_port = htons((unsigned short)local_port);

  if (bind(sock,
           (struct sockaddr *)&local_addr,
           (socklen_t)sizeof(local_addr)) == SOCKET_ERROR) {
#if defined(_WIN32)
    fprintf(stderr, "bind() failed: %d\n", WSAGetLastError());
#else
    perror("bind");
#endif
#if defined(_WIN32)
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif
    return 1;
  }

  struct sockaddr_in peer_addr;
  memset(&peer_addr, 0, sizeof(peer_addr));
  peer_addr.sin_family = AF_INET;
  peer_addr.sin_port = htons((unsigned short)peer_port);

  unsigned long addr = inet_addr(peer_ip);
  if (addr == INADDR_NONE) {
    fprintf(stderr, "Invalid peer IP address: %s\n", peer_ip);
#if defined(_WIN32)
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif
    return 1;
  }
  peer_addr.sin_addr.s_addr = addr;

  if (set_nonblock(sock) != 0) {
    fprintf(stderr,
            "Warning: failed to set non-blocking mode, using blocking socket.\n");
  }

  printf("Local  : port %d\n", local_port);
  printf("Peer   : %s:%d\n", peer_ip, peer_port);
  printf("Sending one packet per second. Press Ctrl+C to stop.\n");

  char buf[512];
  unsigned long counter = 0;
  time_t last_send = 0;

  for (;;) {
    /* Receive all pending packets */
    for (;;) {
      struct sockaddr_in from_addr;
      socklen_t from_len = (socklen_t)sizeof(from_addr);
      int n = (int)recvfrom(sock,
                            buf,
                            sizeof(buf) - 1,
                            0,
                            (struct sockaddr *)&from_addr,
                            &from_len);
      if (n < 0) {
#if defined(_WIN32)
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK) {
          break;  /* no more data */
        }
        fprintf(stderr, "recvfrom() error: %d\n", err);
        break;
#else
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
          break;  /* no more data */
        }
        perror("recvfrom");
        break;
#endif
      } else if (n == 0) {
        /* No data (not very meaningful for UDP) */
        break;
      } else {
        buf[n] = '\0';
        printf("RECV from %s:%d: %s\n",
               inet_ntoa(from_addr.sin_addr),
               (int)ntohs(from_addr.sin_port),
               buf);
      }
    }

    /* Send once per second */
    time_t now = time(NULL);
    if (now != last_send) {
      last_send = now;
      int len = snprintf(buf, sizeof(buf), "hello %lu", counter++);
      if (len < 0) {
        len = 0;
      }
      int sent = (int)sendto(sock,
                             buf,
                             (size_t)len,
                             0,
                             (struct sockaddr *)&peer_addr,
                             (socklen_t)sizeof(peer_addr));
      if (sent < 0) {
#if defined(_WIN32)
        int err = WSAGetLastError();
        fprintf(stderr, "sendto() error: %d\n", err);
#else
        perror("sendto");
#endif
      } else {
        printf("SEND to %s:%d: %s\n",
               inet_ntoa(peer_addr.sin_addr),
               (int)ntohs(peer_addr.sin_port),
               buf);
      }
    }

    tiny_sleep();
  }

  /* Not reached in this sample */
#if defined(_WIN32)
  closesocket(sock);
  WSACleanup();
#else
  close(sock);
#endif

  return 0;
}
