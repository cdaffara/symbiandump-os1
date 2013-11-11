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
// Data structures for generic access point information
// 
//

/**
 @file
 @publishedPartner
 @released
*/



#ifndef _ES_ACCESSPOINTSTATUS_H_
#define _ES_ACCESSPOINTSTATUS_H_


namespace ConnectionServ
{

// Type TAccessPointStatus (containing TAccessPointFlagValue instances) represents the status flags for an access point.
// As well as the value of the flag True/False, 
//  they include EUnknown, the case where the value isn't (yet) known / wasn't retrieved.

struct TAccessPointStatusFilter;

enum TAccessPointFlagValue
	{
	EAccessPointFlagUnknown = 0,
	EAccessPointFlagTrue = 1,
	EAccessPointFlagFalse = 2
	};

struct TAccessPointStatus
	{
	union
		{
		struct
			{
			TUint iConfigured:2;
			TUint iRestricted:2;
			TUint iAvailable:2;
			TUint iStarted:2;
			TUint iActive:2;
			} iS;
		TInt64 iAsInt;
		} iU;
	
	TAccessPointStatus(TInt64 aI)
		{
		iU.iAsInt = aI;
		}

	TAccessPointStatus()
		{
		iU.iAsInt = TInt64(0);
		}

	TInt64 AsInt64() const { return iU.iAsInt; }

	void Configured(TAccessPointFlagValue aVal) {iU.iS.iConfigured = aVal;}
	TAccessPointFlagValue Configured() const {return static_cast<TAccessPointFlagValue>(iU.iS.iConfigured);}

	void Restricted(TAccessPointFlagValue aVal) {iU.iS.iRestricted = aVal;}
	TAccessPointFlagValue Restricted() const {return static_cast<TAccessPointFlagValue>(iU.iS.iRestricted);}

	void Available(TAccessPointFlagValue aVal) {iU.iS.iAvailable = aVal;}
	TAccessPointFlagValue Available() const {return static_cast<TAccessPointFlagValue>(iU.iS.iAvailable);}

	void Started(TAccessPointFlagValue aVal) {iU.iS.iStarted = aVal;}
	TAccessPointFlagValue Started() const {return static_cast<TAccessPointFlagValue>(iU.iS.iStarted);}

	void Active(TAccessPointFlagValue aVal) {iU.iS.iActive = aVal;}
	TAccessPointFlagValue Active() const {return static_cast<TAccessPointFlagValue>(iU.iS.iActive);}

	void UpdateWith(const TAccessPointStatus& aSrc)
		{
		// Fields:	CRAS

		// Initial:	CR??
		// +
		// New:		C?a?
		// =
		// Result	CRa?

		// +
		// New:		C?A?
		// =
		// Result:	CRA?

		TAccessPointFlagValue value = static_cast<TAccessPointFlagValue>(iU.iS.iConfigured);
		TAccessPointStatus::PickUpIfSrcKnown( value, aSrc.Configured() );
		iU.iS.iConfigured = value;

		value = static_cast<TAccessPointFlagValue>(iU.iS.iRestricted);
		TAccessPointStatus::PickUpIfSrcKnown( value, aSrc.Restricted() );
		iU.iS.iRestricted = value;

		value = static_cast<TAccessPointFlagValue>(iU.iS.iAvailable);
		TAccessPointStatus::PickUpIfSrcKnown( value, aSrc.Available() );
		iU.iS.iAvailable = value;

		value = static_cast<TAccessPointFlagValue>(iU.iS.iStarted);
		TAccessPointStatus::PickUpIfSrcKnown( value, aSrc.Started() );
		iU.iS.iStarted = value;

		value = static_cast<TAccessPointFlagValue>(iU.iS.iActive);
		TAccessPointStatus::PickUpIfSrcKnown( value, aSrc.Active() );
		iU.iS.iActive = value;
		}

	static void PickUpIfSrcKnown(TAccessPointFlagValue& aDest, const TAccessPointFlagValue aSrc )
		{
		if(aSrc != EAccessPointFlagUnknown)
			{
			aDest = aSrc;
			}
		}

	inline void CopyFieldsRequiredByQueryFrom(const TAccessPointStatusFilter& aFilter,const TAccessPointStatus& aSource);

