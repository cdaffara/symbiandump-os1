/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file ctlbsx3pdummyao.h
*/
#if (!defined __CTLBSX3PDUMMYAO_H__)
#define __CTLBSX3PDUMMYAO_H__

#include <e32base.h>
#include <lbs.h>
//#include <lbsx3p.h>


_LIT(KCT_LbsX3PDummyAO, "CT_LbsX3PDummyAO");


// Async callbacks
class MT_LbsX3PDummyObserver
	{
public:
	virtual void MT_LbsX3PDummyCallbackHandler(TRequestStatus& aStatus) = 0;	// do we need the aStatus...
	};
	
	
class CT_LbsX3PDummyAO: public CActive
	{
public:
	~CT_LbsX3PDummyAO();
	static CT_LbsX3PDummyAO* NewL(MT_LbsX3PDummyObserver* aObserver);

	
protected:
	//	Functions from CActive:
	virtual void DoCancel();
	virtual void RunL();
	virtual TInt RunError(TInt aError);
	
private:
	CT_LbsX3PDummyAO(MT_LbsX3PDummyObserver* aObserver);
	//void CT_LbsX3PDoTransmitPos::ConstructL();
	
	// caller
	MT_LbsX3PDummyObserver* iObserver;
	
	};

#endif // __CTLBSX3PDUMMYAO_H__