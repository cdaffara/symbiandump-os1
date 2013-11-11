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

#ifndef LBSLASDATASOURCE_H
#define LBSLASDATASOURCE_H

//-----------------------------------------------------------------------------
//System
#include <e32base.h>

class MLasDataSourceObserver 
	{
public:
	virtual void HandleData(const TDesC8& aData, TInt aErr) = 0;
	};

//-----------------------------------------------------------------------------
//   CLasDataSource
//-----------------------------------------------------------------------------
class CLasDataSource : public CBase
	{
public:
	static CLasDataSource* NewL(MLasDataSourceObserver* aObserver);
	virtual ~CLasDataSource();

	virtual void OpenDataSource();
	virtual void CloseDataSource();

	virtual void MonitorDataSource();
	virtual void CancelMonitor();
	
protected:
	virtual void ConstructL();
	CLasDataSource(MLasDataSourceObserver* aObserver);
	};


#endif // LBSLASDATASOURCE_H