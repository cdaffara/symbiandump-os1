// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
 
*/
#ifndef RRLP_UTILSE_H
#define RRLP_UTILSE_H


/**
RRLP Utilities class

@internalTechnology

*/
class RrlpUtils
	{
public:
	/** Translates errors returned by the ASN1 runtime library */
	static TInt ProcessAsn1Error(TInt aError);
	};

#endif //RRLP_UTILSE_H


