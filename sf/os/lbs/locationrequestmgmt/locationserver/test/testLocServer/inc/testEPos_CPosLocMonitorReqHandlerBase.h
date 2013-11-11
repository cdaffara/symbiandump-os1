/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  	The base class that provides the common functionalities of 
*				all the requests to the Location Monitor*
*/

#include <e32property.h>
#include <e32base.h>
#include <e32err.h> 						
#include <lbspositioninfo.h>
#include <LbsErrors.h>



#ifndef CPOSLOCMONITORREQHANDLERBASE_H_
#define CPOSLOCMONITORREQHANDLERBASE_H_

class CPosLocMonitorReqHandlerBase : public CActive
	{
public:
	void NotifySessionClosed(const CSession2* aSessionPtr);
	void NotifyServerShutDown();
	void NotifySubSessionClosed(const RMessage2& aMessage);
	
	virtual ~CPosLocMonitorReqHandlerBase();  

public:
	enum TLocMonitorRequestType
		{
		EReqOnSession,
		EReqOnSubSession,
		};

protected:
	CPosLocMonitorReqHandlerBase();
	// Destructors
    void RequestComplete(const RMessage2& aMessage, TInt aCompleteCode);
    void QRequestsComplete(TInt aCompleteCode);

    void CheckAndAddReqToQueueL(TLocMonitorRequestType aRequestType, const RMessage2& aMessage);
    void CancelRequest(TLocMonitorRequestType aRequestType, const RMessage2& aMessage);

private:
	static TBool MatchSessionAndSubsession(const RMessage2& qClient, const RMessage2& newClient);
    static TBool MatchSession(const RMessage2& qClient, const RMessage2& newClient);

	TInt MatchRequestInQueue (TLocMonitorRequestType aRequestType, const RMessage2& aMessage);
	void FindRequestAndCancel(TLocMonitorRequestType aRequestType, const RMessage2& aMessage);
	
protected:
	// The queue of requests
	RArray<RMessage2> 				iLocMonitorReqQ;


private:
	// A pointer to the function that compares two RMessage2 entries
	// Utilised in the call to the "Find" method of RArray
	TIdentityRelation<RMessage2> 	iSubsessionIdentity;  
	TIdentityRelation<RMessage2> 	iSessionIdentity;


	};



#endif /*CPOSLOCMONITORREQHANDLERBASE_H_*/
