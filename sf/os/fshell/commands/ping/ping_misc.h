// ping_misc.h
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __PING_MISC_H
#define __PING_MISC_H

#define IPv4	0
#define IPv6	1

class CPing;

//this class is used as pure interface,
//should be named as M class by Symbian convention.
//the class name is retained for the purpose of 
//reference to original class name in pingapp.exe

class CPingContainer
{
public:
	//TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	virtual void WriteHostL(TDes& aHostname) = 0;
	virtual void UpdateStatisticsL() = 0;
	virtual void WriteLine(const TDesC& abuf) = 0;
	virtual void OnEnd() = 0; 
	//void ResetScreen();

//protected:
	//void FocusChanged(TDrawNow aDrawNow);
	};

#endif
