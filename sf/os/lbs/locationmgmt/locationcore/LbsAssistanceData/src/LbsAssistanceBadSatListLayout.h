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

#ifndef LBSASSISTANCEBADSATLISTLAYOUT_H
#define LBSASSISTANCEBADSATLISTLAYOUT_H


#include <e32def.h>

struct TBadSatListArrayLayout
	{
	TUint iArray[32];
	};

struct TBadSatListLayout
	{
	TBadSatListArrayLayout iBadSatIdArray;
	};


#endif //LBSASSISTANCEBADSATLISTLAYOUT_H

