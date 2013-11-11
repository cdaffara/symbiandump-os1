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
* Description:  AccDBImporter is a tool for importing data to a database.
*
*/



// INCLUDE FILES
#include    "AccDBImporter.h"
#include    "acc_debug.h"
#include    "AccPolicyDBNames.h"
#include    <AccPolGIDHeader.h>
#include    <AccPolCommonNameValuePairs.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TChar KLineEnd1( '\n' );              //line feed, new line
const TChar KLineEnd2( '\r' );              //carriage return
const TChar KDelimiter( ' ' );              //space
const TChar KSubblockEndDelimiter( ',' );   //after KSubblockEndDelimiter are values
const TChar KNameDelimiter( '#' );          //e.g. "1#1#2048"
const TChar KCommentLine( '#' );
const TChar KRulesDelimiter( ';' );			//Rules delimiter, after this begins selection rule

const TInt KCommentLength  = 100;
const TInt KBufSize        = 10000;
const TInt KBufSizeForCaps = 256;
const TInt KCol1 = 1;
const TInt KCol2 = 2;
const TInt KCol3 = 3;

_LIT( KSQLInsert1, "SELECT " );             //SQL functions
_LIT( KSQLInsert2, ", " );
_LIT( KSQLInsert3, " FROM " );

_LIT( KCapRulesPath, "c:\\private\\1020504A\\AccPolCapRules.ini" );
_LIT( KWildChar, "*.dat" );
_LIT( PathToDat, "c:\\private\\1020504A\\" );

const TDesC aScanDir( PathToDat );
const TDesC aWildName( KWildChar );


// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAccDBImporter::CAccDBImporter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccDBImporter::CAccDBImporter()
    {
    }

// -----------------------------------------------------------------------------
// CAccDBImporter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccDBImporter::ConstructL()
    {
    User::LeaveIfError( iFsSession.Connect() );
    User::LeaveIfError( iDbServer.Connect() );
    
    TInt err( KErrNone );
    err = iDb.Create( iFsSession, KImportDBName, KAccDbSecureFormat );
    if ( err )
        {
        // Delete existing file and try again
        User::LeaveIfError( iFsSession.Delete( KImportDBName ) );

        // If still not successfull just leave...
        User::LeaveIfError( iDb.Create( iFsSession, KImportDBName , KAccDbSecureFormat ) );

        }
    
    }

// -----------------------------------------------------------------------------
// CAccDBImporter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAccDBImporter* CAccDBImporter::NewL()
    {
    COM_TRACE_( "[ACCPOLICYDBIMP] TAccSubblockNameArrayAccessor::NewL()" );
    CAccDBImporter* self = new( ELeave ) CAccDBImporter;

    CleanupStack::PushL( self );
    self->ConstructL();

    CleanupStack::Pop( self );

    return self;
    }


// Destructor
CAccDBImporter::~CAccDBImporter()
    {

    iDb.Close();
    iDbServer.Close();
    iFsSession.Close();

    }

// -----------------------------------------------------------------------------
// CAccDBImporter::DoImportL
// -----------------------------------------------------------------------------
//
void CAccDBImporter::DoImportL()
    {
    CreateTablesL();
    DoFindDatsL();
    ImportCapRulesL();
    iDb.Compact();
    }

// -----------------------------------------------------------------------------
// CAccDBImporter::DoImportL
// -----------------------------------------------------------------------------
//
void CAccDBImporter::DoFindDatsL()
    {
    RFs aSession;
    User::LeaveIfError( aSession.Connect() );

    TFindFile file_finder(aSession); 
    CDir* file_list; 
    TInt err = file_finder.FindWildByDir(KWildChar, PathToDat, file_list); 
    while (err==KErrNone)
    {
        TInt i;
        for (i=0; i<file_list->Count(); i++) 
            {
            TParse fullentry;
            fullentry.Set((*file_list)[i].iName,& file_finder.File(),NULL); 
            // Do something with the full Symbian OS filename
            //DoOneFile(aSession, fullentry.FullName()); 
            iFileName.Copy(fullentry.FullName());
            ImportGIDIntsL();
            }
        delete file_list; 
        err=file_finder.FindWild(file_list); 
    }
    
    }


