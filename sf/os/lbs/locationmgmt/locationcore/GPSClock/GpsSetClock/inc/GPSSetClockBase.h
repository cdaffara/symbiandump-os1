// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef GPSSETCLOCKBASE_H
#define GPSSETCLOCKBASE_H

#include <e32base.h>

const TUint KGPSSetClockInterfaceUidValue = 0x10281D47;
const TUid KGPSSetClockInterfaceUid = {KGPSSetClockInterfaceUidValue};


/** Optional, licensee specific Set Clock plug-in interface definition.
The plug-in is allowed to allocate memory only at construction stage.
@publishedPartner
@released
 */
class CGPSSetClockBase : public CBase
	{
public:
	IMPORT_C static CGPSSetClockBase* NewL(TUid aImplementationId);
	IMPORT_C virtual ~CGPSSetClockBase();
	
	/** SetUTCTime must not allocate memory */
	virtual TInt SetUTCTime(const TTime &aUTCTime) = 0;

protected:
	IMPORT_C CGPSSetClockBase();

protected:
	/**
	Method which allow polymorphic extensions to the API without breaking BC.
	Currently not implemented.
	*/
	IMPORT_C virtual TAny* ExtendedInterface(TInt aFunctionNumber, TAny* aPtr1, TAny* aPtr2);

private:
	TUid iDtorIDKey;
	};

#endif //GPSSETCLOCKBASE_H

