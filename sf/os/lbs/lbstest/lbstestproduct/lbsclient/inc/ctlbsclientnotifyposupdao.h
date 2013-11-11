/**
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
* Description:
*
*/



/**
 @file ctlbsclientnotifyposupdao.h
*/
#if (!defined __CT_LBS_CLIENT_NOTIFY_POS_UPD_AO_H__)
#define __CT_LBS_CLIENT_NOTIFY_POS_UPD_AO_H__

#include <e32base.h>
#include <lbs.h>


class MT_NotifyPosUpdateObserver
	{
	public:
	virtual void NotifyPositionUpdateCallback(TRequestStatus& aStatus) = 0;	
	};


class CT_LbsClientNotifyPosUpdAO: public CActive
	{
public:
	// destructor
	~CT_LbsClientNotifyPosUpdAO();

	// constructor
	static CT_LbsClientNotifyPosUpdAO* NewL(MT_NotifyPosUpdateObserver& aCaller);

	// wrapper for async positioner function NotifyPositionUpdate()
	void CT_LbsClientNotifyPosUpdAO::NotifyPosUpdateL(RPositioner& aPositioner, TPositionInfo& aPosInfo);
	
protected:
//	CActive:
	// Redeclares the base class CActive::DoCancel(). 
	virtual void DoCancel();
	// Redeclares the base class CActive::RunL(). 
	virtual void RunL();
	// Redeclares the base class CActive::RunError(). 
	virtual TInt RunError(TInt aError);
	
private:
	// our status, so we know when request completes:
	// positioner currently in use:
	RPositioner iPositioner;
	// type of request outstanding:
	TInt		iRequestId;
	// caller
	MT_NotifyPosUpdateObserver& iCaller;
		
	CT_LbsClientNotifyPosUpdAO(MT_NotifyPosUpdateObserver& aCaller);
	};

#endif // __CT_LBS_CLIENT_NOTIFY_POS_UPD_AO_H__
