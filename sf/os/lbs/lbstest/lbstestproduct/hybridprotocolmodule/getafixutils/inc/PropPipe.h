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
// PropPipe.h
//
//
//
/** 
 @file
 @internalTechnology
 @test
*/

#ifndef __LBS_GETAFIX_PROP_PIPE_H__
#define __LBS_GETAFIX_PROP_PIPE_H__

#include <e32base.h>
#include <e32std.h>
#include <e32property.h>

#include <lbs/test/propwatcher.h>
#include <lbs/test/memorymanager.h>
//-------------------------------------------------------------------------------------------------

class CPropPipeBase : public CBase
	{
private:
	class CPropPipeBaseTimeOut : public CTimer
		{
	public:
		static CPropPipeBaseTimeOut* NewL(CPropPipeBase& aParent);
	private:
		CPropPipeBaseTimeOut(CPropPipeBase& aParent);
		void RunL();
	private:
		CPropPipeBase& iParent;
		};

	class CPropPipeBaseWatcher : public CActive
		{
	public:
		CPropPipeBaseWatcher(CPropPipeBase& aParent, RProperty& aPropFlow);
		void Watch();
		void CompleteRequest();
		
	private:
		void RunL();
		void DoCancel();
	private: 
		CPropPipeBase& iParent;
		RProperty&	iPropFlow;
		};
public:
	virtual ~CPropPipeBase();

protected:
	CPropPipeBase();
	void BaseConstructL(TUint32 aKey, TUint32 aKeyFlowCtrl);	
	
	void  WriteL(const TDesC8& aBuf, TInt32 aTimeout);
	void  ReadL (TDes8& aBuf, TInt32 aTimeout);
	void  ReadL (HBufC8* aBuf, TInt32 aTimeout, TRequestStatus& aStatus);
	void CancelWait();

	RProperty& FlowCtrl() { return iPropFlow; };
		
	enum TFlowControl { EReadyToRead, EReadyToWrite };	
	TBool WaitForStateL (TFlowControl aDesiredState, TInt32 aTimeout);
	
	virtual void SendResponse();
	virtual void SendTimeOut();
	
private:
	void StateChangedL();
	void TimedOutL();
private:
	TInt  CreateAndAttachKey(TUint32 aKey, TInt aAttr, RProperty& aProp, TBool& aOwnedFlag);
	
private:
	RProperty   iPropPipe;
	TUint32		iPipeUid;
	TBool		iPropPipeOwned;

	RProperty	iPropFlow;
	TUint32		iFlowUid;	
	TBool		iPropFlowOwned;
	CPropPipeBaseTimeOut* iTimeOut;
	CPropPipeBaseWatcher* iWatcher;
	HBufC8* iReadBuf;
	};

//-------------------------------------------------------------------------------------------------

class CReadPropPipe : public CPropPipeBase
	{
public:
	IMPORT_C static CReadPropPipe* NewL(TUint32 aKey, TUint32 aKeyFlowCtrl);
	IMPORT_C virtual ~CReadPropPipe();

	IMPORT_C CReadPropPipe& operator>>(TDes8& aTgt);
	IMPORT_C CReadPropPipe& operator>>(TDes16& aTgt);
	IMPORT_C CReadPropPipe& operator>>(TInt32& aTgt);
	
	IMPORT_C void RefreshL(TInt32 aTimeout);
	IMPORT_C void RefreshL(TInt32 aTimeout, TRequestStatus& aStatus);
	IMPORT_C void CancelRefresh();
	CMemoryManager* iMemoryManager;
protected:
	void ConstructL(TUint32 aKey, TUint32 aKeyFlowCtrl, TInt32 aBufSize);
	virtual void SendResponse();
	virtual void SendTimeOut();
	
private:
	HBufC8*		iBuf;
	TInt32		iMark;
	
	TRequestStatus* iClientStatus;
	};

//-------------------------------------------------------------------------------------------------

class CNotifyReadPropPipe;
	
class MReadPropPipeObserver
	{
public:	
	virtual void OnReadyToReadL(CReadPropPipe& aReadPropPipe) = 0;
	};

class CNotifyReadPropPipe : public CReadPropPipe, protected MPropWatcherObserver
	{
public:
	IMPORT_C static CNotifyReadPropPipe* NewL(TUint32 aKey, TUint32 aKeyFlowCtrl, MReadPropPipeObserver& aObs);
	IMPORT_C virtual ~CNotifyReadPropPipe();

protected:
	CNotifyReadPropPipe(MReadPropPipeObserver& aObserver);
	void ConstructL(TUint32 aKey, TUint32 aKeyFlowCtrl, TInt32 aBufSize);
	
protected: // From MPropWatcherObserver
	virtual void OnChange(RProperty& aProperty);
	
private:
	MReadPropPipeObserver& iObserver;
	CPropWatcher*  		   iPropWatch;	
	
	};
	
//-------------------------------------------------------------------------------------------------

class CWritePropPipe : public CPropPipeBase, public MPropWatcherObserver
	{
public:
	IMPORT_C static CWritePropPipe* NewL(TUint32 aKey, TUint32 aKeyFlowCtrl);
	IMPORT_C virtual ~CWritePropPipe();
	
	IMPORT_C CWritePropPipe& operator<<(const TDesC8& aSrc);
	IMPORT_C CWritePropPipe& operator<<(const TDesC16& aSrc);
	IMPORT_C CWritePropPipe& operator<<(const TInt32& aSrc);
	
	IMPORT_C void CommitL(TInt32 aTimeout);
	IMPORT_C void CommitAndWaitL(TInt32 aTimeout);
	IMPORT_C void ResetL();
	
protected: // From MPropWatcherObserver
	virtual void OnChange(RProperty& aProperty);
	
protected:
	virtual void CommitDataL(TInt32 aTimeout);
	
private:
	void ConstructL(TUint32 aKey, TUint32 aKeyFlowCtrl, TInt aBufSize);
	
private:
	HBufC8*		iBuf;
	TInt32		iBufSize;
	
	CPropWatcher*	iPropWatch;
	TBool			iReadyToWrite;
	TInt			iNumberOfOutstandingMsgs;
	};
	
#endif // __LBS_GETAFIX_PROP_PIPE_H__	

