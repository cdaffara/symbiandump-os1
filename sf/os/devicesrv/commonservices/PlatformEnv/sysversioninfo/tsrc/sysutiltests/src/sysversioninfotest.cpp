/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Tests of sysutil version information
*
*/


// INCLUDE FILES
#include <sysversioninfo.h>
#include <stiftestinterface.h>
#include "sysutiltests.h"
#include <e32svr.h>
#include <e32base.h>
#include <f32file.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
_LIT( KModelVersionFileName, "Z:\\resource\\versions\\model.txt" );
_LIT( KFWIDVersionFileName, "Z:\\resource\\versions\\fwid*.txt" );
_LIT( KOPIDVersionFileName, "Z:\\resource\\versions\\customersw.txt" );
_LIT( KProductVersionFile, "Z:\\resource\\versions\\product.txt" );
_LIT( KPlatformVersionFile, "Z:\\resource\\versions\\platform.txt" );

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES
static TInt DoTest(
        const TDesC& aFile,
        const SysVersionInfo::TVersionInfoType aType,
        TDes& aDes,
        TInt& aFileErr )
    {
    TInt fileErr( KErrNone );
    if ( aFile.Length() )
        {
        fileErr = CSysUtiltests::CheckIsFileExist( aFile );
        }
    TInt ret( SysVersionInfo::GetVersionInfo( aType, aDes ) );
    if ( fileErr != KErrNone && ret == fileErr )
        {
        ret = KErrNone;
        }
    aFileErr = fileErr;
    return ret;
    }

static TInt DoTest(
        const TDesC& aFile,
        const SysVersionInfo::TVersionInfoType aType,
        TDes& aDes,
        TInt& aFileErr,
        RFs& aFs )
    {
    TInt fileErr( KErrNone );
    if ( aFile.Length() )
        {
        fileErr = CSysUtiltests::CheckIsFileExist( aFile );
        }
    TInt ret( SysVersionInfo::GetVersionInfo( aType, aDes, aFs ) );
    if ( fileErr != KErrNone && ret == fileErr )
        {
        ret = KErrNone;
        }
    aFileErr = fileErr;
    return ret;
    }

static TInt DoTestWild(
        const TDesC& aFile,
        const SysVersionInfo::TVersionInfoType aType,
        TDes& aDes,
        TInt& aFileErr,
        RFs& aFs )
    {
    TFindFile find( aFs );
    CDir* files = NULL;
    TInt fileErr( find.FindWildByPath( aFile, NULL, files ) );
    if ( fileErr == KErrNone && files )
        {
        if ( !files->Count() )
            {
            fileErr = KErrNotSupported;
            }
        }
    if ( fileErr == KErrNotFound || fileErr == KErrPathNotFound )
        {
        fileErr = KErrNotSupported;
        }
    delete files;

    TInt ret( SysVersionInfo::GetVersionInfo( aType, aDes, aFs ) );
    if ( fileErr != KErrNone && ret == fileErr )
        {
        ret = KErrNone;
        }
    aFileErr = fileErr;
    return ret;
    }

// FORWARD DECLARATIONS
// ---------------------------------------------------------
// CSysUtiltests::GetVersionInfo
// Returns: KErrNone
// ---------------------------------------------------------
TInt CSysUtiltests::GetVersionInfo( TTestResult& aResult )
{
	TInt err=KErrNone;
	TInt fileErr=KErrNone;

	TestModuleIf().Printf( 0, _L("RunTestCase"), 
	    _L("GetVersionInfo"));

    TBuf<1> tooSmallBuffer;
    TBuf<128> buffer;

    if ( !err )
        {
        err = DoTest(
            KOPIDVersionFileName, SysVersionInfo::EOPVersion, tooSmallBuffer, fileErr );
        if ( err != KErrTooBig && fileErr != KErrNotSupported )
            {
            err = KErrGeneral;
            }
        else
            {
            err = KErrNone;
            }
        }
    if ( !err )
        {
        err = DoTest(
            KOPIDVersionFileName, SysVersionInfo::EOPVersion, buffer, fileErr );
        if ( err )
            {
            err = KErrGeneral;
            }
        }

    if ( !err )
        {
        err = DoTest(
            KModelVersionFileName, SysVersionInfo::EModelVersion, buffer, fileErr );
        if ( err )
            {
            err = KErrGeneral;
            }
        }

	if(err)
		{
		aResult.iResultDes.Copy( _L("GetVersionInfo failed") );
		aResult.iResult = err;
		}
	return KErrNone;
}

