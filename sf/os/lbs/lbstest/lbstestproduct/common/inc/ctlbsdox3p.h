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
 @file ctlbsdox3p.h
*/

#ifndef __CT_LBS_DO_X3P_H__
#define __CT_LBS_DO_X3P_H__


#include <e32base.h>
#include <lbs.h>
#include <lbs/lbsx3p.h>


//class CT_LbsX3PDoTransmitPos;
_LIT(KCT_LbsDoX3P, "CT_LbsDoX3P");

// Async callbacks
class MT_LbsDoX3PObserver
	{
public:
	virtual void MT_LbsDoX3PCallback(TInt aTransmitId, TRequestStatus& aStatus) = 0;
	};
	
	
class CT_LbsDoX3P: public CActive
	{
public:
	~CT_LbsDoX3P();
	static CT_LbsDoX3P* NewL(MT_LbsDoX3PObserver* aObserver, TInt aTransmitId = 0);

	TInt SetOptions(const TLbsTransmitPositionOptions& aTransmitOptions);

	void StartL(const TDesC& aDestinationID, TUint aTransmitPriority, TPositionInfo & aTransmittedPosInfo);
	
protected:
	//	Functions from CActive:
	virtual void DoCancel();
	virtual void RunL();
	virtual TInt RunError(TInt aError);
	
private:
	CT_LbsDoX3P(MT_LbsDoX3PObserver* aObserver, TInt aTransmitId);
	void CT_LbsDoX3P::ConstructL();
	
	// Caller.
	MT_LbsDoX3PObserver* iObserver;
	
	// Transmit server.
	RLbsTransmitPositionServer iServer;
	RLbsTransmitPosition iTransmitter;

	// Id to allow multiple X3P to be carried out.
	TInt iTransmitId;
	};

#endif // __CT_LBS_DO_X3P_H__
