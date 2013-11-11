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
* Description:   ?Description
*
*/



// INCLUDE FILES
#include <e32math.h>
#include <f32file.h>
#include "PlatformEnvTest.h"
#include <PathInfo.h>
#include <DriveInfo.h>

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPlatformEnvTest::Case
// Returns a test case by number.
//
// This function contains an array of all available test cases 
// i.e pair of case name and test function. If case specified by parameter
// aCaseNumber is found from array, then that item is returned.
// 
// The reason for this rather complicated function is to specify all the
// test cases only in one place. It is not necessary to understand how
// function pointers to class member functions works when adding new test
// cases. See function body for instructions how to add new test case.
// -----------------------------------------------------------------------------
//
const TCaseInfo CPlatformEnvTest::Case ( 
    const TInt aCaseNumber ) const 
     {

    /**
    * To add new test cases, implement new test case function and add new 
    * line to KCases array specify the name of the case and the function 
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to 
    *    PlatformEnvTest.cpp file and to PlatformEnvTest.h 
    *    header file.
    *
    * 2) Add entry to following KCases array either by using:
    *
    * 2.1: FUNCENTRY or ENTRY macro
    * ENTRY macro takes two parameters: test case name and test case 
    * function name.
    *
    * FUNCENTRY macro takes only test case function name as a parameter and
    * uses that as a test case name and test case function name.
    *
    * Or
    *
    * 2.2: OOM_FUNCENTRY or OOM_ENTRY macro. Note that these macros are used
    * only with OOM (Out-Of-Memory) testing!
    *
    * OOM_ENTRY macro takes five parameters: test case name, test case 
    * function name, TBool which specifies is method supposed to be run using
    * OOM conditions, TInt value for first heap memory allocation failure and 
    * TInt value for last heap memory allocation failure.
    * 
    * OOM_FUNCENTRY macro takes test case function name as a parameter and uses
    * that as a test case name, TBool which specifies is method supposed to be
    * run using OOM conditions, TInt value for first heap memory allocation 
    * failure and TInt value for last heap memory allocation failure. 
    */ 

    static TCaseInfoInternal const KCases[] =
        {
        // To add new test cases, add new items to this array
        //FUNCENTRY( PrintTest ),
        ENTRY( "[PLATFORMENV] PathTest", PathTest ),
        ENTRY( "[PLATFORMENV] GetPathTest", GetPathTest ),
        
        
        ENTRY( "[PLATFORMENV] GetRootPathTest", GetRootPathTest ),
        ENTRY( "[PLATFORMENV] GetFullPathTest", GetFullPathTest ),
        ENTRY( "[PLATFORMENV] PathTypeTest", PathTypeTest ),
        ENTRY( "[PLATFORMENV] GetListOfPathsTest", GetListOfPathsTest ),
        ENTRY( "[PLATFORMENV] GetDefaultDriveTest", GetDefaultDriveTest ),
        ENTRY( "[PLATFORMENV] GetDefaultDriveTest2", GetDefaultDriveTest2 ),
        ENTRY( "[PLATFORMENV] GetDriveStatusTest", GetDriveStatusTest ),
        ENTRY( "[PLATFORMENV] GetUserVisibleDrivesTest", GetUserVisibleDrivesTest ),
        ENTRY( "[PLATFORMENV] StripUserHiddenDrivesTest", StripUserHiddenDrivesTest ),
        ENTRY( "[PLATFORMENV] DriveCountTest", DriveCountTest ),
        ENTRY( "[PLATFORMENV] DriveArrayTest", DriveArrayTest ),
        ENTRY( "[PLATFORMENV] GetUserVisibleDrivesTest2", GetUserVisibleDrivesTest2 )
                
        // Example how to use OOM functionality
        //OOM_ENTRY( "Loop test with OOM", LoopTest, ETrue, 2, 3),
        //OOM_FUNCENTRY( PrintTest, ETrue, 1, 3 ),
        };

    // Verify that case number is valid
    if( (TUint) aCaseNumber >= sizeof( KCases ) / 
                               sizeof( TCaseInfoInternal ) )
        {
        // Invalid case, construct empty object
        TCaseInfo null( (const TText*) L"" );
        null.iMethod = NULL;
        null.iIsOOMTest = EFalse;
        null.iFirstMemoryAllocation = 0;
        null.iLastMemoryAllocation = 0;
        return null;
        } 

    // Construct TCaseInfo object and return it
    TCaseInfo tmp ( KCases[ aCaseNumber ].iCaseName );
    tmp.iMethod = KCases[ aCaseNumber ].iMethod;
    tmp.iIsOOMTest = KCases[ aCaseNumber ].iIsOOMTest;
    tmp.iFirstMemoryAllocation = KCases[ aCaseNumber ].iFirstMemoryAllocation;
    tmp.iLastMemoryAllocation = KCases[ aCaseNumber ].iLastMemoryAllocation;
    return tmp;

    }

