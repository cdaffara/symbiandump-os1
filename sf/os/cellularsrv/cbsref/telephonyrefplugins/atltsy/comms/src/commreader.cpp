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
// CCommReader.
// 

//user include
#include "commreader.h"
#include "mcommobserver.h"
#include "commengine.h"

//Class CCommReader

CCommReader::CCommReader(CCommEngine* aComm, TInt aPriority)
	: CActive(aPriority), iComm(aComm)
	{
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

void CCommReader::Activate()
	{
	SetActive();
	}

TRequestStatus& CCommReader::StatusRef()
	{ 
	return iStatus; 
	}
//
// End of file
