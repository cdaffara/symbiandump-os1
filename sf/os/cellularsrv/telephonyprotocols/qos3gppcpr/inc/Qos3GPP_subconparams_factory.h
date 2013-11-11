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
// Header file for the 3GPP SubConnection Extension Parameters. Splitted.
// 
//



/**
 @file
 @internalComponent
*/


#ifndef QOS3GPP_SUBCONPARAMS_FACTORY_H
#define QOS3GPP_SUBCONPARAMS_FACTORY_H

/** Factory used to create instances of IP SubConnection Parameters.

@internalComponent
@released Since 9.3
*/
class CSubCon3GPPExtParamsFactory : public CBase
	{
public:
	static CSubConExtensionParameterSet* NewL(TAny* aConstructionParameters);
	};

#endif // QOS3GPP_SUBCONPARAMS_FACTORY_H
