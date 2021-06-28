#include <string>
#include <vector>
#include <storage/file.h>
#include <storage/directory.h>
#include <gui/requesters.h>
#include "configfile.h"

using namespace std;



string ConfigFile::empty_string;



ConfigFile::ConfigFile(const char *sname, const char *uname) :
		Locker("ConfigFile-locker") {
	m_pzSName = sname;
	m_pzUName = uname;
	m_pcListener = NULL;
}

void ConfigFile::SetListener(ConfigFileListener *pcListener) {
	m_pcListener = pcListener;
}


void ConfigFile::AddStringOption(const char *opt, const string &defvalue) {
	Option o;
	
	o.name = opt;
	o.type = 'S';
	o.sval = defvalue;
	m_cOpts.push_back(o);
}

void ConfigFile::AddIntOption(const char *opt, int defvalue) {
	Option o;
	
	o.name = opt;
	o.type = 'I';
	o.ival = defvalue;
	m_cOpts.push_back(o);
}

void ConfigFile::AddBoolOption(const char *opt, bool defvalue) {
	Option o;
	
	o.name = opt;
	o.type = 'B';
	o.ival = defvalue?1:0;
	m_cOpts.push_back(o);
}


int ConfigFile::FindOption(const char *opt, char type) const {
	int size = m_cOpts.size();
	int i;
	
	for (i = 0; i < size; i ++) {
		if (strcmp(m_cOpts[i].name,opt) == 0) {
			if (m_cOpts[i].type == type) {
				return i;
			}
			else {
				if (m_pcListener)
					m_pcListener->BadType(opt,m_cOpts[i].type,type);
				return -1;
			}
		}
	}
	if (m_pcListener)
		m_pcListener->InvalidOption(opt);
	return -1;
}


const string &ConfigFile::GetStringValue(const char *opt) const {
	int i;
	if ((i = FindOption(opt,'S')) >= 0)
		return m_cOpts[i].sval;
	else
		return empty_string;
}

int ConfigFile::GetIntValue(const char *opt) const {
	int i;
	if ((i = FindOption(opt,'I')) >= 0)
		return m_cOpts[i].ival;
	else
		return 0;
}
	
bool ConfigFile::GetBoolValue(const char *opt) const {
	int i;
	if ((i = FindOption(opt,'B')) >= 0)
		return m_cOpts[i].ival != 0;
	else
		return false;
}

void ConfigFile::SetStringValue(const char *opt, const string &value) {
	int i;
	if ((i = FindOption(opt,'S')) >= 0)
		m_cOpts[i].sval = value;
}
	
void ConfigFile::SetIntValue(const char *opt, int value) {
	int i;
	if ((i = FindOption(opt,'I')) >= 0)
		m_cOpts[i].ival = value;
}

void ConfigFile::SetBoolValue(const char *opt, bool value) {
	int i;
	if ((i = FindOption(opt,'B')) >= 0)
		m_cOpts[i].ival = value?1:0;
}


int ConfigFile::FindOptionByName(const char *prefix,
								 const char *opt, int optlen) const {
	int prefixlen = strlen(prefix);
	int i;
	int size = m_cOpts.size();
	const char *name;
	
	for (i = 0; i < size; i ++) {
		name = m_cOpts[i].name;
		if (prefixlen > 0) {
			if (strncmp(name,prefix,prefixlen) != 0)
				continue;
			if (name[prefixlen] != '/')
				continue;
			name += prefixlen+1;
		}
		if (strncmp(name,opt,optlen) != 0 || name[optlen] != 0)
			continue;
		return i;
	}
	return -1;
}

