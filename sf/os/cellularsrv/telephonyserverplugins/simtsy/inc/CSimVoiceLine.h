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

#ifndef __CSIMVOICELINE_H__
#define __CSIMVOICELINE_H__

/**
 * @file
 * Declares the class CSimVoiceLine
 * @internalAll
 */

#include "CSimLine.h"

/**
 * CSimVoiceLine - Handles voice specific line functionality
 *
 * Created and owned by the CSimPhone class.
 */
class CSimVoiceCall;
class CSimVoiceLine : public CSimLine
	{
public:
	static CSimVoiceLine* NewL(CSimPhone* aPhone,const TDesC& aName);
	CSimVoiceLine(CSimPhone* aPhone);
	~CSimVoiceLine();

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
	TInt FindActiveVoiceCall(CSimVoiceCall*& aCall);
	virtual TUint Caps();
	virtual TInt FindActiveCall(CSimCall*& aCall);

protected:
	void ConstructL(const TName& aName);
	};

#endif
