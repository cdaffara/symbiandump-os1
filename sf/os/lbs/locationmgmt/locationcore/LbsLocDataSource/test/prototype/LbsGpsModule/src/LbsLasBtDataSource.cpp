// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "LbsLasCommon.h"
#include "LbsLasBtDataSource.h"
#include "LbsLasBtDataReader.h"

//-----------------------------------------------------------------------------	
//-----------------------------------------------------------------------------	
CLasBtDataSource::CLasBtDataSource(MLasDataSourceObserver* aObserver)
	:
	CLasDataSource(aObserver),
	iObserver(aObserver)
	{
	}

CLasBtDataSource* CLasBtDataSource::NewL(MLasDataSourceObserver* aObserver)
	{
	CLasBtDataSource* source = new(ELeave) CLasBtDataSource(aObserver);

	CleanupStack::PushL(source);
	source->ConstructL();
	CleanupStack::Pop();

	return source;
	}
   	
void CLasBtDataSource::ConstructL()
	{
	iDataReader = CLasBtDataReader::NewL(iObserver);
	}

CLasBtDataSource::~CLasBtDataSource()
	{
	delete iDataReader;
	iDataReader = NULL;
	}	
//-----------------------------------------------------------------------------	
//-----------------------------------------------------------------------------	


//-----------------------------------------------------------------------------	
//-----------------------------------------------------------------------------	
void CLasBtDataSource::OpenDataSource()
	{
	iDataReader->OpenDataSource();
	}

void CLasBtDataSource::CloseDataSource()
	{
	iDataReader->CloseDataSource();
	}

void CLasBtDataSource::MonitorDataSource()
	{
	iDataReader->MonitorDataSource();
	}

void CLasBtDataSource::CancelMonitor()
	{
	iDataReader->CancelMonitor();
	}
