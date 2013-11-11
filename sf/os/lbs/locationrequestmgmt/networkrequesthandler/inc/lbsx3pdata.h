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
// Definitions of types used to simplify data transfer between
// client & server for X3P.
// 
//

/**
 @file
 @internalTechnology
 @released
*/
#ifndef LBSX3PDATA_H__
#define LBSX3PDATA_H__

#include <lbs/lbsx3p.h>


class TTransmitPositionParams
    {
public:
	TUint iPriority;				/* Priority of this request */
	TUint32 iPositionInfoClassType;	/* Class type of the position info to write back to
									   @see _TPositionInfoClassType */
	TUint iPositionInfoClassSize;	/* Size of the position info to write back to */
	TBuf<64> iDestinationId;		/* Buffer to store the destinationId */
    };


#endif // LBSX3PDATA_H__
