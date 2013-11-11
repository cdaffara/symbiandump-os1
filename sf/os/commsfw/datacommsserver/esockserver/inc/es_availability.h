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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef SYMBIAN_AVAILABILITY_H
#define SYMBIAN_AVAILABILITY_H

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESock_Availabili, "ESock_Availabili");
#endif

namespace ESock
{

/**
Container for availability status. Used by availability plugins to pass availability information into the mesh machine.
*/
class TAvailabilityStatus
	{
public:
	enum
		{
		EUnknownAvailabilityScore = -1,
		EMinAvailabilityScore = 0,
		EMaxAvailabilityScore = 100
		};

	TAvailabilityStatus()
	:	iScore(EUnknownAvailabilityScore)
		{
		}

	TAvailabilityStatus(TInt aScore)
	:	iScore(aScore)
		{
		}

	/**
	@return ETrue if availability is known, EFalse otherwise.
	*/
	TBool IsKnown() const
		{
		return iScore >= EMinAvailabilityScore && iScore <= EMaxAvailabilityScore;
		}

	/**
	@param aScore the availability score. Must be >= EMinAvailabilityScore and <= EMinAvailabilityScore.
	*/
	void SetScore(TInt aScore)
		{
		__ASSERT_DEBUG(aScore >= EMinAvailabilityScore && aScore <= EMaxAvailabilityScore, User::Panic(KSpecAssert_ESock_Availabili, 1));
		iScore = aScore;
		}

	/**
	Set the availability status to unknown
	*/
	void SetUnknown()
		{
		iScore = EUnknownAvailabilityScore;
		}

	/**
	@return the availability score
	*/
	TInt Score() const { return iScore; }

private:
	TInt iScore;
	};

/**
Collection of options used to register to availability notifications
*/
struct TAvailabilitySubscriptionOptions
	{
public:
	/**
	 Subscription option flags:
	 ENotAnyChange    - the client is specifying availability score thresholds he wishes to be notified on crossing;
	 EAnyChange       - the client is interestes in any change in the availability. Only change in calculated score will be reported.
	 EAnyNestedChamge - the client is interested in any change in the availability even if it does not affect the calculated score
	                    e.g.: one of many available access points ceases to be available (and doesn't affect the score calculated against the remaning access points) 
	 */
	enum TChangeScopeFlags
		{
		ENotAnyChange = 0,
		EAnyChange,
		EAnyNestedChange,
		};
	
	/**
	@param aAnyChange If ETrue, notify on all changes in the calculated availability. If EFalse, use following parameters to decide what to notify on.
	@param aScoreExceeds Notify if availability score exceeds this
	@param aScoreDropsBelow Notify if availability score drops below this
	@param aScoreIncreasesByMoreThan Notify if availability score increased by more than this
	@param aScoreDescreasesByMoreThan Notify if availability score decreased by more than this
	*/
	TAvailabilitySubscriptionOptions(TBool aAnyChange = ETrue,
	                                 TUint aScoreExceeds = 0,
	                                 TUint aScoreDropsBelow = 0,
	                                 TUint aScoreIncreasesByMoreThan = 0,
	                                 TUint aScoreDecreasesByMoreThan = 0)
	:	iScoreExceeds(aScoreExceeds),
		iScoreDropsBelow(aScoreDropsBelow),
		iScoreIncreasesByMoreThan(aScoreIncreasesByMoreThan),
		iScoreDecreasesByMoreThan(aScoreDecreasesByMoreThan),
		iFlags(aAnyChange? EAnyChange : ENotAnyChange)
		{
		__ASSERT_DEBUG(aScoreExceeds >= TAvailabilityStatus::EMinAvailabilityScore && aScoreExceeds <= TAvailabilityStatus::EMaxAvailabilityScore, User::Panic(KSpecAssert_ESock_Availabili, 2));
		__ASSERT_DEBUG(aScoreDropsBelow >= TAvailabilityStatus::EMinAvailabilityScore && aScoreDropsBelow <= TAvailabilityStatus::EMaxAvailabilityScore, User::Panic(KSpecAssert_ESock_Availabili, 3));
		__ASSERT_DEBUG(aScoreIncreasesByMoreThan >= TAvailabilityStatus::EMinAvailabilityScore && aScoreIncreasesByMoreThan <= TAvailabilityStatus::EMaxAvailabilityScore, User::Panic(KSpecAssert_ESock_Availabili, 4));
		__ASSERT_DEBUG(aScoreDecreasesByMoreThan >= TAvailabilityStatus::EMinAvailabilityScore && aScoreDecreasesByMoreThan <= TAvailabilityStatus::EMaxAvailabilityScore, User::Panic(KSpecAssert_ESock_Availabili, 5));
		}
	

	
	/**
	@param aFlags, TChangeScopeFlags describing the subscrption.
	@see TChangeScopeFlags.
	*/
	TAvailabilitySubscriptionOptions(TChangeScopeFlags aFlags)
	:	iScoreExceeds(0),
		iScoreDropsBelow(0),
		iScoreIncreasesByMoreThan(0),
		iScoreDecreasesByMoreThan(0),
		iFlags(aFlags)
		{
		ASSERT(aFlags != ENotAnyChange); //illegal to ask for thersholds without specifying them.
		}

