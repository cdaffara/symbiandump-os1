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
//

/**
 @file
 @internalComponent 
*/


#ifndef __ATBASE_H
#define	__ATBASE_H

#include "Te_LoopBackSCHAT.H"
#include "Te_LoopBackATSTD.H"

// Event sources which can be reported to the event signallers.
enum TEventSource
		{
		EReadCompletion,		//< A Read Has Completed
		EWriteCompletion,		//< A Write Has Completed
		ETimeOutCompletion,		//< A Timeout has occurred.
		ERestartSignal			//< A Restart Signal has been received
		};

const TUint KTxBufferSize=400;
const TUint KRxBufferSize=400;

/**
 * This class is used to help manage data from the modem.  Though a linked list of objects of
 * this class contained in the CATBase definition, this class is apparently unused in the 
 * regression test harness.  There is never a new called to create one of these objects in 
 * code that is executed by the test harness.
 */
class CATParamListEntry : public CBase
	{
public:
	CATParamListEntry(const TPtrC8& aPtr);
	~CATParamListEntry();
	void Deque();
public:
	TPtrC8 iResultPtr;			//< Ptr into received buffer (not used in regression test harness)
private:
	TDblQueLink iLink;			//< Doubly linked list (apparently not used in regression test harness) 
	friend class CATBase;
	};

class CATIO;
class CCommChatString;
/**
 * This class inherits from CBase and is the super-class of CATScriptEng (and thus all tests).
 * This is the lowest level Regression Test Harness class for emulator side functions.
 */
class CATBase : public CBase
	{
public:
	void GenericEventSignal(TEventSource aEventSource, TInt aStatus);
protected:
	static CATBase* NewL();
	CATBase();
	void ConstructL();
	~CATBase();

	// Called by the Generic Event Signaller when no error has been detected to return event to 
	// the inheriting class.
	virtual void EventSignal(TEventSource aEventSource)=0;

	// Called by the Generic Event Signaller when an error has been detected to return event to
	// the inheriting class.  Complete methods typically shut down the Regression Test Harness.
	virtual void Complete(TInt aError)=0;
	void StandardWriteCompletionHandler(TEventSource aSource,TInt aLengthOfPause);
	void Write(const TPtrC8& aCommand,TInt aTimeOut = 5);
	void Write(const TPtrC8& aCommand,TInt aTimeOut,TInt aValue);
	void WriteExpectingResults(const TPtrC8& aCommand,TInt aTimeOut);
	void AddStdExpectStrings();
	void ValidateExpectStringL();
	void RemoveStdExpectStrings();
	void ParseBufferLC();
protected:
	CCommChatString* iOKExpectString;		//< Ptr to standard "OK" chat string, not currently initialized.
	CCommChatString* iErrorExpectString;	//< Ptr to stander "ERROR" chat string, not currently initialized.
	CATIO* iIo;								//< Ptr to CATIO object, new'ed in the ConstructL method.
protected:
	TBuf8<KTxBufferSize> iTxBuffer;			//< Used to write commands back to tsy from the scripts.
	TDblQue<CATParamListEntry> iRxResults;	//< Doubly Linked List storing rx results (apparently not used).
	TPtrC8 iBuffer;							//< Not currently used by regression test harness.
	TInputBufferMark iBufferMarker;			//< Used as buffer marker in receive (from gsmtsy) buffer.
	};

#endif
