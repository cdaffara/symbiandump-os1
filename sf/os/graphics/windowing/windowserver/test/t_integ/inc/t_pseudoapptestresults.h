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
//

/**
 @file
 @test
 @internalComponent
*/

#ifndef __T_PSEUDOAPPTESTRESULTS_H__
#define __T_PSEUDOAPPTESTRESULTS_H__

#include <e32base.h>

class TGceTestResults
	{
public:
	IMPORT_C TGceTestResults();
	IMPORT_C ~TGceTestResults();

public:
	TUint32 iElapsedTime;
	TUint32	iTotalCompoTime;
	};

#endif //__T_PSEUDOAPPTESTRESULTS_H__


