#ifndef __CONFIGFILE_H
#define __CONFIGFILE_H


#include <string>
#include <vector>
#include <util/locker.h>


/*
 * Every line in config file must match one of these patterns:
 * 1)   SP [ '#' comment ]
 * 2)   SP '[' SP group SP ']' SP
 * 3)   SP  option  SP  ':'  SP value SP
 * 
 * where comment stands for any sequence of characters, SP stands for
 * any sequence (including 0 length) mixed of HTAB or SPACE characters
 * (0x09, 0x20).
 * 
 * The option-name as accessible from API is made of most recent group
 * found in the file before option line followed by "/" followed by option.
 * There is one exception to this rule: When no group can be found or
 * the associated group is empty (ie zero length), the option-name equals
 * to option, ie. no '/' is prepended.
 * 
 * ConfigFile class recognizes three types of values:
 * 
 * - boolean:
 *    values starting with 'y' or 'Y' are recognized as 'true'
 *    values starting with char 'n' or 'N' are 'false'
 * 
 * - integer:
 *    value is interpreted as decimal number (if possible)
 * 
 * - string:
 *    (trimmed) character sequence after ':' to the end of line.
 * 
 */


struct Option {
	const char *name;
	char type;   // 'B' | 'I' | 'S'
	int ival;
	std::string sval;
};

class ConfigFileListener {
public:
	virtual ~ConfigFileListener() { }
	virtual void R_InvalidOption(const char *fn, int line, const char *opt) {}
	virtual void R_SyntaxError(const char *fn, int line) {}
	virtual void R_UserConfigNotFound(const char *fn) {}
	
	virtual void BadType(const char *opt, char type, char reqtype) {}
	virtual void InvalidOption(const char *opt) {}
};

class VerboseConfigFileListener : public ConfigFileListener {
public:
	virtual void R_InvalidOption(const char *fn, int line, const char *opt);
	virtual void R_SyntaxError(const char *fn, int line);
	virtual void R_UserConfigNotFound(const char *fn);
	
	virtual void BadType(const char *opt, char type, char reqtype);
	virtual void InvalidOption(const char *opt);

protected:
	virtual void Alert(const char *str);
};



class ConfigFile : public os::Locker {
public:
	ConfigFile(const char *sname, const char *uname);
	
	void SetListener(ConfigFileListener *pcListener);
	
	void AddStringOption(const char *opt, const std::string &defvalue);
	void AddIntOption(const char *opt, int defvalue);
	void AddBoolOption(const char *opt, bool defvalue);
	
	const std::string &GetStringValue(const char *opt) const;
	int GetIntValue(const char *opt) const;
	bool GetBoolValue(const char *opt) const;

	void SetStringValue(const char *opt, const std::string &value);
	void SetIntValue(const char *opt, int value);
	void SetBoolValue(const char *opt, bool value);
	
	bool Read();       // reads both system-wide and user config
	bool UWrite();     // writes the user config... typically ~/config/bla
	
private:
	int FindOption(const char *opt, char type) const;
	int FindOptionByName(const char *prefix, const char *opt, int optlen) const;
	bool ReadFile(const char *filename);
	bool IsSpace(char ch) { return ch==32 || ch=='\t'; }
	
	const char *m_pzSName;
	const char *m_pzUName;
	ConfigFileListener *m_pcListener;
	
	std::vector<Option> m_cOpts;
	
	static std::string empty_string;
	static const char *m_pzHeader;
};


#endif






