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
// Define types used within RFCOMM
// 
//

#ifndef RFCOMMTYPES_H
#define RFCOMMTYPES_H

#include "rfcommconsts.h"

struct TRfcommPortParams
	{
	TRfcommPortParams()
		: iPriority(0),
		  iMaxFrameSize(KRfcommDefaultMTU),
		  iCreditIndicator(0),
		  iInitialCredit(KInitialCredit)
		{
		};
	TUint8 iPriority;
	TUint16 iMaxFrameSize;
	TUint8 iCreditIndicator;
	TUint8 iInitialCredit;
	};


#endif