// -----------------------------------------------------------------------------
// CAccDBImporter::CreateTablesL
// Create tables.
// -----------------------------------------------------------------------------
//
void CAccDBImporter::CreateTablesL()
    {
    COM_TRACE_( "[ACCPOLICYDBIMP] TAccSubblockNameArrayAccessor::CreateTablesL()" );
    iDb.Begin();
    // GenericID integer table:
    CDbColSet* colSet = CDbColSet::NewL();

    CleanupStack::PushL( colSet );
    colSet->AddL( TDbCol ( KACCDBGIDIntKeyIDCol, EDbColInt32 ) );
    colSet->AddL( TDbCol ( KACCDBGIDIntDeviceIDCol, EDbColUint32 ) );
    colSet->AddL( TDbCol ( KACCDBGIDIntGIDCol, EDbColLongText8 ) );
    User::LeaveIfError( iDb.CreateTable( KACCDBGIDIntTable, *colSet ) );
    CleanupStack::Pop( colSet );

    delete colSet;
	
 	 // Capability rules table
    colSet = CDbColSet::NewL();
    CleanupStack::PushL( colSet );
    colSet->AddL( TDbCol ( KACCDBCapRulesName, EDbColInt64 ) );
    colSet->AddL( TDbCol ( KACCDBCapRulesValue, EDbColLongText16 ) );
    User::LeaveIfError( iDb.CreateTable( KACCDBCapRulesTable, *colSet ) );
    CleanupStack::Pop( colSet );
    
    delete colSet;
    
    // Selection table
    colSet = CDbColSet::NewL();
    CleanupStack::PushL( colSet );
    colSet->AddL( TDbCol ( KACCDBSelectionIndex, EDbColUint32 ) );
    colSet->AddL( TDbCol ( KACCDBCapSelectionValue, EDbColUint32 ) );
    User::LeaveIfError( iDb.CreateTable( KACCDBSelectionTable, *colSet ) );
    CleanupStack::Pop( colSet );
    
    iDb.Commit();
	delete colSet;
  	   
    }

