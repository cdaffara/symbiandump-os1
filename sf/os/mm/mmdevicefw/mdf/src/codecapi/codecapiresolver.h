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

/**
 @file
 @publishedPartner
 @prototype
*/

#ifndef CODECAPIRESOLVER_H
#define CODECAPIRESOLVER_H

#include <ecom/resolver.h>

/**
Custom resolver for the Codec API implementations.
It looks in the opaque data for:
\<i\>uid type \<s\>input port data type \<d\>output port data type
*/
class CCodecApiResolver : public CResolver
	{
public:
	static CCodecApiResolver* NewL (MPublicRegistry& aRegistry);
	virtual TUid IdentifyImplementationL(TUid aInterfaceUid,
		const TEComResolverParams& aAdditionalParameters) const;
	virtual RImplInfoArray* ListAllL(TUid aInterfaceUid,
		const TEComResolverParams& aAdditionalParameters) const;
	virtual ~CCodecApiResolver();
protected:
	CCodecApiResolver(MPublicRegistry& aRegistry);
	};
#endif // CODECAPIRESOLVER_H