// -----------------------------------------------------------------------------
// CPlatformEnvTest::PrintTest
// Simple printing to UI test.
// -----------------------------------------------------------------------------
//
TInt CPlatformEnvTest::PrintTest( 
    TTestResult& aResult )
    {
     /* Simple print test */
    _LIT( KPrintTest, "PrintTest" );
    _LIT( KEnter, "Enter" );
    _LIT( KOnGoing, "On-going" );
    _LIT( KExit, "Exit" );

    TestModuleIf().Printf( 0, KPrintTest, KEnter );
           
    TestModuleIf().Printf( 1, KPrintTest, KOnGoing );
    
    TestModuleIf().Printf( 0, KPrintTest, KExit );

    // Test case passed

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "PrintTest passed" );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CPlatformEnvTest::LoopTest
// Another printing to UI test.
// -----------------------------------------------------------------------------
//
TInt CPlatformEnvTest::LoopTest( TTestResult& aResult )
    {

    /* Simple print and wait loop */
    _LIT( KState, "State" );
    _LIT( KLooping, "Looping" );

    TestModuleIf().Printf( 0, KState, KLooping );

    _LIT( KRunning, "Running" );
    _LIT( KLoop, "%d" );
    for( TInt i=0; i<10; i++)
        {
        TestModuleIf().Printf( 1, KRunning, KLoop, i);
        User::After( 1000000 );
        }

    _LIT( KFinished, "Finished" );
    TestModuleIf().Printf( 0, KState, KFinished );

    // Test case passed

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "LoopTest passed" );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;

    }


// -----------------------------------------------------------------------------
// CPlatformEnvTest::PathTest
//
// -----------------------------------------------------------------------------
//
TInt CPlatformEnvTest::PathTest( TTestResult& aResult )
    {

	TBuf<50> iRomBootPath,
	iPhoneMemoryRootPath,
	iMemoryCardRootPath,
	iGamesPath,
	iInstallsPath,
	iOthersPath,
	iVideosPath,
	iImagesPath,
	iPicturesPath,
	iGmsPicturesPath,
	iMmsBackgroundImagesPath,
	iPresenceLogosPath,
	iSoundsPath,
	iDigitalSoundsPath,
	iSimpleSoundsPath,
	iImagesThumbnailPath,
	iMemoryCardContactsPath = _L("");
		
	iRomBootPath = PathInfo::RomRootPath();
	iPhoneMemoryRootPath = PathInfo::PhoneMemoryRootPath();
	iMemoryCardRootPath = PathInfo::MemoryCardRootPath();
	iGamesPath = PathInfo::GamesPath();
	iInstallsPath = PathInfo::InstallsPath();
	iOthersPath = PathInfo::OthersPath();
	iVideosPath = PathInfo::VideosPath();
	iImagesPath = PathInfo::ImagesPath();
	iPicturesPath = PathInfo::PicturesPath();
	iGmsPicturesPath = PathInfo::GmsPicturesPath();
	iMmsBackgroundImagesPath = PathInfo::MmsBackgroundImagesPath();
	iPresenceLogosPath = PathInfo::PresenceLogosPath();
	iSoundsPath = PathInfo::SoundsPath();
	iDigitalSoundsPath = PathInfo::DigitalSoundsPath();
	iSimpleSoundsPath = PathInfo::SimpleSoundsPath();
	iImagesThumbnailPath = PathInfo::ImagesThumbnailPath();
	iMemoryCardContactsPath = PathInfo::MemoryCardContactsPath();
	
	iLog->Log( _L( "PlatformEnvTest Paths:" ));
	iLog->Log( iRomBootPath );
	iLog->Log( iPhoneMemoryRootPath );
	iLog->Log( iMemoryCardRootPath );
	iLog->Log( iGamesPath );
	iLog->Log( iInstallsPath );
	iLog->Log( iOthersPath );
	iLog->Log( iVideosPath );
	iLog->Log( iImagesPath );
	iLog->Log( iPicturesPath );
	iLog->Log( iGmsPicturesPath );
	iLog->Log( iMmsBackgroundImagesPath );
	iLog->Log( iPresenceLogosPath );
	iLog->Log( iSoundsPath );
	iLog->Log( iDigitalSoundsPath );
	iLog->Log( iSimpleSoundsPath );
	iLog->Log( iImagesThumbnailPath );
	iLog->Log( iMemoryCardContactsPath );
		
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "Path Test passed" );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;

    }


