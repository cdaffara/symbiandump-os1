// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__TSCDVSCALING_H__)
#define __TSCDVSCALING_H__

#include "TGraphicsHarness.h"


class CTScalingStep : public CTGraphicsStep
	{
public:
	CTScalingStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	};

_LIT(KTScalingStep,"TScaling");


#endif
