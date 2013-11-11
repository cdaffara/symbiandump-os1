/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Module test's header file for CSensorDataCompensator API.
*
*/



#ifndef SENSORDATACOMPENSATORTEST_H
#define SENSORDATACOMPENSATORTEST_H

// INCLUDES
#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#include <testscripterinternal.h>
#include <sensordatacompensator.h>
#include <sensrvaccelerometersensor.h>
#include <sensrvorientationsensor.h>
#include <sensrvmagnetometersensor.h>
#include <sensrvmagneticnorthsensor.h>
#include <sensrvtappingsensor.h>
#include "sensordatacompensatortesttypes.h"

class CSensorDataCompensatorTest;
class TCaseInfo;

// DESCRIPTION
// This a Test Module interface template
// that does not really do anything.
typedef TInt (CSensorDataCompensatorTest::* TestFunction)(TTestResult&);

NONSHARABLE_CLASS( CSensorDataCompensatorTest ) : public CScriptBase
    {
public: // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CSensorDataCompensatorTest* NewL( CTestModuleIf& aTestModuleIf );

    /**
    * Destructor.
    */
    virtual ~CSensorDataCompensatorTest();

public: // Functions from base classes

    TInt RunMethodL( CStifItemParser& aItem );

protected: // New functions

    TInt CreateCompensatorL( CStifItemParser& aItem );
    TInt DestroyCompensatorL( CStifItemParser& aItem );
    TInt SetStateL( CStifItemParser& aItem );
    TInt AppendDataItemL( CStifItemParser& aItem );
    TInt ResetDataItemsL( CStifItemParser& aItem );
    TInt CompensateAndVerifyDataItemsL( CStifItemParser& aItem );

private:

    /**
    * C++ default constructor.
    */
    CSensorDataCompensatorTest( CTestModuleIf& aTestModuleIf );

    /**
    * By default Symbian OS constructor is private.
    */
    void ConstructL();

    /**
    * Function returning test case name and pointer to test case function
    */
    const TCaseInfo Case ( const TInt aCaseNumber ) const;

    void ResetAllDataItems();

    void CompensateDataAndCheckErrorL( TDes8& aDes, TInt aCount );

private: // Data

    // Pointer of a SensorDataCompensator.
    CSensorDataCompensator* iCompensatorApi;

    // TInt iDataType;
    TSensrvChannelDataTypeId iDataType;

    TSensrvAccelerometerAxisData    iInputAccelerometerAxisData;
    TSensrvAccelerometerAxisData    iExpectedAccelerometerAxisData;

    TSensrvMagnetometerAxisData iInputMagnetometerAxisData;
    TSensrvMagnetometerAxisData iExpectedMagnetometerAxisData;

    TSensrvMagneticNorthData        iInputMagneticNorthData;
    TSensrvMagneticNorthData        iExpectedMagneticNorthData;

    TSensrvTappingData              iInputTappingData;
    TSensrvTappingData              iExpectedTappingData;

    HBufC8* iInputCrap;
    TInt iExpectedError;
    TInt iOutputDataThreshold;

    };

// Function pointer related internal definitions
#ifndef __GCC32__
    #define GETPTR
#else
    #define GETPTR &
#endif


// An internal structure containing a test case name and
// the pointer to function doing the test
class TCaseInfoInternal
    {
    public:
        const TText* iCaseName;
        TestFunction iMethod;
    };

// An internal structure containing a test case name and
// the pointer to function doing the test
class TCaseInfo
    {
    public:
        TPtrC iCaseName;
        TestFunction iMethod;

    TCaseInfo( const TText* a ) : iCaseName( (TText*) a )
        {
        };

    };


#endif // SENSORDATACOMPENSATORTEST_H

// End of File