bool ConfigFile::ReadFile(const char *filename) {
	os::File f;
	char buf[10000]; // the size of buffer limits maximum line length
	char prefix[sizeof(buf)];
	int top,pos,i,j,k,l,n,optindex;
	int line;
	
	if (f.SetTo(filename,O_RDONLY) != 0)
		return false;
	
	pos = 0;
	top = 0;
	line = 1;
	prefix[0] = 0;
	while (true) {
		for (i = pos; i < top && buf[i] != '\n'; i ++);
		
		// no more lines in the buffer?
		if (i == top) {
			memmove(buf,buf+pos,top-pos);
			top -= pos;
			i -= pos;
			pos = 0;
			n = f.Read(buf+top,sizeof(buf)-top);
			if (n > 0)
				top += n;
			
			if (top == 0)
				break;  // no more bytes in the buffer nor in the file
		}
		
		// try to find '\n' again
		for (; i < top && buf[i] != '\n'; i ++);
		
		// now, i points to the end of line (either '\n' or EOF)
		// (ok, not perfectly true, but if we have short lines it is)

		// skip spaces at the beginning of line
		for (j = pos; j < i && IsSpace(buf[j]); j ++);
		
		// group
		if (j != i && buf[j] == '[') {
			// skip the '[' char and any spaces that may follow
			for (j++; j<i && IsSpace(buf[j]); j ++);
			
			for (k = j; k < i && !IsSpace(buf[k]) && buf[k] != ']'; k ++);
			
			memcpy(prefix,buf+j,k-j);
			prefix[k-j] = 0; // don't worry, this is ok
		}
		// option
		else if (j != i && buf[j] != '#') {
			// k will point right after the option name
			for (k = j; k < i && !IsSpace(buf[k]) && buf[k] != ':'; k ++);
			
			if (k == i) {
				if (m_pcListener)
					m_pcListener->R_SyntaxError(filename,line);
			}
			else { // colon or space found
				optindex = FindOptionByName(prefix,buf+j,k-j);
				if (optindex < 0) {
					if (m_pcListener) {
						char ch = buf[k];
						buf[k] = 0;
						m_pcListener->R_InvalidOption(filename,line,buf+j);	
						buf[k] = ch;
					}
				}
				else {  // option is valid
					for (j = k; j < i && buf[j] != ':'; j ++);
					if (j == i) {
						if (m_pcListener)
							m_pcListener->R_SyntaxError(filename,line);
					}
					else { // colon found
						// skip colon and spaces
						for (j++; j < i && IsSpace(buf[j]); j ++);
						for (k=i; k > j && IsSpace(buf[k-1]); k--);
						
						// now the value lies at indexes j..k-1
						switch(m_cOpts[optindex].type) {
						case 'S':
							m_cOpts[optindex].sval = string(buf+j,k-j);
							break;
						case 'I':
							n = 0;
							for (l = j; l < k; l ++) {
								if (buf[l] < '0' || buf[l] > '9')
									break;
								n = n*10 + buf[l] - '0';
							}
							if (j == k || l < k) {
								if (m_pcListener)
									m_pcListener->R_SyntaxError(filename,line);
							}
							else
								m_cOpts[optindex].ival = n;
							break;
						case 'B':
							if (j == k || (buf[j] != 'y' && buf[j] != 'Y'
									&& buf[j] != 'n' && buf[j] != 'N')) {
								if (m_pcListener)
									m_pcListener->R_SyntaxError(filename,line);
							}
							else
								m_cOpts[optindex].ival = 
									(buf[j] == 'y' || buf[j] == 'Y') ? 1 : 0;
							break;
						}
					}
				}
			}
		}

		pos = i==top ? i : i+1;
		line ++;
	}
	
	f.Unset();

	return true;
}
	
bool ConfigFile::Read() {
	bool s,u;
	
	s = ReadFile(m_pzSName);
	u = ReadFile(m_pzUName);
	
	if (!u && m_pcListener)
		m_pcListener->R_UserConfigNotFound(m_pzUName);
	
	return s || u; // at least some config found
}

const char *ConfigFile::m_pzHeader =
	"# Created by ConfigFile\n\n";
	


	

