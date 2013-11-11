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
* Description:  Store Manager in SUPL PSY.
*
*/


// INCLUDE FILES

#include "suplpsylogging.h" 
#include "suplpsyinfostoremanager.h"
#include "suplpsycrkeys.h"


//const TUid KCRUidSuplFW = { 0x102750A0 };
//const TUint32 KSuplFwCId = 0x0000000c;

_LIT(KFileName,"\\private\\101F97b2\\SuplPsyLocationInfo.dat");

// ============================ MEMBER FUNCTIONS OF CSuplPsyInfoStoreManager ===============================

// -----------------------------------------------------------------------------
// CSuplPsyInfoStoreManager::CSuplPsyInfoStoreManager,constructor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

CSuplPsyInfoStoreManager::CSuplPsyInfoStoreManager() 
					    : iRepoSuplPsy( NULL ),
					      iRepoSuplFw( NULL ) 
	{
    }

// -----------------------------------------------------------------------------
// CSuplPsyInfoStoreManager::ConstructL,two phase constuctor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSuplPsyInfoStoreManager::ConstructL()
    {
    TRACESTRING( "CSuplPsyInfoStoreManager::ConstructL start" )
    
    TRACESTRING( "Opening Supl psy cenrep" )
    iRepoSuplPsy = CRepository::NewL( KCRUidSuplPsy );

    
    TRACESTRING( "Opening Supl fw cenrep" )
    // TODO put back
    //iRepoSuplFw = CRepository::NewL( KCRUidSuplFW );
    
    TRACESTRING( "Opening File server" )
    User::LeaveIfError( iFs.Connect() );
    iFs.CreatePrivatePath( EDriveC );
    
    TRACESTRING( "Set session path" )
    User::LeaveIfError( iFs.SetSessionToPrivate( EDriveC ) );

    TInt err = iFile.Open( iFs, KFileName, EFileRead|EFileWrite|EFileStream );
    if ( err )
        {
        err = iFile.Create( iFs, KFileName, EFileRead|EFileWrite|EFileStream );
        TRACESTRING( "File didnt exist,file created" )
        }
    User::LeaveIfError( err );
    TRACESTRING( "CSuplPsyInfoStoreManager::ConstructL end" )
    }

