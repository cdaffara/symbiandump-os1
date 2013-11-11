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
// writer.cpp - all the active reader code
// 
//

#include <cs_port.h>
#include "btcomm.h"
#include "btcommactive.h"


CBTPortWriter *CBTPortWriter::NewL(CBTPortProxy *aParent)
/**
	Create new CBTPortWriter.
	This function creates a new CBTPortWriter active object.
**/
	{
	CBTPortWriter *cc;
	cc=new (ELeave) CBTPortWriter();
	CleanupStack::PushL(cc);
	cc->InitL(aParent);
	CleanupStack::Pop();
	CActiveScheduler::Add(cc);
	return cc;
	}

void CBTPortWriter::InitL(CBTPortProxy *aParent)
	{
	iPortProxy=aParent;
	}

CBTPortWriter::~CBTPortWriter()
/**
	CBTPortWriter destructor.	
**/
	{
	Cancel();
	}

void CBTPortWriter::RunL()
/**
	CBTPortWriter RunL.
	Active status cleared.  We can now call the DoWriteCompleted 
	function on the corresponding port proxy object.
**/
	{
    iPortProxy->DoWriteCompleted(iStatus.Int());
	}

void CBTPortWriter::QueueWrite(const TDesC8& aData)
/**
	CBTPortWriter QueueWrite.
**/
	{
	iPortProxy->iSocket.Write(aData,iStatus);
	SetActive();
	}

void CBTPortWriter::DoCancel()
/**
	CBTPortWriter DoCancel.
	Nothing to cancel.
**/
	{
	iPortProxy->iSocket.CancelWrite();
	}

CBTPortWriter::CBTPortWriter() 
	: CActive (EPriorityStandard)
/**
	CBTPortWriter private c'tor.
**/
	{
	}
