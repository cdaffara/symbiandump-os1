// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalAll 
*/


#ifndef __CSIMSERCOMM_H__
#define __CSIMSERCOMM_H__

#include <e32std.h>
#include <e32base.h>
#include <c32comm.h>

class CCommWriter;
class CCommReader;

class MComm
	{
protected:
	MComm();
	virtual ~MComm();
	void CommConstructL(TInt aReadPriority, TInt aWritePriority);
	 TInt CommOpen(const TDesC& aName, TCommAccess aAccess);
	 TInt CommOpen(const TDesC& aDll, const TDesC& aName, TCommAccess aAccess);
	 void CommClose();
	 void CommCancel();
	 void CommWrite(const TDesC8& aDes);
	 void CommWriteReady();
	 void CommWriteCancel();
	 void CommRead(TDes8& aDes);
	 void CommReadOneOrMore(TDes8& aDes);
	 void CommReadReady();
	 void CommReadCancel();
	virtual void CommReadComplete(TInt aStatus)=0;
	virtual void CommWriteComplete(TInt aStatus)=0;
protected:
	friend class CCommWriter;
	friend class CCommReader;
	RCommServ iCommServer;
	RComm iCommPort;
	CCommReader* iCommReader;
	CCommWriter* iCommWriter;
	TBuf8<1> iBuf;
	};

class CCommReader : public CActive
	{
public:
	CCommReader(MComm* aComm, TInt aPriority);
	virtual ~CCommReader();
	inline void Activate() { SetActive(); }
	inline TRequestStatus& StatusRef() { return iStatus; }
protected:
	virtual void RunL();
	virtual void DoCancel();
private:
	MComm *iComm;
	};

class CCommWriter : public CActive
	{
public:
	CCommWriter(MComm* aComm, TInt aPriority);
	virtual ~CCommWriter();
	inline void Activate() { SetActive(); }
	inline TRequestStatus& StatusRef() { return iStatus; }
protected:
	virtual void RunL();
	virtual void DoCancel();
private:
	MComm *iComm;
	};

#endif
