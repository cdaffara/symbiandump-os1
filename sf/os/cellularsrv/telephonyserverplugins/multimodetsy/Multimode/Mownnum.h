// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Deal with GetOwnNumber
// 
//

/**
 @file
 @internalAll 
*/

#ifndef __MOWNNUM_H__
#define __MOWNNUM_H__

#include "ATBASE.H"

class CATOwnNumbers : public CATCommands
	{
public:
	static CATOwnNumbers* NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	~CATOwnNumbers();
//
	void GetInfo(TTsyReqHandle aTsyReqHandle,TDes8* aInfo);
	void GetEntry(TTsyReqHandle aTsyReqHandle,TDes8* aEntry);
	RMobilePhone::TMobileTON NumberTypefromGSMToMM(TUint aTypeOfNumberGSM);
private:
	CATOwnNumbers(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
//
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource, TInt aStatus);
//
	virtual void Complete(TInt aError, TEventSource aSource);
	void ParseResponseL();
	void Info() const;
	TInt Entry(TInt aIndex) const;
private:
	RMobileONStore::TMobileONEntryV1 iONEntry;
	enum {
		KAsynchModem=0,
		KSynchModem=1,
		KPADAccess=2,
		KPacketAccess=3,
		KServiceVoice=4,
		KServiceFax=5
		};
	enum {KItc3_1kHz=0,KItcUdi=1};
private:
	enum {EInfo,EEntry} iCommand;
	enum {
		EATNotInProgress,
		EATImsiWaitForWriteComplete,
		EATImsiReadComplete,
		EATWaitForStopState,
		EATCompleted,
		EATNotSupported
		} iState;
		
	TAny* iResultPtr;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg* iInfoPckg;
	RMobileONStore::TMobileONEntryV1Pckg* iEntryPckg;
	TInt iResultIndex;
	CMobilePhoneONList* iOwnNumbers;
	};

#endif
