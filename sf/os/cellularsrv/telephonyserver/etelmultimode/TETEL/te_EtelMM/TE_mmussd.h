// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**@file
   @internalComponent */
#ifndef _TE_MMUSSD_H_
#define _TE_MMUSSD_H_

#include "TE_EtelMMTestStepBase.h"
#include <etelmm.h>
#include <e32base.h>
#include <mmretrieve.h>
#include "TE_mmcanceller.h"


class CTestUssdMessaging : public CEtelMMTestStepBase
	{
public:
	TInt iTestCount;
	CTestUssdMessaging() ;
	~CTestUssdMessaging(){} ;
	virtual enum TVerdict doTestStepL();
	/*static*/ void ValidateUssdMessage(const TDesC8& aMsg, const RMobileUssdMessaging::TMobileUssdAttributesV1 &aAtts);

private:
	};


#endif
