
/**
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef __MESSAGES_H_
#define __MESSAGES_H_

enum Messages
{
	// Menu Items
	M_MENU_START,
	M_MENU_STOP,
	M_MENU_LOGIN,
	M_MENU_JOINCHANNEL,
	M_MENU_LEAVECHANNEL,
	M_MENU_SETTINGS,

	// To looper
	MSG_TOLOOPER_START,
	MSG_TOLOOPER_STOP,
	MSG_TOLOOPER_RECEIVE,
	MSG_TOLOOPER_NEW_MESSAGE,

	// From looper
	MSG_FROMLOOPER_NEW_MESSAGE,

	// Used where no message is required
	MSG_VOID
};

#endif

