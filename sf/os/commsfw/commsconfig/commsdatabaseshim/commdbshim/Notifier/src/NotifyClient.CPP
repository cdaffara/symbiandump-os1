// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/

#include "NotifyClient.H"
#include "NotifierServ.H"

const TInt KCommsDatServerMaxOuterLoopRetryCount=2;
const TInt KCommsDatServerMaxInnerLoopRetryCount=10;
const TInt KCommsDatServerRetryTimeout=200000;	// 200 msec

//**********************************
//RCommsdatNotifier
//**********************************

EXPORT_C RCommsdatNotifier::RCommsdatNotifier()
	/** Constructor. */
	{}

		
TInt RCommsdatNotifier::StartNotifierProcess()
	{	
	RProcess server;
	
	TInt ret=server.Create(KCommsdatNotifierExecutable, KNullDesC);
	
	if (ret!=KErrNone)
		{
		return ret;	
		}
	TRequestStatus stat;	
	server.Rendezvous(stat);
	server.Resume();
	server.Close();
	User::WaitForRequest(stat);
	return stat.Int();		
	}	
	
EXPORT_C TInt RCommsdatNotifier::Connect()
	/** Connects to the server.
	*
	* 
	* @return	KErrNone if successful; otherwise, a system-wide error code. */
	{
	TInt ret = CreateSession(COMMSDATNOTIFIER_SERVER_NAME,Version(),-1);

	TInt outerLoopRetryCount = 0;
	while ((ret == KErrNotFound) && (outerLoopRetryCount++ < KCommsDatServerMaxOuterLoopRetryCount))
		{		
		ret=StartNotifierProcess();
		if (ret==KErrNone || ret==KErrAlreadyExists)
			{
			ret = CreateSession(COMMSDATNOTIFIER_SERVER_NAME,Version(),-1);
			TInt innerLoopRetryCount = 0;
			while ((ret == KErrNotFound) && (innerLoopRetryCount++ < KCommsDatServerMaxInnerLoopRetryCount))
				{
				User::After(KCommsDatServerRetryTimeout);
				ret = CreateSession(COMMSDATNOTIFIER_SERVER_NAME,Version(),-1);				
				}
			
			}
			else
			{
				break; // we can't do anything more
			}
		}      	
	return ret;
	}


EXPORT_C TVersion RCommsdatNotifier::Version() const
	/** Gets the client side version number.
	*
	* @return	The client version number. */
	{
	return(TVersion(KCommsdatNotifierMajorVersionNumber,KCommsdatNotifierMinorVersionNumber,KCommsdatNotifierBuildVersionNumber));
	}
	
	

EXPORT_C void RCommsdatNotifier::RequestNotification(TRequestStatus& aStatus)
	{
        SendReceive(ERequestNotify, TIpcArgs(),aStatus);
	}
	
EXPORT_C void RCommsdatNotifier::CancelNotification()
	{
	SendReceive(ECancelNotify, TIpcArgs());
	}	
	
EXPORT_C void RCommsdatNotifier::Close()
    {
	RHandleBase::Close();
    }	
