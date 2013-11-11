// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
@internalComponent
*/

#ifndef INTERFACEBASEEXTENSION_H
#define INTERFACEBASEEXTENSION_H

#include <e32base.h>

NONSHARABLE_CLASS(CRemConInterfaceBaseExtension)
	: public CBase
	{
public:
	CRemConInterfaceBaseExtension();
	~CRemConInterfaceBaseExtension();
	
	void SetBulk(TBool aBulk);
	TBool Bulk() const;
	
private:
	TBool iBulk;
	};

#endif // INTERFACEBASEEXTENSION_H
