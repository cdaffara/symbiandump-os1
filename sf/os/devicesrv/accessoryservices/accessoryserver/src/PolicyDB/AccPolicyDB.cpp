/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This class implements the Accessory Policy DB interface.
*
*/


// INCLUDE FILES
#include "AccPolicyDB.h"
#include "AccPolicyDBNames.h"
#include "AccPolCapabilityStorage.h"
#include "acc_debug.h"
#include <AccPolSubblockNameArray.h>
#include <AccPolCommonNameValuePairs.h>
#include <AccPolGIDHeader.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
_LIT( KSQLSelect, "SELECT " );
_LIT( KSQLFrom, " FROM " );
_LIT( KSQLAsterisk, " * " );
_LIT( KSQLWhere, " WHERE " );
_LIT( KSQLEquals, " = ");
_LIT( KSQLAnd, " AND ");

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAccPolicyDB::CAccPolicyDB
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolicyDB::CAccPolicyDB()
    {

    }

// -----------------------------------------------------------------------------
// CAccPolicyDB::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccPolicyDB::ConstructL()
    {
    COM_TRACE_( "[ACCPOLICYDB] CAccPolicyDB::ConstructL()" );

    User::LeaveIfError( iFsSession.Connect() );

    COM_TRACE_( "[ACCPOLICYDB] CAccPolicyDB::ConstructL() return void" );
    }

// -----------------------------------------------------------------------------
// CAccPolicyDB::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CAccPolicyDB* CAccPolicyDB::NewL()
    {
    API_TRACE_( "[ACCPOLICYDB] CAccPolicyDB::NewL()" );

    CAccPolicyDB* self = new( ELeave ) CAccPolicyDB;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    API_TRACE_( "[ACCPOLICYDB] CAccPolicyDB::NewL() return" );
    return self;
    }

// Destructor
EXPORT_C CAccPolicyDB::~CAccPolicyDB()
    {
    API_TRACE_( "[ACCPOLICYDB] CAccPolicyDB::~CAccPolicyDB()" );

    iView.Close();
    CloseDB();
    iFsSession.Close();

    API_TRACE_( "[ACCPOLICYDB] CAccPolicyDB::~CAccPolicyDB() return void" );
    }

// -----------------------------------------------------------------------------
// CAccPolicyDB::OpenDBL
// Opens the database
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CAccPolicyDB::OpenDBL()
    {
    API_TRACE_( "[ACCPOLICYDB] CAccPolicyDB::OpenDBL()" );
    
    TInt       err( KErrNone );
    TFileName  romFullPath;
    TDriveName zDrive = TDriveUnit( EDriveZ ).Name();
    romFullPath.Append( zDrive );
    romFullPath.Append( KDBPath );

    err = iDb.Open(iFsSession, romFullPath, KAccDbSecureFormat(), RDbNamedDatabase::EReadOnly );

    COM_TRACE_1( "[ACCPOLICYDB] CAccPolicyDB::OpenDBL - Open DB: %d", err);

    User::LeaveIfError( err );

    API_TRACE_( "[ACCPOLICYDB] CAccPolicyDB::OpenDBL - return void" );    
    }

// -----------------------------------------------------------------------------
// CAccPolicyDB::CloseDB
// Closes the database connection
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CAccPolicyDB::CloseDB()
    {
    API_TRACE_( "[ACCPOLICYDB] CAccPolicyDB::CloseDB()" );

    iDb.Close();

    API_TRACE_( "[ACCPOLICYDB] CAccPolicyDB::CloseDB() return void" );
    }

