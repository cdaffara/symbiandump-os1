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
#include "LbsLasDataSource.h"

#include "LbsLasBtDataSource.h"
#include "LbsLasComDataSource.h"


//-----------------------------------------------------------------------------	
//-----------------------------------------------------------------------------	
CLasDataSource::CLasDataSource(MLasDataSourceObserver* /*aObserver*/)
	{
	}

CLasDataSource* CLasDataSource::NewL(MLasDataSourceObserver* aObserver)
	{
#ifdef __WINS__
	CLasDataSource* source = CLasComDataSource::NewL(aObserver);
#else
	CLasDataSource* source = CLasBtDataSource::NewL(aObserver);
#endif

	return source;
	}
   	
void CLasDataSource::ConstructL()
	{
	}

CLasDataSource::~CLasDataSource()
	{
	}	
	
void CLasDataSource::OpenDataSource()
	{
	}

void CLasDataSource::CloseDataSource()
	{
	}

void CLasDataSource::MonitorDataSource()
	{
	}

void CLasDataSource::CancelMonitor()
	{
	}

//-----------------------------------------------------------------------------	
//-----------------------------------------------------------------------------	