// ---------------------------------------------------------
// CSysUtiltests::GetVersionInfo2
// Returns: KErrNone
// ---------------------------------------------------------
TInt CSysUtiltests::GetVersionInfo2( TTestResult& aResult )
{

	TInt err=KErrNone;
	TInt fileErr=KErrNone;

	TestModuleIf().Printf( 0, _L("RunTestCase"), 
	    _L("GetVersionInfo2"));
	    
    TBuf<1> tooSmallBuffer;
    TBuf<128> buffer;
    
    if ( !err )
        {
        err = DoTestWild(
            KFWIDVersionFileName, SysVersionInfo::EFWVersion, tooSmallBuffer, fileErr, iFs );
        if ( err != KErrTooBig && fileErr != KErrNotSupported )
            {
            err = KErrGeneral;
            }
        else
            {
            err = KErrNone;
            }
        }
    if ( !err )
        {
        err = DoTestWild(
            KFWIDVersionFileName, SysVersionInfo::EFWVersion, buffer, fileErr, iFs );
        if ( err )
            {
            err = KErrGeneral;
            }
        }

    if ( !err )
        {
        err = DoTest(
            KOPIDVersionFileName, SysVersionInfo::EOPVersion, tooSmallBuffer, fileErr, iFs );
        if ( err != KErrTooBig && fileErr != KErrNotSupported )
            {
            err = KErrGeneral;
            }
        else
            {
            err = KErrNone;
            }
        }
    if ( !err )
        {
        err = DoTest(
            KOPIDVersionFileName, SysVersionInfo::EOPVersion, buffer, fileErr, iFs );
        if ( err )
            {
            err = KErrGeneral;
            }
        }

    if ( !err )
        {
        err = DoTest(
            KModelVersionFileName, SysVersionInfo::EModelVersion, buffer, fileErr, iFs );
        if ( err )
            {
            err = KErrGeneral;
            }
        }

	if(err)
		{
		aResult.iResultDes.Copy( _L("GetVersionInfo2 failed") );
		aResult.iResult = err;
		}
	return KErrNone;
}


// ---------------------------------------------------------
// CSysUtiltests::GetOSVersionInfo
// Returns: KErrNone
// ---------------------------------------------------------
TInt CSysUtiltests::GetOSVersionInfo( TTestResult& aResult )
    {
    TestModuleIf().Printf( 0, _L("RunTestCase"),
        _L("SysUtil::GetOSVersionInfo"));

    TInt fileErr = CheckIsFileExist( KPlatformVersionFile );
    if ( fileErr == KErrNotFound )
        {
        fileErr = KErrNotSupported;
        }

    aResult.iResult = KErrNone;
    SysVersionInfo::TSymbianOSVersion osVersion;
    TInt err( SysVersionInfo::GetVersionInfo( osVersion ) );

    if ( err == KErrNone )
        {
        if ( !osVersion.iMajorVersion || !osVersion.iMinorVersion )
            {
            err = KErrGeneral;
            }
        }

    if ( err != KErrNone && fileErr != err )
        {
        aResult.iResultDes.Copy( _L("GetOSVersionInfo failed ") );
        aResult.iResult = err;
        }
    return KErrNone;
    }

// ---------------------------------------------------------
// CSysUtiltests::GetOSVersionInfo
// Returns: KErrNone
// ---------------------------------------------------------
TInt CSysUtiltests::GetProductVersionInfo( TTestResult& aResult )
    {
    TestModuleIf().Printf( 0, _L("RunTestCase"),
        _L("SysUtil::GetProductVersionInfo"));

    aResult.iResult = KErrNone;
    TInt fileErr = CheckIsFileExist( KProductVersionFile );
    if ( fileErr == KErrNotFound )
        {
        fileErr = KErrNotSupported;
        }
    SysVersionInfo::TProductVersion productVersion;
    TInt err( SysVersionInfo::GetVersionInfo( productVersion ) );

    if ( err == KErrNone )
        {
        if ( !productVersion.iManufacturer.Length() ||
             !productVersion.iModel.Length() ||
             !productVersion.iProduct.Length() ||
             !productVersion.iRevision.Length() )
            {
            err = KErrGeneral;
            }
        }

    if ( err != KErrNone && fileErr != err )
        {
        aResult.iResultDes.Copy( _L("GetProductVersionInfo failed ") );
        aResult.iResult = err;
        }
    return KErrNone;
    }

