// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CUSTOMAVIPLAYCONTROLLER_H
#define CUSTOMAVIPLAYCONTROLLER_H

#include "aviplaycontroller.h"

class CCustomMMFMessageHolder : public CBase
	{
public:
	static CCustomMMFMessageHolder* NewL(TMMFMessage& aMessage)
		{return new CCustomMMFMessageHolder(aMessage);}
	TMMFMessage iMessage;
private:
	CCustomMMFMessageHolder(TMMFMessage& aMessage): CBase(), iMessage(aMessage){}
	};

class CCustomAviPlayController: public CAviPlayController 
	{
public:
	static CMMFController* NewL() ;
    virtual ~CCustomAviPlayController();
	
	virtual void ResetL();
	virtual void PrimeL(TMMFMessage& aMessage);
	virtual void CustomCommand(TMMFMessage& aMessage);
	
protected:
	CCustomAviPlayController();
	
private:
	void ConstructL();
	void DoMemoryAllocTest1(TMMFMessage& aMessage);
	void DoPrepareMemoryAllocTest2(TMMFMessage& aMessage);
	void DoMemoryAllocTest3(TMMFMessage& aMessage);
	void DoSetPrimetTestEnable(TMMFMessage& aMessage);
	void DoSetPrimetTestFailCount(TMMFMessage& aMessage);

	
private:
	TBool iMemAllocPrimeTestEnabled;
	TInt iMemAllocPrimeFailCount;
	MDataSource* iDataSource;
	MDataSink* iDataSink;
	};

#endif
