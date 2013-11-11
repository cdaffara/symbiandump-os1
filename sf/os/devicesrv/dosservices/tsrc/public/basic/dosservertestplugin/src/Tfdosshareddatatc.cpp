/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


#include <e32std.h>
#include <e32svr.h> // RDebug
#include <e32base.h>

#include <Sysutil.h>
#include "CTFDosSharedDataTC.h"

//FORWARD DECLARATIONS


// CONSTANTS
_LIT( KSDReserveFileName, "C:\\Private\\101f6efa\\reserve.bin" );
_LIT( KFileName, "C:\\logs\\MemoryEaterFile" );

// -----------------------------------------------------------------------------
// CSdDisk::CSdDisk
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CSdDisk::CSdDisk() 
    {
    }

// -----------------------------------------------------------------------------
// CSdDisk* CSdDisk::NewL
// 
// 
// -----------------------------------------------------------------------------
CSdDisk* CSdDisk::NewL() 
    {
	CSdDisk* sdn = new (ELeave) CSdDisk();
	CleanupStack::PushL( sdn );
	sdn->ConstructL();
	CleanupStack::Pop();
	return sdn;
    }

// -----------------------------------------------------------------------------
// CSdDisk::ConstructL()
// 
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CSdDisk::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CSdDisk::~CSdDisk
// Destructor
// 
// -----------------------------------------------------------------------------
CSdDisk::~CSdDisk() 
    {
    }

// -----------------------------------------------------------------------------
// CSdDisk::ReleaseFiles
// This method deletes files created for filling the drive C
// 
// -----------------------------------------------------------------------------
void CSdDisk::ReleaseDisk(RFs& aFs)
    {
	// RDebug::Print(_L("Shareddatatest ## ReleaseDisk"));
	TInt i = 0;
	TInt err = KErrNone;
		
	do
	    {
		TBuf16<35> str;
		str.Copy(KFileName);
		str.AppendNum(i,EDecimal);
		
		err = aFs.Delete (str) ;
		
		i++;
	    }
	while ( err == KErrNone );
}

// -----------------------------------------------------------------------------
// CSdDisk::CheckDiskSpace()
// This method checks free disk space 
//
// Method is the almost the same than FreeSpaceL(RFs* aFs, TInt aDrive) in sysutil.cpp
// -----------------------------------------------------------------------------
TReal64 CSdDisk::CheckDiskSpace(RFs& aFs)
    {
	// RDebug::Print(_L("Shareddatatest ## CheckDiskSpace"));
    TInt err = KErrNone;

	TVolumeInfo vinfo;

	err = aFs.Volume(vinfo, EDriveC);
	
	if ( err != KErrNone )
	    {
	    RDebug::Print(_L("Shareddatatest ## Error = %d"), err);
		User::LeaveIfError(err);
	    }

	// RDebug::Print(_L("Shareddatatest ## Disk space: %f "), TReal64(vinfo.iFree));
	
	return TReal64(vinfo.iFree);
    }

// -----------------------------------------------------------------------------
// CSdDisk::CheckDiskSpace()
// This method checks the reserve file size
//
// -----------------------------------------------------------------------------
TInt CSdDisk::CheckReserveFileSize(RFs& aFs)
    {
    RFile iReserveFile;
    
    TInt err = iReserveFile.Open( aFs, KSDReserveFileName, EFileShareAny | EFileWrite );
    
    if (err == KErrNone)
        {
        TInt iReserveFileSize(0);
        iReserveFile.Size(iReserveFileSize);
        iReserveFile.Close();
        
        RDebug::Print( _L("Shareddatatest ## Reservefilesize: %d"), iReserveFileSize );
        
        return iReserveFileSize;
        }
    else
        {
        return KErrGeneral;
        }
    }

// -----------------------------------------------------------------------------
// CSdDisk::FillDisk()
// Fills the diskspace 
// 
// -----------------------------------------------------------------------------
void CSdDisk::FillDisk(RFs& aFs, TInt& aFreeDiskSpaceToLeave)
    {
	RFile iEaterFile;
	TInt fileSize(0);	
	TInt err(KErrNone);
	
	TReal64 diskFree = CheckDiskSpace(aFs);
	// RDebug::Print(_L("Shareddatatest ## Eat disk space") );
	
	// For loop to create eaterfiles
	for ( TInt i = 0 ; diskFree != 0 ; i++ )
	    {	
		TBuf16<30> str;
		str.Copy(KFileName);
		str.AppendNum(i,EDecimal);
			
		// RDebug::Print(_L("Shareddatatest ## Open file %s"), &str);
	
		err = iEaterFile.Open( aFs, str, EFileShareAny | EFileWrite );

		if ( err == KErrNotFound )	// file does not exist - create it
		    {
			err = iEaterFile.Create( aFs, str, EFileShareAny | EFileWrite );
		    }

		if ( err == KErrNone )
		    {
			// Create maximum size eaterfile if there is more disk space available than the maximum file size
			if ( diskFree > TReal64(KMaxTInt) )
			    {
				// RDebug::Print( _L("Shareddatatest ## Creating %d. EaterFile"), i+1);
				err = iEaterFile.SetSize( KMaxTInt );
				
				diskFree = diskFree - TReal64(KMaxTInt);
			    }
			
			// ... And if available disk space is less than maximum file size
			else
			    {
				// If aFreeDiskSpaceToLeave parameter says we have to leave some free disk
				if ( aFreeDiskSpaceToLeave != 0 )
				    {
					// RDebug::Print( _L("Shareddatatest ## Creating %d. EaterFile"), i+1);
					TInt sizeToSet = STATIC_CAST(TInt, I64INT(diskFree));
					err = iEaterFile.SetSize( sizeToSet - aFreeDiskSpaceToLeave * 1024 );
					iEaterFile.Size( fileSize );
					// RDebug::Print( _L("Shareddatatest ## Last EaterFileSize: %d"), fileSize);
					diskFree = 0;	
				    }
				
				// Otherwise try to eat all the disk space
				else
				    {
					// RDebug::Print( _L("Shareddatatest ## Creating %d. EaterFile"), i+1);
					TInt sizeToSet = STATIC_CAST(TInt, I64INT(diskFree));
					// RDebug::Print( _L("Shareddatatest ## Filesize in the end should be around %d bytes"), sizeToSet);
					
					for ( TInt j = 64 ; err != KErrDiskFull ; j-- )
					    {	
						err = iEaterFile.SetSize( sizeToSet - j * 1024 );
					    }
					iEaterFile.Size( fileSize );
					// RDebug::Print( _L("Shareddatatest ## Last EaterFileSize: %d bytes"), fileSize);
					// RDebug::Print( _L("Shareddatatest ## There should be %d bytes of free disk"),sizeToSet - fileSize);
					diskFree = 0;
				    }
			    }
		    }
		
		else
		    {
			RDebug::Print(_L("Shareddatatest ## Error = %d"), err);
			diskFree = 0;
		    }

		// RDebug::Print(_L("Shareddatatest ## Close file"));
		iEaterFile.Close();
	    }
    }