// ---------------------------------------------------------
// CSysUtiltests::GetOSVersionInfo2
// Returns: KErrNone
// ---------------------------------------------------------
TInt CSysUtiltests::GetOSVersionInfo2( TTestResult& aResult )
    {
    TestModuleIf().Printf( 0, _L("RunTestCase"),
        _L("SysUtil::GetOSVersionInfo2"));

    TInt fileErr = CheckIsFileExist( KPlatformVersionFile );
    if ( fileErr == KErrNotFound )
        {
        fileErr = KErrNotSupported;
        }

    aResult.iResult = KErrNone;
    SysVersionInfo::TSymbianOSVersion osVersion;
    TInt err( SysVersionInfo::GetVersionInfo( osVersion, iFs ) );

    if ( err == KErrNone )
        {
        if ( !osVersion.iMajorVersion || !osVersion.iMinorVersion )
            {
            err = KErrGeneral;
            }
        }

    if ( err != KErrNone && fileErr != err )
        {
        aResult.iResultDes.Copy( _L("GetOSVersionInfo2 failed ") );
        aResult.iResult = err;
        }
    return KErrNone;
    }

// ---------------------------------------------------------
// CSysUtiltests::GetOSVersionInfo2
// Returns: KErrNone
// ---------------------------------------------------------
TInt CSysUtiltests::GetProductVersionInfo2( TTestResult& aResult )
    {
    TestModuleIf().Printf( 0, _L("RunTestCase"),
        _L("SysUtil::GetProductVersionInfo2"));

    aResult.iResult = KErrNone;
    TInt fileErr = CheckIsFileExist( KProductVersionFile );
    if ( fileErr == KErrNotFound )
        {
        fileErr = KErrNotSupported;
        }
    SysVersionInfo::TProductVersion productVersion;
    TInt err( SysVersionInfo::GetVersionInfo( productVersion, iFs ) );

    if ( err == KErrNone )
        {
        if ( !productVersion.iManufacturer.Length() ||
             !productVersion.iModel.Length() ||
             !productVersion.iProduct.Length() ||
             !productVersion.iRevision.Length() )
            {
            err = KErrGeneral;
            }
        }

    if ( err != KErrNone && fileErr != err )
        {
        aResult.iResultDes.Copy( _L("GetProductVersionInfo2 failed ") );
        aResult.iResult = err;
        }
    return KErrNone;
    }

// ---------------------------------------------------------
// CSysUtiltests::GetProductCode
// Returns: KErrNone
// ---------------------------------------------------------
TInt CSysUtiltests::GetProductCode( TTestResult& aResult )
    {
    TInt err=KErrNone;

    TestModuleIf().Printf( 0, _L("RunTestCase"), _L("GetProductCode"));

    TBuf<1> tooSmallBuffer;
    TBuf<128> buffer;

    if ( !err )
        {
        err = SysVersionInfo::GetVersionInfo(  SysVersionInfo::EProductCode, tooSmallBuffer );
  
        if ( err == KErrTooBig || err == KErrNotSupported )
            {
            err = KErrNone;
            }
        else
            {
            err = KErrGeneral;
            }
        }
    
    if ( !err )
        {
        err =  SysVersionInfo::GetVersionInfo(  SysVersionInfo::EProductCode, buffer );
        
        if ( err && err != KErrNotSupported )
            {
            err = KErrGeneral;
            }
        else
            {
            err = KErrNone;
            }
        }

    if( err )
        {
        aResult.iResultDes.Copy( _L("GetProductCode failed") );
        aResult.iResult = err;
        }
    else
        {
        aResult.iResultDes.Format( _L("Get productCode success: code=%S, length=%d"), &buffer, buffer.Length());
        }
    return KErrNone;
    }

// End of File
