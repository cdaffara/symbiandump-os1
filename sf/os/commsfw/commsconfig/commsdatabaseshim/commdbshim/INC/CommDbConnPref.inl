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
//

inline TBool TCommDbConnPref::operator==(const TCommDbConnPref& aPref) const
/**
Overloaded function for operator "==" 

@param aPref Reference to the class TCommDbConnPref
@return ETrue if Successful if both the objects are equal else EFalse
*/
	{
	return (Compare(aPref) == 0);
	}

inline TBool TCommDbConnPref::operator!=(const TCommDbConnPref& aPref) const
/**
Overloaded function for operator "!=" 

@param aPref Reference to the class TCommDbConnPref
@return Etrue if Successful if the objects are not equal else EFalse
*/
	{
	return (Compare(aPref) != 0);
	}

inline TCommDbConnPref::SCommDbConnPref* TCommDbConnPref::PrefPtr() const
	{
	return reinterpret_cast<SCommDbConnPref*>(UserPtr());
	}

inline TCommDbConnPref& TCommDbConnPref::Cast(const TConnPref& aPref)
/**
Casts the TConnPref object to TCommDbConnPref

@param aPref Connection preferences.
@return Reference to TCommDbConnPref
*/
	{
	// converting const TConnPref* to const TCommDbConnPref* and then casting away the constness
	return *const_cast<TCommDbConnPref*>((reinterpret_cast<const TCommDbConnPref*>(&aPref)));
	}

inline TCommDbMultiConnPref::SCommDbMultiConnPref* TCommDbMultiConnPref::PrefPtr() const
	{
	return reinterpret_cast<SCommDbMultiConnPref*>(UserPtr());
	}

inline TCommDbMultiConnPref& TCommDbMultiConnPref::Cast(const TConnPref& aPref)
/**
Casts the TConnPref object to TCommDbMultiConnPref

@param aPref Connection preferences.
@return Reference to TCommDbMultiConnPref
*/
	{
	// converting const TConnPref* to const TCommDbMultiConnPref* and then casting away the constness
	return *const_cast<TCommDbMultiConnPref*>((reinterpret_cast<const TCommDbMultiConnPref*>(&aPref)));
	}
