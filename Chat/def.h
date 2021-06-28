#ifndef __DEF_H
#define __DEF_H


#include <gui/requesters.h>
#include <stdio.h>

// config file options
#define OPT_HOST            "server/host"
#define OPT_PORT            "server/port"
#define OPT_USER            "server/user"
#define OPT_PASSWD          "server/password"
#define OPT_RESOURCE        "server/resource"
#define OPT_FOCUSCHAT       "events/rcvfocuschat"

static inline void bad_asumption(const char *file, int line, const char *msg) {
	char buf[2048];
	snprintf(buf,sizeof(buf),"%s:%d: %s", file,line,msg);
	(new os::Alert("BAD ASSUPTION!",buf,0,"UGH!",NULL))->Go();
}

#define breakpoint(msg) bad_asumption(__FILE__,__LINE__,msg)



#endif










