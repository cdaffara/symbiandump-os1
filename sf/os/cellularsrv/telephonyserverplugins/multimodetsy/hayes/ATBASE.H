// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// AT Command Base Class Header
// 
//

/**
 @file
 @internalAll 
*/


#ifndef __ATBASE_H__
#define	__ATBASE_H__

#include "ATSTD.H"
#include <et_tsy.h>
#include "SCHAT.H"
/**
@internalComponent
*/
enum TEventSource
		{
		EReadCompletion,
		EWriteCompletion,
		ETimeOutCompletion
		};

/**
@internalTechnology
*/
const TUint KGenericBufferSize=200; // Changed from 100 to 200 hs

class CATParamListEntry : public CBase
/**
@internalTechnology
*/
	{
public:
	CATParamListEntry(const TDesC8& aPtr);
	~CATParamListEntry();
	void Deque();
	static void EntryValL(CATParamListEntry* aEntry,TInt& aValue);
	static TInt EntryValL(CATParamListEntry* aEntry);
public:
	TPtrC8 iResultPtr;
private:
	TDblQueLink iLink;
	friend class CATBase;
	};


class CTelObject;
class CCompleteRelinquish : public CAsyncOneShot
/**
CCompleteRelinquish calls RelinquishOwnershipCompleted() in ETel server so that the
server only closes down the session once the TSY's I/O has finished processing

@internalTechnology
*/
	{
public:
	static CCompleteRelinquish* New(CTelObject* aTelObject);
	~CCompleteRelinquish();
	void SetWhichCompletion(TPanicOccurred aPanicOccurred) {iPanicOccurred = aPanicOccurred;}
protected:
	CCompleteRelinquish(CTelObject* aTelObject);
	virtual void RunL();
private:
	CTelObject* iTelObject;
	TPanicOccurred iPanicOccurred;
	};


class CATIO;
class CATBase : public CBase
/**
@internalTechnology
*/
	{
public:
	void GenericEventSignal(TEventSource aEventSource, TInt aStatus);
	CTelObject* Owner();
	TCallInfoTSY* CallInfo();
	void AppendWildCardChar(TDes8& aString);
protected:
	CATBase(CATIO* aIo,	CTelObject* aTelObject,CPhoneGlobals* aPhoneGlobals);
	~CATBase();
	virtual void EventSignal(TEventSource aEventSource)=0;
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus)=0;
	void ChangeCallStatus(RMobileCall::TMobileCallStatus aCallStatus);
	void ChangeLineStatus(RCall::TStatus aLineStatus);
	void StandardWriteCompletionHandler(TEventSource aSource,TInt aLengthOfPause);
	void Write(const TDesC8& aCommand,TInt aTimeOut = 5);
	void Write(const TDesC8& aCommand,TInt aTimeOut,TInt aValue);
	void Write(const TInt aTimeOut);
	void WriteExpectingResults(const TDesC8& aCommand,TInt aTimeOut);
	void AddStdExpectStrings();
	TInt ValidateExpectString();		// New version which does not leave 13/Sep/01
	void RemoveStdExpectStrings();
	void RemoveUnsolicitedStrings();    // Removes any unsolicited strings received during initialisation
	void AddCmsErrorExpectString();
	void RemoveCmsErrorExpectString();
	void ParseBufferLC(TBool aReportLists=EFalse, TUint aSeparatorChar=',');
	void ParseLineLC(TBool aReportLists=EFalse, TUint aSeparatorChar=',');
	void ParseLC(TBool aReportLists, TUint aSeparatorChar);
	void AddParamL(const TDesC8& aPtr);
	void SetToNotInitialised();
	void RxResultsPushLC();
	static void CleanupRxResults(TAny *aCATBase);	// for TCleanupOperation
protected:
	CCommChatString* iOKExpectString;
	CCommChatString* iErrorExpectString;
	CCommChatString* iCmsExpectString;
	CATIO* iIo;
protected:
	TBuf8<KGenericBufferSize> iTxBuffer;
	TDblQue<CATParamListEntry> iRxResults;
	TPtrC8 iBuffer;
	TTsyReqHandle iReqHandle;
	CPhoneGlobals* iPhoneGlobals;
	CCompleteRelinquish* iComplete;
	CTelObject* iTelObject;
	TCallInfoTSY* iCallInfo;
	};

class CATInit;
class CATSetToOnlineCommandMode;
class CATCommands : public CATBase
/**
@internalTechnology
*/
	{
public:
	void ExecuteCommand(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	void ExecuteCommand(TTsyReqHandle aTsyReqHandle, TAny* aParams,TCallInfoTSY* aCallInfo);
	void CancelCommand(TTsyReqHandle aTsyReqHandle);
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams)=0;
	virtual void Stop(TTsyReqHandle aTsyReqHandle)=0;
protected:
	CATCommands(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	void ConstructL();
	~CATCommands();
	virtual void Complete(TInt aError,TEventSource aSource);
protected:
	CATInit* iInit;
	CATSetToOnlineCommandMode* iATSetToOnlineCommandMode;
	};

#endif
