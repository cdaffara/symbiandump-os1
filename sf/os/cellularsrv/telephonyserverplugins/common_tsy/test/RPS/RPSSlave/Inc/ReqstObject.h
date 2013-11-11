// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __REQSTOBJECT_H
#define __REQSTOBJECT_H

#include <e32base.h>
#include <e32cmn.h>
#include "EtelMngr.h"

const TInt KMaxTelNumberSize = 255;

class MObserver;
//
// MSubject
// Mixin class offering Observable subject interface
//
class MSubject
	{
public:
	virtual void AttachL(MObserver* aObserver)=0;
	virtual void DetachL(MObserver* aObserver)=0;
	virtual void NotifyL()=0;
	virtual void Abort()=0;
	};


//
// CReqstObjectBase.
// Abstract base class for the Request Active Objects.
// It has MSubject behaviour.
//
class CReqstObjectBase : public CActive, public MSubject
	{
public:
	virtual ~CReqstObjectBase();
 
 	virtual void DoCancel()=0;	// Must be tailored by derived 
 	
 	virtual void StartL()=0;  	// Must be tailored by derived 
 	
 	// From MSubject
 	void AttachL(MObserver* aObserver);
	void DetachL(MObserver* aObserver);
	void NotifyL();
	void Abort();

protected:
	CReqstObjectBase(CEtelMngr&	aEtel, TEtelLine aLine);
	
	virtual void RunL();	// Can be tailored by derived

public:
	CEtelMngr&		iEtel;	// Not owned
	TEtelLine 		iLine;
	
protected:
	// Array of Observers interested in us
	// (in practice there's only likely to be one)
	RPointerArray<MObserver> iObservArray;
	};
	
	
//
// CCallNumber - Concrete Request AO
// Tells Etel to call the specified tel. number.
// Completes when the call becomes connected.
//
class CCallNumber : public CReqstObjectBase
	{
public:
	static CCallNumber* NewL(CEtelMngr& aEtel, TEtelLine aLine, const TDesC8& aTelNum);
	~CCallNumber();
	void DoCancel();
	void StartL();
	
private:
	CCallNumber(CEtelMngr& aEtel, TEtelLine aLine);
	void ConstructL(const TDesC8& aTelNum);
	
private:
	TBuf<KMaxTelNumberSize> iTelNumber;
	};
	
class MCallNotifyObserver
	{
public:
	virtual	void HlprObjAborted() = 0;
	virtual void HlprObjCompletedL() = 0;
	};
//
// CCallNumberNotifyStatusChange - Concrete Request AO
// Tells Etel to call the specified tel. number.
// Completes when the line used to make the call reaches the state specified.
//
class CCallNotifyStatusChange;
class CCallNumberNotifyStatusChange : public CReqstObjectBase, public MCallNotifyObserver
	{
public:
	static CCallNumberNotifyStatusChange* NewL(CEtelMngr& aEtel, TEtelLine aLine, const TDesC8& aTelNum, RCall::TStatus aDesiredStatus);
	~CCallNumberNotifyStatusChange();
	
	void DoCancel();
	void StartL();
	
	//from MCallNotifyObserver
	virtual void HlprObjAborted();
	virtual void HlprObjCompletedL();
	
private:
	CCallNumberNotifyStatusChange(CEtelMngr& aEtel, TEtelLine aLine, RCall::TStatus aDesiredStatus);
	void RunL();
	void ConstructL(const TDesC8& aTelNum);
	
private:
	TBuf<KMaxTelNumberSize> iTelNumber;
	RCall::TStatus 	iDesiredStatus;	
	CCallNotifyStatusChange*  iNotifyStatusChangeHlprObj;
	};
	
//
// CCallNotifyStatusChange
// Completes when the line used to make the call reaches the state specified.
//

class CCallNotifyStatusChange : public CActive
	{
public:
	static CCallNotifyStatusChange* NewL(RMobileCall& aCall, RCall::TStatus aDesiredStatus, MCallNotifyObserver& aRqstObj);
	~CCallNotifyStatusChange();
	void DoCancel();
	void StartL();
	
private:
	CCallNotifyStatusChange(RMobileCall& aCall, RCall::TStatus aDesiredStatus, MCallNotifyObserver& aRqstObj);
	void RunL();
	void ConstructL();
	
private:
	RCall::TStatus 					iDesiredStatus;
	RCall& 					iSession;  	// Not owned
	MCallNotifyObserver&	iRqstObj;  	// Not owned
public:
	RCall::TStatus 		iCallStatus;
	};
	
//
// CLineNotifyStatusChange
// Completes when the line used to make the call reaches the state specified.
//

//NOTE: This class is basically the same as CCallNotifyStatusChange and therefore a template class
//should be used to combine both together

