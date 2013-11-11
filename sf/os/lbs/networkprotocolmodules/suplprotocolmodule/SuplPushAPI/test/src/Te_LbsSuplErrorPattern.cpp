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
// An utility classes for specifying and applying the error pattern
// 
//


#include "Te_LbsSuplErrorPattern.h"


/**
The internal implementation of the RTe_LbsSuplErrorPattern class.

@see RTe_LbsSuplErrorPattern
*/

class CTe_LbsSuplErrorPattern : public CBase
	{
private:
	
	/**
	The utility nested T-class for storing the error specification.
	*/
	class TErrorSpec
		{
	public:	
		TErrorSpec(TInt aFunction, TInt aCount, TInt aError, TBool aLeave);

	public:
		/** The function id on calling which the error may happen */
		TInt iFunction;
		/** Number of func calls (including the current) after which the error happens */
		TInt iCount;
		/** The error code */
		TInt iError;
		/** If the function must leave or just return the error code */
		TBool iLeave;
		};
		
public:		
	static CTe_LbsSuplErrorPattern* NewL();
	virtual ~CTe_LbsSuplErrorPattern();

	void Reset();
	void AppendNextErrorL(TInt aFunction, TInt aCount, TInt aError, TBool aLeave);
	inline TInt ErrorReached();
		
	TInt CheckNextErrorL(TInt aFunction);
	
private:
	CTe_LbsSuplErrorPattern();

private:
	/** The array of the error specs */
	RArray<TErrorSpec> iErrors;	
	/** How many errors occured since the error pattern was reseted the last time */
	TInt iErrorReached;
	};
	
/**
Constructor.

@param aFunction [In] The function id on calling which the error may happen.
@param aCount	 [In] Number of func calls (including the current) after which the error happens
@param aError	 [In] A error code
@param aLeave	 [In] If the function must leave or just return the error code
*/
CTe_LbsSuplErrorPattern::TErrorSpec::TErrorSpec(TInt aFunction, TInt aCount, TInt aError, TBool aLeave) :
iFunction(aFunction), iCount(aCount), iError(aError), iLeave(aLeave)
	{
	}

/**
Creates an instance of the CTe_LbsSuplErrorPattern class.

@return An instance of the class. The calling object becomes the
owner of the returned instance and is responsible its disposal.
*/
CTe_LbsSuplErrorPattern* CTe_LbsSuplErrorPattern::NewL()
	{
	CTe_LbsSuplErrorPattern* self = new (ELeave) CTe_LbsSuplErrorPattern();
	return self;
	}

/**
Constructor.
*/
CTe_LbsSuplErrorPattern::CTe_LbsSuplErrorPattern() 
	{
	//Intentionally left blank
	}


/**
Destructor. Closesthe array of the error specs
*/
CTe_LbsSuplErrorPattern::~CTe_LbsSuplErrorPattern()
	{
	iErrors.Close();
	}

/**
Resets the pattern and prepares it to be filled and run again.
*/
void CTe_LbsSuplErrorPattern::Reset()
	{
	iErrors.Reset();
	iErrorReached = 0;
	}
/**
Appends the next error to the error pattern.

@param aFunction [In] The function id on calling which the error may happen.
@param aCount	 [In] Number of func calls (including the current) after which the error happens
@param aError	 [In] A error code
@param aLeave	 [In] If the function must leave or just return the error code

@leave If a error happens, it leaves with one of the system error codes.
*/
void CTe_LbsSuplErrorPattern::AppendNextErrorL(TInt aFunction, TInt aCount, TInt aError, TBool aLeave)
	{
	__ASSERT_ALWAYS(aCount>0, User::Invariant());
	iErrors.AppendL(TErrorSpec(aFunction, aCount, aError, aLeave));
	}


/**
Returns the number of errors occured up to now since the pattern was reseted.

@return The number of errors occured.
*/ 
inline TInt CTe_LbsSuplErrorPattern::ErrorReached()
	{
	return iErrorReached;
	}


