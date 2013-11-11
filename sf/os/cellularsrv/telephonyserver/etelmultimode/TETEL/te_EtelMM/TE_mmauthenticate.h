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
// This is the header file for MM Autorization Data
// 
//

#if (!defined __ETELMMAUTHENTICATE_H__)
#define __ETELMMAUTHENTICATE_H__

#include "ETELMM.H"
#include "TE_EtelMMTestStepBase.h"

NONSHARABLE_CLASS(CEtelMMTestAuthenticate) : public CEtelMMTestStepBase
{
public:
	CEtelMMTestAuthenticate();
	~CEtelMMTestAuthenticate();

	virtual enum TVerdict doTestStepL( void );
};

//
#endif //(__ETELMMAUTHENTICATE_H__)
