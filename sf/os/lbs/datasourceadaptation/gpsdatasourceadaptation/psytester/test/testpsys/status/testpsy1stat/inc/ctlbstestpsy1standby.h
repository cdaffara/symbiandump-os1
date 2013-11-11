/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CT_LBS_TESTPSY1STANDBY_H__
#define __CT_LBS_TESTPSY1STANDBY_H__

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// FORWARD DECLARATIONS


// CLASS DECLARATION
class CT_LbsTestPsyBase;

class CT_LbsTestPsy1Standby : public CActive
    {
   public:  // Constructors and destructor

    /**
	* Two-phased constructor.
	*/
    static CT_LbsTestPsy1Standby* NewL(CT_LbsTestPsyBase& aPsyBase);

    /**
	* Two-phased constructor.
	*/
    static CT_LbsTestPsy1Standby* NewLC(CT_LbsTestPsyBase& aPsyBase);

    /**
	* Destructor.
	*/
    ~CT_LbsTestPsy1Standby();

	 public: // New methods
		 
	/**
	* Starts timer
	*/ 
	void Start();

	TBool IsStandby();

    public:  // Functions from base class
		
	/**
	* From CActive. Handles an active object’s request completion
	* event.
	*/
        void RunL();

    /**
	* From CActive. Implements cancellation of an outstanding
	* request
	*/
    void DoCancel();

    private:

    /**
          * C++ default constructor.
	*/
    CT_LbsTestPsy1Standby(CT_LbsTestPsyBase& aPsyBase);
		
	/**
	* By default EPOC constructor is private.
	*/
    void ConstructL();

    // By default, prohibit copy constructor
    CT_LbsTestPsy1Standby( const CT_LbsTestPsy1Standby& );
    // Prohibit assigment operator
    CT_LbsTestPsy1Standby& operator= ( const CT_LbsTestPsy1Standby& );

	 private: // Data

		CT_LbsTestPsyBase&	iPsyBase;
		RTimer				iTimer;
		TBool				iStandby;
    };

#endif      // __CT_LBS_TESTPSY1STANDBY_H__
            
// End of File
