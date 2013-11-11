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



// INCLUDE FILES
#include <e32base.h>
#include <lbs.h>
#include <lbserrors.h>
#include "lbsdevloggermacros.h"

#include "suplproxyutils.h"

// ================= LOCAL FUNCTIONS =======================

// ================= MEMBER FUNCTIONS =======================

/**
 * Read and kill the client if it leaves. If the returned error code 
 * is negative, the RMessage must not be completed. Complete after 
 * client is paniced crashes the server with USER 70.
 *
 * @param aMessage Message instance
 * @param aPtr Pointer to read from
 * @param aDes Descriptor to read to
 * @param aOffset offset to start read
 * @return Error code
 */
TInt SuplProxyUtils::Read(const RMessage2& aMessage, const TInt aParam, TDes8& aDes, TInt aOffset)
	{
	LBSLOG(ELogP1, "SuplProxyUtils::Read(3+TDes8 ) Begin\n");

    TInt ret = aMessage.Read(aParam, aDes, aOffset);
    if (ret != KErrNone)
        {
        aMessage.Panic(KPosClientFault, EPositionBadDescriptor);
        }

	LBSLOG(ELogP1, "SuplProxyUtils::Read() End\n");

    return ret;
    }

/**
 * Write and kill the client if it leaves. If the returned error code 
 * is negative, the RMessage must not be completed. Complete after 
 * client is paniced crashes the server with USER 70.
 *
 * @param aMessage Message instance
 * @param aPtr Pointer to write to
 * @param aDes Descriptor to write from
 * @param aOffset offset to start write
 * @return Error code
 */
TInt SuplProxyUtils::Write(const RMessage2& aMessage, const TInt aParam, const TDesC8& aDes, TInt aOffset)
	{
	LBSLOG(ELogP1, "SuplProxyUtils::Write() Begin\n");

	TInt ret = aMessage.Write(aParam, aDes, aOffset);
	if (ret != KErrNone)
        {
        aMessage.Panic(KPosClientFault, EPositionBadDescriptor);
        }

	LBSLOG(ELogP1, "SuplProxyUtils::Write() End\n");

    return ret;
	}

//  End of File  