// -----------------------------------------------------------------------------
// CSuplPsyInfoStoreManager::NewL,two phase constuctor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSuplPsyInfoStoreManager* CSuplPsyInfoStoreManager::NewL()
    {
    CSuplPsyInfoStoreManager* self = new( ELeave ) CSuplPsyInfoStoreManager();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSuplPsyInfoStoreManager::~CSuplPsyInfoStoreManager,Destructor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSuplPsyInfoStoreManager::~CSuplPsyInfoStoreManager()
    {
	TRACESTRING( "CSuplPsyInfoStoreManager::~CSuplPsyInfoStoreManager start" )
	delete iRepoSuplPsy;
	iRepoSuplPsy = NULL;
	
	delete iRepoSuplFw;
	iRepoSuplFw = NULL;
	
	iFile.Close();
    iFs.Close();
	TRACESTRING( "CSuplPsyInfoStoreManager::~CSuplPsyInfoStoreManager end" )
	}
            
// -----------------------------------------------------------------------------
// CSuplPsyInfoStoreManager::ReadSuplPsyLocationInfo,Reads stored Location Information from file.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSuplPsyInfoStoreManager::ReadSuplPsyLocationInfo( HPositionGenericInfo& aPosInfo )
    {
	TRACESTRING( "CSuplPsyInfoStoreManager::ReadSuplPsyLocationInfo start" )
	if ( iFs.Handle() && iFile.SubSessionHandle() )
        {
        TPckg<HPositionGenericInfo> positionDes( aPosInfo );
        TInt pos = 0;
        TInt seekErr = iFile.Seek( ESeekStart,pos );
		if( KErrNone != seekErr )
			{
			return seekErr;	
			}
		TInt err = iFile.Read( positionDes );
		if( KErrNone == err )
			{
			TInt size = positionDes.Size();
			TRACESTRING2("pos data size=%d",size )
			if( positionDes.Size() > 0 )
				{
				TRACESTRING( "File contains data" )
				return KErrNone;	
				}	
			TRACESTRING( "File doesnt contain any data" )
			return KErrNotFound;
			}
		return err;	
		}
	
	TRACESTRING( "CSuplPsyInfoStoreManager::ReadSuplPsyLocationInfo end" )
	return KErrNotFound;
	}  
      
// -----------------------------------------------------------------------------
// CSuplPsyInfoStoreManager::WriteSuplPsyLocationInfo,Writes Location Information to file.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSuplPsyInfoStoreManager::WriteSuplPsyLocationInfo( const HPositionGenericInfo& aPosInfo )
    {
	TRACESTRING( "CSuplPsyInfoStoreManager::WriteSuplPsyLocationInfo start" )
	if ( iFs.Handle() && iFile.SubSessionHandle() )
        {
        TPckg<HPositionGenericInfo> positionDes( aPosInfo );
        TInt err1 = iFile.Write( 0, positionDes );
        TInt err2 = iFile.Flush(); 
               
        return ( err1 + err2 );
        }
    TRACESTRING( "CSuplPsyInfoStoreManager::WriteSuplPsyLocationInfo end" )
    return KErrNotFound;       
    }

// -----------------------------------------------------------------------------
// CSuplPsyInfoStoreManager::ReadSuplPsyCellIdInfo,Reads stored Cell Id Information from SUPL PSY CR Key.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSuplPsyInfoStoreManager::ReadSuplPsyCellIdInfo( TCellIdInfo& aCellIdInfo )
    {
	TRACESTRING( "CSuplPsyInfoStoreManager::ReadSuplPsyCellIdInfo start" )
	TPckg<TCellIdInfo> positionDes( aCellIdInfo );
	TInt err = iRepoSuplPsy->Get( KPSYCIdInfo,positionDes );
	TRACESTRING2("Read psy CR key error code=%d",err )
	
	if( KErrNone == err)
		{
		if( positionDes.Size() > 0 )
			{
			TRACESTRING( "CId data present" )
			return KErrNone;	
			}	
		
		TRACESTRING( "No CId data" )
		return KErrNotFound; 
		}
		
	TRACESTRING( "CSuplPsyInfoStoreManager::ReadSuplPsyCellIdInfo end" )
	return err; 
    }
    
// -----------------------------------------------------------------------------
// CSuplPsyInfoStoreManager::WriteSuplPsyCellIdInfo,Writes Cell Id Information to SUPL PSY CR Key.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSuplPsyInfoStoreManager::WriteSuplPsyCellIdInfo( const TCellIdInfo& aCellIdInfo )
    {
	TRACESTRING( "CSuplPsyInfoStoreManager::WriteSuplPsyCellIdInfo start" )
	TPckg<TCellIdInfo> positionDes( aCellIdInfo );
	return ( iRepoSuplPsy->Set( KPSYCIdInfo,positionDes ) );
	}
        
// -----------------------------------------------------------------------------
// CSuplPsyInfoStoreManager::ReadSuplFwCellIdInfo,Reads Cell Id Information from SUPL FW CR key.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSuplPsyInfoStoreManager::ReadSuplFwCellIdInfo( TCellIdInfo& /*aCellIdInfo*/ )
    {
    return KErrNone;
    //TODO temp removal put back
   
//	TRACESTRING( "CSuplPsyInfoStoreManager::ReadSuplFwCellIdInfoL start" )
//	TPckg<TCellIdInfo> positionDes( aCellIdInfo );
//	TInt err = iRepoSuplFw->Get( KSuplFwCId,positionDes ); 
//	TRACESTRING2("Read fw CR key error code=%d",err )
//	
//	
//	if( KErrNone == err)
//		{
//		if( positionDes.Size() > 0 )
//			{
//			TRACESTRING( "CId data present" )
//			return KErrNone;	
//			}	
//		
//		TRACESTRING( "No Cid data" )
//		return KErrNotFound; 
//		}
//		
//	TRACESTRING( "CSuplPsyInfoStoreManager::ReadSuplFwCellIdInfo end" )
//    return err; 
	}
//  End of File  
