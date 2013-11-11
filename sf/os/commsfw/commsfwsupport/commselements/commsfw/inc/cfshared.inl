// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__CFSHARED_INL__)
#define __CFSHARED_INL__

inline TCFModuleNameF::TCFModuleNameF()
    {
    }

inline TCFModuleNameF::TCFModuleNameF(const TDesC8& aModule)
: TCFModuleName(aModule)
    {
    }

inline TBool TCFModuleNameF::operator==(const TDesC8& aRHS) const
    {
    return CompareF(aRHS) == 0;
    }

inline TBool TCFModuleNameF::operator!=(const TDesC8& aRHS) const
    {
    return CompareF(aRHS) != 0;
    }

inline TCFSubModuleNameF::TCFSubModuleNameF()
    {
    }

inline TCFSubModuleNameF::TCFSubModuleNameF(const TDesC8& aSubModule)
: TCFModuleName(aSubModule)
    {
    }

inline TBool TCFSubModuleNameF::operator==(const TDesC8& aRHS) const
    {
    return CompareF(aRHS) == 0;
    }

inline TBool TCFSubModuleNameF::operator!=(const TDesC8& aRHS) const
    {
    return CompareF(aRHS) != 0;
    }

inline const TCFModuleNameF& TCFSubModuleAddress::Module() const 
/**
@return Reference to the module name part of the address.
@publishedPartner
@released
*/
	{
	return iModule;
	}

inline const TCFSubModuleNameF& TCFSubModuleAddress::SubModule() const 
/**
@return Submodule name part of the address.
@publishedPartner
@released
*/
	{
	return iSubModule;
	}


#endif // __CFSHARED_INL__

