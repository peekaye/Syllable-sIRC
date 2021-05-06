//	sIRC.h
/*****************************************************************************/
/*                                                                           */
/*  This program is free software; you can redistribute it and/or modify     */
/*  it under the terms of the GNU General Public License as published by     */
/*  the Free Software Foundation; either version 2 of the License, or        */
/*  (at your option) any later version.                                      */
/*                                                                           */
/*  This program is distributed in the hope that it will be useful,          */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/*  GNU General Public License for more details.                             */
/*                                                                           */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program; if not, write to the Free Software              */
/*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA  */
/*                                                                           */
/*****************************************************************************///	sIRC21.h


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <poll.h>

#include <iostream>
#include <fstream>
#include <vector>

#include <gui/window.h>
#include <gui/view.h>
#include <gui/rect.h>
#include <gui/menu.h>
#include <gui/requesters.h>
#include <gui/font.h>
#include <gui/checkmenu.h>
#include <gui/layoutview.h>
#include <gui/textview.h>

#include <util/application.h>
#include <util/message.h>
#include <util/looper.h>
#include <util/string.h>
#include <util/resources.h>
#include <util/settings.h>
#include <util/messenger.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

#include <atheos/time.h>
#include <util/datetime.h>

int argc;
char *argv[4096];
int sockfd, go, gowrite, messagetosend;
int numbytes;  			
char buf[50000];
char buf1[5000];
int a = 0;
int erase = 0;

using namespace os;
using namespace std;

//#define SLEEP 100000000
#define SLEEP 999999999

//new enum
enum {
	MSG_TOLOOPER_START,
	MSG_TOLOOPER_RECEIVE,
	MSG_TOLOOPER_SEND,
//old enum	
//enum { 
	M_MENU_QUIT, 
	M_MENU_ABO, 
	M_MENU_CONNECT, 
	M_MENU_DISCONNECT,  
	MSG_SEND, 
	MSG_RESET, 
	M_MENU_SENDMSG, 
	MSG_JOINCHANNEL, 
	MSG_UPDATE, 
	MSG_SETTINGS, 
	MSG_SINGLEUPDATE,
	MSG_BTN_START,
	MSG_BTN_STOP,
	MSG_TOLOOPER_STOP,
	MSG_TOLOOPER_ACK,
	MSG_FROMLOOPER_NEW_MESSAGE,
	MSG_VOID,
	M_MENU_LOGIN
	};

//#define PORT 6667 // the port client will be connecting to 

//#define MAXDATASIZE 100 // max number of bytes we can get at once 








