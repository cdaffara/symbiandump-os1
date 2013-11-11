/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//
// TransmitServerWatch.h
//
//
/** 
 @file
 @internalTechnology
 @test
*/

#ifndef __LBS_GETAFIX_TEST_POS_SERVER_WATCH_H__
#define __LBS_GETAFIX_TEST_POS_SERVER_WATCH_H__

#include <e32base.h>
#include <lbspositioninfo.h>
#include <lbs/lbsx3p.h>
#include <lbsipc.h>

class RPositioner;


class MTransmitServerObserver 
	{
	public:
		virtual void OnTransmitRefPosition(TInt32 aErr, const TPositionInfoBase& aPosInfo) = 0;
		virtual void OnTransmitPosition(TInt32 aErr, const TPositionInfoBase& aPosInfo) = 0;
	};
	
	
class CTLbsX3pRefLocWaiter: public CActive
	{
	public:
		~CTLbsX3pRefLocWaiter();
		static CTLbsX3pRefLocWaiter* NewL(MTransmitServerObserver* aObserver);
		void Activate();
		
	protected:
		void RunL();
		virtual TInt RunError(TInt aError);
		void DoCancel();
		
	private:
		CTLbsX3pRefLocWaiter(MTransmitServerObserver* aObserver);
		void ConstructL();
		
	private:
		MTransmitServerObserver* iObserver;

	public:
		TPositionInfo iRefPosInfo;		
	};
	
	
class CTLbsX3pTransmitLocWaiter: public CActive
	{
	public:
		~CTLbsX3pTransmitLocWaiter();
		static CTLbsX3pTransmitLocWaiter* NewL(MTransmitServerObserver* aObserver);
		void Activate();
		
	protected:
		void RunL();
		virtual TInt RunError(TInt aError);
		void DoCancel();
		
	private:
		CTLbsX3pTransmitLocWaiter(MTransmitServerObserver* aObserver);
		void ConstructL();
		
		
	private:
		MTransmitServerObserver* iObserver;
		
	public:
		TPositionInfo iTransmitPosInfo;		
	};
	
	
		
class CTransmitServerWatcher : public CBase
	{
public:
	IMPORT_C static CTransmitServerWatcher* NewLC(RLbsTransmitPosition& aTransmitter, MTransmitServerObserver* aObserver);
	IMPORT_C ~CTransmitServerWatcher();
	
	
	IMPORT_C void IssueTransmitPosition(const TDesC& aDestinationID, const TUint aTransmitPriority);
	
	IMPORT_C void CancelTransmit();
	
private:
	
	CTransmitServerWatcher(RLbsTransmitPosition& aTransmitter, MTransmitServerObserver* aObserver);
	void ConstructL();
	
private:	
	MTransmitServerObserver* iObserver;
	RLbsTransmitPosition&   iTransmitter;
	
	CTLbsX3pRefLocWaiter* iRefPosWaiter;
	CTLbsX3pTransmitLocWaiter* iTransmitPosWaiter;
	};


#endif // __LBS_GETAFIX_TEST_POS_SERVER_WATCH_H__

