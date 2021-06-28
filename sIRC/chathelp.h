
#ifndef CHATHELP_H
#define CHATHELP_H

#include <sstream>
#include <string>

static std::string ChatHelp()
{
	std::stringstream ss;
	ss << "sIRC is a IRC client using the Syllable API (libsyllable) as a GUI tookit.\n";
	ss << "\n";
	ss << "Internet Relay Chat (IRC) is a system for live interactive Internet text messaging (chat).\n";
	ss << "It is mainly designed for group communication in discussion forums, called channels.\n";
	ss << "To take part in a discussion you connect to an IRC server using an IRC client program like sIRC\n";
	ss << "\n";
	ss << "sIRC has a big multi-line textview (output field) which displays all messages sent from the\n";
	ss << "server, and the discussion in the selected channel plus any server messages.\n";
	ss << "Enter your message in the single-line textview at the bottom if you want to contribute.\n";
	ss << "\n";
	ss << "To connect to a different server first edit the user settings at the top of sIRC32.cpp file.\n";
	ss << "Select your nickname and/or username, etc... and then recompile this application for\n";
	ss << "the settings to take affect. After you connect and get the greeting of the server, select\n";
	ss << "in the Channel menu \"Join Channel\" to enter the default #syllable channel.\n";
	ss << "Only 1 channel is hard coded!, use the input field to join a different channel.\n";
	ss << "\n";
	ss << "In the menu a select \"Join Channel\" will alter the input field message and the\n";
	ss << "server messages displayed in the main textview. If \"Leave Channel\" is selected in the menu,\n";
	ss << "then this setting will return to the default of raw and unmodified messages.\n";
	ss << "\n";
	ss << "Commands not available via the menu can be entered in the input field.\n";
	ss << "So \"JOIN #debian\" (without quotes) will send \"JOIN #debian\" to the server\n";
	ss << "and thus change the messages to this channel.\n";
	ss << "\n";
	ss << "Other examples are: \"PRIVMSG #debian :hello\", \"PART #debian\", \"WHOIS sIRC_\".\n";
	ss << "If you send commands that require uppercase parameters you have to enter them as uppercase letters.\n";
	ss << "\n";
	ss << "There are also options where you can list by passing \"HELP\" in the input field.\n";
	ss << "The connected server will send back a help listing.\n";
	ss << "\n";
	ss << "June 2021 David Kent\n";

	return ss.str();
}

#endif

