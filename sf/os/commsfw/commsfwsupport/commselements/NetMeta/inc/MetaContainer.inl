// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __E_METACONTAINER_INL__
#define __E_METACONTAINER_INL__

namespace Meta
{

template<class ELEMENT>
RMetaDataContainer<ELEMENT>* RMetaDataContainer<ELEMENT>::LoadL(TPtrC8& aDes)
/**
 * This function instantiates a container of meta objects and loads its content
 * from a descriptor.
 */
    {
    RMetaDataContainer<ELEMENT>* cont = new (ELeave) RMetaDataContainer<ELEMENT>;
    User::LeaveIfError(cont->GetTypeId().Check(aDes)? KErrNone : KErrArgument);
	User::LeaveIfError(cont->Load(aDes));
	return cont;
    }

template<class ELEMENT>
inline RMetaDataContainer<ELEMENT>::RMetaDataContainer()
/**
 * Constructor
 */
	{
	}

template<class ELEMENT>
SMetaData* RMetaDataContainer<ELEMENT>::LoadElementL(TPtrC8& aDes)
/**
 * This helper function instantiates a single meta object (element) and loads
 * its content from a descriptor.
 */
	{
	return ELEMENT::LoadL(aDes);
	}

}	// namespace Meta
#endif	// __E_METACONTAINER_INL__