	/**
	@param aReported Currently reported availability
	@param aNew Newly reported availability
	@return ETrue if the change is one which should be reported
	*/
	TBool IsChangeSignificant(const TAvailabilityStatus& aReported, const TAvailabilityStatus& aNew) const
		{
		__ASSERT_DEBUG(aNew.IsKnown(), User::Panic(KSpecAssert_ESock_Availabili, 6)); //Use only with known availability!
		return ShouldNotifyAnyChange(aReported, aNew)
			|| ShouldNotifyScoreExceeds(aReported, aNew)
			|| ShouldNotifyScoreDropsBelow(aReported, aNew)
			|| ShouldNotifyScoreIncreasesByMoreThan(aReported, aNew)
			|| ShouldNotifyScoreDecreasesByMoreThan(aReported, aNew);
		}

private:
	TBool ShouldNotifyAnyChange(const TAvailabilityStatus& aReported, const TAvailabilityStatus& aNew) const
		{
		return (iFlags == EAnyNestedChange) || 
			   (iFlags == EAnyChange && aNew.Score() != aReported.Score());
		}

	TBool ShouldNotifyScoreExceeds(const TAvailabilityStatus& aReported, const TAvailabilityStatus& aNew) const
		{
		return iScoreExceeds != 0 && aNew.Score() > iScoreExceeds && aReported.Score() <= iScoreExceeds;
		}

	TBool ShouldNotifyScoreDropsBelow(const TAvailabilityStatus& aReported, const TAvailabilityStatus& aNew) const
		{
		return iScoreDropsBelow != 0 && aNew.Score() < iScoreDropsBelow && aReported.Score() >= iScoreDropsBelow;
		}

	TBool ShouldNotifyScoreIncreasesByMoreThan(const TAvailabilityStatus& aReported, const TAvailabilityStatus& aNew) const
		{
		return iScoreIncreasesByMoreThan != 0 && (aNew.Score() - aReported.Score()) > iScoreIncreasesByMoreThan;
		}

	TBool ShouldNotifyScoreDecreasesByMoreThan(const TAvailabilityStatus& aReported, const TAvailabilityStatus& aNew) const
		{
		return iScoreDecreasesByMoreThan != 0 && (aReported.Score() - aNew.Score()) > iScoreDecreasesByMoreThan;
		}

private:
	TUint iScoreExceeds:7;
	TUint iScoreDropsBelow:7;
	TUint iScoreIncreasesByMoreThan:7;
	TUint iScoreDecreasesByMoreThan:7;
	TUint iFlags:4;
	};


} //namespace ESock



#endif // SYMBIAN_AVAILABILITY_H


