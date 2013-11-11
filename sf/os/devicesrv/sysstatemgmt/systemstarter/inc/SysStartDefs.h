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
//


#if !defined (__SYSSTARTDEFS_H__)
#define __SYSSTARTDEFS_H__

/** RProperty category to display a splash screen during startup.
@publishedPartner
@deprecated
*/
const TUid KSysStartPropertyCat={0x10205C44};

/** RProperty key to display a splash screen during startup.
If the property has a value of zero a splash screen will be displayed by the emulator.
If the property has a value of one the splash screen will be removed by the emulator.
@publishedPartner
@deprecated
*/
const TUint KSplashPropertyKey = 0x01;

/**The DLL functions are intended for licensee use. The function enables the
licensee to provide a DLL containing custom functions along with data structs
containing data to be passed to the functions. The system starter will load the
DLL and invoke the required function, passing a reference to the data provided.

They must conform to TInt functionName(const TDesC8&) signature.

A function of this type takes a single const TDesc8& type argument, and returns
a TInt type.

The function must return an error code only if it makes sense to re-try the
invocation. Otherwise it should return KErrNone.

@publishedPartner
@deprecated
*/

typedef TInt (*Dllfunctiontype)(const TDesC8&);

#endif // __SYSSTARTDEFS_H__

