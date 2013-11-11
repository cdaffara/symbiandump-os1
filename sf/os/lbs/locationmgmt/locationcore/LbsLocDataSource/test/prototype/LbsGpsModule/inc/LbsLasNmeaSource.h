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
// LbsNMEASource.h
// 
//

#ifndef LBSLASNMEASOURCE_H
#define LBSLASNMEASOURCE_H

#include <e32base.h>

#include "LbsLasDataSource.h"

const TInt KLbsMaxNmeaSentence = 200;
const TInt KMaxNmeaFields      = 40;

typedef TPtrC8 CLasNmeaFieldsArray;

class MLasNmeaSourceObserver
	{
public:
	//virtual void HandleNmeaSentence(const TDesC8& aData) = 0;
	virtual void HandleNmeaSentence(const TPtrC8* aNmeaFields, TInt aNumFields) = 0;
	};

class CLasNmeaSource : public CBase, public MLasDataSourceObserver
	{
public:
	static CLasNmeaSource* NewL(MLasNmeaSourceObserver* aObserver);
   ~CLasNmeaSource();
	   
	void OpenNmeaSource();
	void CloseNmeaSource();
	
	void MonitorNmeaSentence();
	void CancelMonitor();

	void HandleData(const TDesC8& aData, TInt aErr);
	
private:
   CLasNmeaSource(MLasNmeaSourceObserver* aObserver);
   void ConstructL();  
   
   void AppendNmeaFieldChar(TUint aChar);
	void StoreNmeaString(const TDesC8& aData);
	
private:
	enum TNmeaReadState {ENmeaWantStart, ENmeaWantTerminator, ENmeaWantCheck1, ENmeaWantCheck2};

private:
	MLasNmeaSourceObserver* iObserver;
	TBool iMonitoring;

	CLasDataSource* iDataSource;

	TNmeaReadState iNmeaReadState;

	TBuf8<KLbsMaxNmeaSentence> iNmeaSentence;
	TInt iNmeaCheckSum;
	TInt iNmeaCheckValue;
	
	TPtrC8 iNmeaFields[KMaxNmeaFields];
	TInt iNmeaNumFields;
	TInt iNmeaFieldStart;
	TInt iNmeaFieldLength;
	};

#endif //LBSLASNMEASOURCE_H