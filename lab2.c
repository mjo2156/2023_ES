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
  int buffer_out[192];
  int buffer[128];
  int column = 0;
  int row = 19;
  count = 0;
  int cursor[4];
  sprintf(cursor, "|");
  for (;;) {
    libusb_interrupt_transfer(keyboard, endpoint_address,(unsigned char *) &packet, sizeof(packet),&transferred, 0);
    if (transferred == sizeof(packet)) 
    {
      if(column == 64){
        column = 0;
        row++;
      }
      //sprintf(keystate, "%02x %02x %02x", packet.modifiers, packet.keycode[0], packet.keycode[1]);
      //printf("%s\n", keystate);
      //fbputs(keystate, 6, 0);

      //fbputs(buffer,sizeof(buffer),0)

      if (packet.keycode[0] == 0x29) { /* ESC pressed? */
	        break;
      }

      if (packet.keycode[0] == 0x28){     // enter = send over ascii
          // send buffer over network
          // network stuff here
      }
      if(packet.keycode[0] == 0x4f){      // Keyboard Right Arrow
          // move pointer in buffer to right unless at furthest point
          if(count <= 190){
            count++;
          }
          column++;
      }
      if(packet.keycode[0] == 0x50){      // Keyboard Left Arrow
          // move pointer in buffer to left unless at furthest point
          if(count == 0){
            if(row != 0){
              row--;
              column = 64;
              count--;
            }
          } else {
            column--;
            count--;
          }
      }
      if(packet.keycode[0] == 0x2a){      // Back spcae
        // delete from buffer
        buffer_out[count] = 32;
        count--;
      }
      if(packet.keycode[0] == 0x2c){      // space
        buffer_out[count] = 32;
        sprintf(buffer, " ");
        fbputs(buffer,row,column);
        column++;
        count++;
      }

      if ((packet.modifiers == 0x02) || (packet.modifers == 0x20 || (packet.modifers == 0x22))) // check for other shift cases too modifers
      {
        //different switch case for shifted values
        switch(packet.keycode[0]){
          case 0x04: // Keyboard a 
            buffer_out[count] = 65;
            sprintf(buffer,"A");
            break;
          case 0x05: // Keyboard b
            buffer_out[count] = 66; 
            sprintf(buffer,"B");
            break;
          case 0x06: // Keyboard c
            buffer_out[count] = 67;
            sprintf(buffer,"C");
            break;
          case 0x07: // Keyboard d
            buffer_out[count] = 68;
            sprintf(buffer,"D");
            break;
          case 0x08: // Keyboard e
            buffer_out[count] = 69;
            sprintf(buffer,"E");
            break;
          case 0x09: // Keyboard f
            buffer_out[count] = 70;
            sprintf(buffer,"F");
            break;
          case 0x0a: // Keyboard g
            buffer_out[count] = 71;
            sprintf(buffer,"G");
            break;
          case 0x0b: // Keyboard h
            buffer_out[count] = 72;
            sprintf(buffer,"H");
            break;
          case 0x0c: // Keyboard i
            buffer_out[count] = 73;
            sprintf(buffer,"I");
            break;
          case 0x0d: // Keyboard j
            buffer_out[count] = 74;
            sprintf(buffer,"J");
            break;
          case 0x0e: // Keyboard k
            buffer_out[count] = 75;
            sprintf(buffer,"K"); 
            break;
          case 0x0f: // Keyboard l
            buffer_out[count] = 76;
            sprintf(buffer,"L");
            break;
          case 0x10: // Keyboard m
            buffer_out[count] = 77;
            sprintf(buffer,"M");
            break;
          case 0x11: // Keyboard n
            buffer_out[count] = 78;
            sprintf(buffer,"N");
            break;
          case 0x12: // Keyboard o
            buffer_out[count] = 79;
            sprintf(buffer,"O");
            break;
          case 0x13: // Keyboard p
            buffer_out[count] = 80;
            sprintf(buffer,"P");
            break;
          case 0x14: // Keyboard q
            buffer_out[count] = 81; 
            sprintf(buffer,"Q");
            break;
          case 0x15: // Keyboard r
            buffer_out[count] = 82;
            sprintf(buffer,"R");
            break;
          case 0x16: // Keyboard s
            buffer_out[count] = 83;
            sprintf(buffer,"S");
            break;
          case 0x17: // Keyboard t
            buffer_out[count] = 84; 
            sprintf(buffer,"T");
            break;
          case 0x18: // Keyboard u
            buffer_out[count] = 85;
            sprintf(buffer,"U"); 
            break;
          case 0x19: // Keyboard v
            buffer_out[count] = 86; 
            sprintf(buffer,"V");
            break;
          case 0x1a: // Keyboard w
            buffer_out[count] = 87; 
            sprintf(buffer,"W");
            break;
          case 0x1b: // Keyboard x
            buffer_out[count] = 88;
            sprintf(buffer,"X");
            break;
          case 0x1c: // Keyboard y
            buffer_out[count] = 89;
            sprintf(buffer,"Y");
            break;
          case 0x1d: // Keyboard z
            buffer_out[count] = 90;
            sprintf(buffer,"Z");
            break;

          case 0x1e: // Keyboard !
            buffer_out[count] = 33;
            sprintf(buffer,"!");
            break;
          case 0x1f: // Keyboard @
            buffer_out[count] = 64;
            sprintf(buffer,"@");
            break;
          case 0x20: // Keyboard #
            buffer_out[count] = 35;
            sprintf(buffer,"B");
            break;
          case 0x21: // Keyboard $
            buffer_out[count] = 36;
            sprintf(buffer,"$");
            break;
          case 0x22: // Keyboard %
            buffer_out[count] = 37;
            sprintf(buffer,"%%");
            break;                      
          case 0x23: // Keyboard ^
            buffer_out[count] = 94;
            sprintf(buffer,"^");
            break;
          case 0x24: // Keyboard &
            buffer_out[count] = 38;
            sprintf(buffer,"&");
            break;
          case 0x25: // Keyboard *
            buffer_out[count] = 42;
            sprintf(buffer,"*");
            break;
          case 0x26: // Keyboard (
            buffer_out[count] = 40;
            sprintf(buffer,"(");
            break;
          case 0x27: // Keyboard )
            buffer_out[count] = 41;
            sprintf(buffer,")");
            break;
          case 0x00:
            somthing = 0;
            break;
        }
        if(somthing == 1){
          fbputs(buffer_out,row,column);
          column++;
        }
      } 
      else
      {
        switch(packet.keycode[0]){
          case 0x04: // Keyboard a 
            buffer_out[count] = 97;
            sprintf(buffer,"a");
            break;
          case 0x05: // Keyboard b 
            buffer_out[count] = 98;
            sprintf(buffer,"b");
            break;
          case 0x06: // Keyboard c
            buffer_out[count] = 99;
            sprintf(buffer,"c");
            break;
          case 0x07: // Keyboard d
            buffer_out[count] = 100;
            sprintf(buffer,"d");
            break;
          case 0x08: // Keyboard e
            buffer_out[count] = 101;
            sprintf(buffer,"e");
            break;
          case 0x09: // Keyboard f
            buffer_out[count] = 102;
            sprintf(buffer,"f");
            break;
          case 0x0a: // Keyboard g
            buffer_out[count] = 103;
            sprintf(buffer,"g");
            break;
          case 0x0b: // Keyboard h
            buffer_out[count] = 104;
            sprintf(buffer,"h");
            break;
          case 0x0c: // Keyboard i
            buffer_out[count] = 105;
            sprintf(buffer,"i");
            break;
          case 0x0d: // Keyboard j
            buffer_out[count] = 106;
            sprintf(buffer,"j");
            break;
          case 0x0e: // Keyboard k
            buffer_out[count] = 107;
            sprintf(buffer,"k"); 
            break;
          case 0x0f: // Keyboard l
            buffer_out[count] = 108;
            sprintf(buffer,"l");
            break;
          case 0x10: // Keyboard m
            buffer_out[count] = 109;
            sprintf(buffer,"m");
            break;
          case 0x11: // Keyboard n
            buffer_out[count] = 110;
            sprintf(buffer,"n");
            break;
          case 0x12: // Keyboard o
            buffer_out[count] = 111;
            sprintf(buffer,"o");
            break;
          case 0x13: // Keyboard p
            buffer_out[count] = 112;
            sprintf(buffer,"p");
            break;
          case 0x14: // Keyboard q 
            buffer_out[count] = 113;
            sprintf(buffer,"q");
            break;
          case 0x15: // Keyboard r
            buffer_out[count] = 114;
            sprintf(buffer,"r");
            break;
          case 0x16: // Keyboard s
            buffer_out[count] = 115;
            sprintf(buffer,"s"); 
            break;
          case 0x17: // Keyboard t 
            buffer_out[count] = 116;
            sprintf(buffer,"t");
            break;
          case 0x18: // Keyboard u
            buffer_out[count] = 117;
            ssprintf(buffer,"u"); 
            break;
          case 0x19: // Keyboard v 
            buffer_out[count] = 118;
            sprintf(buffer,"v");
            break;
          case 0x1a: // Keyboard w 
            buffer_out[count] = 119;
            sprintf(buffer,"w");
            break;
          case 0x1b: // Keyboard x
            buffer_out[count] = 120;
            sprintf(buffer,"x");
            break;
          case 0x1c: // Keyboard y
            buffer_out[count] = 121;
            sprintf(buffer,"y");
            break;
          case 0x1d: // Keyboard z
            buffer_out[count] = 122;
            sprintf(buffer,"z");
            break;
          case 0x1e: // Keyboard 1
            buffer_out[count] = 49;
            sprintf(buffer,"1");
            break;
          case 0x1f: // Keyboard 2
            buffer_out[count] = 50;
            sprintf(buffer,"2");
            break;
          case 0x20: // Keyboard 3
            buffer_out[count] = 51;
            sprintf(buffer,"3");
            break;
          case 0x21: // Keyboard 4
            buffer_out[count] = 52;
            sprintf(buffer,"4");
            break;
          case 0x22: // Keyboard 5
            buffer_out[count] = 53;
            sprintf(buffer,"5");
            break;
          case 0x23: // Keyboard 6
            buffer_out[count] = 54;
            sprintf(buffer,"6");
            break;
          case 0x24: // Keyboard 7
            buffer_out[count] = 55;
            sprintf(buffer,"7");
            break;
          case 0x25: // Keyboard 8
            buffer_out[count] = 56;
            sprintf(buffer,"8");
            break;
          case 0x26: // Keyboard 9
            buffer_out[count] = 57;
            sprintf(buffer,"9");
            break;
          case 0x27: // Keyboard 0
            buffer_out[count] = 48;
            sprintf(buffer,"0");
            break;
          case 0x00:
            somthing = 0;
            break;
        }
        if(somthing == 1){
          fbputs(buffer,row,column);
          if(column == 64){
            if(row != 3){
              fbputs(cursor,row+1,0);
            }   
          } else {
          fbputs(cursor,row,column + 1);
          }
          column++;
          count++;
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

