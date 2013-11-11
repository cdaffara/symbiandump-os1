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
// Comms reader and writer mixin
// 
//

#include "ATSTD.H"
#include "SCOMM.H"

MComm::MComm()
	{
	iCommReader = NULL;
	iCommWriter = NULL;
	}

MComm::~MComm()
	{
    delete iCommReader;
	delete iCommWriter;
	iCommPort.Close();
	iCommServer.Close();
	}

TInt MComm::CommOpen(const TDesC& aDll, const TDesC& aName, TCommAccess aMode)
	{
	TInt err;
	if (err = iCommServer.Connect(), err!=KErrNone)
		return err;
	if (aDll.Length()>0)
		{
		if (err = iCommServer.LoadCommModule(aDll), err!=KErrNone)
			{
			iCommServer.Close();
			return err;
			}
		}
	if (err = iCommPort.Open(iCommServer, aName, ECommExclusive), err!=KErrNone)
		// if any other TSY (or other app) is using comm port, we want to return gracefully
		// with error
		{
		iCommServer.Close();
		return err;
		}
	if (aMode==ECommShared)
		{
		iCommPort.Close();
		if (err = iCommPort.Open(iCommServer, aName, aMode), err!=KErrNone)
			{
			iCommServer.Close();
			return err;
			}
		}
	return KErrNone;
	}

TInt MComm::CommOpen(const TDesC& aName, TCommAccess aMode)
	{
	TInt err;
	if (err = iCommServer.Connect(), err!=KErrNone)
		return err;
	if (err = iCommPort.Open(iCommServer, aName, ECommExclusive), err!=KErrNone)
		// if any other TSY (or other app) is using comm port, we want to return gracefully
		// with error
		{
		iCommServer.Close();
		return err;
		}
	if (aMode==ECommShared)
		{
		iCommPort.Close();
		if (err = iCommPort.Open(iCommServer, aName, aMode), err!=KErrNone)
			{
			iCommServer.Close();
			return err;
			}
		}
	return KErrNone;
	}

void MComm::CommClose()
	{
	iCommReader->Cancel();
	iCommWriter->Cancel();
	iCommPort.Close();
	iCommServer.Close();
	}

void MComm::CommConstructL(TInt aReadPriority, TInt aWritePriority)
	{
	iCommReader = new (ELeave) CCommReader(this, aReadPriority);
	iCommWriter = new (ELeave) CCommWriter(this, aWritePriority);
	};

void MComm::CommWrite(const TDesC8& aDes)
	{
	__ASSERT_ALWAYS(iCommWriter!=NULL, Panic(EATCommand_NotConstructed));
	iCommPort.Write(iCommWriter->StatusRef(), aDes);
	iCommWriter->Activate();
	}

void MComm::CommWriteReady()
	{
	__ASSERT_ALWAYS(iCommWriter!=NULL, Panic(EATCommand_NotConstructed));
	iCommPort.Write(iCommWriter->StatusRef(), TPtrC8(NULL, 0));
	iCommWriter->Activate();
	}

void MComm::CommRead(TDes8& aDes)
	{
	__ASSERT_ALWAYS(iCommReader!=NULL, Panic(EATCommand_NotConstructed));
	iCommPort.Read(iCommReader->StatusRef(), aDes, aDes.Length());
	iCommReader->Activate();
	}

void MComm::CommReadOneOrMore(TDes8& aDes)
	{
	__ASSERT_ALWAYS(iCommReader!=NULL, Panic(EATCommand_NotConstructed));
	iCommPort.ReadOneOrMore(iCommReader->StatusRef(), aDes);
	iCommReader->Activate();
	}

void MComm::CommReadReady()
	{
	__ASSERT_ALWAYS(iCommReader!=NULL, Panic(EATCommand_NotConstructed));
//	TRequestStatus aStatus;
//	iCommPort.Read(aStatus,iBuf,0);
//	User::WaitForRequest(aStatus);
// JoeF 26.11.99. IrComm.Csy can't handle a read into a zero-length buffer and needs fixing. The
// above lines have been removed from the TSY as a temporary workaround only, until a proper fix
// can be implemented in IrComm.Csy
	}

void MComm::CommCancel()
	{
	if (iCommWriter)
		iCommWriter->Cancel();
	if (iCommReader)
		iCommReader->Cancel();
	}

void MComm::CommWriteCancel()
	{
	if (iCommWriter)
		iCommWriter->Cancel();
	}

void MComm::CommReadCancel()
	{
	if (iCommReader)
		iCommReader->Cancel();
	}

//
// CCommWriter
//
CCommWriter::CCommWriter(MComm* aComm, TInt aPriority)
	: CActive(aPriority), iComm(aComm)
	{
	__DECLARE_NAME(_S("CCommWriter"));
	CActiveScheduler::Add(this);
	}

CCommWriter::~CCommWriter()
	{
	Cancel();
	}

void CCommWriter::RunL()
	{
	iComm->CommWriteComplete(iStatus.Int());
	}

void CCommWriter::DoCancel()
	{
	iComm->iCommPort.WriteCancel();
	}

//
// CCommReader
//
CCommReader::CCommReader(MComm* aComm, TInt aPriority)
	: CActive(aPriority), iComm(aComm)
	{
	__DECLARE_NAME(_S("CCommReader"));
	CActiveScheduler::Add(this);
	}

CCommReader::~CCommReader()
	{
	Cancel();
	}

void CCommReader::RunL()
	{
	iComm->CommReadComplete(iStatus.Int());
	}

void CCommReader::DoCancel()
	{
	iComm->iCommPort.ReadCancel();
	}
