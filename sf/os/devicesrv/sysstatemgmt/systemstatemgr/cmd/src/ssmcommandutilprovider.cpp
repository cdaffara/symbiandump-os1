// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ssmcommandutilprovider.h"
#include "ssmdebug.h"

_LIT( KSysMonProxyDLL, "loadsysmon.dll" );
typedef MSsmLoadSysMon* (*TFuncCreateL)( void );

/**
Factory method that returns pointer to the object of CSsmCommandUtilProvider.
*/
EXPORT_C CSsmCommandUtilProvider* CSsmCommandUtilProvider::NewL()
	{
	CSsmCommandUtilProvider* self = new(ELeave) CSsmCommandUtilProvider();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);	
	return self;
	}

/**
Returns CStartSafe object
*/
CSsmStartSafe& CSsmCommandUtilProvider::StartSafe()
	{
	return *iStartSafe;
	}

/**
DTOR
*/
EXPORT_C CSsmCommandUtilProvider::~CSsmCommandUtilProvider()
	{
	delete iSysMonSession;
	delete iStartSafe;
	iSysMonProxyLib.Close();
	iFs.Close();
	}

/**
Default CTOR
*/
CSsmCommandUtilProvider::CSsmCommandUtilProvider()
	{
	}

/**
Returns SysMonSession.
@return System monitor session.
@leave KErrNotSupported If NULL is assigned to iSysMonSession.
@leave One of the system wide error code.
@see MSsmLoadSysMon
*/
MSsmLoadSysMon& CSsmCommandUtilProvider::SysMonSessionL()
	{
	if(!iSysMonSession)
		{
		// if iSysMonSession is NULL, then sysmon lib is not installed.
		User::Leave(KErrNotSupported);
		}
	if(iSysMonSession)
		{
		//close the existing session and send the new one to avoid the memory leak.
		iSysMonSession->Close();
		iSysMonSession->OpenL();
		}

	return *iSysMonSession;
	}

/**
This function connects to the RFs if Handle is NULL, and returns the connected RFs.
@return Connected RFs
@
*/
RFs& CSsmCommandUtilProvider::RfsL()
	{
	if (iFs.Handle() == NULL)
		{	
		SSMLOGLEAVEIFERROR(iFs.Connect());		// if not already connected then connect 
		}
	return iFs;
	}

/**
 Load the dll interfacing between us and the System Monitor component, if installed.
*/
void CSsmCommandUtilProvider::LoadMonitorProxyLibL()
	{
	if( KErrNone != iSysMonProxyLib.Load(KSysMonProxyDLL) )
	 	{
	 	return;	
	 	}

	TFuncCreateL sysMonProxyCreateL = reinterpret_cast<TFuncCreateL>(iSysMonProxyLib.Lookup(1));

	iSysMonSession = sysMonProxyCreateL();
	}

	/**
Constructs the object, Creates the startsafe object and SysMonSession.
*/
void CSsmCommandUtilProvider::ConstructL()
	{
	iStartSafe = CSsmStartSafe::NewL(); // Creates CStartSafe object.
	LoadMonitorProxyLibL();
	}


