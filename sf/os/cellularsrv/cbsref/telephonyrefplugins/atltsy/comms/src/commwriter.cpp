// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CCommWriter.
// 

//user include
#include "commwriter.h"
#include "mcommobserver.h"
#include "commengine.h"

// CCommWriter

CCommWriter::CCommWriter(CCommEngine* aComm, TInt aPriority)
	: CActive(aPriority), iComm(aComm)
	{
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

void CCommWriter::Activate() 
	{ 
	SetActive(); 
	}

TRequestStatus& CCommWriter::StatusRef() 
	{ 
	return iStatus; 
	}
//
// End of file