// -----------------------------------------------------------------------------
// CPlatformEnvTest::GetPathTest
// Testing relations between retrieving paths via direct methods and 
// via GetPath( aPath) method.
// Note: This case results KErrGeneral even if just one of the path pairs differ 
// -----------------------------------------------------------------------------
//
TInt CPlatformEnvTest::GetPathTest( TTestResult& aResult )
    { 

    //Array for paths retrieved via GetPath-method  
    RArray<TFileName> getPaths;
          
    getPaths.Append( PathInfo::GetPath( PathInfo::ERomRootPath ) );             //0
    getPaths.Append( PathInfo::GetPath( PathInfo::EPhoneMemoryRootPath ) );     //1
    getPaths.Append( PathInfo::GetPath( PathInfo::EMemoryCardRootPath ) );      //2
    getPaths.Append( PathInfo::GetPath( PathInfo::EGamesPath ) );               //3
    getPaths.Append( PathInfo::GetPath( PathInfo::EInstallsPath ) );            //4
    getPaths.Append( PathInfo::GetPath( PathInfo::EOthersPath ) );              //5
    getPaths.Append( PathInfo::GetPath( PathInfo::EVideosPath ) );              //6
    getPaths.Append( PathInfo::GetPath( PathInfo::EImagesPath ) );              //7
    getPaths.Append( PathInfo::GetPath( PathInfo::EGsmPicturesPath ) );         //8
    getPaths.Append( PathInfo::GetPath( PathInfo::EMmsBackgroundImagesPath ) ); //9
    getPaths.Append( PathInfo::GetPath( PathInfo::EPresenceLogosPath ) );       //10
    getPaths.Append( PathInfo::GetPath( PathInfo::ESoundsPath ) );              //11
    getPaths.Append( PathInfo::GetPath( PathInfo::EDigitalSoundsPath ) );       //12
    getPaths.Append( PathInfo::GetPath( PathInfo::ESimpleSoundsPath ) );        //13
    getPaths.Append( PathInfo::GetPath( PathInfo::EImagesThumbnailPath ) );     //14
    getPaths.Append( PathInfo::GetPath( PathInfo::EMemoryCardContactsPath ) );  //15
     
    //Array for paths retrieve via direct path functions
    // NOTE!!! Paths in both arrays must be set on corresponding indexes in order to get the test to work    
    RArray<TFileName> funcPaths;
    
    funcPaths.Append( PathInfo::RomRootPath() );            //0
    funcPaths.Append( PathInfo::PhoneMemoryRootPath() );    //1
    funcPaths.Append( PathInfo::MemoryCardRootPath() );     //2
    funcPaths.Append( PathInfo::GamesPath() );              //3
    funcPaths.Append( PathInfo::InstallsPath() );           //4
    funcPaths.Append( PathInfo::OthersPath() );             //5
    funcPaths.Append( PathInfo::VideosPath() );             //6    
    funcPaths.Append( PathInfo::ImagesPath() );             //7
    funcPaths.Append( PathInfo::GmsPicturesPath() );        //8    
    funcPaths.Append( PathInfo::MmsBackgroundImagesPath() );//9
    funcPaths.Append( PathInfo::PresenceLogosPath() );      //10
    funcPaths.Append( PathInfo::SoundsPath() );             //11
    funcPaths.Append( PathInfo::DigitalSoundsPath() );      //12
    funcPaths.Append( PathInfo::SimpleSoundsPath() );       //13
    funcPaths.Append( PathInfo::ImagesThumbnailPath() );    //14
    funcPaths.Append( PathInfo::MemoryCardContactsPath() ); //15
    
    
 
    //if the size of the arrays ain't the same, fail the test
    if( funcPaths.Count() != getPaths.Count() )
        {
        _LIT( KDescription, "GetPathTest::Path Array setup failed" );
        aResult.SetResult( KErrGeneral, KDescription );
        funcPaths.Close();
        getPaths.Close(); 
        return KErrNone;
        }
    
    _LIT( KDescription, "GetPathTest executed. ( Failed indexes: " );
    _LIT( KFailIndex, "%d,");
    _LIT( KTextNone, "NONE )" );
    _LIT( KTextParCl, " )" );
     
    TBuf<100> description;    
    TInt err(KErrNone);
    TInt count = getPaths.Count();
 
    description.Format( KDescription ) ;   
 
    for( TInt i(0); i < count; i++ )
        {
        iLog -> Log(getPaths[i]);
        iLog -> Log(funcPaths[i]);
        
        if( getPaths[i].Compare( funcPaths[i] ) != 0 )
            {
            description.AppendFormat( KFailIndex, i );
            err = KErrGeneral;
            }
        }
 
 
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    if( err == KErrNone )
        {
        description.Append( KTextNone );
        }
    else
        {
        //strip the last comma
        description.Delete( description.Length() -1 , 1);
        description.Append( KTextParCl );
        }
    
    
    aResult.SetResult( err, description );
  
    funcPaths.Close();
    getPaths.Close();     
  
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPlatformEnvTest::GetRootPathTest
// Testing relations between retrieving paths via direct methods and 
// via GetRootPathTest( aPath) method.
// Note: This case results KErrGeneral even if just one of the path pairs differ 
// -----------------------------------------------------------------------------
//
TInt CPlatformEnvTest::GetRootPathTest( TTestResult& aResult )
    { 

    //Array for paths retrieved via GetRootPath-method  
    RArray<TFileName> getPaths;
    TFileName root;

    for ( TInt i( EDriveA ); i <= EDriveZ; ++i )
        {
        User::LeaveIfError( PathInfo::GetRootPath( root, i ) ); 
        getPaths.Append( root );
        }

    //Array for paths retrieve via direct path functions
    // NOTE!!! Paths in both arrays must be set on corresponding indexes in order to get the test to work    
    RArray<TFileName> funcPaths;
    funcPaths.Append( PathInfo::RomRootPath() );
    funcPaths.Append( PathInfo::PhoneMemoryRootPath() );
    funcPaths.Append( PathInfo::MemoryCardRootPath() );
    
    _LIT( KDescription, "GetRootPathTest executed. ( Failed indexes: " );
    _LIT( KFailIndex, "%d,");
    _LIT( KTextNone, "NONE )" );
    _LIT( KTextParCl, " )" );
     
    TBuf<100> description;    
    TInt err(KErrNone);
    TInt pathCount = getPaths.Count();
    TInt funcCount = funcPaths.Count();
 
    description.Format( KDescription );
 
    for( TInt i(0); i < pathCount; i++ )
        {
        TFileName& getPath = getPaths[i];
        
        iLog -> Log(getPath);
        
        TBool found( EFalse );
        for ( TInt j( 0 ); j < funcCount; ++j )
            {
            TFileName& funcPath = funcPaths[j];
            if ( getPath[ 0 ] == funcPath[ 0 ] )
                {
                found = ETrue;
                if ( getPath.Compare( funcPath ) )
                    {
                    description.AppendFormat( KFailIndex, i );
                    err = KErrGeneral;
                    }
                }
            }
        if ( !found )
            {
            TParsePtrC parse( getPath );
            if ( !parse.DrivePresent() || parse.DriveAndPath().Compare( getPath ) )
                {
                description.AppendFormat( KFailIndex, i );
                err = KErrGeneral;
                }
            }
        }
 
 
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    if( err == KErrNone )
        {
        description.Append( KTextNone );
        }
    else
        {
        //strip the last comma
        description.Delete( description.Length() -1 , 1);
        description.Append( KTextParCl );
        }
    
    
    aResult.SetResult( err, description );
  
    funcPaths.Close();
    getPaths.Close();     
  
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPlatformEnvTest::GetFullPathTest
// Testing relations between retrieving paths via direct methods and 
// via GetRootPathTest( aPath) method.
// Note: This case results KErrGeneral even if just one of the path pairs differ 
// -----------------------------------------------------------------------------
//
TInt CPlatformEnvTest::GetFullPathTest( TTestResult& aResult )
    { 

    //Array for paths retrieved via GetFullPath-method  
    const TInt KLastPath = 15;
    RArray<TFileName> getPaths;
    TFileName path;

    for ( TInt i( EDriveA ); i <= EDriveZ; ++i )
        {
        for ( TInt j( -1 ); j <= KLastPath + 1; ++j )
            {
            if ( PathInfo::GetFullPath( path, i, j ) == KErrNone )
                {
                getPaths.Append( path );
                }
            }
        }

    _LIT( KDescription, "GetFullPathTest executed. ( Failed indexes: " );
    _LIT( KFailIndex, "%d,");
    _LIT( KTextNone, "NONE )" );
    _LIT( KTextParCl, " )" );
     
    TBuf<100> description;    
    TInt err(KErrNone);
    TInt count = getPaths.Count();
 
    description.Format( KDescription ) ;   
 
    for( TInt i(0); i < count; i++ )
        {
        TFileName& getPath = getPaths[i];
        iLog -> Log(getPath);

        if ( PathInfo::PathType( getPath ) == PathInfo::ENotSystemPath )
            {
            description.AppendFormat( KFailIndex, i );
            err = KErrGeneral;
            }
        }
 

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    if( err == KErrNone )
        {
        description.Append( KTextNone );
        }
    else
        {
        //strip the last comma
        description.Delete( description.Length() -1 , 1);
        description.Append( KTextParCl );
        }
    
    
    aResult.SetResult( err, description );
  
    getPaths.Close();     
  
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPlatformEnvTest::PathTypeTest
// Testing relations between retrieving paths via direct methods and 
// via GetRootPathTest( aPath) method.
// Note: This case results KErrGeneral even if just one of the path pairs differ 
// -----------------------------------------------------------------------------
//
TInt CPlatformEnvTest::PathTypeTest( TTestResult& aResult )
    { 

    //Array for paths retrieved via GetFullPath-method  
    const TInt KLastPath = 15;
    RArray<TFileName> getPaths;
    TFileName path;

    for ( TInt i( EDriveA ); i <= EDriveZ; ++i )
        {
        for ( TInt j( 0 ); j <= KLastPath; ++j )
            {
            if ( PathInfo::GetFullPath( path, i, j ) == KErrNone )
                {
                getPaths.Append( path );
                }
            }
        }
    // Thumbnail path may exists in any folder
    PathInfo::GetFullPath( path, EDriveA, PathInfo::EImagesPath );
    path.Append( PathInfo::ImagesThumbnailPath()  );
    getPaths.Append( path );

    // Array of invalid paths
    RArray<TFileName> invalidPaths;
    invalidPaths.Append( _L("E:\\not_a_system_path\\") );

    PathInfo::GetFullPath( path, EDriveA, PathInfo::EImagesPath );
    path.Append( _L("invalid\\") );
    invalidPaths.Append( path );
    

    _LIT( KDescription, "PathTypeTest executed. ( Failed indexes: " );
    _LIT( KFailIndex, "%d,");
    _LIT( KTextNone, "NONE )" );
    _LIT( KTextParCl, " )" );
     
    TBuf<100> description;    
    TInt err(KErrNone);
    TInt count = getPaths.Count();
 
    description.Format( KDescription ) ;   
 
    // Test valid paths
    for( TInt i(0); i < count; i++ )
        {
        TFileName& getPath = getPaths[i];
        iLog -> Log(getPath);

        if ( PathInfo::PathType( getPath ) == PathInfo::ENotSystemPath )
            {
            description.AppendFormat( KFailIndex, i );
            err = KErrGeneral;
            }
        }
    // Test invalid paths
    TInt invCount = invalidPaths.Count();
    for( TInt i(0); i < invCount; i++ )
        {
        TFileName& invPath = invalidPaths[i];
        iLog -> Log(invPath);

        if ( PathInfo::PathType( invPath ) != PathInfo::ENotSystemPath )
            {
            description.AppendFormat( KFailIndex, i );
            err = KErrGeneral;
            }
        }
  

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    if( err == KErrNone )
        {
        description.Append( KTextNone );
        }
    else
        {
        //strip the last comma
        description.Delete( description.Length() -1 , 1);
        description.Append( KTextParCl );
        }
    
    
    aResult.SetResult( err, description );
  
    getPaths.Close();     
  
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPlatformEnvTest::GetListOfPathsTest
// Testing relations between retrieving paths via direct methods and 
// via GetRootPathTest( aPath) method.
// Note: This case results KErrGeneral even if just one of the path pairs differ 
// -----------------------------------------------------------------------------
//
TInt CPlatformEnvTest::GetListOfPathsTest( TTestResult& aResult )
    { 
    RPointerArray< CDesCArray > allPaths;

    for ( TInt i( EDriveA ); i <= EDriveZ; ++i )
        {
        if ( i & 1)
            {
            CDesCArray* paths = PathInfo::GetListOfPathsLC( i );
            allPaths.Append( paths );
            CleanupStack::Pop( paths );
            }
        else
            {
            CDesCArray* paths = PathInfo::GetListOfPathsL( i );
            allPaths.Append( paths );
            }
        }

    _LIT( KDescription, "GetListOfPathsTest executed. ( Failed indexes: " );
    _LIT( KFailIndex, "%d,");
    _LIT( KTextNone, "NONE )" );
    _LIT( KTextParCl, " )" );

    TInt allCount = allPaths.Count();
    TInt err(KErrNone);
    TBuf<100> description;  
    description.Format( KDescription ) ;   
 
    // Test valid paths
    for( TInt i(0); i < allCount; i++ )
        {
        CDesCArray* paths = allPaths[ i ];
        TInt count = paths->MdcaCount();
        for( TInt j(0); j < count; j++ )
            {
            TPtrC path = paths->MdcaPoint(j);
            iLog -> Log(path);

            if ( PathInfo::PathType( path ) == PathInfo::ENotSystemPath )
                {
                description.AppendFormat( KFailIndex, i );
                err = KErrGeneral;
                }
            }
        }
  
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    if( err == KErrNone )
        {
        description.Append( KTextNone );
        }
    else
        {
        //strip the last comma
        description.Delete( description.Length() -1 , 1);
        description.Append( KTextParCl );
        }
    
    
    aResult.SetResult( err, description );
  
    allPaths.ResetAndDestroy();
    allPaths.Close();
  
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPlatformEnvTest::GetDefaultDriveTest
// Testing relations between retrieving paths via direct methods and 
// via GetRootPathTest( aPath) method.
// Note: This case results KErrGeneral even if just one of the path pairs differ 
// -----------------------------------------------------------------------------
//
TInt CPlatformEnvTest::GetDefaultDriveTest( TTestResult& aResult )
    { 
    //Array for paths retrieved via GetDefaultDriveTest-method  
    RArray<TInt> drives;
    const TInt KLastDrive = 5 + 1; // +1 for invalid default drive
    
    for ( TInt i( 0 ); i <= KLastDrive; i++ )
        {
        TInt drive( KErrNotFound );
        if ( DriveInfo::GetDefaultDrive( i, drive ) == KErrNone )
            {
            drives.Append( drive );
            }
        }

    _LIT( KDescription, "GetDefaultDriveTest executed. ( Failed indexes: " );
    _LIT( KFailIndex, "%d,");
    _LIT( KTextNone, "NONE )" );
    _LIT( KTextParCl, " )" );

    TInt count = drives.Count();
    TInt err(KErrNone);
    TBuf<100> description;  
    description.Format( KDescription ) ;   
 
    for( TInt i(0); i < count; i++ )
        {
        TInt drive = drives[i];
        TDriveInfo dummy;
        if ( drive < EDriveA || drive > EDriveZ || iFs.Drive( dummy, drive ) != KErrNone )
            {
            description.AppendFormat( KFailIndex, i );
            err = KErrGeneral;
            }
        }
  
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    if( err == KErrNone )
        {
        description.Append( KTextNone );
        }
    else
        {
        //strip the last comma
        description.Delete( description.Length() -1 , 1);
        description.Append( KTextParCl );
        }
    
    
    aResult.SetResult( err, description );
  
    drives.Close();
  
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPlatformEnvTest::GetDefaultDriveTest2
// Testing relations between retrieving paths via direct methods and 
// via GetRootPathTest( aPath) method.
// Note: This case results KErrGeneral even if just one of the path pairs differ 
// -----------------------------------------------------------------------------
//
TInt CPlatformEnvTest::GetDefaultDriveTest2( TTestResult& aResult )
    { 
    //Array for paths retrieved via GetDefaultDriveTest-method  
    RArray<TChar> drives;
    const TInt KLastDrive = 5 + 1; // +1 for invalid default drive
    
    for ( TInt i( 0 ); i <= KLastDrive; i++ )
        {
        TChar driveLetter( 0 );
        if ( DriveInfo::GetDefaultDrive( i, driveLetter ) == KErrNone )
            {
            drives.Append( driveLetter );
            }
        }

    _LIT( KDescription, "GetDefaultDriveTest2 executed. ( Failed indexes: " );
    _LIT( KFailIndex, "%d,");
    _LIT( KTextNone, "NONE )" );
    _LIT( KTextParCl, " )" );

    TInt count = drives.Count();
    TInt err(KErrNone);
    TBuf<100> description;  
    description.Format( KDescription ) ;   
 
    for( TInt i(0); i < count; i++ )
        {
        TChar driveLetter = drives[i];
        driveLetter.UpperCase();
        if ( driveLetter < 'A' || driveLetter > 'Z' )
        
            {
            description.AppendFormat( KFailIndex, i );
            err = KErrGeneral;
            }
        }
  
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    if( err == KErrNone )
        {
        description.Append( KTextNone );
        }
    else
        {
        //strip the last comma
        description.Delete( description.Length() -1 , 1);
        description.Append( KTextParCl );
        }
    
    
    aResult.SetResult( err, description );
  
    drives.Close();
  
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPlatformEnvTest::GetDriveStatusTest
// Testing relations between retrieving paths via direct methods and 
// via GetRootPathTest( aPath) method.
// Note: This case results KErrGeneral even if just one of the path pairs differ 
// -----------------------------------------------------------------------------
//
TInt CPlatformEnvTest::GetDriveStatusTest( TTestResult& aResult )
    { 
    //Array for paths retrieved via GetDriveStatus-method  
    RArray<TUint> driveStatus;
    
    for ( TInt i( EDriveA - 1 ); i <= EDriveZ + 1; i++ )
        {
        TUint status( 0 );
        if ( DriveInfo::GetDriveStatus( iFs, i, status ) == KErrNone )
            {
            driveStatus.Append( status );
            }
        }

    _LIT( KDescription, "GetDriveStatusTest executed. ( Failed indexes: " );
    _LIT( KFailIndex, "%d,");
    _LIT( KTextNone, "NONE )" );
    _LIT( KTextParCl, " )" );

    TInt count = driveStatus.Count();
    TInt err(KErrNone);
    TBuf<100> description;  
    description.Format( KDescription ) ;   
 
    for( TInt i(0); i < count; i++ )
        {
        TUint status = driveStatus[i];
        if ( !status )
            {
            description.AppendFormat( KFailIndex, i );
            err = KErrGeneral;
            }
        }
  
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    if( err == KErrNone )
        {
        description.Append( KTextNone );
        }
    else
        {
        //strip the last comma
        description.Delete( description.Length() -1 , 1);
        description.Append( KTextParCl );
        }
    
    
    aResult.SetResult( err, description );
  
    driveStatus.Close();
  
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPlatformEnvTest::GetUserVisibleDrivesTest
// Testing relations between retrieving paths via direct methods and 
// via GetRootPathTest( aPath) method.
// Note: This case results KErrGeneral even if just one of the path pairs differ 
// -----------------------------------------------------------------------------
//
TInt CPlatformEnvTest::GetUserVisibleDrivesTest( TTestResult& aResult )
    { 
    TDriveList allDrives;
    TInt allDriveCount( 0 );
    DriveInfo::GetUserVisibleDrives( iFs, allDrives, allDriveCount );

    TDriveList intDrives;
    TInt intDriveCount( 0 );
    DriveInfo::GetUserVisibleDrives( iFs, intDrives, intDriveCount, KDriveAttInternal );

    TDriveList remDrives;
    TInt remDriveCount( 0 );
    DriveInfo::GetUserVisibleDrives( iFs, remDrives, remDriveCount, KDriveAttRemovable );

    _LIT( KDescription, "GetUserVisibleDrivesTest executed. ( Failed indexes: " );
    _LIT( KFailIndex, "%d,");
    _LIT( KTextNone, "NONE )" );
    _LIT( KTextParCl, " )" );

    TInt count = allDrives.Length();
    TInt err(KErrNone);
    TBuf<100> description;  
    TInt checkCount( 0 );
    description.Format( KDescription );   

    if ( !allDriveCount || !count )
        {
        description.AppendFormat( _L("List is empty") );
        err = KErrGeneral;
        }

    // All drives
    for( TInt i(0); i < count; i++ )
        {
        if ( allDrives[ i ] )
            {
            ++checkCount;
            TUint status( 0 );
            if ( DriveInfo::GetDriveStatus( iFs, i, status ) == KErrNone )
                {
                if ( !( status & DriveInfo::EDriveUserVisible ) )
                    {
                    description.AppendFormat( KFailIndex, i );
                    err = KErrGeneral;
                    }
                }
            else
                {
                description.AppendFormat( KFailIndex, i );
                err = KErrGeneral;
                }
            }
        }
      if ( checkCount != allDriveCount )
        {
        description.AppendFormat( _L("All count does not match") );
        err = KErrGeneral;
        }

    // Internal drives
    count = intDrives.Length();
    checkCount = 0;
    for( TInt i(0); i < count; i++ )
        {
        if ( intDrives[ i ] )
            {
            ++checkCount;
            TUint status( 0 );
            if ( DriveInfo::GetDriveStatus( iFs, i, status ) == KErrNone )
                {
                if ( !( status & DriveInfo::EDriveInternal ) )
                    {
                    description.AppendFormat( KFailIndex, i );
                    err = KErrGeneral;
                    }
                }
            else
                {
                description.AppendFormat( KFailIndex, i );
                err = KErrGeneral;
                }
            }
        }
      if ( checkCount != intDriveCount )
        {
        description.AppendFormat( _L("Internal count does not match") );
        err = KErrGeneral;
        }

    // Removable drives
    count = remDrives.Length();
    checkCount = 0;
    for( TInt i(0); i < count; i++ )
        {
        if ( remDrives[ i ] )
            {
            ++checkCount;
            TUint status( 0 );
            if ( DriveInfo::GetDriveStatus( iFs, i, status ) == KErrNone )
                {
                if ( !( status & DriveInfo::EDriveRemovable ) )
                    {
                    description.AppendFormat( KFailIndex, i );
                    err = KErrGeneral;
                    }
                }
            else
                {
                description.AppendFormat( KFailIndex, i );
                err = KErrGeneral;
                }
            }
        }
      if ( checkCount != remDriveCount )
        {
        description.AppendFormat( _L("Removable count does not match") );
        err = KErrGeneral;
        }

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    if( err == KErrNone )
        {
        description.Append( KTextNone );
        }
    else
        {
        //strip the last comma
        description.Delete( description.Length() -1 , 1);
        description.Append( KTextParCl );
        }
    
    
    aResult.SetResult( err, description );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPlatformEnvTest::StripUserHiddenDrivesTest
// Testing relations between retrieving paths via direct methods and 
// via GetRootPathTest( aPath) method.
// Note: This case results KErrGeneral even if just one of the path pairs differ 
// -----------------------------------------------------------------------------
//
TInt CPlatformEnvTest::StripUserHiddenDrivesTest( TTestResult& aResult )
    { 
    TDriveList allDrives;
    iFs.DriveList( allDrives );

    TDriveList drives = allDrives;
    TInt driveCount = DriveInfo::StripUserHiddenDrives( drives );

    _LIT( KDescription, "StripUserHiddenDrivesTest executed. ( Failed indexes: " );
    _LIT( KFailIndex, "%d,");
    _LIT( KTextNone, "NONE )" );
    _LIT( KTextParCl, " )" );

    TInt count = allDrives.Length();
    TInt err(KErrNone);
    TBuf<100> description;  
    description.Format( KDescription );   

    if ( !driveCount || !count )
        {
        description.AppendFormat( _L("List is empty") );
        err = KErrGeneral;
        }
    TInt checkCount( 0 );
    for( TInt i(0); i < count; i++ )
        {
        if ( drives[ i ] )
            {
            ++checkCount;
            }

        if ( allDrives[ i ] )
            {
            TUint status( 0 );
            if ( DriveInfo::GetDriveStatus( iFs, i, status ) == KErrNone )
                {
                if ( !( status & DriveInfo::EDriveUserVisible ) )
                    {
                    if ( drives[ i ] )
                        {
                        description.AppendFormat( KFailIndex, i );
                        err = KErrGeneral;
                        }
                    }
                }
            else
                {
                description.AppendFormat( KFailIndex, i );
                err = KErrGeneral;
                }
            }
        }
  
    if ( checkCount != driveCount )
        {
        description.AppendFormat( _L("Count does not match") );
        err = KErrGeneral;
        }

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    if( err == KErrNone )
        {
        description.Append( KTextNone );
        }
    else
        {
        //strip the last comma
        description.Delete( description.Length() -1 , 1);
        description.Append( KTextParCl );
        }
    
    
    aResult.SetResult( err, description );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPlatformEnvTest::DriveCountTest
// Testing relations between retrieving paths via direct methods and 
// via GetRootPathTest( aPath) method.
// Note: This case results KErrGeneral even if just one of the path pairs differ 
// -----------------------------------------------------------------------------
//
TInt CPlatformEnvTest::DriveCountTest( TTestResult& aResult )
    { 
    TDriveList allDrives;
    iFs.DriveList( allDrives );
    TInt driveCount = DriveInfo::DriveCount( allDrives );

    _LIT( KDescription, "DriveCountTest executed. ( " );
    _LIT( KTextNone, "NONE )" );
    _LIT( KTextParCl, " )" );

    TInt count = allDrives.Length();
    TInt err(KErrNone);
    TBuf<100> description;  
    description.Format( KDescription );   

    if ( !driveCount || !count )
        {
        description.AppendFormat( _L("List is empty") );
        err = KErrGeneral;
        }
 
    TInt checkCount( 0 );
    for( TInt i(0); i < count; i++ )
        {
        if ( allDrives[ i ] )
            {
            ++checkCount;
            }
        }

    if ( checkCount != driveCount )
        {
        description.AppendFormat( _L("Count does not match") );
        err = KErrGeneral;
        }
  
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    if( err == KErrNone )
        {
        description.Append( KTextNone );
        }
    else
        {
        //strip the last comma
        description.Delete( description.Length() -1 , 1);
        description.Append( KTextParCl );
        }
    
    
    aResult.SetResult( err, description );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPlatformEnvTest::DriveArrayTest
// Testing relations between retrieving paths via direct methods and 
// via GetRootPathTest( aPath) method.
// Note: This case results KErrGeneral even if just one of the path pairs differ 
// -----------------------------------------------------------------------------
//
TInt CPlatformEnvTest::DriveArrayTest( TTestResult& aResult )
    { 
    _LIT( KDescription, "DriveArrayTest executed. ( " );
    _LIT( KTextNone, "NONE )" );
    _LIT( KFailIndex, "%d,");
    _LIT( KTextParCl, " )" );
    TInt err(KErrNone);
    TBuf<100> description;  
    description.Format( KDescription );   

    TDriveList allDrives;
    iFs.DriveList( allDrives );
    
    DriveInfo::TDriveArray driveArray( allDrives );
    if ( !driveArray.Count() )
        {
        description.AppendFormat( _L("Count does not match") );
        err = KErrGeneral;
        }
    driveArray.Reset();
    if ( err == KErrNone && driveArray.Count() )
        {
        description.AppendFormat( _L("Count does not match") );
        err = KErrGeneral;
        }
    driveArray.Set( allDrives );
    TInt driveCount( driveArray.Count() );    
    for( TInt i(0); i < driveCount; i++ )
        {
        TInt drive = driveArray[ i ];
        TChar driveLetter = driveArray.LetterAt( i );
        driveLetter.UpperCase();
        if ( drive < EDriveA || drive > EDriveZ )
            {
            description.AppendFormat( KFailIndex, i );
            err = KErrGeneral;
            break;
            }
        else if ( driveLetter < 'A' || driveLetter > 'Z' )
            {
            description.AppendFormat( KFailIndex, i );
            err = KErrGeneral;
            break;
            }
        }

    TInt checkCount( DriveInfo::DriveCount( allDrives ) );
    if ( checkCount != driveCount )
        {
        description.AppendFormat( _L("Count does not match") );
        err = KErrGeneral;
        }
  
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    if( err == KErrNone )
        {
        description.Append( KTextNone );
        }
    else
        {
        //strip the last comma
        description.Delete( description.Length() -1 , 1);
        description.Append( KTextParCl );
        }
    
    
    aResult.SetResult( err, description );
    
    return KErrNone;
   }

// -----------------------------------------------------------------------------
// CPlatformEnvTest::GetUserVisibleDrivesTest2
// Testing relations between retrieving paths via direct methods and 
// via GetRootPathTest( aPath) method.
// Note: This case results KErrGeneral even if just one of the path pairs differ 
// -----------------------------------------------------------------------------
//
TInt CPlatformEnvTest::GetUserVisibleDrivesTest2( TTestResult& aResult )
    { 
    DriveInfo::TDriveArray allDrives;
    DriveInfo::GetUserVisibleDrives( iFs, allDrives );

    DriveInfo::TDriveArray intDrives;
    DriveInfo::GetUserVisibleDrives( iFs, intDrives, KDriveAttInternal );

    DriveInfo::TDriveArray remDrives;
    DriveInfo::GetUserVisibleDrives( iFs, remDrives, KDriveAttRemovable );

    _LIT( KDescription, "GetUserVisibleDrivesTest2 executed. ( Failed indexes: " );
    _LIT( KFailIndex, "%d,");
    _LIT( KTextNone, "NONE )" );
    _LIT( KTextParCl, " )" );

    TInt count = allDrives.Count();
    TInt err(KErrNone);
    TBuf<100> description;
    description.Format( KDescription );   

    if ( !count )
        {
        description.AppendFormat( _L("List is empty") );
        err = KErrGeneral;
        }

    // All drives
    TInt drive( 0 );
    TUint driveStatus( 0 );
    TInt i( 0 );

    for( ; i < count; i++ )
        {
        drive = allDrives[ i ];
        if ( DriveInfo::GetDriveStatus( iFs, drive, driveStatus ) == KErrNone )
            {
            if ( !( driveStatus & DriveInfo::EDriveUserVisible ) )
                {
                description.AppendFormat( KFailIndex, i );
                err = KErrGeneral;
                }
            }
        else
            {
            description.AppendFormat( KFailIndex, i );
            err = KErrGeneral;
            }
        }

    // Internal drives
    count = intDrives.Count();
    for( i = 0; i < count; i++ )
        {
        drive = intDrives[ i ];
        if ( DriveInfo::GetDriveStatus( iFs, drive, driveStatus ) == KErrNone )
            {
            if ( !( driveStatus & DriveInfo::EDriveInternal ) )
                {
                description.AppendFormat( KFailIndex, i );
                err = KErrGeneral;
                }
            }
        else
            {
            description.AppendFormat( KFailIndex, i );
            err = KErrGeneral;
            }
        }

    // Removable drives
    count = remDrives.Count();
    for( i = 0; i < count; i++ )
        {
        drive = remDrives[ i ];
        if ( DriveInfo::GetDriveStatus( iFs, drive, driveStatus ) == KErrNone )
            {
            if ( !( driveStatus & DriveInfo::EDriveRemovable ) )
                {
                description.AppendFormat( KFailIndex, i );
                err = KErrGeneral;
                }
            }
        else
            {
            description.AppendFormat( KFailIndex, i );
            err = KErrGeneral;
            }
        }

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    if( err == KErrNone )
        {
        description.Append( KTextNone );
        }
    else
        {
        //strip the last comma
        description.Delete( description.Length() -1 , 1);
        description.Append( KTextParCl );
        }
    
    
    aResult.SetResult( err, description );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
?type ?classname::?member_function(
   ?arg_type arg,
   ?arg_type arg )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ?function_name implements...
// ?implementation_description.
// Returns: ?value_1: ?description
//          ?value_n: ?description
//                    ?description
// -----------------------------------------------------------------------------
//
/*
?type  ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg )  // ?description
    {

    ?code

    }
*/
//  End of File
