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


#ifndef __CSENDDTMF_H__
#define __CSENDDTMF_H__

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>
#include <etel3rdparty.h>


#include "CISVAPIAsync.h"
#include "CMainMenu.h"

/**
Sends DTMF tones down the phone line to be retrieved by the remote party on the
active call. Tones are specified as the argument to DoStartRequestL()
*/
class CSendDTMF : public CISVAPIAsync
	{
	// Methods
public:
	static CSendDTMF* NewL(MExecAsync* aController);
	~CSendDTMF();

	void DoStartRequestL(const TDesC& aNumber);

private:
	CSendDTMF(MExecAsync* aController);
	void ConstructL();

	void RunL();
	void DoCancel();

	// Data
private:
	/**
	Set to ETrue if a single tone is to be sent, EFalse otherwise.
	*/
	TBool iSingleTone;
	};

#endif // __CSENDDTMF_H__
