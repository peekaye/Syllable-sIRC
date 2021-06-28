#include <util/message.h>
#include <util/application.h>
#include <gui/window.h>
#include <gui/requesters.h>
#include <gui/stringview.h>
#include <gui/textview.h>
#include <gui/checkbox.h>
#include "woptions.h"
#include "wroster.h"
#include "app.h"
#include "configfile.h"
#include "def.h"


#define W      (RIGHT+1+LEFT)
#define H      (STATUSTOP+STATUSH)


#define TOP(n)    (10+(n)*24)
#define BOTTOM(n) (10+(n)*24+19)

#define LABELLEFT  10
#define LABELRIGHT 70
#define FIELDLEFT  80
#define FIELDRIGHT 240

#define LEFT LABELLEFT
#define RIGHT FIELDRIGHT

#define BTNTOP     TOP(7)
#define BTNH       24
#define BTNW       80
#define BTNSP      8

#define STATUSTOP  (BTNTOP+BTNH+2)
#define STATUSH    20


using namespace os;


WOptions::WOptions(WRoster *pcWRoster)
		:Window(Rect(80,20,80+W-1,20+H-1),"login_window",
				"Maja: Options")
{
	Button *b;
	ConfigFile *conf = g_pcApplication->GetConfig();
	
	AddChild( new StringView(Rect(LABELLEFT,TOP(0),LABELRIGHT,BOTTOM(0)),
							 "label_host","server:",ALIGN_LEFT) );
	AddChild( new StringView(Rect(LABELLEFT,TOP(1),LABELRIGHT,BOTTOM(1)),
							 "label_port","port:",ALIGN_LEFT) );
	AddChild( new StringView(Rect(LABELLEFT,TOP(2),LABELRIGHT,BOTTOM(2)),
							 "label_user","user:",ALIGN_LEFT) );
	AddChild( new StringView(Rect(LABELLEFT,TOP(3),LABELRIGHT,BOTTOM(3)),
							 "label_pass","password:",ALIGN_LEFT) );
	AddChild( new StringView(Rect(LABELLEFT,TOP(4),LABELRIGHT,BOTTOM(4)),
							 "label_resource","resource:", ALIGN_LEFT) );

	
	conf->Lock();

	m_pcFieldHost = new TextView(Rect(FIELDLEFT,TOP(0),FIELDRIGHT,BOTTOM(0)),
			"textfield_host",conf->GetStringValue(OPT_HOST).c_str());
	m_pcFieldPort = new TextView(Rect(FIELDLEFT,TOP(1),FIELDRIGHT,BOTTOM(1)),
			"textfield_port",String().Format("%d",conf->GetIntValue(OPT_PORT)).c_str());
	m_pcFieldUser = new TextView(Rect(FIELDLEFT,TOP(2),FIELDRIGHT,BOTTOM(2)),
			"textfield_user",conf->GetStringValue(OPT_USER).c_str());
	m_pcFieldPass = new TextView(Rect(FIELDLEFT,TOP(3),FIELDRIGHT,BOTTOM(3)),
			"textfield_pass",conf->GetStringValue(OPT_PASSWD).c_str());
	m_pcFieldResource = new TextView(Rect(FIELDLEFT,TOP(4),FIELDRIGHT,BOTTOM(4)),
			"textfield_resource",conf->GetStringValue(OPT_RESOURCE).c_str());

	m_pcCBFocusChat = new CheckBox(Rect(LEFT,TOP(5),RIGHT,BOTTOM(5)),
			"checkbox_focuschat","Focus chat when received message",NULL);
	m_pcCBFocusChat->SetValue(conf->GetBoolValue(OPT_FOCUSCHAT));
			
	
	conf->Unlock();


	m_pcStatus = new StringView(Rect(LEFT,STATUSTOP,RIGHT,STATUSTOP+STATUSH-1),
								"status", "", ALIGN_LEFT,
								CF_FOLLOW_LEFT|CF_FOLLOW_RIGHT
								|CF_FOLLOW_BOTTOM);

	m_pcFieldPass->SetPasswordMode(true);
	m_pcFieldPort->SetNumeric(true);
	
	AddChild(m_pcFieldHost);
	AddChild(m_pcFieldPort);
	AddChild(m_pcFieldUser);
	AddChild(m_pcFieldPass);
	AddChild(m_pcFieldResource);
	AddChild(m_pcCBFocusChat);
	
	AddChild( b = new Button(Rect(RIGHT-BTNW-BTNSP-BTNW+1,BTNTOP,
								  RIGHT-BTNW-BTNSP,BTNTOP+BTNH),
							 "btn_ok", "Ok", new Message(ID_BOK),
							 CF_FOLLOW_RIGHT|CF_FOLLOW_BOTTOM) );
	
	AddChild( new Button(Rect(RIGHT-BTNW+1,BTNTOP,RIGHT,BTNTOP+BTNH),
						 "btn_quit", "Cancel", new Message(ID_BCANCEL),
						 CF_FOLLOW_RIGHT|CF_FOLLOW_BOTTOM) );
	
	AddChild(m_pcStatus);
	
	SetDefaultButton(b);
	
	m_pcWRoster = pcWRoster;
}

void WOptions::HandleMessage( Message *pcMessage ) {
	ConfigFile *conf = g_pcApplication->GetConfig();

	switch(pcMessage->GetCode()) {
	case ID_BOK:
		conf->Lock();
		conf->SetStringValue(OPT_HOST,m_pcFieldHost->GetBuffer()[0]);
		conf->SetIntValue(OPT_PORT,atoi(m_pcFieldPort->GetBuffer()[0].c_str()));
		conf->SetStringValue(OPT_USER,m_pcFieldUser->GetBuffer()[0]);
		conf->SetStringValue(OPT_PASSWD,m_pcFieldPass->GetBuffer()[0]);
		conf->SetStringValue(OPT_RESOURCE,m_pcFieldResource->GetBuffer()[0]);
		conf->SetBoolValue(OPT_FOCUSCHAT,m_pcCBFocusChat->GetValue().AsBool());
		conf->Unlock();
		
		if (!conf->UWrite()) {
			(new Alert("Error writing file","Cannot save options!",0,"OK",NULL))->Go();
		}

		m_pcWRoster->ReqCloseOptions();		
/*
		g_pcApplication->ReqConnect(m_pcFieldHost->GetBuffer()[0].c_str(),
									atoi(m_pcFieldPort->GetBuffer()[0].c_str()),
									m_pcFieldUser->GetBuffer()[0].c_str(),
									m_pcFieldPass->GetBuffer()[0].c_str(),
									m_pcFieldResource->GetBuffer()[0].c_str());
*/
		break;
		
	case ID_BCANCEL:
		m_pcWRoster->ReqCloseOptions();
		break;
		
	default:
		Window::HandleMessage( pcMessage );
	}
}

bool WOptions::OkToQuit() {
	m_pcWRoster->ReqCloseOptions();
	return false;
}










