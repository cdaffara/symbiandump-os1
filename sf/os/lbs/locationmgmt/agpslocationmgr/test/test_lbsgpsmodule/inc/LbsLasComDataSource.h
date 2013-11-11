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

#ifndef LBSLASCOMDATASOURCE_H
#define LBSLASCOMDATASOURCE_H

//************************************************************************************************************
#include "LbsLasDataSource.h"

class CLasComDataReader;

//************************************************************************************************************
//   CLasComDataSource
//************************************************************************************************************
class CLasComDataSource : public CLasDataSource
	{
public:
	static CLasComDataSource* NewL(MLasDataSourceObserver* aObserver);
	~CLasComDataSource();

	void OpenDataSource();
	void CloseDataSource();
	
	void MonitorDataSource();
	void CancelMonitor();
	
protected:
	void ConstructL();
	CLasComDataSource(MLasDataSourceObserver* aObserver);
	
private:
	MLasDataSourceObserver* iObserver;
	CLasComDataReader* iDataReader;
	};


#endif // LBSLASCOMDATASOURCE_H