// -----------------------------------------------------------------------------
// CAccPolicyDB::FindGIDIntL
// Search if DeviceID has a GIDInteger defined in DB
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CAccPolicyDB::FindGIDIntL(
    const TUint64& aDeviceID,
    TDes8& aGIDInt )
    {
    API_TRACE_( "[ACCPOLICYDB] CAccPolicyDB::FindGIDIntL()" );

    TInt res( 0 );

    iSQL = KSQLSelect;
    iSQL.Append( KACCDBGIDIntGIDCol );
    iSQL.Append( KSQLFrom );
    iSQL.Append( KACCDBGIDIntTable );
    iSQL.Append( KSQLWhere );
    iSQL.Append( KACCDBGIDIntDeviceIDCol );
    iSQL.Append( KSQLEquals );
    iSQL.AppendNum( aDeviceID );
   

    iView.Close();

    iView.Prepare( iDb, TDbQuery( iSQL ) );
    if ( iView.Unevaluated() )
        {
        iView.EvaluateAll();
        }

#ifdef _DEBUG

    TRAPD( firstLErr, res = iView.FirstL() )
    COM_TRACE_1( "[ACCPOLICYDB] CAccPolicyDB::FindGIDIntL firstLErr: %d", firstLErr );
    User::LeaveIfError( firstLErr );

#else

    res = iView.FirstL();

#endif // _DEBUG

    if ( res ) // Check if found a hit
        {
        COM_TRACE_( "[ACCPOLICYDB] CAccPolicyDB::FindGIDIntL() A GIDInt for this Device ID was found" );
        RDbColReadStream in;
        CDbColSet* colSet = iView.ColSetL();
        CleanupStack::PushL( colSet );
        TDbColNo col = colSet->ColNo( KACCDBGIDIntGIDCol );
        iView.GetL();
        in.OpenL( iView, col );
        in.PushL();
        in.ReadL( aGIDInt, iView.ColLength( col ) );
        in.Pop();
        in.Close();
        CleanupStack::PopAndDestroy( colSet );
        }

    iView.Reset();
    iView.Close();

    API_TRACE_( "[ACCPOLICYDB] CAccPolicyDB::FindGIDIntL() return void" );
    }

// -----------------------------------------------------------------------------
// CAccPolicyDB::FindGIDIntL
// Search if DeviceID has a GIDInteger defined in DB
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CAccPolicyDB::FindGIDIntL(
    const TUint64& aDeviceID,
    TDes8& aGIDInt,
    TAccGIDtype aType )
    {
    API_TRACE_( "[ACCPOLICYDB] CAccPolicyDB::FindGIDIntL()" );

    TInt res( 0 );

    iSQL = KSQLSelect;
    iSQL.Append( KACCDBGIDIntGIDCol );
    iSQL.Append( KSQLFrom );
    iSQL.Append( KACCDBGIDIntTable );
    iSQL.Append( KSQLWhere );
    iSQL.Append( KACCDBGIDIntDeviceIDCol );
    iSQL.Append( KSQLEquals );
    iSQL.AppendNum( aDeviceID );
    iSQL.Append( KSQLAnd );
    iSQL.Append( KACCDBGIDIntKeyIDCol );
    iSQL.Append( KSQLEquals );
    iSQL.AppendNum( aType );

    iView.Close();

    iView.Prepare( iDb, TDbQuery( iSQL ) );
    if ( iView.Unevaluated() )
        {
        iView.EvaluateAll();
        }

#ifdef _DEBUG

    TRAPD( firstLErr, res = iView.FirstL() )
    COM_TRACE_1( "[ACCPOLICYDB] CAccPolicyDB::FindGIDIntL firstLErr: %d", firstLErr );
    User::LeaveIfError( firstLErr );

#else

    res = iView.FirstL();

#endif // _DEBUG

    if ( res ) // Check if found a hit
        {
        COM_TRACE_( "[ACCPOLICYDB] CAccPolicyDB::FindGIDIntL() A GIDInt for this Device ID was found" );
        RDbColReadStream in;
        CDbColSet* colSet = iView.ColSetL();
        CleanupStack::PushL( colSet );
        TDbColNo col = colSet->ColNo( KACCDBGIDIntGIDCol );
        iView.GetL();
        in.OpenL( iView, col );
        in.PushL();
        in.ReadL( aGIDInt, iView.ColLength( col ) );
        in.Pop();
        in.Close();
        CleanupStack::PopAndDestroy( colSet );
        }

    iView.Reset();
    iView.Close();

    API_TRACE_( "[ACCPOLICYDB] CAccPolicyDB::FindGIDIntL() return void" );
    }    

