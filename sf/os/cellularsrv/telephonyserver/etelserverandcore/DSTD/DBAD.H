// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
   @internalAll */
#ifndef __DBAD_H
#define __DBAD_H

#include <e32base.h>
#include "ET_PHONE.H"
#include "ACQUIRE.H"

//
// CPhoneFactoryBad object
//
class CLibUnloader;
class CPhoneFactoryBad: public CPhoneFactoryBase
/**
@internalComponent
*/
	{
public:
	CPhoneFactoryBad();
	~CPhoneFactoryBad();
	virtual TInt GetPhoneInfo(const TInt aIndex, RTelServer::TPhoneInfo& aPhoneFactoryBaseInfo);
	virtual TInt EnumeratePhones();
	virtual CPhoneBase* NewPhoneL(const TDesC& aName);
	virtual TBool IsSupported(const TInt aMixin);
	};

#endif
