/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#ifndef TC_TESTSCHEDULER_H
#define TC_TESTSCHEDULER_H

// INCLUDES
#include <e32base.h>
#include <e32std.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Activeobject that is used to break scheduling on timeout
*
*/
class CTimeout : public CActive
{
public:	

	// returns true if timeout has expired
	TBool Timed() const
	{
		return iTimed;
	}
    
    /**
    * Constructor.
    */
    CTimeout();

	/**
    * Destructor.
    */
	~CTimeout();

	/**
    * Starts waiting.
	* 
    * @param  TInt aTimeoutInMilliSecs: in: timeout in milliseconds
    * @return none
    */
	void Start(TInt aTimeoutInMilliSecs); 
		
	/**
    * Cancels waiting.
	* 
    * @param  none
    * @return none
    */
	virtual void DoCancel();

	/**
    * Stops ActiveScheduler and sets iTimed to true.
	* 
    * @param  none
    * @return none
    */
	virtual void RunL();
        
protected:  // New functions
        
 
protected:  // Functions from base classes

 
private:

	/**
    * C++ default constructor.
    */
    //Cvibractrltest();

	/**
    * By default Symbian 2nd phase constructor is private.
    */
    //void ConstructL();

	
public:     // Data
    

protected:  // Data


private:    // Data

	// Timer
	RTimer iTimer;

	// notifies is the timer stopped by timeout
	TBool  iTimed;


public:     // Friend classes


protected:  // Friend classes


private:    // Friend classes  
	
};

/**
*  Controls the handling of asynchronous requests as represented by active objects.
*
*/
class CTestScheduler : public CActiveScheduler
{	
protected:


private:


public:	
    
    /**
    * Constructor.
    */
    CTestScheduler();

	/**
    * Destructor.
    */
	virtual ~CTestScheduler();

	/**
    * Waits for any request from user
	* 
    * @param  none
    * @return none
    */
	virtual void WaitForAnyRequest(); // jlof 13-12-2001
	
	/**
    * Handles the result of a leave occurring in an active object's RunL() function.
	* 
    * @param  TInt anError: error reason
    * @return none
    */
	virtual void Error(TInt anError) const;

	/**
    * Start scheduler for given amount of time
	* 
    * @param  TInt aMillisecs: time to run the scheduler 
    * @return none
    */
	static TBool StartAndStopL( TInt aMillisecs );
	
	/**
    * Two-phased constructor.
    */
	static CTestScheduler* NewL();

        
protected:  // New functions
        
 
protected:  // Functions from base classes

 
private:

	/**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

	
public:     // Data
    

protected:  // Data


private:    // Data

	//timer
	CTimeout *iTimeout;


public:     // Friend classes


protected:  // Friend classes


private:    // Friend classes  

};


#endif // TC_CTESTSCHEDULER_H
