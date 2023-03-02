/*
 *
 * CSEE 4840 Lab 2 for 2019
 *
 * Name/UNI: Michael Ozymy (mjo2156)
 *  	     Joseph Han	(jh4632)
 *	     Lennart Shulze (ls3932)
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
#define NUM_KEYS 3
#define BUFFER_SIZE 128
#define SCREEN_ROWS 17
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
  //char keystate[NUM_KEYS][12];
  char keystate[12];

  if ((err = fbopen()) != 0) {
    fprintf(stderr, "Error: Could not open framebuffer: %d\n", err);
    exit(1);
  }



  /* Draw rows of asterisks across the top and bottom of the screen */
  for (col = 0 ; col < 64 ; col++) {
    fbputchar('*', 0, col);
    fbputchar('-', 18, col);
    fbputchar('*', 23, col);
  }

  fbputs("Hello CSEE 4840 World!", 4, 10);

  /* Clear the screen when the program starts */
  for (col = 0 ; col < 64; col++) {
    fbputchar('*', 0, col);
    fbputchar('-', 18, col);
    fbputchar('*', 23, col);
    for (int row = 0; row < 24; row++) {
      fbputchar(' ', row, col);
    }
  }

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
  char buffer_out[256];
  char old_buffer_out[256];
 // char buffer_out_char[256];
  char out_char;
  out_char  = 'a';
  char buffer[128];
  int column = 0;
  int row = 19;
  int count = 0;
  char cursor = '_';
  int maxcount = 0;
  int maxrow = 0;
  int maxcol = 0;
  int max = 0;
  int prev = 0;
  int input_code = 0;
  for (;;) {
    if ( ((row == 20) && (column >=62)) || (row>20) ){
	row = 20;
	maxrow = 20;
	column = 62;
	maxcol = 62;
	maxcount = 125;	
    	count = 125;
    } 
   if ( ((maxrow == 20) && (maxcol>=63)) || (maxrow > 20) ){
        maxrow = 20;
        maxcol = 62;
        maxcount = 125;
    }

    if (row == 18){
	row = 19;
	column = 0;
	count = 0;
    }
    //if(maxcount != count){
    //    fbputchar(' ',maxrow,maxcol+1);
    //}

    //fbputchar(maxcount,23)
	//printf("%d:", count);
	//printf("%d:", maxcount);
	//printf("%d:", row);
	//printf("%d:", column);
	//printf("%d:", maxrow);
	//printf("%d:::", maxcol);
    if (maxcount >= 126){
	max = 1;
    } else {
	max = 0;
    }
    
    for (int i =0;i<maxcount;i++){
	old_buffer_out[i] = buffer_out[i];
    }
    if(row>maxrow){
	maxrow=row;
    }
    if (count == maxcount){
	maxrow = row;
	maxcol = column;
    }
    int something = 1;
    libusb_interrupt_transfer(keyboard, endpoint_address,(unsigned char *) &packet, sizeof(packet),&transferred, 0);
    if (transferred == sizeof(packet)) 
    {
      if(column == 63 && packet.keycode[0] != 0x2a){
        column = 0;
        row++;
      }


      if (packet.keycode[0] == 0x29) { /* ESC pressed? */
	        break;
      }

      if (packet.keycode[0] == 0x28){     // enter = send over ascii
        // send buffer over network
        // network stuff here
	// sprintf(keystate, "enter begin");
	//buffer_out_char = buffer_out;
	char send_buffer[256];
	for(int t = 0; t<maxcount+1;t++){
		send_buffer[t] = buffer_out[t];
	}  
	if (write(sockfd, &send_buffer, maxcount+1) < 0){
		printf("error");	
	} else {
	  printf("test: %c", out_char);
	}
	for(int i = 19;i<23;i++){
		for(int j=0;j<64;j++){
			fbputchar(' ',i,j);
		}
	}
	count=0;
	maxcount=0;
	maxrow=19;
	maxcol=0;
	row=19;
	column=0;
      }
      if(packet.keycode[0] == 0x4f){      // Keyboard Right Arrow
	  if (count!=maxcount){
	  	fbputchar(buffer_out[count],row,column);
          	if(count <= 190){
            		count++;
          	}
          	column++;
      	  }
      }
      if((packet.keycode[0] == 0x50)&&(count != 0)){      // Keyboard Left Arrow
        if(count==maxcount){
		fbputchar(' ',row,column);
	}		
	else {
		fbputchar(buffer_out[count],row,column);  
	}
	if(column == 0){
            	if(row != 0){
              		row--;
            		column = 62;
              		count=count-2;
            	}
        } else {
            	column--;
            	count--;
         }
      }
      if((packet.keycode[0] == 0x2a) && (count!=0)){      // Back spcae
        if(count == maxcount){
		buffer_out[count] = 32; // insert space at end of buffer where last char was
		fbputchar(' ',row,column);
		column--;
		maxcount--;
		count--;
		maxcol--;
	} else {
		fbputchar(' ',maxrow,maxcol);
		maxcol--;
		int temprow = row;
		if (column > 0){
			int tempcol = column-1;	
			for(int i = count;i<(maxcount);i++){
				buffer_out[i-1] = old_buffer_out[i];
				fbputchar(old_buffer_out[i], temprow, tempcol++);
			}
			column--;
			maxcount--;
			count--;
			if (maxcol < 0){
				maxrow--;
				maxcol = 62;	
			}
		}
	} 
	if (column <= 0) { // wrap around
		fbputchar(' ', row, 0);
		row--;
		column = 62;
	}
      }

        if((prev == packet.keycode[0]) && (packet.keycode[1] != 0x00)){  // holding and pressing  multiple keys at the same time
                input_code = packet.keycode[1];
        } else if (prev == 0x00){
                input_code = packet.keycode[0];
        } else {
                input_code = packet.keycode[1];
        }
        prev = packet.keycode[0];

      if((input_code == 0x2c)&&(max==0)){      // space
       	if(count == maxcount){
		buffer_out[count] = 32;
        	sprintf(buffer, " ");
        	fbputs(buffer,row,column);
        	column++;
        	count++;
		maxcount++;
	} else {
		fbputchar(' ',row,column);
	        buffer_out[count] = 32;
                int temprow = row;
                int tempcol = column;
                for (int i=count;i<maxcount;i++){
                        if((i==63)||(i==127)||(i==191)){
                                tempcol = 0;
                                temprow++;
                        }
                        if((column == 0) && (row != 19)) {
                                for(int k=0;k<64;k++){
                                        fbputchar(' ',temprow,k);
                                }
                                continue;
                        }
                        buffer_out[i+1] = old_buffer_out[i];
                        fbputchar(buffer_out[i+1], temprow, ++tempcol);
                }
                column++;
                count++;
                maxcount++;
                maxcol++;
	 }
      }
      if ((max==0)&&((packet.modifiers == 0x02) || (packet.modifiers == 0x20) || (packet.modifiers == 0x22))) // check for other shift cases too modifers
      {
        switch(input_code){
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
            sprintf(buffer,"#");
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
	  case 0x36: //comma
	    buffer_out[count] = 44; 
 	    sprintf(buffer,",");
	    break;
	  case 0x34: //apos
	    buffer_out[count] = 39;
	    sprintf(buffer,"'");
	    break;
	  case 0x37: //period
	    buffer[count] = 46;
	    sprintf(buffer,".");
	    break;
          case 0x00:
            something = 0;
            break;
	  default:
	    something = 0;
	    break;
        }
      if(something == 1){
          if(maxcount == count){
          	fbputs(buffer,row,column);
		if (row != 20){
          		if(column >= 62){
            			fbputchar(cursor,row+1,0);
            		} else {
				fbputchar(cursor,row,column + 1);
			}
          	} else {
			if (column < 62){
              			fbputchar(cursor,row,column + 1);
			}
          	}
          	column++;
          	count++;
          	maxcount++;
          	maxcol++;
          } else {
                
                fbputs(buffer,row,column);
                int temprow = row;
                int tempcol = column;
                for (int i=count;i<maxcount;i++){
                        if (i==63){
			//if((i==63)||(i==127)||(i==191)){
                                tempcol = 0; //CHNGD:0
                                temprow++;
                        }
                        if((column == 0) && (row != 19)) {
                                for(int k=0;k<64;k++){
                                        fbputchar(' ',temprow,k);
                                }
                                continue;
                        }
                        buffer_out[i+1] = old_buffer_out[i];
                        fbputchar(buffer_out[i+1], temprow, ++tempcol);
                }
                column++;
                count++;
                maxcount++;
		maxcol++;
          }
        } else {
           fbputchar(cursor,row,column);
        }

      } 
      else if (max == 0)
      {
        switch(input_code){
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
            sprintf(buffer,"u"); 
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
	  case 0x36: //comma
            buffer_out[count] = 44;
            sprintf(buffer,",");
            break;
          case 0x37: //period
            buffer[count] = 46;
            sprintf(buffer,".");
            break;
          case 0x34: //apos
            buffer_out[count] = 39;
            sprintf(buffer,"'");
            break;
          case 0x00:
            something = 0;
            break;
	  default:
	    something = 0;
	    break;
        }
        if(something == 1){
	  if(maxcount == count){
          	fbputs(buffer,row,column);
          	if (row != 20){
                        if(column >= 62){
                                fbputchar(cursor,row+1,0);
                        } else {
                                fbputchar(cursor,row,column + 1);
                        }
                } else {
                        if (column < 62){
                                fbputchar(cursor,row,column + 1);
                        }
                }
		column++;
          	count++;
	 	maxcount++;
	  	maxcol++;
	  } else {
		//maxcol++;
		fbputs(buffer,row,column);
		int temprow = row;
		int tempcol = column;
		for (int i=count;i<maxcount;i++){
			if (i==63){
			//if((i==63)||(i==127)||(i==191)){
				tempcol = 0;
				temprow++;
			}
			if((column == 0)&&(row!=19)) {
				for(int k=0;k<64;k++){
					fbputchar(' ',temprow,k);
				}
				continue;
			}
			buffer_out[i+1] = old_buffer_out[i];
			fbputchar(buffer_out[i+1], temprow, ++tempcol);
		}
		column++;
		maxcol++;
		count++;
		maxcount++;
	  }
        } else {
	   fbputchar(cursor,row,column);
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



  /* Look for and handle keypresses 
  char buffer[128];
  int row = 19;
  int column = 0;
  for (;;) {
  int something = 1;
    libusb_interrupt_transfer(keyboard, endpoint_address,
			      (unsigned char *) &packet, sizeof(packet),
			      &transferred, 0);
    for (int i = 0; i < NUM_KEYS; i++) for (int j = 0; j < 12; j++)
      keystate[i][j] = '\0';
    if (transferred == sizeof(packet)) {
      sprintf(keystate[1], "%02x %02x %02x", packet.modifiers, packet.keycode[0],
	      packet.keycode[1]);
      printf("%s\n", keystate[1]);
      printf("keystate 0 %s\n", keystate[0]);
      //fbputs(keystate, 17, 0);
      fbputs(keystate[0], 17, 0);
      fbputs(keystate[1], 16, 0);
      for (int i = 0; i < 12; i++) {
        keystate[0][i] = keystate[1][i];
      }
      if (packet.keycode[0] == 0x29) {
	break;
      }
      if(column == 64){
	column = 0;
	row++;
      }
      switch (packet.keycode[0]){
	case 0x04:
		sprintf(buffer,"a");
		break;
	case 0x05:
		sprintf(buffer,"b");
		break;
	case 0x06:
		sprintf(buffer,"c");
		break;
	case 0x07: 
		sprintf(buffer,"d");
		break;
	case 0x00:
		something = 0;
		break;
      }
      if(something == 1){
      	fbputs(buffer,row,column);
      	column++;
      }
    }
  }

  // Terminate the network thread 
  pthread_cancel(network_thread);

  // Wait for the network thread to finish
  pthread_join(network_thread, NULL);

  return 0;
} 
*/

void *network_thread_f(void *ignored)
{
  char recvBuf[SCREEN_ROWS][BUFFER_SIZE];
  for (int i = 0; i < SCREEN_ROWS; i++) {
    for (int j = 0; j < BUFFER_SIZE; j++) {
      recvBuf[i][j] = '\0';
    }
  }
  printf("Debug to check buffer value 7 %s", recvBuf[7]); 
  int n;
  /* Receive data */
  int latest = 0;
  char emptybuf[64];
  for (int i = 0; i < 64; i++) {
    emptybuf[i] = ' ';
  }
  while ( (n = read(sockfd, &recvBuf[latest], 64)) > 0 ) {
    recvBuf[latest][(n == 64) ? n : n-1] = '\0';              // For handling mid-sentence cutoffs
    if (recvBuf[latest][n-2] == '\r') recvBuf[latest][n-2] = '\0'; // For handling carriage return characters '\r\n' from proper HTTP protocols
    for (int col = 0 ; col < 64; col++) {
      fbputchar('*', 0, col);
      fbputchar('-', 18, col);
      fbputchar('*', 23, col);
    }
    printf("The data received was %s\n", recvBuf[latest]);
    /*for (int row = 1; row < 18; row++) {
      for (int col = 0; col < 64; col++) {
	fbputchar(' ', row, col);
      }
    }*/
    fbputs(emptybuf, latest + 1, 0);
    fbputs(recvBuf[latest], latest + 1, 0);
    latest = (latest + 1) % 17;
    if (latest == 0) {
      break;
    }
  }

  while ( (n = read(sockfd, &recvBuf[latest], 64)) > 0 ) {
    recvBuf[latest][(n == 64) ? n : n-1] = '\0';              // For handling mid-sentence cutoffs
    if (recvBuf[latest][n-2] == '\r') recvBuf[latest][n-2] = '\0'; // For handling carriage return characters '\r\n' from proper HTTP protocols
    for (int col = 0 ; col < 64; col++) {
      fbputchar('*', 0, col);
      fbputchar('-', 18, col);
      fbputchar('*', 23, col);
    }
    printf("The data received was %s\n", recvBuf[latest]);
    /*for (int row = 1; row < 18; row++) {
      for (int col = 0; col < 64; col++) {
	fbputchar(' ', row, col);
      }
    }*/
    for (int ind = 1; ind < 18; ind++) {
      fbputs(emptybuf, ind, 0);
      fbputs(recvBuf[(latest + ind)%17], ind, 0);
    }
    latest = (latest + 1) % 17;
  }

  return NULL;
}

