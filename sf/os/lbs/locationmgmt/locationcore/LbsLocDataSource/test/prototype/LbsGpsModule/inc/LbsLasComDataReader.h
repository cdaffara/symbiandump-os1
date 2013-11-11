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

#ifndef LBSLASCOMDATAREADER_H
#define LBSLASCOMDATAREADER_H

//************************************************************************************************************
//System
#include <e32base.h>
#include <c32comm.h>

class MLasDataSourceObserver;

const TInt KMaxComDataRead = 200;

//************************************************************************************************************
//   CLasComDataReader
//************************************************************************************************************
class CLasComDataReader : public CActive
	{
public:
	static CLasComDataReader* NewL(MLasDataSourceObserver* aObserver);
	void ConstructL();
	~CLasComDataReader();

	// From CActive
	virtual void RunL();
	virtual void DoCancel();
	
	TInt OpenDataSource();
	void CloseDataSource();
	void MonitorDataSource();
	void CancelMonitor();

private:
	CLasComDataReader(MLasDataSourceObserver* aObserver);

	TInt ConfigPort(RComm& aCommPort);
	
private:
	enum TConnectState {EStateUnknown, EStateOnLine, EStateError};

	RCommServ     iCommServ;
	RComm         iCommPort;

	TConnectState iConnectState;

	MLasDataSourceObserver* iObserver;
	TBool iMonitoring;

	TBuf8<KMaxComDataRead> iData;
	TInt iErrorCount;
	};


#endif // LBSLASCOMDATAREADER_H
