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
// This file contains the class definition used by the Modem emulation portion of the Etel
// regression test harness code to interface ports to the communications server.  Classes 
// MComm, CCommReader, and CCommWriter are defined here.
// 
//

/**
 @file
 @internalComponent
*/
#ifndef _MCOMM_H_
#define _MCOMM_H_

#include <e32std.h>
#include <e32base.h>
#include <c32comm.h>

class CCommWriter;
class CCommReader;

/**
 * This class abstracts the port portion of the regression test harness.  It uses the
 * communications server and serial port definitions.  Though this class starts with
 * an M prefix, it is NOT a mixin class, as it contains member data and has substansial
 * method functions.
 */
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
	// Invoked by the RunL of the CCommReader active object for Read completion
	virtual void CommReadComplete(TInt aStatus)=0;
	// Invoked by the RunL of the CCommWriter active object for Write completion
	virtual void CommWriteComplete(TInt aStatus)=0;
protected:
	friend class CCommWriter;
	friend class CCommReader;
	RCommServ iCommServer;			//< Handle for the Communications Server
	RComm iCommPort;				//< Handle for a Serial Port
	CCommReader* iCommReader;		//< Pointer to the Comm Reader associated with this xxxx
	CCommWriter* iCommWriter;		//< Pointer to the Comm Writer associated with this xxxx
	TBuf8<1> iBuf;					//< A single byte buffer used during intialization read
	};

/**
 *  A CActive object that is used for reading data from the ports in the regression test harness.
 */
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
	MComm *iComm;					//< Pointer to the MComm class associated with this reader.
	};

/**
 *  A CActive object that is used for writing data from the ports in the regression test harness.
 */
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
	MComm *iComm;		//< Pointer to the MComm class associated with this writer.
	};

#endif
