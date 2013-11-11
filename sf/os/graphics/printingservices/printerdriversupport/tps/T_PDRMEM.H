// Copyright (c) 1996-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__T_PDRMEM_H__)
#define __T_PDRMEM_H__

#include "TGraphicsHarness.h"
#include "prnsetupfacade.h"


class CTPdrMemStep : public CTGraphicsStep
	{
public:
	CTPdrMemStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTPdrMemStep,"TPdrMem");

#endif
