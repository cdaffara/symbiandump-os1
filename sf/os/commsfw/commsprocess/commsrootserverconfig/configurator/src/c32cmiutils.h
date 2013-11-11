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
// Utility functions for rootserver startup configuration classes
// 
//

#if !defined(__C32CMIUTILS_H__)
#define __C32CMIUTILS_H__

#include <e32base.h>

/**
Static utility methods assisting parsing a .CMI file for the Comms Configurator.
@internalComponent
*/
class C32CmiUtils 
	{
public:
	static TInt ThreadPriorityNameToEnum(const TDesC8& aPriorityName, TThreadPriority& aPriority);
	};

#endif // __C32CMIUTILS_H__

