// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//
 
#ifndef __AGTNOTIFY_H__
#define __AGTNOTIFY_H__

#include "dummyEtel.h"
#include <etelmm.h>   // RMobilePhone

// QoS commands to Dummy ETEL to either reduce or restore QoS
enum QoSWarningNotification
	{	
	EReduce,
	ERestore
	};

class MAgtNotify 
	{
public:
	virtual void SetTestCase(TInt aTestCase)=0;
	virtual void NotifyAgt(enum QoSWarningNotification aNotification)=0;
	virtual void SetMode(RTelServer::TMobilePhoneNetworkMode aMode)=0;
	};


// Dummy concrete implementation of NOTIFY, will be replaced with James's stuff later
/*class SteveNotify : public MAgxNotify
	{
public:
	virtual void SetTestCase(TInt ) {}
	virtual void NotifyAgx(enum QoSWarningNotification ) {}
	virtual void SetMode(RTelServer::TMobilePhoneNetworkMode ) {}
	};
*/
class TRealAgtNotify : public MAgtNotify
	{
public:
	TRealAgtNotify();
	virtual ~TRealAgtNotify();
public: // from MAgxNotify
	virtual void SetTestCase(TInt aTestCase);
	virtual void NotifyAgt(enum QoSWarningNotification aNotification);
	virtual void SetMode(RTelServer::TMobilePhoneNetworkMode aMode);
private:
	RTelServer iEtelSess;
	TInt iTestCase;
	};


#endif // ifndef __AGTNOTIFY_H__
