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

#ifndef TSGIMAGECOLLECTIONMBX_H
#define TSGIMAGECOLLECTIONMBX_H

#include "tgraphicsresourceteststepbase.h"

class CTSgImageCollectionMbx : public CTSgTestStepBase
	{
public:
	CTSgImageCollectionMbx();
	~CTSgImageCollectionMbx();
private:
	// From CTestStep
	virtual TVerdict doTestStepL();
	};

_LIT(KTSgImageCollectionMbx,"TSgImageCollectionMbx");

#endif /*TSGIMAGECOLLECTIONMBX_H*/
