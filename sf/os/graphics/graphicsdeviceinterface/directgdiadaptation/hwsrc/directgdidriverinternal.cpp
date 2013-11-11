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

#include "directgdidriverimpl.h"
#include <graphics/directgdidriverinternal.h>

/**
Creates an object of the derived class i.e. CDirectGdiDriverImpl

@param 	aInternal	A reference to a CDirectGdiDriverInternal pointer.
@param	aLibrary	A handle to the adapter library. Only used on WINS.

@pre 	CDirectGdiDriver object has been initialised from the calling thread.
@post 	Instance of CDirectGdiDriverImpl has been created.

@return KErrNone if successful, otherwise one of the system-wide error codes.
*/
EXPORT_C TInt CDirectGdiDriverInternal::New(CDirectGdiDriverInternal*& aInternal, RLibrary aLibrary)
	{
	return CDirectGdiDriverImpl::New(aInternal, aLibrary);
	}

