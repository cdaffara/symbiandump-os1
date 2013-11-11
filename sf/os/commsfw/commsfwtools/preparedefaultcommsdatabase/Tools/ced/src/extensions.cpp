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

#ifdef __COMMDB_ROHID_SUPPORT__

/**
@file 
@internalComponent 

Implementation of CED extended processing stubs. 
*/

#include <database.h>


/**
Stub: Does nothing

@return EFalse always
*/
TBool DBAccess::ExtendedProcessingColumn()
	{
	return EFalse;
	}


/**
Stub: Does nothing
	
@return EFalse always
*/
TBool DBAccess::DoExtendedColumnProcessing()
	{
	// This should be never called.
	return EFalse;
	}

/*
Stub: does nothing
	
@param table Not used
@return EFalse always
*/
TInt DBAccess::ExtendedTableProcessing(TDesC& /* table */)
	{
	return EFalse;
	}


/**
Stub: does nothing
	
@return KErrNotSupported always.
*/
TInt DBAccess::DoExtendedTableProcessing()
	{
	// This should be never called.
	return KErrNotSupported;
	}

#endif //__COMMDB_ROHID_SUPPORT__
