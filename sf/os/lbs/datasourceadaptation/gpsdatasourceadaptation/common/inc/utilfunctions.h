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
// utility fucntions etc for various bits of the location server
// 
//

/**
 @file
 @InternalComponent
*/

#ifndef __LBSUTILFUNCTIONS_H__
#define __LBSUTILFUNCTIONS_H__

#include <e32base.h>
#include <lbspositioninfo.h>

TInt CopyPositionTypes(TPositionInfoBase& aTo, const TPositionInfoBase& aFrom);
TInt CopyPositionTypes2(TPositionInfoBase& aTo, const TPositionInfoBase& aFrom);
TInt CopyGenericPositionType(TPositionInfoBase& aTo, const TPositionInfoBase& aFrom);
TInt SupportedType(const TUint32& aType, const TInt& aSize);
TBool Partial(const TPosition& aPos);

#endif // __LBSUTILFUNCTIONS_H__
