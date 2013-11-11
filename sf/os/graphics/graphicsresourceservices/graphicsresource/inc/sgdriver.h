// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#ifndef SGDRIVER_H
#define SGDRIVER_H

#include <e32std.h>
#include <pls.h>


class MSgDriverAdapter;

/**
@internalComponent

This class encapsulates the global data in the Graphics Resource DLL.
*/
NONSHARABLE_CLASS(XSgDriverPls)
	{
public:
	/**
	Default constructor, called when the Graphics Resource DLL is loaded into a
	process. Creates the mutex used to synchronise access to the global data. The
	mutex will be automatically destroyed by the kernel on process termination.
	*/
	XSgDriverPls();
public:
	/**
	Result code of the constructor.
	*/
	TInt iError;
	/**
	Handle to the mutex used to synchronise access to the global data.
	*/
	RMutex iMutex;
#ifdef __WINS__
	/**
	Handle to the Graphics Resource Adapter DLL owned by this process.
	*/
	RLibrary iLibrary;
#endif
	/**
	Number of times SgDriver::Open() has been called from this process without
	a corresponding call to SgDriver::Close().
	*/
	TInt iOpenCount;
	/**
	Pointer to the Graphics Resource Adapter singleton in this process.
	*/
	MSgDriverAdapter* iDriver;
	};


/**
@internalComponent

The UID3 value in the compound identifier of the Graphics Resource DLL.
*/
const TUid KSgResourceLibraryUid = {0x10285A70};


/**
@internalComponent

The global data in the Graphics Resource DLL.
*/
#ifdef __WINS__
#define gPls (*Pls<XSgDriverPls>(KSgResourceLibraryUid))
#else
extern XSgDriverPls gPls;
#endif


#endif // SGDRIVER_H
