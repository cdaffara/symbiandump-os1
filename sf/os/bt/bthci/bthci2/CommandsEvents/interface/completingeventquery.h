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

/**
 @file
 @publishedPartner
 @released
*/


#ifndef COMPLETINGEVENTQUERY_H
#define COMPLETINGEVENTQUERY_H

#include <e32base.h>

const TInt KCompletingEventExpectUid = 0x1028307E;


NONSHARABLE_CLASS(MHCICompletingEventQuery)
	{
public:
	virtual TBool MhceqCompletingEventExpected() = 0;
	};


#endif // COMPLETINGEVENTQUERY_H


