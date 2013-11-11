// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CSIMDATALINE_H__
#define __CSIMDATALINE_H__

/**
 * @file
 * Declares the class CSimDataLine
 * @internalAll
 */

#include "CSimLine.h"

/**
 * CSimDataLine - Handles data specific line functionality
 *
 * Created and owned by the CSimPhone class.
 */

class CSimNtRas;
class CSimDataCall;
class CSimDataLine : public CSimLine
	{
public:
	static CSimDataLine* NewL(CSimPhone* aPhone,const TDesC& aName);
	CSimDataLine(CSimPhone* aPhone);
	~CSimDataLine();

// MTelObjectTSY pure virtuals
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);

// MLineBaseTSY pure virtuals
	virtual TInt EnumerateCall(const TTsyReqHandle aTsyReqHandle,TInt* aCount);
    virtual TInt GetCallInfo(const TTsyReqHandle,TCallInfoIndex*);
// MSubSessionExtBaseTSY
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
// CLineBase
	virtual TInt CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle);

	virtual CSimCall* CreateNewCallL(TDes& aNewName,TCallType aCallType);
	virtual TUint Caps();
	virtual TInt FindActiveCall(CSimCall*& aCall);

protected:
	void ConstructL(const TName& aName);
	};

#endif
