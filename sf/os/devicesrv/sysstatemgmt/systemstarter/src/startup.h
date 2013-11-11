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

 
#if !defined (__STARTUP_H__)
#define __STARTUP_H__

class MStartupStateInfo;

/**
This class retrieves the next stage that the system needs to move into
for example 'critical static'   
@internalComponent
@deprecated
*/
class MStartup
	{
public:
	virtual ~MStartup() {};
	virtual const MStartupStateInfo* GetNextStateL() const = 0;
	};

#endif // __STARTUP_H__