// -----------------------------------------------------------------------------
// CAccPolicyDB::ReadAllCriticalCapabilitiesL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CAccPolicyDB::ReadAllCriticalCapabilitiesL()
    {
    API_TRACE_( "[ACCPOLICYDB] CAccPolicyDB::ReadAllCriticalCapabilitiesL()" );
    
    iSQL = KSQLSelect;
    iSQL.Append( KSQLAsterisk );
    iSQL.Append( KSQLFrom );
    iSQL.Append( KACCDBCapRulesTable );
    
    iView.Close();
    iView.Prepare( iDb, TDbQuery( iSQL ) );
    
    if ( iView.Unevaluated() )
        {
        iView.EvaluateAll();
        }

	API_TRACE_1( "[ACCPOLICYDB] CAccPolicyDB::ReadAllCriticalCapabilitiesL - return %d ", iView.CountL() );        

	return iView.CountL();
    }

// -----------------------------------------------------------------------------
// CAccPolicyDB::GetNextCriticalCapabilityL
// -----------------------------------------------------------------------------
//
EXPORT_C void CAccPolicyDB::GetNextCriticalCapabilityL( 
    TUint32& aName, 
    TInt32& aValue )
    {    
    API_TRACE_( "[ACCPOLICYDB] CAccPolicyDB::GetNextCriticalCapabilityL()" );
    TBuf8<KSBMaxValueLength> bufValue;
    
    if ( iView.NextL() ) // Check if found a hit
        {  
        RDbColReadStream in;
        CDbColSet* colSet = iView.ColSetL();
        CleanupStack::PushL( colSet );
                
        TDbColNo col = colSet->ColNo( KACCDBCapRulesName );
        iView.GetL();
        in.OpenL( iView, col );
        in.PushL();
        aName = in.ReadUint32L();
        in.Pop();
        in.Close();
        
        col = colSet->ColNo( KACCDBCapRulesValue );       
        iView.GetL();
        in.OpenL( iView, col );
        in.PushL();        
        in.ReadL( bufValue, iView.ColLength( col ) );        
        TLex8 lex( bufValue);
        User::LeaveIfError( lex.Val( aValue ) );        
        in.Pop();
        in.Close();
        
        CleanupStack::PopAndDestroy( colSet );        
        }
     else
     	{
    	iView.Reset();
    	iView.Close(); 	
     	}

    API_TRACE_2( "[ACCPOLICYDB] CAccPolicyDB::GetNextCriticalCapabilityL - name=0x%x value=%d - return void", aName, aValue );
    }

// -----------------------------------------------------------------------------
// CAccPolicyDB::
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CAccPolicyDB::FilterGIDByTypeL(
    TAccGIDtype aType)
    {

	API_TRACE_( "[ACCPOLICYDB] CAccPolicyDB::FilterGIDByType()" );
    
    iSQL = KSQLSelect;
    iSQL.Append( KSQLAsterisk );
    iSQL.Append( KSQLFrom );
    iSQL.Append( KACCDBGIDIntTable );
    iSQL.Append( KSQLWhere );
    iSQL.Append( KACCDBGIDIntKeyIDCol );
    iSQL.Append( KSQLEquals );
    iSQL.AppendNum( aType );

    iView.Close();

    iView.Prepare( iDb, TDbQuery( iSQL ) );
    if ( iView.Unevaluated() )
        {
        iView.EvaluateAll();
        }

	API_TRACE_( "[ACCPOLICYDB] CAccPolicyDB::FilterGIDByType() return " );        

	return iView.CountL();
    }

