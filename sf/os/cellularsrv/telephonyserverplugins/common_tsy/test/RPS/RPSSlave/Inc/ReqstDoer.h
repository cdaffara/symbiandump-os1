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

#ifndef __REQSTDOER_H
#define __REQSTDOER_H

#include <e32base.h>
#include <badesca.h>
#include "EtelMngr.h"


class MSubject;
class CReqstObjectBase;
class CEtelMngr;
class MReqstObserver;
//
// MObserver interface class.  Used to callback the Doer objects from the CReqstObjectBase objects
//
class MObserver
	{
public:
	virtual void UpdateL(MSubject* aSubject) =0;
	
	virtual void Abort() =0;
	};
	
//used to notify the ReqstObserver of completing doer requests
class MReqstProcesserSubject
	{
public:
	virtual void RegisterReqstObserver(MReqstObserver* aObserver)=0;
	virtual void DeRegisterReqstObserver()=0;
	};

	
	
//
// CReqstDoer.
// This class contains the algorithm for executing a reqst
//
class CReqstDoer : public CBase, public MObserver, public MReqstProcesserSubject
	{
public:
	virtual ~CReqstDoer();
	
	virtual void DoReqstL();
 
	virtual void UpdateL(MSubject* aSubject);
	
	virtual void Abort();
	
	//from MReqstProcesserSubject
	virtual void RegisterReqstObserver(MReqstObserver* aObserver);
	virtual void DeRegisterReqstObserver();
	
protected:
    CReqstDoer();
	CReqstDoer(MReqstObserver& aReqstObserver);	
	void RegRqstObjL(CReqstObjectBase* aRqstObj);
	TEtelLine GetLine(TDesC8& aLine);

private:
	MReqstObserver* iReqstObserver;
	RPointerArray<CReqstObjectBase> iReqstObjArray;
	
	};

/*
These are the concrete Doer objects which map to the corresponding ReqstObjects
New Doer objects (and any new corresponding ReqstObjects) will need to be created
as and when new request types become necessary.
*/

class CRpsMsg;
// class CCallNumberDoer
class CCallNumberDoer : public CReqstDoer
	{
public:
	static CCallNumberDoer* NewL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	~CCallNumberDoer();

protected:
	CCallNumberDoer();
private:
	void ConstructL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	};
	
// class CHangUpDoer
class CHangUpDoer : public CReqstDoer
	{
public:
	static CHangUpDoer* NewL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	~CHangUpDoer();
	
protected:
	CHangUpDoer();
	
private:
	void ConstructL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	};
	
// class CHoldDoer
class CHoldDoer : public CReqstDoer
	{
public:
	static CHoldDoer* NewL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	~CHoldDoer();

protected:
	CHoldDoer();
		
private:
	void ConstructL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	};
	
// class CResumeDoer
class CResumeDoer : public CReqstDoer
	{
public:
	static CResumeDoer* NewL( CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	~CResumeDoer();
	
protected:
	CResumeDoer();
	
private:
	void ConstructL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	};
	
// class CCallNumberHangUpDoer
class CCallNumberHangUpDoer : public CReqstDoer
	{
public:
	static CCallNumberHangUpDoer* NewL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	~CCallNumberHangUpDoer();
	
protected:
	CCallNumberHangUpDoer();
	
private:
	void ConstructL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	};

// class CCallNumberHangUpBeforeConnectDoer
class CCallNumberHangUpBeforeConnectDoer : public CReqstDoer
	{
public:
	static CCallNumberHangUpBeforeConnectDoer* NewL( CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	~CCallNumberHangUpBeforeConnectDoer();
	
protected:
	CCallNumberHangUpBeforeConnectDoer();
	
private:
	void ConstructL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	};

// class CCallNumberHangUpAfterConnectDoer
class CCallNumberHangUpAfterConnectDoer : public CReqstDoer
	{
public:
	static CCallNumberHangUpAfterConnectDoer* NewL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	~CCallNumberHangUpAfterConnectDoer();
	
protected:
	CCallNumberHangUpAfterConnectDoer();
	
private:
	void ConstructL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	};
	
// class CAnswerNextCallDoer
class CAnswerNextCallDoer : public CReqstDoer
	{
public:
	static CAnswerNextCallDoer* NewL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	~CAnswerNextCallDoer();
	
protected:
	CAnswerNextCallDoer();
	
private:
	void ConstructL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	};
	
	
// class CAnswerNextCallHangUpDoer
class CAnswerNextCallHangUpDoer : public CReqstDoer
	{
public:
	static CAnswerNextCallHangUpDoer* NewL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	~CAnswerNextCallHangUpDoer();

protected:
	CAnswerNextCallHangUpDoer();
		
private:
	void ConstructL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	};
	

// class CAnswerNextCallHoldDoer
class CAnswerNextCallHoldDoer : public CReqstDoer
	{
public:
	static CAnswerNextCallHoldDoer* NewL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	~CAnswerNextCallHoldDoer();
	
protected:
	CAnswerNextCallHoldDoer();
	
private:
	void ConstructL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	};
	
// class CCallNumberHangUpBeforeConnectDoer
class CHangUpNextCallBeforeConnectDoer : public CReqstDoer
	{
public:
	static CHangUpNextCallBeforeConnectDoer* NewL( CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	~CHangUpNextCallBeforeConnectDoer();
	
protected:
	CHangUpNextCallBeforeConnectDoer();
	
private:
	void ConstructL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	};
	
// class CSendDataBackOnDataPortDoer
class CSendDataBackOnDataPortDoer : public CReqstDoer
	{
public:
	static CSendDataBackOnDataPortDoer* NewL( CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	~CSendDataBackOnDataPortDoer();
	
protected:
	CSendDataBackOnDataPortDoer();
	
private:
	void ConstructL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	};

// class CCleanupUpDoer
class CCleanupUpDoer : public CReqstDoer
	{
public:
	static CCleanupUpDoer* NewL( CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	~CCleanupUpDoer();
	
protected:
	CCleanupUpDoer();
	
private:
	void ConstructL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg);
	};


#endif
