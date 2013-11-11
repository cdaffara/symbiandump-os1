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

 
#if !defined (__APALSSESSIONPROVIDER_H__)
#define __APALSSESSIONPROVIDER_H__

class RApaLsSession;

/** The Application starter class needs an RApaLsSession object. This interface
is provided to allow a longer-lived object than the application starter to
own the session and avoid the cost of continually creating it.
@internalComponent
@deprecated
*/
class MApaLsSessionProvider
	{
public:
	virtual RApaLsSession& LsSession() = 0;
	};

#endif
