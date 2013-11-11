// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Defines the interface to the BCA.
// 
//

/**
 @file 
*/

#ifndef BCACONTROLLER_H
#define BCACONTROLLER_H

#include <e32base.h>
#include <c32comm.h>
#include <networking/bca2.h>
#include <networking/bca2factory.h>
#include "RawIP2Flow.h"

using namespace BasebandChannelAdaptation2;


void Panic(TRawIP2NifPanic aPanic);
typedef MBca2Factory* (*TNewBca2FactoryL)();

/** class used to load, open and shutdown the BCA.
@internalComponent 
*/
class CBcaController : public CActive
	{
public:
	CBcaController(CRawIP2Flow& aRawIPFlow);	
	~CBcaController();

	static CBcaController* NewL(CRawIP2Flow& aRawIPFlow);
	void ConstructL();
public:
	void StartLoadL(const CBCAProvision* aBCAProvisionConfig,MUpperControl* aControl, 
			MUpperDataReceiver* aData);
	void Stop(TInt aError);
    inline MBca2* Bca();

#ifdef WCDMA_STUB
    inline TUint Nsapi() { return KErrNotFound; }
#endif
    
protected:
	// Inherited from CActive.
	virtual void RunL();
	virtual void DoCancel();	

private:
		enum TBcaState
		{
		/** Bca ready to start */
		EIdling,
		/** Bca IAP is set*/
		EIAPSet
		};
	
private:
	
	/** BCA owned by NIF*/	
	MBca2* iMBca;
	/** used to load library*/
	TAutoClose<RLibrary> iBcaDll;
	/** BCA state*/
	TBcaState iState;
	
	CRawIP2Flow& iRawIPFlow;
	MUpperControl* iUpperControl; //BasebandChannelAdaptation2
	MUpperDataReceiver* iUpperDataReceiver; //BasebandChannelAdaptation2
	const CBCAProvision* iBCAProvisionConfig; // for obtaining pond
	MBca2::TBcaParams* iBcaParams;
	
	};
	
/** returns the BCA pointer. */	
inline MBca2* CBcaController::Bca()
	{
	return iMBca;
	}	

#endif // BCAIOCONTROLLER_H

