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
 @file ctlbsx3pdotransmitpos.h
*/
#if (!defined __CTLBSX3PDOTRANSMITPOS_H__)
#define __CTLBSX3PDOTRANSMITPOS_H__

#include <e32base.h>
#include <lbs.h>
#include <lbs/lbsx3p.h>


_LIT(KCT_LbsX3PDoTransmitPos, "CT_LbsX3PDoTransmitPos");

class CT_LbsX3PDoTransmitPos;

// Async callbacks
class MT_LbsX3PDoTransmitPosObserver
	{
	public:
		virtual void MT_LbsX3PDoTransmitPosCallback(TRequestStatus& aStatus, CT_LbsX3PDoTransmitPos *aDoTransmitPos=NULL) = 0;	// do we need the aStatus...
		virtual void MT_LbsX3PDoRefPosCallback(const TRequestStatus& aStatus,const TPositionInfo& aRefPosInfo) = 0;
	};
	

class CT_LbsX3pRefLocWaiter: public CActive
	{
	public:
		~CT_LbsX3pRefLocWaiter();
		static CT_LbsX3pRefLocWaiter* NewL(MT_LbsX3PDoTransmitPosObserver* aObserver);
		void Activate();
		
	protected:
		void RunL();
		virtual TInt RunError(TInt aError);
		void DoCancel();
		
	private:
		CT_LbsX3pRefLocWaiter(MT_LbsX3PDoTransmitPosObserver* aObserver);
		void ConstructL();
		
	public:
		TPositionInfo iRefPosInfo;
		
	private:
		MT_LbsX3PDoTransmitPosObserver* iObserver;
	};



class CT_LbsX3PDoTransmitPos: public CActive
	{
	public:
		~CT_LbsX3PDoTransmitPos();
		static CT_LbsX3PDoTransmitPos* NewL(MT_LbsX3PDoTransmitPosObserver* aObserver, RLbsTransmitPosition& aPositionTransmitter);

		void TransmitPosition(const TDesC& aDestinationID, TUint aTransmitPriority, TPositionInfo & aTransmittedPosInfo);
		void TransmitPositionReportingRefLoc(const TDesC& aDestinationID, TUint aTransmitPriority, TPositionInfo & aTransmittedPosInfo);
		CT_LbsX3pRefLocWaiter* GetRefLocWaiter(void) { return iRefLocWaiter; }

	protected:
		//	Functions from CActive:
		virtual void DoCancel();
		virtual void RunL();
		virtual TInt RunError(TInt aError);
		
	private:
		CT_LbsX3PDoTransmitPos(MT_LbsX3PDoTransmitPosObserver* aObserver, RLbsTransmitPosition& aPositionTransmitter);
		void CT_LbsX3PDoTransmitPos::ConstructL();
		

	private:
		// caller
		MT_LbsX3PDoTransmitPosObserver* iObserver;
		
		// Self locate server.
		RLbsTransmitPosition& iPositionTransmitter;
		CT_LbsX3pRefLocWaiter* iRefLocWaiter;

	};

#endif // __CTLBSX3PDOTRANSMITPOS_H__
