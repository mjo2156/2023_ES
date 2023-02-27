/*
 *
 * CSEE 4840 Lab 2 for 2019
 *
 * Name/UNI: Please Changeto Yourname (pcy2301)
 */
#include "fbputchar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "usbkeyboard.h"
#include <pthread.h>

/* Update SERVER_HOST to be the IP address of
 * the chat server you are connecting to
 */
/* arthur.cs.columbia.edu */
#define SERVER_HOST "128.59.19.114"
#define SERVER_PORT 42000

#define BUFFER_SIZE 128

/*
 * References:
 *
 * http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
 * http://www.thegeekstuff.com/2011/12/c-socket-programming/
 * 
 */

int sockfd; /* Socket file descriptor */

struct libusb_device_handle *keyboard;
uint8_t endpoint_address;

pthread_t network_thread;
void *network_thread_f(void *);

int main()
{
  int err, col;

  struct sockaddr_in serv_addr;

  struct usb_keyboard_packet packet;
  int transferred;
  char keystate[12];

  if ((err = fbopen()) != 0) {
    fprintf(stderr, "Error: Could not open framebuffer: %d\n", err);
    exit(1);
  }

  /* Draw rows of asterisks across the top and bottom of the screen */
  for (col = 0 ; col < 64 ; col++) {
    fbputchar('*', 0, col);
    fbputchar('*', 23, col);
  }

  fbputs("Hello CSEE 4840 World!", 4, 10);

  /* Open the keyboard */
  if ( (keyboard = openkeyboard(&endpoint_address)) == NULL ) {
    fprintf(stderr, "Did not find a keyboard\n");
    exit(1);
  }
    
  /* Create a TCP communications socket */
  if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    fprintf(stderr, "Error: Could not create socket\n");
    exit(1);
  }

  /* Get the server address */
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(SERVER_PORT);
  if ( inet_pton(AF_INET, SERVER_HOST, &serv_addr.sin_addr) <= 0) {
    fprintf(stderr, "Error: Could not convert host IP \"%s\"\n", SERVER_HOST);
    exit(1);
  }

  /* Connect the socket to the server */
  if ( connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    fprintf(stderr, "Error: connect() failed.  Is the server running?\n");
    exit(1);
  }
 
  /* Start the network thread */
  pthread_create(&network_thread, NULL, network_thread_f, NULL);

  /* Look for and handle keypresses */
  for (;;) {
    libusb_interrupt_transfer(keyboard, endpoint_address,(unsigned char *) &packet, sizeof(packet),&transferred, 0);
    if (transferred == sizeof(packet)) 
    {
      sprintf(keystate, "%02x %02x %02x", packet.modifiers, packet.keycode[0], packet.keycode[1]);
      printf("%s\n", keystate);
      fbputs(keystate, 6, 0);

      if (packet.keycode[0] == 0x29) { /* ESC pressed? */
	        break;
      }

      if (packet.keycode[0] == 0x28){     // enter = send over ascii
          // send buffer over network
      }
      if(packet.keycode[0] == 0x4f){      // Keyboard Right Arrow
          // move pointer in buffer to right unless at furthest point
      }
      if(packet.keycode[0] == 0x50){      // Keyboard Left Arrow
          // move pointer in buffer to left unless at furthest point
      }
      if(packet.keycode[0] == 0x2a){      // Back spcae
        // delete from buffer
      }
      if(packet.keycode[0] == 0x2c){
        sprintf(" ");
      }

      if ((packet.modifiers == 0x02) || (packet.modifers == 0x20 || (packet.modifers == 0x22))) // check for other shift cases too modifers
      {
        //different switch case for shifted values
        switch(packet.keycode[0]){
          case 0x04: // Keyboard a 
            sprintf("A");
          case 0x05: // Keyboard b 
            sprintf("B");
          case 0x06: // Keyboard c
            sprintf("C");
          case 0x07: // Keyboard d
            sprintf("D");
          case 0x08: // Keyboard e
            sprintf("E");
          case 0x09: // Keyboard f
            sprintf("F");
          case 0x0a: // Keyboard g
            sprintf("G");
          case 0x0b: // Keyboard h
            sprintf("H");
          case 0x0c: // Keyboard i
            sprintf("I");
          case 0x0d: // Keyboard j
            sprintf("J");
          case 0x0e: // Keyboard k
            sprintf("K"); 
          case 0x0f: // Keyboard l
            sprintf("L");
          case 0x10: // Keyboard m
            sprintf("M");
          case 0x11: // Keyboard n
            sprintf("N");
          case 0x12: // Keyboard o
            sprintf("O");
          case 0x13: // Keyboard p
            sprintf("P");
          case 0x14: // Keyboard q 
            sprintf("Q");
          case 0x15: // Keyboard r
            sprintf("R");
          case 0x16: // Keyboard s
            sprintf("S"); 
          case 0x17: // Keyboard t 
            sprintf("T");
          case 0x18: // Keyboard u
            sprintf("U"); 
          case 0x19: // Keyboard v 
            sprintf("V");
          case 0x1a: // Keyboard w 
            sprintf("W");
          case 0x1b: // Keyboard x
            sprintf("X");
          case 0x1c: // Keyboard y
            sprintf("Y");
          case 0x1d: // Keyboard z
            sprintf("Z");

          case 0x1e: // Keyboard !
            sprintf("!");
          case 0x1f: // Keyboard @
            sprintf("@");
          case 0x20: // Keyboard #
            sprintf("#");
          case 0x21: // Keyboard $
            sprintf("$");
          case 0x22: // Keyboard %
            sprintf("%");                       // ???
          case 0x23: // Keyboard ^
            sprintf("^");
          case 0x24: // Keyboard &
            sprintf("&");
          case 0x25: // Keyboard *
            sprintf("*");
          case 0x26: // Keyboard (
            sprintf("(");
          case 0x27: // Keyboard )
            sprintf(")");

        }
      } 

      else
      {
        switch(packet.keycode[0]){
          case 0x04: // Keyboard a 
            sprintf("a");
          case 0x05: // Keyboard b 
            sprintf("b");
          case 0x06: // Keyboard c
            sprintf("c");
          case 0x07: // Keyboard d
            sprintf("d");
          case 0x08: // Keyboard e
            sprintf("e");
          case 0x09: // Keyboard f
            sprintf("f");
          case 0x0a: // Keyboard g
            sprintf("g");
          case 0x0b: // Keyboard h
            sprintf("h");
          case 0x0c: // Keyboard i
            sprintf("i");
          case 0x0d: // Keyboard j
            sprintf("j");
          case 0x0e: // Keyboard k
            sprintf("k"); 
          case 0x0f: // Keyboard l
            sprintf("l");
          case 0x10: // Keyboard m
            sprintf("m");
          case 0x11: // Keyboard n
            sprintf("n");
          case 0x12: // Keyboard o
            sprintf("o");
          case 0x13: // Keyboard p
            sprintf("p");
          case 0x14: // Keyboard q 
            sprintf("q");
          case 0x15: // Keyboard r
            sprintf("r");
          case 0x16: // Keyboard s
            sprintf("s"); 
          case 0x17: // Keyboard t 
            sprintf("t");
          case 0x18: // Keyboard u
            sprintf("u"); 
          case 0x19: // Keyboard v 
            sprintf("v");
          case 0x1a: // Keyboard w 
            sprintf("w");
          case 0x1b: // Keyboard x
            sprintf("x");
          case 0x1c: // Keyboard y
            sprintf("y");
          case 0x1d: // Keyboard z
            sprintf("z");

          case 0x1e: // Keyboard 1
            sprintf("1");
          case 0x1f: // Keyboard 2
            sprintf("2");
          case 0x20: // Keyboard 3
            sprintf("3");
          case 0x21: // Keyboard 4
            sprintf("4");
          case 0x22: // Keyboard 5
            sprintf("5");
          case 0x23: // Keyboard 6
            sprintf("6");
          case 0x24: // Keyboard 7
            sprintf("7");
          case 0x25: // Keyboard 8
            sprintf("8");
          case 0x26: // Keyboard 9
            sprintf("9");
          case 0x27: // Keyboard 0
            sprintf("0");
          default:
        }
      }

      

    }
  }

  /* Terminate the network thread */
  pthread_cancel(network_thread);

  /* Wait for the network thread to finish */
  pthread_join(network_thread, NULL);

  return 0;
}

void *network_thread_f(void *ignored)
{
  char recvBuf[BUFFER_SIZE];
  int n;
  /* Receive data */
  while ( (n = read(sockfd, &recvBuf, BUFFER_SIZE - 1)) > 0 ) {
    recvBuf[n] = '\0';
    printf("%s", recvBuf);
    fbputs(recvBuf, 8, 0);
  }

  return NULL;
}