bool ConfigFile::UWrite() {
	int i,j,size;
	const char *last,*name;
	int lastcount,count;
	char buf[10000];
	int len;
	os::File f;
	os::Directory dir("/");

	i = 0;
	for (j = 0; m_pzUName[j]; j ++) {
		if (m_pzUName[j] == '/') {
			if (dir.SetTo(std::string(m_pzUName,j+1)) != 0 &&
					dir.CreateDirectory(std::string(m_pzUName+i,j-i),&dir) != 0)
				return false;
			i = j+1;
		}
	}
	
	if (f.SetTo(m_pzUName,O_CREAT|O_TRUNC|O_WRONLY) != 0) {
		return false;
	}

	len = strlen(m_pzHeader);
	if (f.Write(m_pzHeader,len) != len) {
		f.Unset();
		return false;
	}
	
	last = "";
	lastcount = 0;
	
	size = m_cOpts.size();
	for (i = 0; i < size; i ++) {
		name = m_cOpts[i].name;
		
		count = 0;
		for (j = 0; name[j]; j ++) {
			if (name[j] == '/')
				count = j+1;
		}
		if (lastcount != count || strncmp(last,name,count) != 0) {
			if (count > 0) {
				buf[0] = '\n';
				buf[1] = '[';
				len = count-1<(int)sizeof(buf)-4 ? count-1 : (int)sizeof(buf)-4;
				memcpy(buf+2,name,len);
				buf[2+len] = ']';
				buf[3+len] = '\n';
				len += 4;
			}
			else {
				len = snprintf(buf,sizeof(buf),"\n[]\n");
			}
		}
		else {  // prefix se nemeni
			len = 0;
		}

		len += snprintf(buf+len,sizeof(buf)-len,"%s: ",name+count);
		
		switch (m_cOpts[i].type) {
		case 'S':
			// FIXME: co kdyz tam bude '\n'?!!!
			len += snprintf(buf+len,sizeof(buf)-len,"%s\n",
							m_cOpts[i].sval.c_str());
			break;
		case 'I':
			len += snprintf(buf+len,sizeof(buf)-len,"%d\n",
							m_cOpts[i].ival);
			break;
		case 'B':
			len += snprintf(buf+len,sizeof(buf)-len,"%s\n",
							m_cOpts[i].ival?"Yes":"No");
			break;
		}
		if (f.Write(buf,len) != len) {
			f.Unset();
			return false;
		}
		lastcount = count;
		last = name;
	}
		
	f.Unset();
	
	return true;
}



/* simple listener implementation */
void VerboseConfigFileListener::R_InvalidOption(const char *fn, int line,
				const char *opt) {
	char buf[1024];
	snprintf(buf,sizeof(buf),"Error in config file '%s', line %d\n"
			"Invalid option '%s'",fn,line,opt);
	Alert(buf);
}
	
void VerboseConfigFileListener::R_SyntaxError(const char *fn, int line) {
	char buf[1024];
	snprintf(buf,sizeof(buf),"Syntax error in config file '%s', line %d",
			fn,line);
	Alert(buf);
}

void VerboseConfigFileListener::R_UserConfigNotFound(const char *fn) {
	char buf[1024];
	snprintf(buf,sizeof(buf),"User config file '%s' not found!", fn);
	Alert(buf);
}
	
void VerboseConfigFileListener::BadType(const char *opt, char type,
		char reqtype) {
	char buf[1024];
	snprintf(buf,sizeof(buf),"Application ERROR: Bad type requested\n"
			"option: %s\ntype: %c\nrequested type: %c", opt, type, reqtype);
	Alert(buf);
}

void VerboseConfigFileListener::InvalidOption(const char *opt) {
	char buf[1024];
	snprintf(buf,sizeof(buf),"Application ERROR: Invalid option\n"
			"option: %s", opt);
	Alert(buf);
}

void VerboseConfigFileListener::Alert(const char *s) {
	(new os::Alert("Error", s, 0, "OK", NULL))->Go();
}