class CLineNotifyStatusChange : public CActive
	{
public:
	static CLineNotifyStatusChange* NewL(RMobileLine& aLine, RCall::TStatus aDesiredStatus, MCallNotifyObserver& aRqstObj);
	~CLineNotifyStatusChange();
	void DoCancel();
	void StartL();
	
private:
	CLineNotifyStatusChange(RMobileLine& aCall, RCall::TStatus aDesiredStatus, MCallNotifyObserver& aRqstObj);
	void RunL();
	void ConstructL();
	
private:
	RCall::TStatus 					iDesiredStatus;
	RLine& 					iSession;  	// Not owned
	MCallNotifyObserver&	iRqstObj;  	// Not owned
public:
	RCall::TStatus 		iCallStatus;
	};

//
// CHangUp - Concrete Request AO
// This performs a syncronous delay (which blocks), then calls HangUp on call1.
// Note the HangUp command only works when a Dial has been used and the call is in connected state.
//
class CHangUp : public CReqstObjectBase
	{
public:
	static CHangUp* NewL(CEtelMngr& aEtel, TEtelLine aLine, const TDesC8& aDelay);
	~CHangUp();
	void DoCancel();
	void StartL();
	
private:
	CHangUp(CEtelMngr& aEtel, TEtelLine aLine);
	void ConstructL(const TDesC8& aDelay);
	
private:
	TTimeIntervalSeconds 	iDelay;
	};
	
//
// CHold - Concrete Request AO
// This performs a syncronous delay (which blocks), then calls Hold on call1.
// Note the Hold command only works when the call is in connected state.
// There is no gaurantee the call supports or is in a state to be put on hold.
//
class CHold : public CReqstObjectBase
	{
public:
	static CHold* NewL(CEtelMngr& aEtel, TEtelLine aLine, const TDesC8& aDelay);
	~CHold();
	void DoCancel();
	void StartL();
	
private:
	CHold(CEtelMngr& aEtel, TEtelLine aLine);
	void ConstructL(const TDesC8& aDelay);
	
private:
	TTimeIntervalSeconds 	iDelay;
	};	

//
// CResume - Concrete Request AO
// This performs a syncronous delay (which blocks), then calls Resume on call1.
// The call is expected to be in a hold state.
// There is no gaurantee the call supports or is in a state to be resumed.
//
class CResume : public CReqstObjectBase
	{
public:
	static CResume* NewL(CEtelMngr& aEtel, TEtelLine aLine, const TDesC8& aDelay);
	~CResume();
	void DoCancel();
	void StartL();
	
private:
	CResume(CEtelMngr& aEtel, TEtelLine aLine);
	void ConstructL(const TDesC8& aDelay);
	
private:
	TTimeIntervalSeconds 	iDelay;
	};
	
//
// CDialCancel - Concrete Request AO
// This performs an async delay, then calls DialCancel on call1.
// Note this should only be used after a Dial command has been issued and the call is in
// either dialing or connecting state
//
class CDialCancel : public CReqstObjectBase
	{
public:
	static CDialCancel* NewL(CEtelMngr& aEtel, TEtelLine aLine, const TDesC8& aDelay);
	~CDialCancel();
	void DoCancel();
	void StartL();
	
private:
	CDialCancel(CEtelMngr& aEtel, TEtelLine aLine);
	void RunL();
	void ConstructL(const TDesC8& aDelay);
	
private:
	TTimeIntervalSeconds 	iDelay;
	RTimer 					iTimer;
	};
	
//
// CAnswerNextCall - Concrete Request AO
// Tells Etel to get ready to answer the next incoming call on line1
//	
class CAnswerNextCall : public CReqstObjectBase
	{
public:
	static CAnswerNextCall* NewL(CEtelMngr& aEtel, TEtelLine aLine);
	~CAnswerNextCall();
	void DoCancel();
	void StartL();
	
private:
	CAnswerNextCall(CEtelMngr& aEtel, TEtelLine aLine);
	void ConstructL();
	};
	
class CLineNotifyStatusChangeWrapper : public CReqstObjectBase, public MCallNotifyObserver
	{
public:
	static CLineNotifyStatusChangeWrapper* NewL(CEtelMngr& aEtel, TEtelLine aLine, RCall::TStatus aDesiredStatus);
	~CLineNotifyStatusChangeWrapper();
	
	void DoCancel();
	void StartL();
	
	//from MCallNotifyObserver
	virtual void HlprObjAborted();
	virtual void HlprObjCompletedL();
	
private:
	CLineNotifyStatusChangeWrapper(CEtelMngr& aEtel, TEtelLine aLine, RCall::TStatus aDesiredStatus);
	void RunL();
	void ConstructL();
	
private:
	RCall::TStatus 	iDesiredStatus;	
	CLineNotifyStatusChange*  iNotifyStatusChangeHlprObj;
	};
	
//
// CSendDataBackOnDataPort - Concrete Request AO
// Sends data on the Data line.
//
class CSendDataBackOnDataPort : public CReqstObjectBase
	{
public:
	static CSendDataBackOnDataPort* NewL(CEtelMngr& aEtel, const TDesC8& aDataToSend);
	~CSendDataBackOnDataPort();
	void DoCancel();
	void StartL();
	
private:
	CSendDataBackOnDataPort(CEtelMngr& aEtel);
	void ConstructL(const TDesC8& aDataToSend);
	
private:
	RBuf8	iDataToSend;
	};
#endif
