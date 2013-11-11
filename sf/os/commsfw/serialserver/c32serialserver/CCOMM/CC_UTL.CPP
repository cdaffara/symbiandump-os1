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

#include <e32std.h>
#include <e32svr.h>
#include <c32comm.h>
#include "COMMIPC.H"
#include "C32LOG.H"

/** @file
 *
 * Implements the StartC32 function, used to start up C32
 */

#ifdef _DEBUG
#define STARTC32LOG(a) a
#else
#define STARTC32LOG(a) 
#endif

/** File name of the rootserver client DLL.
@internalComponent
*/
_LIT(KRootServerDll, "c32root.dll");

/** Ordinal number of the StartC32() entrypoint in the rootserver client library.
@internalComponent
*/
const TInt StartC32EntryOrdinal=1;

//
// NOTE: These literals were previously #define'd as _L()
//       By using _LIT and functions returning reference to them
//       we save some ROM bytes for avoiding the duplicated strings.
//


const TDesC& KCommServerName(void)
/**
 * returns the name of the Comm Server thread
 */
	{
	_LIT(KCommServerNameLit, "!CommServer");

	return KCommServerNameLit;
	}


EXPORT_C TInt StartC32()
  /**
   * Start the Rootserver and services, 
   * this function in C32 is DEPRECATED and will be removed
   * in the near future.
   *
   * Instead one should link to c32root.dll and use the 
   * StartC32() from that library.
   *
   * @return TInt - An error code
   */
  	{
	C32_STATIC_LOG(KC32Warning,_L8("WARNING: StartC32() CALLED IN C32.DLL. THIS IS DEPRECATED. MUST USE C32ROOT.DLL!!"));

	RLibrary library;
	TInt res = library.Load(KRootServerDll);

    if (res!=KErrNone && res!=KErrAlreadyExists)
		{
		C32_STATIC_LOG2(KC32Warning, _L8("StartC32 from C32.DLL: Unable to load RS DLL %d."), res);

        return res;
		}

	C32_STATIC_LOG(KC32Bootup,      _L8("StartC32 from C32.DLL: Loaded RS library."));

    TLibraryFunction StartC32Entry=library.Lookup(StartC32EntryOrdinal);
    
    if (StartC32Entry==NULL)
		{
		C32_STATIC_LOG2(KC32Warning, _L8("StartC32 from C32.DLL: Unable to load StartC32 ordinal %d."), StartC32EntryOrdinal);
        return KErrNoMemory;
		}

	C32_STATIC_LOG(KC32Bootup, _L8("StartC32 from C32.DLL: Calling StartC32 ordinal from RS."));

	res = StartC32Entry();
	library.Close();
  	return res;
	}


// EOF - CC_UTL.CPP