/**
Checks for the function specified if it is time for a error to happen.

@param aFunction [In] The function for that the check is made.

@return If it is time for the error to happen, returns the error code specified. 
Otherwise returns KErrNone.

@leave If it is time for the error to happen and iLeave flag is set to ETrue for the error spec,
leaves with the error code specified. 

@see Te_LbsSuplErrorId
*/	
TInt CTe_LbsSuplErrorPattern::CheckNextErrorL(TInt aFunction)
	{
	if(iErrors.Count()==0)
		{
		return KErrNone;
		}
	
	TErrorSpec& err = iErrors[0];
	if(err.iFunction!=aFunction)
		{
		return KErrNone;
		}
	
	err.iCount--;
	if(err.iCount>0)
		{
		return KErrNone;
		}
	
	iErrorReached++;
	TInt e=err.iError;
	TBool leave = err.iLeave;
	iErrors.Remove(0);
	if(leave)
		{
		User::Leave(e);
		}
	return e;
	}

CTe_LbsSuplErrorPattern* RTe_LbsSuplErrorPattern::iImpl=0;


/**
Opens the error pattern and makes it ready for inserting errors

@leave If a error happens, it leaves with one of the system error codes.

@panic If the code tries to open the pattern more than once without calling Close(), it panics with
a USER 0 panic.

@see RTe_LbsSuplErrorPattern::Close
*/
void RTe_LbsSuplErrorPattern::OpenL()
	{
	__ASSERT_ALWAYS(iImpl==0, User::Invariant());
	iImpl = CTe_LbsSuplErrorPattern::NewL();
	}

/**
Deletes the internal implementation.		
*/
void RTe_LbsSuplErrorPattern::Close()
	{
	delete iImpl;
	iImpl = 0;
	}

/**
Removes all the errors from the pattern and set counter of the reached errors to zero.
*/
void RTe_LbsSuplErrorPattern::Reset()
	{
	__ASSERT_ALWAYS(iImpl!=0, User::Invariant());
	iImpl->Reset();
	}

/**
Appends the next error to the error pattern.

@param aFunction [In] The function id on calling which the error may happen.
@param aCount	 [In] Number of func calls (including the current) after which the error happens
@param aError	 [In] A error code
@param aLeave	 [In] If the function must leave or just return the error code

@leave If a error happens, it leaves with one of the system error codes.
*/
void RTe_LbsSuplErrorPattern::AppendNextErrorL(TInt aFunction, TInt aCount, TInt aError, TInt aLeave)
	{
	__ASSERT_ALWAYS(iImpl!=0, User::Invariant());
	iImpl->AppendNextErrorL(aFunction,aCount,aError,aLeave);
	}

/**
Returns the number of errors occured up to now since the pattern was reseted.

@return The number of errors occured.
*/ 
TInt RTe_LbsSuplErrorPattern::ErrorReached()
	{
	__ASSERT_ALWAYS(iImpl!=0, User::Invariant());
	return iImpl->ErrorReached();
	}

/**
Checks for the function specified if it is time for a error to happen.

@param aFunction [In] The function for that the check is made.

@return If it is time for the error to happen, returns the error code specified. 
Otherwise returns KErrNone.

@leave If it is time for the error to happen and iLeave flag is set to ETrue for the error spec,
leaves with the error code specified. 

@see Te_LbsSuplErrorId
*/
TInt RTe_LbsSuplErrorPattern::CheckNextErrorL(TInt aFunction)
	{
	if(iImpl)
		{
		return iImpl->CheckNextErrorL(aFunction);
		}
	return KErrNone;
	}

/**
Non-leaving version of RTe_LbsSuplErrorPattern::CheckNextErrorL.

@param aFunction [In] The function for that the check is made.

@return If it is time for the error to happen, returns the error code specified. 
Otherwise returns KErrNone. 

@see RTe_LbsSuplErrorPattern::CheckNextErrorL
*/
TInt RTe_LbsSuplErrorPattern::CheckNextError(TInt aFunction)
	{
	TInt err=KErrNone;
	TRAPD(leave, err=CheckNextErrorL(aFunction));
	__ASSERT_ALWAYS(leave==KErrNone, User::Invariant());
	return err;
	}
