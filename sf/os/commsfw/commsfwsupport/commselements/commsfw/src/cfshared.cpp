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

#include <cfshared.h>

using namespace CommsFW;

EXPORT_C TCFSubModuleAddress::TCFSubModuleAddress()
/**
Default Constructor for TCFSubModuleAddress.
@see TCFSubModuleAddress
@publishedPartner
@released
*/
	{
	}

EXPORT_C TCFSubModuleAddress::TCFSubModuleAddress(const TCFSubModuleAddress& aAddress)
/**
Copy Constructor for TCFSubModuleAddress.
@param aAddress Address Instance to copy from.
@see TCFSubModuleAddress
@publishedPartner
@released
*/
	{
	*this = aAddress;
	}

EXPORT_C TCFSubModuleAddress::TCFSubModuleAddress(const TCFModuleName& aModule, const TCFSubModuleName& aSubModule):
	iModule(aModule), iSubModule(aSubModule)
/**
Constructor for TCFSubModuleAddress which takes each part of address separately.
@param aModule Module name to copy.
@param aSubModule Submodule name to copy.
@see TCFModuleName
@see TCFSubModuleName
@publishedPartner
@released
*/
	{
	}

EXPORT_C void TCFSubModuleAddress::SetModule(const TCFModuleName& aModule) 
/**
Set the module name part of the address.
@param aModule module name to set.
@publishedPartner
@released
*/
	{
	iModule.Copy(aModule);
	}

EXPORT_C void TCFSubModuleAddress::SetSubModule(const TCFSubModuleName& aSubModule)
/**
Set the sub-module part of the address.
@param aSubModule Submodule name to set.
@publishedPartner
@released
*/
	{
	iSubModule.Copy(aSubModule);
	}

EXPORT_C TBool TCFSubModuleAddress::operator==(const TCFSubModuleAddress& aAddress) const
/**
@param aAddress Address to compare to.
@return result of comparison. ETrue if equality otherwise EFalse.
@publishedPartner
@released
*/
	{
	return iModule == aAddress.iModule && iSubModule == aAddress.iSubModule;
	}

EXPORT_C TBool TCFSubModuleAddress::operator!=(const TCFSubModuleAddress& aAddress) const
/**
@param aAddress Address to compare to.
@return result of comparison. ETrue if no equality otherwise EFalse.
@publishedPartner
@released
*/
	{
	return !operator==(aAddress);
	}
	
EXPORT_C TCFSubModuleAddress& TCFSubModuleAddress::operator=(const TCFSubModuleAddress& aAddress)
/**
@param aAddress Address to copy.
@return Reference to result of comparison. ETrue if match, EFalse if no equality.
@publishedPartner
@released
*/
	{
	if( this != &aAddress )
		{
		iModule.Copy(aAddress.iModule);
		iSubModule.Copy(aAddress.iSubModule);
		}
	return *this;
	}

EXPORT_C TCFFullModuleName& TCFSubModuleAddress::Printable(TCFFullModuleName& aFullName) const
/**
@return Printable descriptor of the address
@publishedPartner
@released
*/
	{
	aFullName.Copy(iModule);
	if (iSubModule.Length() > 0)
		{
		aFullName.Append(_L(":"));
		aFullName.Append(iSubModule);
		}
	return aFullName;
	}



