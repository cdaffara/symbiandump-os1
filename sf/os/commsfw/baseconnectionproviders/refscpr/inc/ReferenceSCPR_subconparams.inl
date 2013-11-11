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
// Inline Functions file for the Reference SubConnection Parameter Set.
// 
//

/**
 @file
 @internalComponent
*/


inline CSubConReferenceParamSet* CSubConReferenceParamSet::NewL(CSubConParameterFamily& aFamily, CSubConParameterFamily::TParameterSetType aType)
	{
	CSubConReferenceParamSet* obj = NewL();
	CleanupStack::PushL(obj);
	aFamily.AddExtensionSetL(*obj, aType);
	CleanupStack::Pop(obj);
	return obj;
	}

inline CSubConReferenceParamSet* CSubConReferenceParamSet::NewL()
	{
	STypeId typeId(KSubConReferenceParamsUid, KSubConReferenceParamsType);
	return static_cast<CSubConReferenceParamSet*>(CSubConParameterSet::NewL(typeId));
	}

inline CSubConReferenceParamSet::CSubConReferenceParamSet()
	: CSubConExtensionParameterSet(), 
	iDummyTIntParameter(0),
	iDummyName()
	{
	}
	
	
inline TInt CSubConReferenceParamSet::GetDummyTIntParameter() const
	{
	return iDummyTIntParameter;
	}
	
inline const TName& CSubConReferenceParamSet::GetDummyTNameParameter() const
	{
	return iDummyName;
	}
	
inline void CSubConReferenceParamSet::SetDummyTIntParameter(TInt aDummyTIntParameter)
	{
	iDummyTIntParameter = aDummyTIntParameter;
	}
	
inline void CSubConReferenceParamSet::SetDummyTNameParameter(const TName& aDummyName)
	{
	iDummyName = aDummyName;	
	}