// -----------------------------------------------------------------------------
// CAccPolicyDB::
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CAccPolicyDB::GetNextGIDL(
    TUint64& aHWDeviceID,
    TDes8& aGIDInt )
    {

	API_TRACE_( "[ACCPOLICYDB] CAccPolicyDB::GetNextGID()" );
    
    if ( iView.NextL() ) // Check if found a hit
        {      
        
        RDbColReadStream in;
        CDbColSet* colSet = iView.ColSetL();
        CleanupStack::PushL( colSet );
                
        TDbColNo col = colSet->ColNo( KACCDBGIDIntDeviceIDCol );
        iView.GetL();
        in.OpenL( iView, col );
        in.PushL();
        aHWDeviceID = in.ReadInt32L();
        in.Pop();
        in.Close();
        
        col = colSet->ColNo( KACCDBGIDIntGIDCol );       
        iView.GetL();
        in.OpenL( iView, col );
        in.PushL();
        in.ReadL( aGIDInt, iView.ColLength( col ) );
        in.Pop();
        in.Close();
        
        CleanupStack::PopAndDestroy( colSet );        
        }
     else
     	{
    	iView.Reset();
    	iView.Close(); 	
     	}

    API_TRACE_( "[ACCPOLICYDB] CAccPolicyDB::GetNextGID() return void" );
    
    }

// -----------------------------------------------------------------------------
// CAccPolicyDB::
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CAccPolicyDB::GetSelectionsL(
    TUint32 ruleIndex,
    TUint32& aSelectionListBitMask,
    TBool aGetAllDeviceTypes )
    {

    API_TRACE_2( "[ACCPOLICYDB] CAccPolicyDB::GetSelectionsL(rule index=%d GetAll=%d)", ruleIndex, aGetAllDeviceTypes );
    
    iSQL = KSQLSelect;
    
    if( !aGetAllDeviceTypes )
        {        
        iSQL.Append( KSQLAsterisk );
        iSQL.Append( KSQLFrom );
        iSQL.Append( KACCDBSelectionTable );
        iSQL.Append( KSQLWhere );
        iSQL.Append( KACCDBSelectionIndex );
        iSQL.Append( KSQLEquals );
        iSQL.AppendNum( ruleIndex );
        }
    else
        {        
        iSQL.Append( KSQLAsterisk );
        iSQL.Append( KSQLFrom );
        iSQL.Append( KACCDBSelectionTable );
        }
    
    iView.Close();

    iView.Prepare( iDb, TDbQuery( iSQL ) );
    if ( iView.Unevaluated() )
        {
        iView.EvaluateAll();
        }

	if ( iView.FirstL() ) // Check if found a hit
        {

        RDbColReadStream in;

        CDbColSet* colSet = iView.ColSetL();
        CleanupStack::PushL( colSet );
        TDbColNo col = colSet->ColNo( KACCDBCapSelectionValue ); // Ordinal position of value column
        while( !iView.AtEnd() )
            {
			TUint32 value;

            iView.GetL();
            in.OpenL( iView, col );
            in.PushL();
            value = in.ReadUint32L();
            in.Pop();
            in.Close();
            iView.NextL();

			User::LeaveIfError( value );
			            
            aSelectionListBitMask = aSelectionListBitMask | value;
            
            }
        CleanupStack::PopAndDestroy( colSet );
        COM_TRACE_1( "[ACCPOLICYDB] CAccPolicyDB::GetSelectionsL() - selection list = 0b%b", aSelectionListBitMask );
        }
    else
        {
        COM_TRACE_( "[ACCPOLICYDB] CAccPolicyDB::GetSelectionsL() - No values(s) specified for this rule" );
        }

    iView.Reset();
    iView.Close();

    API_TRACE_( "[ACCPOLICYDB] CAccPolicyDB::GetSelectionsL() - return" );    
    
    }
			    
//  End of File