void CAccDBImporter::ImportGIDIntsL()
    {
    
    COM_TRACE_( "[ACCPOLICYDBIMP] TAccSubblockNameArrayAccessor::ImportGIDIntsL()" );
    RFs session;
    RFile file;
    HBufC8* buf = HBufC8::NewL( KBufSize ); // The max size will be set to file size during read
    CleanupStack::PushL( buf );
    TPtr8 ptrBuf( buf->Des() );
    User::LeaveIfError( session.Connect() );
    User::LeaveIfError( file.Open( session, iFileName, EFileRead ) );
    TBool rulesDatFile( EFalse );
    
    // Read GID Int file to descriptor
    User::LeaveIfError( file.Read( ptrBuf ) );
    TLex8 l( ptrBuf );
    _LIT(KRulesName, "c:\\private\\1020504A\\rules.dat");
    _LIT(KRule, "RULE");
    _LIT(KSelection, "SELECTION");
    TAccGIDtype enumerationOfType = EAccNormal;
    
    if((iFileName.Compare(KRulesName))==0)
    {
    	rulesDatFile = ETrue;
    }
    
    while( !l.Eos() )
        {
        TBuf8<KHWDeviceIDMaxLength> importedDeviceID;
        TBuf8<KGIDIntMaxLength> importedGidInt;
        TBuf8<KGIDIntMaxLength> importedAccessoryName;//This is not used
                                                      //at the moment.
                                                      //When taken in to
                                                      //use, size must be
                                                      //defined correctly.
        TBuf8<KCommentLength> importedComments;    //Debugging purposes
        TBuf8<KGIDIntMaxLength> importedValues;
        TBuf8<100> importedRuleType;
        TBuf8<100> importedRuleIndex;
        TBuf8<100> importedSelections;
        TBuf<100> comp;
        TBool Rule( EFalse );
    	TBool Selection( EFalse );
        
        // Skip possible comment line(s)
        while( l.Peek() == KCommentLine )
            {
            // Skip this line to the end
            while( ( l.Peek() != KLineEnd1 ) && ( !l.Eos() ) )
                {
                importedComments.Append( l.Get () );//l increment
                }
            importedComments.Append( l.Get () );//l increment
            importedComments.Copy( KNullDesC8 );
            }
        if( l.Eos() )
            {
            break;
            }
        if(rulesDatFile)
        {
            while( l.Peek() != KDelimiter )
            {
                importedRuleType.Append( l.Get () );
            }
            
            comp.Copy(importedRuleType);
		
			if((comp.Compare(KRule))==0)
	    	{
	    		Rule = ETrue;
	    		enumerationOfType = EAccRule;
	    	}
			
			if((comp.Compare(KSelection))==0)
	    	{
	    		Selection = ETrue;
	    		enumerationOfType = EAccSelection;
	    	}
    	
        	l.Inc();// Skip over delimiter between Accessory name and device ID
      	    
      	    while( l.Peek() != KDelimiter )
            {
              	importedRuleIndex.Append( l.Get () );
            }

            l.Inc();// Skip over delimiter between Accessory name and device ID
        }
        // Parse Accessory name
        while( l.Peek() != KDelimiter )
            {
            importedAccessoryName.Append( l.Get () );
            }
        
        l.Inc();// Skip over delimiter between Accessory name and device ID

        // Parse device ID
        while( l.Peek() != KDelimiter )
            {
            importedDeviceID.Append( l.Get () );
            }
        // Convert to numerical format to remove leading zero
        // e.g. 0101 -> 101
        TInt64 idd( 0 );
        TLex8 dd( importedDeviceID );
        dd.Val( idd, EHex );
        importedDeviceID.Copy( KNullDesC );
        importedDeviceID.Num( idd, EHex );

        l.Inc(); // Skip over delimiter between device ID and GID Int
		
		importedGidInt.Append( importedAccessoryName );
		importedGidInt.Append( KDelimiter );
		importedGidInt.Append( importedDeviceID );
		importedGidInt.Append( KDelimiter );
		
        // Parse GID Int
        while( ( l.Peek() != KLineEnd1 ) &&
               ( l.Peek() != KLineEnd2 ) &&
               ( !l.Eos() ) &&
               ( ( !rulesDatFile ) || ( l.Peek() != KRulesDelimiter ) ))
            {
            importedGidInt.Append( l.Get () );
            }
        
        
        if( rulesDatFile && !Selection )
        {
        	if(Rule)
        	{
        		l.Inc(); // Skip over delimiter between device ID and GID Int	
        	}
            while( ( l.Peek() != KLineEnd1 ) &&
                   ( l.Peek() != KLineEnd2 ) &&
                   ( !l.Eos() ) )
           		{
           		importedSelections.Append( l.Get () );
           		}
        }

       if(!rulesDatFile)
       {
       	    //Write down the accessories the are going to the database
        
	        RFs dbOfAccsFs;
	        CleanupClosePushL( dbOfAccsFs );
	        User::LeaveIfError( dbOfAccsFs.Connect() );
	        _LIT( KFileName, "c:\\AccNameToDB.tmp" );
	        RFileWriteStream fWrite;
	        CleanupClosePushL( fWrite );
	        TInt errCode( KErrNone );
	        errCode = fWrite.Create( dbOfAccsFs, KFileName, EFileWrite );
	        
	        if( errCode )
	        {
	        	RFile filu;
	        	TInt sizeOfF( 0 );
	        	( void )filu.Open( dbOfAccsFs, KFileName, EFileWrite );
	        	filu.Size( sizeOfF );
	        	fWrite.Attach( filu, sizeOfF );
	        }
	        
	        
	        fWrite << importedAccessoryName;
	        fWrite.CommitL();
	        
	        CleanupStack::PopAndDestroy(&fWrite);
	        
	        CleanupStack::PopAndDestroy(&dbOfAccsFs);
       	
       }
       else
       {
       		RFs dbOfAccsFs;
	        CleanupClosePushL( dbOfAccsFs );
	        User::LeaveIfError( dbOfAccsFs.Connect() );
	        _LIT( KFileName, "c:\\RuleNameToDB.tmp" );
	        _LIT( KFileNameSel, "c:\\SelectionNameToDB.tmp" );
	        RFileWriteStream fWrite;
	        CleanupClosePushL( fWrite );
	        TInt errCode( KErrNone );
	        
	        if( Rule )
	        {
	        	errCode = fWrite.Create( dbOfAccsFs, KFileName, EFileWrite );
	        }
	        
	        else
	        {
	        	errCode = fWrite.Create( dbOfAccsFs, KFileNameSel, EFileWrite );
	        }
	        
	        if( errCode )
	        {
	        	RFile filu;
	        	TInt sizeOfF( 0 );
	        	if( Rule )
	        	{
	        		( void )filu.Open( dbOfAccsFs, KFileName, EFileWrite );
	        	}
	        	else
	        	{
	        		( void )filu.Open( dbOfAccsFs, KFileNameSel, EFileWrite );
	        	}
	        	filu.Size( sizeOfF );
	        	fWrite.Attach( filu, sizeOfF );
	        }
	        
	        
	        fWrite << importedRuleIndex;
	        fWrite.CommitL();
	        
	        CleanupStack::PopAndDestroy(&fWrite);
	        
	        CleanupStack::PopAndDestroy(&dbOfAccsFs);
       }
       
        
        // Skip over line end char(s)
        if ( l.Peek() == KLineEnd2 )
            {
            l.Inc();
            if ( l.Peek() == KLineEnd1 )
                {
                l.Inc();
                }
            }

            //const TInt KBufLenght( 256 );
            
            TBuf<256> sqlInsert( KSQLInsert1 );
            sqlInsert.Append( KACCDBGIDIntDeviceIDCol );
            sqlInsert.Append( KSQLInsert2 );
            sqlInsert.Append( KACCDBGIDIntKeyIDCol );
            sqlInsert.Append( KSQLInsert2 );
            sqlInsert.Append( KACCDBGIDIntGIDCol );
            sqlInsert.Append( KSQLInsert3 );
            sqlInsert.Append( KACCDBGIDIntTable );
            RDbView view;
            view.Close();
            User::LeaveIfError( view.Prepare( iDb, TDbQuery( sqlInsert ),
                                              RDbView::EInsertOnly ) );
            view.InsertL();
            TLex8 lexOfDevID( importedDeviceID );
            TUint32 intOfDevID( 0 );
            lexOfDevID.Val( intOfDevID, EHex );
            if(rulesDatFile)
            {
            	TLex8 lexOfRuleIndex( importedRuleIndex );
            	TUint32 intOfRuleIndex( 0 );
            	lexOfRuleIndex.Val( intOfRuleIndex, EDecimal );
            	view.SetColL( KCol1, intOfRuleIndex );
            	view.SetColL( KCol2, enumerationOfType );
            }
            else
            {
            	view.SetColL( KCol1, intOfDevID );
            	view.SetColL( KCol2, enumerationOfType );
            }
            view.SetColL( KCol3, importedGidInt );
            
            view.PutL();
            view.Reset();
            view.Close();
            
            if(Rule)
            {
	            TBuf<100> importedSelections1;
	            importedSelections1.Copy( importedSelections );
	            TLex importedLexiSelections( importedSelections1 );
	            TInt32 selectionInInt32;
	            importedLexiSelections.Val( selectionInInt32 );
	            
	            TBuf<100> importedRuleIndex1;
	            importedRuleIndex1.Copy( importedRuleIndex );
	            TLex ruleLexi( importedRuleIndex1 );
	            TInt32 ruleIndexInInt32;
	            ruleLexi.Val( ruleIndexInInt32 );
	            //Set the selection table
	            TBuf<256> sqlInsert1( KSQLInsert1 );
	            sqlInsert1.Append( KACCDBSelectionIndex );
	            sqlInsert1.Append( KSQLInsert2 );
	            sqlInsert1.Append( KACCDBCapSelectionValue );
	            sqlInsert1.Append( KSQLInsert3 );
	            sqlInsert1.Append( KACCDBSelectionTable );
	            RDbView view1;
	            view1.Close();
	            User::LeaveIfError( view1.Prepare( iDb, TDbQuery( sqlInsert1 ),
	                                              RDbView::EInsertOnly ) );
	            view1.InsertL();
	            
	            view1.SetColL( KCol1, ruleIndexInInt32 );
            	view1.SetColL( KCol2, selectionInInt32 );
            	view1.PutL();
            	view1.Reset();
            	view1.Close();
            	
            }
            
#ifdef _DEBUG
            TBuf16<KHWDeviceIDMaxLength> tmpDeviceID;
            tmpDeviceID.Copy( importedDeviceID );
            TBuf16<KGIDIntMaxLength> tmpGidInt;
            tmpGidInt.Copy( importedGidInt );
            TBuf16<KGIDIntMaxLength> tmpAccessoryName;
            tmpAccessoryName.Copy( importedAccessoryName );
            
            if(rulesDatFile)
            {
            	TBuf16<KHWDeviceIDMaxLength> ruleType;
            	ruleType.Copy(importedRuleType);
            	TBuf16<KHWDeviceIDMaxLength> ruleIndex1;
            	ruleIndex1.Copy(importedRuleIndex);
            	COM_TRACE_1( "[ACCPOLICYDBIMP] %S", &tmpAccessoryName );
	            COM_TRACE_1( "                 Device ID = %S", &tmpDeviceID );
	            COM_TRACE_1( "                 Rule Type  = %S ", &ruleType );
	            COM_TRACE_1( "                 Enumeration of type %d", enumerationOfType );
	            COM_TRACE_1( "                 GID Int  = %S", &tmpGidInt );
	            COM_TRACE_1( "                 Rule Index  = %S", &ruleIndex1 );
            }
            else
            {
            	COM_TRACE_1( "[ACCPOLICYDBIMP] %S", &tmpAccessoryName );
	            COM_TRACE_1( "                 Device ID = %S", &tmpDeviceID );
	            COM_TRACE_1( "                 Enumeration of type %d", enumerationOfType );
	            COM_TRACE_1( "                 GID Int  = %S", &tmpGidInt );
            }
            
#endif
        
        // Skip possible comment line(s)
        while( l.Peek() == KCommentLine )
            {
            // Skip this line to the end
            while( ( l.Peek() != KLineEnd1 ) && ( !l.Eos() ) )
                {
                importedComments.Append( l.Get () ); // l increment
                }
            importedComments.Append( l.Get () ); // l increment
            importedComments.Copy( KNullDesC8 );
            }
        }
    file.Close();
    session.Close();

    CleanupStack::Pop( buf );
    delete buf;
    }

