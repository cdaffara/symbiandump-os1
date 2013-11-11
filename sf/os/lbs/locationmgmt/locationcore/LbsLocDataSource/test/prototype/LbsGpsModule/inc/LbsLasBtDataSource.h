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

#ifndef LBSLASBTDATASOURCE_H
#define LBSLASBTDATASOURCE_H

//************************************************************************************************************
#include "LbsLasDataSource.h"

class CLasBtDataReader;


//************************************************************************************************************
//   CLasBtDataSource
//************************************************************************************************************
class CLasBtDataSource : public CLasDataSource
	{
public:
	static CLasBtDataSource* NewL(MLasDataSourceObserver* aObserver);
	~CLasBtDataSource();

	void OpenDataSource();
	void CloseDataSource();
	
	void MonitorDataSource();
	void CancelMonitor();
	
protected:
	void ConstructL();
	CLasBtDataSource(MLasDataSourceObserver* aObserver);
	
private:
	MLasDataSourceObserver* iObserver;
	CLasBtDataReader* iDataReader;
	};


#endif // LBSLASBTDATASOURCE_H