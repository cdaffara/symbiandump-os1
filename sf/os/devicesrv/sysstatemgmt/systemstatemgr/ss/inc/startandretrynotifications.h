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

#ifndef __STARTANDRETRYTIFICATIONS_H__
#define __STARTANDRETRYTIFICATIONS_H__


class CSsmStartSafe;
class MStartAndRetryNotifications
	{
public:
	virtual void RendezvousComplete( TInt aComplete, TInt aCallerIndex  ) = 0;
	virtual void CancellationComplete( TInt aComplete, TInt aCallerIndex  ) = 0;
	};
	
#endif // __STARTANDRETRYTIFICATIONS_H__
	

