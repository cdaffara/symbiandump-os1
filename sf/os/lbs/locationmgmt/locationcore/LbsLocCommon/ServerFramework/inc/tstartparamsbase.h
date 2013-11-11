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
// Definition of class used to define server start parameters 
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef __STARTPARAMSBASE_H__
#define __STARTPARAMSBASE_H__

#include <e32std.h>
#include <e32base.h>


const TUint KServerStartParamsTypeValue	= 0x10282243;
const TUid  KServerStartParamsTypeUid	= {KServerStartParamsTypeValue};

const TUint KProcessStartParamsTypeValue = 10282242;
const TUid  KProcessStartParamsTypeUid	= {KProcessStartParamsTypeValue};


NONSHARABLE_CLASS(TStartParamsBase)
	{
public:
	virtual ~TStartParamsBase();
	const TUid	GetType() const; 
protected:	
	void SetType(const TUid& aType);
	virtual void SetType() = 0;
private:
	TUid	iType;
	};

#endif // __STARTPARAMSBASE_H__