void CAccDBImporter::ImportCapRulesL()
    {
    COM_TRACE_( "[ACCPOLICYDBIMP] TAccSubblockNameArrayAccessor::ImportCapRulesL()" );
    RFs session;
    RFile file;
    HBufC8* buf = HBufC8::NewL( KBufSize ); // The max size will be set to file size during read
    CleanupStack::PushL( buf );

    TPtr8 ptrBuf( buf->Des() );
    User::LeaveIfError( session.Connect() );
    User::LeaveIfError( file.Open( session, KCapRulesPath, EFileRead ) );

    // Read Capability name and value to descriptor
    User::LeaveIfError( file.Read( ptrBuf ) );
    TLex8 l( ptrBuf );

    while( !l.Eos() ) // 1
        {
        TBuf<KSBMaxNameLength>  importedCapName;
        TBuf<KSBMaxValueLength> importedCapValue;
        TInt64 importedTIntValue;
        TBuf8<KCommentLength> importedComments; // Debugging purposes

        // Skip possible comment line(s)
        while( l.Peek() == KCommentLine )
            {
            // Skip this line to the end
            while( ( l.Peek() != KLineEnd1 ) && ( !l.Eos() ) )
                {
                importedComments.Append( l.Get () ); // l increment
                }
            importedComments.Append( l.Get () ); // l increment
            importedComments.Copy( KNullDesC8 );
            }
        if( l.Eos() )
            {
            break;
            }

        // Parse Capability name
        while( l.Peek() != KDelimiter )
            {
            importedCapName.Append( l.Get () );
            }
		
		TLex importedTLexValue(importedCapName);
		importedTLexValue.Val(importedTIntValue, EHex);
        
        l.Inc();// Skip over delimiter between Capability name and its value

        // Parse Capability value
        while( ( l.Peek() != KLineEnd1 ) &&
               ( l.Peek() != KLineEnd2 ) &&
               ( !l.Eos() ) )
            {
            importedCapValue.Append( l.Get () );
            }

        // Skip over line end char(s)
        if ( l.Peek() == KLineEnd2 )
            {
            l.Inc();
            if ( l.Peek() == KLineEnd1 )
                {
                l.Inc();
                }
            }

        // Insert Capability name and value to Acc Policy DB
        TBuf<KBufSizeForCaps> sqlInsert( KSQLInsert1 );
        sqlInsert.Append( KACCDBCapRulesName );
        sqlInsert.Append( KSQLInsert2 );
        sqlInsert.Append( KACCDBCapRulesValue );
        sqlInsert.Append( KSQLInsert3 );
        sqlInsert.Append( KACCDBCapRulesTable );
        RDbView view;
        view.Close();
        User::LeaveIfError( view.Prepare( iDb, TDbQuery( sqlInsert ),
                                         RDbView::EInsertOnly ) );
        view.InsertL();
        view.SetColL( KCol1, importedTIntValue );
        view.SetColL( KCol2, importedCapValue );
        view.PutL();
        
#ifdef _DEBUG

        TBuf<KSBMaxNameLength>  tmpImportedCapName;
        TBuf<KSBMaxValueLength> tmpImportedCapValue;

        tmpImportedCapName.Copy( importedCapName );
        tmpImportedCapValue.Copy( importedCapValue );

        COM_TRACE_( "[ACCPOLICYDBIMP]" );
        COM_TRACE_1( "    Capability name  = %S", &tmpImportedCapName );
        COM_TRACE_1( "    Capability value = %S", &tmpImportedCapValue );
#endif

        //***************************
        // Parse capability table end
        //***************************

        // Skip possible comment line(s)
        while( l.Peek() == KCommentLine ) // 2
            {
            // Skip this line to the end
            while( ( l.Peek() != KLineEnd1 ) && ( !l.Eos() ) ) // 3
                {
                importedComments.Append( l.Get () );//l increment
                } // End of while 1
            importedComments.Append( l.Get () );//l increment
            importedComments.Copy( KNullDesC8 );
            } // End of while 2
         } // End of while 3

    file.Close();
    session.Close();
    
    CleanupStack::Pop( buf );    
    delete buf;
    }

//  End of File
