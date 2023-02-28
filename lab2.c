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
  int buffer[128];
  int count = 0;
  for (;;) {
    libusb_interrupt_transfer(keyboard, endpoint_address,(unsigned char *) &packet, sizeof(packet),&transferred, 0);
    if (transferred == sizeof(packet)) 
    {
      sprintf(keystate, "%02x %02x %02x", packet.modifiers, packet.keycode[0], packet.keycode[1]);
      printf("%s\n", keystate);
      fbputs(keystate, 6, 0);

      fbputs(buffer)

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
            buffer[count] = 65;
            sprintf("A");
          case 0x05: // Keyboard b 
            buffer[count] = 66;
            sprintf("B");
          case 0x06: // Keyboard c
            buffer[count] = 67;
            sprintf("C");
          case 0x07: // Keyboard d
            buffer[count] = 68;
            sprintf("D");
          case 0x08: // Keyboard e
            buffer[count] = 69;
            sprintf("E");
          case 0x09: // Keyboard f
            buffer[count] = 70;
            sprintf("F");
          case 0x0a: // Keyboard g
            buffer[count] = 71;
            sprintf("G");
          case 0x0b: // Keyboard h
            buffer[count] = 72;
            sprintf("H");
          case 0x0c: // Keyboard i
            buffer[count] = 73;
            sprintf("I");
          case 0x0d: // Keyboard j
            buffer[count] = 74;
            sprintf("J");
          case 0x0e: // Keyboard k
            buffer[count] = 75;
            sprintf("K"); 
          case 0x0f: // Keyboard l
            buffer[count] = 76;
            sprintf("L");
          case 0x10: // Keyboard m
            buffer[count] = 77;
            sprintf("M");
          case 0x11: // Keyboard n
            buffer[count] = 78;
            sprintf("N");
          case 0x12: // Keyboard o
            buffer[count] = 79;
            sprintf("O");
          case 0x13: // Keyboard p
            buffer[count] = 80;
            sprintf("P");
          case 0x14: // Keyboard q 
            buffer[count] = 81;
            sprintf("Q");
          case 0x15: // Keyboard r
            buffer[count] = 82;
            sprintf("R");
          case 0x16: // Keyboard s
            buffer[count] = 83;
            sprintf("S"); 
          case 0x17: // Keyboard t 
            buffer[count] = 84;
            sprintf("T");
          case 0x18: // Keyboard u
            buffer[count] = 85;
            sprintf("U"); 
          case 0x19: // Keyboard v 
            buffer[count] = 86;
            sprintf("V");
          case 0x1a: // Keyboard w 
            buffer[count] = 87;
            sprintf("W");
          case 0x1b: // Keyboard x
            buffer[count] = 88;
            sprintf("X");
          case 0x1c: // Keyboard y
            buffer[count] = 89;
            sprintf("Y");
          case 0x1d: // Keyboard z
            buffer[count] = 90;
            sprintf("Z");

          case 0x1e: // Keyboard !
            buffer[count] = 33;
            sprintf("!");
          case 0x1f: // Keyboard @
            buffer[count] = 64;
            sprintf("@");
          case 0x20: // Keyboard #
            buffer[count] = 35;
            sprintf("#");
          case 0x21: // Keyboard $
            buffer[count] = 36;
            sprintf("$");
          /*case 0x22: // Keyboard %
            buffer[count] = 37;
            sprintf("%");*/                      // ???
          case 0x23: // Keyboard ^
            buffer[count] = 94;
            sprintf("^");
          case 0x24: // Keyboard &
            buffer[count] = 38;
            sprintf("&");
          case 0x25: // Keyboard *
            buffer[count] = 42;
            sprintf("*");
          case 0x26: // Keyboard (
            buffer[count] = 40;
            sprintf("(");
          case 0x27: // Keyboard )
            buffer[count] = 41;
            sprintf(")");
          default:
        }
      } 

      else
      {
        switch(packet.keycode[0]){
          case 0x04: // Keyboard a 
            buffer[count] = 97;
            sprintf("a");
          case 0x05: // Keyboard b 
            buffer[count] = 98;
            sprintf("b");
          case 0x06: // Keyboard c
            buffer[count] = 99;
            sprintf("c");
          case 0x07: // Keyboard d
            buffer[count] = 100;
            sprintf("d");
          case 0x08: // Keyboard e
            buffer[count] = 101;
            sprintf("e");
          case 0x09: // Keyboard f
            buffer[count] = 102;
            sprintf("f");
          case 0x0a: // Keyboard g
            buffer[count] = 103;
            sprintf("g");
          case 0x0b: // Keyboard h
            buffer[count] = 104;
            sprintf("h");
          case 0x0c: // Keyboard i
            buffer[count] = 105;
            sprintf("i");
          case 0x0d: // Keyboard j
            buffer[count] = 106;
            sprintf("j");
          case 0x0e: // Keyboard k
            buffer[count] = 107;
            sprintf("k"); 
          case 0x0f: // Keyboard l
            buffer[count] = 108;
            sprintf("l");
          case 0x10: // Keyboard m
            buffer[count] = 109;
            sprintf("m");
          case 0x11: // Keyboard n
            buffer[count] = 110;
            sprintf("n");
          case 0x12: // Keyboard o
            buffer[count] = 111;
            sprintf("o");
          case 0x13: // Keyboard p
            buffer[count] = 112;
            sprintf("p");
          case 0x14: // Keyboard q 
            buffer[count] = 113;
            sprintf("q");
          case 0x15: // Keyboard r
            buffer[count] = 114;
            sprintf("r");
          case 0x16: // Keyboard s
            buffer[count] = 115;
            sprintf("s"); 
          case 0x17: // Keyboard t 
            buffer[count] = 116;
            sprintf("t");
          case 0x18: // Keyboard u
            buffer[count] = 117;
            sprintf("u"); 
          case 0x19: // Keyboard v 
            buffer[count] = 118;
            sprintf("v");
          case 0x1a: // Keyboard w 
            buffer[count] = 119;
            sprintf("w");
          case 0x1b: // Keyboard x
            buffer[count] = 120;
            sprintf("x");
          case 0x1c: // Keyboard y
            buffer[count] = 121;
            sprintf("y");
          case 0x1d: // Keyboard z
            buffer[count] = 122;
            sprintf("z");

          case 0x1e: // Keyboard 1
            buffer[count] = 49;
            sprintf("1");
          case 0x1f: // Keyboard 2
            buffer[count] = 50;
            sprintf("2");
          case 0x20: // Keyboard 3
            buffer[count] = 51;
            sprintf("3");
          case 0x21: // Keyboard 4
            buffer[count] = 52;
            sprintf("4");
          case 0x22: // Keyboard 5
            buffer[count] = 53;
            sprintf("5");
          case 0x23: // Keyboard 6
            buffer[count] = 54;
            sprintf("6");
          case 0x24: // Keyboard 7
            buffer[count] = 55;
            sprintf("7");
          case 0x25: // Keyboard 8
            buffer[count] = 56;
            sprintf("8");
          case 0x26: // Keyboard 9
            buffer[count] = 57;
            sprintf("9");
          case 0x27: // Keyboard 0
            buffer[count] = 48;
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

