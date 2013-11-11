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
* This contains the header file for Get last known position 
* 
*
*/



/**
 @file ctlbsclientgetlastknownposao.h
*/
#if (!defined __CT_LBS_CLIENT_LAST_KNOWN_POS_AO_H__)
#define __CT_LBS_CLIENT_LAST_KNOWN_POS_AO_H__

#include <e32base.h>
#include <lbs.h>

class MT_GetLastKnownPosObserver
	{
	public:
	virtual void GetLastKnownPositionCallback(TRequestStatus& aStatus) = 0;	
	};


class CT_LbsClientGetLastKnownPosAO: public CActive
	{
public:
	~CT_LbsClientGetLastKnownPosAO();
	static CT_LbsClientGetLastKnownPosAO* NewL(MT_GetLastKnownPosObserver& aUser);
	// wrapper for async positioner function GetLastKnownPosition()
	void GetLastKnownPosL(RPositioner& aPositioner, TPositionInfo& aPosInfo);
	
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
	MT_GetLastKnownPosObserver& iCaller;
	
	CT_LbsClientGetLastKnownPosAO(MT_GetLastKnownPosObserver& aCaller);
	};

#endif // _CT_LBS_CLIENT_LAST_KNOWN_POS_AO_H_
