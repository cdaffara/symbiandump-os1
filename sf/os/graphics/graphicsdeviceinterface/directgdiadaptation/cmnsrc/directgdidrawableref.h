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

#ifndef _DIRECTGDIDRAWABLEREF_H_
#define _DIRECTGDIDRAWABLEREF_H_

/** 
@file
@internalComponent
 */


#include "directgdiadapter.h"
#include <e32base.h>

/**
Class to allow a reference count to be held by a drawable object.
 */
NONSHARABLE_CLASS(CDirectGdiDrawableRef) : public CBase
{
public:
	virtual ~CDirectGdiDrawableRef();
	virtual void Open();
	virtual void Close();

private:
	TInt iRefCount;
};

#endif // _DIRECTGDIDRAWABLEREF_H_
