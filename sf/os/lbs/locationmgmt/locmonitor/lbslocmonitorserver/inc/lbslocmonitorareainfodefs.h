/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


/**
 @file
 @internalTechnology
*/

#ifndef LBSLOCMONITORAREAINFODEFS_H
#define LBSLOCMONITORAREAINFODEFS_H

#include <e32def.h>


/**
The base class for classes used to store area information

@internalComponent
@released
 */
class TLbsLocMonitorAreaInfoBase
	{
public:
	/**
	TLbsLocMonitorAreaInfoBase derived classes class types
	 */
	enum TAreaInfoClassType
		{
		EAreaInfoInfoUnknownClass		= 0,
		/** Global Cell Id */
		EAreaGciClass					= 0x01,
		};

public:

	TUint32 AreaInfoClassType() const;

protected:

	TLbsLocMonitorAreaInfoBase();

protected:
	/** The type of the derived class */
	TUint32 iAreaInfoClassType;
	};

/*
Global Cell Id (GCI)
*/
class TLbsLocMonitorAreaInfoGci: public TLbsLocMonitorAreaInfoBase
{
public:

	TLbsLocMonitorAreaInfoGci();

public:
	TUint iMcc; 
	TUint iMnc; 
	TInt iLac;		// this can be set to -1 to indicate not valid
	TUint iCid;

	TBool iValidity;

	TBool iIs3gNetworkMode;
};

#endif // LBSLOCMONITORAREAINFODEFS_H
