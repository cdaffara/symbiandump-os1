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

#ifndef __SSMSWPPOLICYRESOLVERPROXY_H__
#define __SSMSWPPOLICYRESOLVERPROXY_H__

class TSsmSwp;
class CSsmSwpPolicyFrame;

/**
Any class requiring a policy resolver should use this interface.
Any class offering policy resolution should implement this interface.

@internalComponent
@released

*/
class MSsmSwpPolicyResolverProxy
	{
public:
	virtual CSsmSwpPolicyFrame* GetSwpPolicyL(const TSsmSwp& aSwp) = 0;
	};

#endif
