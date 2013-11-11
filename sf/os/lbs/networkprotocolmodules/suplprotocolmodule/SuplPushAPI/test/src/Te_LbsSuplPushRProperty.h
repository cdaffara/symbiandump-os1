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
// A unit test wrapper over the RProperty class
// 
//

#if (!defined __TE_LBSSUPLPUSHRPROPERTY_H__)
#define __TE_LBSSUPLPUSHRPROPERTY_H__

#include <e32property.h>
#include "Te_LbsSuplCommon.h"

/**
A unit test wrapper over the RProperty class
*/
class RTe_LbsSuplPushRProperty : public RProperty
	{
public:	
	static TInt Define(TUint aKey, TInt aAttr, const TSecurityPolicy &aReadPolicy, const TSecurityPolicy &aWritePolicy, TInt aPreallocated=0);
	static TInt Get(TUid aCategory, TUint aKey, TInt &aValue);
	static TInt Get(TUid aCategory, TUint aKey, TDes8 &aValue);
	
	static TInt Set(TUid aCategory, TUint aKey, TInt aValue);
	static TInt Set(TUid aCategory, TUint aKey, const TDesC8 &aValue);
	};
	
#endif //__TE_LBSSUPLPUSHRPROPERTY_H__	
