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
#include "LbsLasComDataSource.h"
#include "LbsLasComDataReader.h"

//-----------------------------------------------------------------------------	
//-----------------------------------------------------------------------------	
CLasComDataSource::CLasComDataSource(MLasDataSourceObserver* aObserver)
	:
	CLasDataSource(aObserver),
	iObserver(aObserver)
	{
	}

CLasComDataSource* CLasComDataSource::NewL(MLasDataSourceObserver* aObserver)
	{
	CLasComDataSource* source = new(ELeave) CLasComDataSource(aObserver);

	CleanupStack::PushL(source);
	source->ConstructL();
	CleanupStack::Pop();

	return source;
	}
   	
void CLasComDataSource::ConstructL()
	{
	iDataReader = CLasComDataReader::NewL(iObserver);
	}

CLasComDataSource::~CLasComDataSource()
	{
	delete iDataReader;
	iDataReader = NULL;
	}	
//-----------------------------------------------------------------------------	
//-----------------------------------------------------------------------------	


//-----------------------------------------------------------------------------	
//-----------------------------------------------------------------------------	
void CLasComDataSource::OpenDataSource()
	{
	iDataReader->OpenDataSource();
	}

void CLasComDataSource::CloseDataSource()
	{
	iDataReader->CloseDataSource();
	}

void CLasComDataSource::MonitorDataSource()
	{
	iDataReader->MonitorDataSource();
	}

void CLasComDataSource::CancelMonitor()
	{
	iDataReader->CancelMonitor();
	}