	static TText FlagValueAsChar(TAccessPointFlagValue aVal)
		{
		switch(aVal)
			{
			case EAccessPointFlagFalse: return 'N';
			case EAccessPointFlagTrue: return 'Y';
			}
		
		return '?';
		}
	};

// Type TAccessPointStatusFilter (containing TAccessPointFlagFilterValue instances) represents a query against the above type.
// As well as the value of the flag True/False, meaning "(find the value and) match true/false"
//  they include EAny, meaning "(retrieve the value and) match either value"
//    ... and .. EIgnore, meaning "don't retrieve the value"
// This is of use for fields such as Availability which may be expensive to retrieve.

enum TAccessPointFlagFilterValue
	{
	EAccessPointFlagIgnore = 0,   // don't care.. signify we don't want to go and find it out if it requires any effort
	EAccessPointFlagMatchTrue = 1,
	EAccessPointFlagMatchFalse = 2,
	EAccessPointFlagMatchAny = 3,     // find the value out
	};

struct TAccessPointStatusFilter
	{
	union
		{
		struct
			{
			TUint iConfigured:2;
			TUint iRestricted:2;
			TUint iAvailable:2;
			TUint iStarted:2;
			TUint iActive:2;
			} iS;
		TInt64 iAsInt;
		} iU;
	
	TAccessPointStatusFilter(TInt64 aI)
		{
		iU.iAsInt = aI;
		}

	TAccessPointStatusFilter()
		{
		iU.iAsInt = TInt64(0);
		}

	TInt64 AsInt64() const { return iU.iAsInt; }

	void Configured(TAccessPointFlagFilterValue aVal) {iU.iS.iConfigured = aVal;}
	TAccessPointFlagFilterValue Configured() const {return static_cast<TAccessPointFlagFilterValue>(iU.iS.iConfigured);}

	void Restricted(TAccessPointFlagFilterValue aVal) {iU.iS.iRestricted = aVal;}
	TAccessPointFlagFilterValue Restricted() const {return static_cast<TAccessPointFlagFilterValue>(iU.iS.iRestricted);}

	void Available(TAccessPointFlagFilterValue aVal) {iU.iS.iAvailable = aVal;}
	TAccessPointFlagFilterValue Available() const {return static_cast<TAccessPointFlagFilterValue>(iU.iS.iAvailable);}

	void Started(TAccessPointFlagFilterValue aVal) {iU.iS.iStarted = aVal;}
	TAccessPointFlagFilterValue Started() const {return static_cast<TAccessPointFlagFilterValue>(iU.iS.iStarted);}

	void Active(TAccessPointFlagFilterValue aVal) {iU.iS.iActive = aVal;}
	TAccessPointFlagFilterValue Active() const {return static_cast<TAccessPointFlagFilterValue>(iU.iS.iActive);}

	// helper function.. whether given flag value matches given filter
	static TBool Matches(TAccessPointFlagFilterValue aFilter,TAccessPointFlagValue aValue)
		{
		if(aFilter == EAccessPointFlagIgnore)
			{
			return ETrue;
			}

		switch(aValue)
			{
			case EAccessPointFlagFalse:
				if(aFilter != EAccessPointFlagMatchTrue) //i.e. is False/MatchAny
					{
					return ETrue;
					}
				break;

			case EAccessPointFlagTrue:
				if(aFilter != EAccessPointFlagMatchFalse) //i.e. is True/MatchAny
					{
					return ETrue;
					}
				break;
			
			case EAccessPointFlagUnknown:
				if(aFilter == EAccessPointFlagMatchAny) // only if we'll match any
					{
					return ETrue;
					}
				break;
			}
		
		return EFalse;
		}

	static TText FilterValueAsChar(TAccessPointFlagFilterValue aVal)
		{
		switch(aVal)
			{
			case EAccessPointFlagMatchFalse: return 'N';
			case EAccessPointFlagMatchTrue: return 'Y';
			case EAccessPointFlagMatchAny: return '?';
			}
		return '-';
		}

	};



inline void TAccessPointStatus::CopyFieldsRequiredByQueryFrom(const TAccessPointStatusFilter& aFilter,const TAccessPointStatus& aSource)
	{
	if(aFilter.Configured() != EAccessPointFlagIgnore)
		{
		Configured(aSource.Configured());
		}

	if(aFilter.Restricted() != EAccessPointFlagIgnore)
		{
		Restricted(aSource.Restricted());
		}

	if(aFilter.Available() != EAccessPointFlagIgnore)
		{
		Available(aSource.Available());
		}

	if(aFilter.Started() != EAccessPointFlagIgnore)
		{
		Started(aSource.Started());
		}

	if(aFilter.Active() != EAccessPointFlagIgnore)
		{
		Active(aSource.Active());
		}
	}

}
#endif

