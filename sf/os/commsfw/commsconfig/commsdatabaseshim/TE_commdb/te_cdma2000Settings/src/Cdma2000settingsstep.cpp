//
//
// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//
//
//

/**
 @file 
 @internalComponent 
 
 CommDB CDMA2000 Settings test implementation
 */

#include "Cdma2000settingsstep.h"
#include <testexecutelog.h>

#include <e32std.h>
#include <e32base.h>

using namespace te_Cdma2000CommDbSettings;

namespace
	{
	// "End of table" marker.
	_LIT(KNoMoreEntriesLit, "NO_MORE_ENTRIES");
	const TPtrC KNoMoreEntries(KNoMoreEntriesLit);
	
	
	//
	// CDMA2000 CommDB column constants:
	//
	// These constants are defined in cdbcols.h and are used 
	// to access CDMA related settings via CommDB. Some constants in the 
	// are aliased to the generic SERVICE_xxx constants, e.g
	//
	// #define CDMA_ENABLE_IP_HEADER_COMP  SERVICE_ENABLE_IP_HEADER_COMP
	//
	// The tables below may contain entries for both the SERVICE_xxx constant
	// and the corresponding CDMA_xxx constant. The purpose of this duplication is 
	// to verify that the particular field is accessible via both the SERVICE and the CDMA 
	// constant. The SERVICE_xxx constants are typically used by the sytem (e.g. PPP) to access 
	// settings common to all the service tables. The specific CDMA_xxx constants may be used by CDMA 
	// specific code to manipulate CDMA tables only.
		
	// GlobalSettings table: TUint32 values.
	const TPtrC KCdmaGlobalSettingsTableTUint32[] =
		{		
		CDMA_OP_CAPABILITY,
		CDMA_SIMIP_MAX_NUM_NAI,
		CDMA_SIMIP_MAX_NAI_LENGTH,
		CDMA_SIMIP_MAX_SS_LENGTH,
		CDMA_SIMIP_AUTH_SUPPORTED,
		CDMA_MIP_MAX_NUM_NAI,
		CDMA_MIP_MAX_NAI_LENGTH,
		CDMA_MIP_MAX_MN_AAA_SS_LENGTH,
		CDMA_MIP_MN_AAA_AUTH_ALGORITHM,
		CDMA_MIP_MAX_MN_HA_SS_LENGTH,
		CDMA_MIP_MN_HA_AUTH_ALGORITHM,
		KNoMoreEntries
		};
	
	// DefaultCDMA2000SettingsTable: TBool values.
	const TPtrC KDefaultCdma2000SettingsTableTBool[] = 
		{
		DEFAULT_CDMA_ENABLE_IP_HEADER_COMP, // #defined as SERVICE_ENABLE_IP_HEADER_COMP
		SERVICE_ENABLE_IP_HEADER_COMP,
		KNoMoreEntries	
		};
		
	// DefaultCDMA2000SettingsTable: TUint32 values.
	const TPtrC KDefaultCdma2000SettingsTableTUint32[] = 
		{
		CDMA_OP_MODE, 
		CDMA_MIP_MAX_NUM_RETRY, 
		CDMA_MIP_FIRST_RETRY_TIMEOUT,
		CDMA_MIP_REREG_THRESHOLD,
		DEFAULT_CDMA_FCH_MUX,
		DEFAULT_CDMA_SCH_MUX,
		KNoMoreEntries	
		};
		
	// CDMA2000PacketServiceTable: TUint32 values.
	const TPtrC KCdma2000PacketServiceTableTUint32[] =
		{		
		CDMA_NAI_TYPE,
		CDMA_SIMIP_AUTH_ALGORITHM,
		CDMA_SIMIP_PAP_SS_HANDLE,
		CDMA_SIMIP_CHAP_SS_HANDLE,			
		CDMA_MIP_T_BIT,			
		CDMA_MIP_MN_AAA_AUTH_ALGORITHM,
		CDMA_MIP_MN_AAA_SPI,
		CDMA_MIP_MN_HA_AUTH_ALGORITHM,
		CDMA_MIP_MN_HA_SPI,
		CDMA_FCH_MUX,
		CDMA_SCH_MUX,
		KNoMoreEntries
		};
	
	// CDMA2000PacketServiceTable: TBool values.
	const TPtrC KCdma2000PacketServiceTableTBool[] = 
		{
		SERVICE_IF_EXTERN_IP_CONFIG_ALWAYS_REJECT_AUTH,
		CDMA_IF_EXTERN_IP_CONFIG_ALWAYS_REJECT_AUTH, // #defined as SERVICE_IF_EXTERN_IP_CONFIG_ALWAYS_REJECT_AUTH
		CDMA_MIP_MN_AAA_SPI_INDICATOR,
		CDMA_MIP_MN_HA_SPI_INDICATOR,
		CDMA_ENABLE_IP_HEADER_COMP, // #defined as SERVICE_ENABLE_IP_HEADER_COMP
		SERVICE_ENABLE_IP_HEADER_COMP,
		SERVICE_ENABLE_SW_COMP,
		KNoMoreEntries
		};
	
	// CDMA2000PacketServiceTable: Text values.	
	const TPtrC KCdma2000PacketServiceTableText[] = 
		{
		CDMA_MIP_HOME_ADDRESS,
		CDMA_MIP_PRIMARY_HOME_AGENT,
		CDMA_MIP_SECONDARY_HOME_AGENT,
		CDMA_MIP_MN_AAA_SS_DATA,
		CDMA_MIP_MN_HA_SS_DATA,
		KNoMoreEntries
		};
			
	// DialOutIsp table: TBool values.
	const TPtrC KDialOutIspTableTBool[] =
		{
		ISP_IF_SERVER_MODE,
		KNoMoreEntries
		};
	
	
	//----------------------------------------------------------------
	// Test for named constants:
	// If the following constants are absent, this code will not compile.	
	const TUint KCdmaEnumValues[] =
		{
		//-------------------------------------------------
		// Global Settings
				
		// CDMA_OP_CAPABILITY values
		ECommDbCdmaOpCapabilitySimpleIp,
		ECommDbCdmaOpCapabilityMobileIp,
		ECommDbCdmaOpCapabilityFallback,
		
		// CDMA_SIMIP_AUTH_SUPPORTED values
		ECommDbCdmaSimpIpCapabilityChap,
		ECommDbCdmaSimpIpCapabilityPap,
		
		// CDMA_MIP_MN_AAA_AUTH_ALGORITHM 
		// CDMA_MIP_MN_HA_AUTH_ALGORITHM values
		ECommDbCdmaMIpMd5,

		
		//---------------------------------------------------
		// DEFAULT_CDMA2000_SETTINGS_TABLE 
				
		//CDMA_OP_MODE
		ECommDbCdmaOpSimpleIp,
		ECommDbCdmaOpFallback,
		ECommDbCdmaOpMobileIp,	
		
		//----------------------------------------------------
		// CDMA2000_PACKET_SERVICE_TABLE
					
		//  CDMA_OP_MODE values
		ECommDbCdmaOpSimpleIp,
		ECommDbCdmaOpFallback,
		ECommDbCdmaOpMobileIp,	
		
		// CDMA_NAI_TYPE values
		ECommDbCdmaNaiSimpleIp,
		ECommDbCdmaNaiMobileIp,
		
		// CDMA_SIMIP_AUTH_ALGORITHM
		ECommDbCdmaSimpIpAuthNone,
		ECommDbCdmaSimpIpAuthChap,
		ECommDbCdmaSimpIpAuthPap,
		ECommDbCdmaSimpIpAuthFallback,
		
		// CDMA_MIP_MN_AAA_AUTH_ALGORITHM
	    // CDMA_MIP_MN_HA_AUTH_ALGORITHM values
		ECommDbCdmaMIpAuthNone,
		ECommDbCdmaMIpAuthMd5	
		};	
	//-------------------------------------------------------------------	
	} // module namespace


/**
Verifies that the specified TBool columns can be read from CommDB.

@param aCommDbTableCols CommDB columns to be read
@return ETrue if all the columns could be read, EFalse otherwise
*/
TBool CCdma2000SettingsTestStep::CheckTableTBool(const TPtrC aCommDbTableCols[])
	{
	TBool isCheckOk = ETrue;
	__UHEAP_MARK;	
	
	// Attempt to read all the columns.
		
	for (TInt curCol = 0; KNoMoreEntries != aCommDbTableCols[curCol]; curCol++)
		{
		TBool value = EFalse; 
		TRAPD(err, iCommDbView->ReadBoolL(aCommDbTableCols[curCol], value));						
	
		switch(err)
			{
			case KErrNone: // Column exists and is not null: OK
			INFO_PRINTF3(_L("OK: Column [%S], value. [%d]"), &aCommDbTableCols[curCol], value);
			break;
			
			case KErrNotFound: // Column does not exist: Error
				INFO_PRINTF3(_L("ERROR.  Column [%S] does not exist. Error code [%d]: KErrNotFound"), &aCommDbTableCols[curCol], err);
				isCheckOk = EFalse;
				break;
			
			case KErrUnknown: // Column exists, but is null: Error
				INFO_PRINTF3(_L("ERROR.  Column [%S] does exists but is null: Error code [%d]: KErrUnknown."), &aCommDbTableCols[curCol], err);
				isCheckOk = EFalse;
				break;
			
			default: // Some other error.
				INFO_PRINTF3(_L("ERROR.  Error code [%d] when reading column [%S]."), err, &aCommDbTableCols[curCol]);
				isCheckOk = EFalse;
			};	
		}	
	
	__UHEAP_MARKEND;
	return isCheckOk;
	}


/**
Verifies that the specified TUint32 columns can be read from CommDB.

@param aCommDbTableCols CommDB columns to be read
@return ETrue if all the columns could be read, EFalse otherwise
*/
TBool CCdma2000SettingsTestStep::CheckTableTUint32(const TPtrC aCommDbTableCols[])
	{
	TBool isCheckOk = ETrue;
	__UHEAP_MARK;
	
	// Attempt to read all the columns.
	
	for(TInt curCol = 0; KNoMoreEntries != aCommDbTableCols[curCol]; curCol++)
		{
		TUint32 value = 0; 
		TInt err = KErrNone;
		if(aCommDbTableCols == KCdmaGlobalSettingsTableTUint32)
			{
			TRAP(err, iCommDb->GetGlobalSettingL(aCommDbTableCols[curCol], value));
			}			
		else
			{
			TRAP(err, iCommDbView->ReadUintL(aCommDbTableCols[curCol], value));
			}			
	
		switch(err)
			{
			case KErrNone: // Column exists and is not null: OK
			INFO_PRINTF3(_L("OK: Column [%S], value. [%d]"), &aCommDbTableCols[curCol], value);
			break;
			
			case KErrNotFound: // Column does not exist: Error
				INFO_PRINTF3(_L("ERROR.  Column [%S] does not exist. Error code [%d]: KErrNotFound"), &aCommDbTableCols[curCol], err);
				isCheckOk = EFalse;
				break;
			
			case KErrUnknown: // Column exists, but is null: Error
				INFO_PRINTF3(_L("ERROR.  Column [%S] does exists but is null Error code [%d]: KErrUnknown."), &aCommDbTableCols[curCol], err);
				isCheckOk = EFalse;
				break;
			
			default: // Some other error.
				INFO_PRINTF3(_L("ERROR.  Error code [%d] when reading column [%S]."), err, &aCommDbTableCols[curCol]);
				isCheckOk = EFalse;
			};	
		}	
	
	__UHEAP_MARKEND;
	return isCheckOk;
	}	
	
/**
Verifies that the specified TText columns can be read from CommDB.

@param aCommDbTableCols CommDB columns to be read
@return ETrue if all the columns could be read, EFalse otherwise
*/	
TBool CCdma2000SettingsTestStep::CheckTableText(const TPtrC aCommDbTableCols[])
	{
	TBool isCheckOk = ETrue;
	__UHEAP_MARK;
	
	// Attempt to read all the columns.
	
	for(TInt curCol = 0; KNoMoreEntries != aCommDbTableCols[curCol]; curCol++)
		{
		TBuf<KCommsDbSvrMaxColumnNameLength> value; 
		TRAPD(err, iCommDbView->ReadTextL(aCommDbTableCols[curCol], value));
		
		switch(err)
			{
			case KErrNone: // Column exists and is not null: OK				
				INFO_PRINTF3(_L("OK: Column [%S], value. [%S]"), &aCommDbTableCols[curCol],  &value);	
				break;
			
			case KErrNotFound: // Column does not exist: Error
				INFO_PRINTF3(_L("ERROR.  Column [%S] does not exist. Error code [%d]: KErrNotFound"), &aCommDbTableCols[curCol], err);
				isCheckOk = EFalse;
				break;
			
			case KErrUnknown: // Column exists, but is null: Error
				INFO_PRINTF3(_L("ERROR.  Column [%S] does exists but is null Error code [%d]: KErrUnknown."), &aCommDbTableCols[curCol], err);
				isCheckOk = EFalse;
				break;
			
			default: // Some other error.
				INFO_PRINTF3(_L("ERROR.  Error code [%d] when reading column [%S]."), err, &aCommDbTableCols[curCol]);
				isCheckOk = EFalse;
			}	
		}
	
	__UHEAP_MARKEND;
	return isCheckOk;
	}


/**
 C++ Constructor
 
 @post Test name is setup. 
 */	
CCdma2000SettingsTestStep::CCdma2000SettingsTestStep():
	iCommDb(NULL),
	iCommDbView(NULL)	
	{
	SetTestStepName(KCdma2000SettingsStep);		
	}

/** C++ Destructor */	
CCdma2000SettingsTestStep::~CCdma2000SettingsTestStep()
	{
	if(iCommDbView) delete iCommDbView; // Lint generates warning if CloseCommDbTable() is used here.
	if(iCommDb)     delete iCommDb;
	}
	
/** Closes CommDB table opened previously with a call to OpenCommDbTableL */
void CCdma2000SettingsTestStep::CloseCommDbTable()
	{
	if(iCommDbView)
		{
		delete iCommDbView;
		iCommDbView = NULL;
		}
	}

/**
Opens a view on the first record of a CommDB table.

@param aTableName the table to open.
@leave if CommDB access methods leave. 
*/
void CCdma2000SettingsTestStep::OpenCommDbTableL(const TPtrC& aTableName)
	{
	ASSERT(!iCommDbView);
	
	const TUint32 KRecordId = 1;
	iCommDbView = iCommDb->OpenViewMatchingUintLC(aTableName, TPtrC(COMMDB_ID), KRecordId);
	// Take iCommDbView off the cleanup stack so it is never deleted without
	// setting the pointer to NULL, but rather with a call to
	// CloseCommDbTable() or in the destructor.
	CleanupStack::Pop(iCommDbView);
	
	TInt err = iCommDbView->GotoFirstRecord(); 
	if (KErrNone != err)
		{
		INFO_PRINTF4(_L("Table[%S]:  Record[%d] does not exist. Error[%d]"), &aTableName, KRecordId, err);				
		User::Leave(err);		
		}
	INFO_PRINTF1(_L(""));
	INFO_PRINTF3(_L("Opened Table[%S].  Record[%d]. ========================== "), &aTableName, KRecordId);	
	}


/**
Carries out the actual test sequence for CDMA2000 CommDB Settings. 

@leave if CommDB access methods leave. 
*/
void CCdma2000SettingsTestStep::DoTestSequenceL()
	{
	__UHEAP_MARK;

	//
	// Create CommDB
	//
	ASSERT(!iCommDb);
	iCommDb = CCommsDatabase::NewL();
	// Don't put iCommDb onto cleanup stack so it is never deleted without
	// setting the pointer to NULL, but rather below or in the destructor.
	iCommDb->ShowHiddenRecords();
	
	//
	// Carry out the testing	
	//
	
	// DefaultCDMA2000SettingsTable	
	OpenCommDbTableL(TPtrC(DEFAULT_CDMA2000_SETTINGS_TABLE));	
	if(CheckTableTUint32(KDefaultCdma2000SettingsTableTUint32) &&
	   CheckTableTBool(KDefaultCdma2000SettingsTableTBool))
		{
		INFO_PRINTF1(_L("===>OK: DefaultCDMA2000Settings table"));
		}
	else
		{
		INFO_PRINTF1(_L("===>ERROR: accessing DefaultCDMA2000Settings table."));
		SetTestStepResult(EFail);
		}
	CloseCommDbTable();
	
	// CDMA2000PacketServiceTable	
	OpenCommDbTableL(TPtrC(CDMA2000_PACKET_SERVICE_TABLE));
	if(CheckTableTUint32(KCdma2000PacketServiceTableTUint32) && // TUint32 columns
	   CheckTableTBool(KCdma2000PacketServiceTableTBool)     && // TBool columns     
	   CheckTableText(KCdma2000PacketServiceTableText))         // Text columns
		{
		INFO_PRINTF1(_L("===>OK: CDMA2000PacketServiceTable table"));
		}
	else
		{
		INFO_PRINTF1(_L("===>ERROR: accessing CDMA2000PacketServiceTable table."));
		SetTestStepResult(EFail);
		}
	CloseCommDbTable();	
	
	// DialOutIsp
	OpenCommDbTableL(TPtrC(DIAL_OUT_ISP));
	if(CheckTableTBool(KDialOutIspTableTBool))
		{
		INFO_PRINTF1(_L("===>OK: DialOutIsp table"));
		}
	else
		{
		INFO_PRINTF1(_L("===>ERROR: accessing DialOutIsp table."));
		SetTestStepResult(EFail);
		}	
	CloseCommDbTable();		
	
	// Global Settings
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Reading Global Settings ================================"));				
	if(CheckTableTUint32(KCdmaGlobalSettingsTableTUint32))
		{
		INFO_PRINTF1(_L("===>OK: GlobalSettings table"));
		}
	else
		{
		INFO_PRINTF1(_L("===>ERROR: accessing GlobalSettings table."));
		SetTestStepResult(EFail);
		}
	

	//
	// Close CommDB
	//
	delete iCommDb;
	iCommDb = NULL;
	
	__UHEAP_MARKEND;
	}

/**
 Verifies that all CDMA2000 settings can be accessed.
 
 @return result of the test: EPass if all settings can be accessed.
 */
TVerdict CCdma2000SettingsTestStep::doTestStepL()
	{
	//---------------------------------------------------------------------
	// Reference KCdmaEnumValues[] to supress ARM5 compiler warning regarding unreferenced variable
	// This has no consequences for the test.
	INFO_PRINTF2(_L("Referencing KCdmaEnumValues: element 0=[%d]"), KCdmaEnumValues[0]);
	//---------------------------------------------------------------------
	
	
	
	SetTestStepResult(EPass);
	
	TRAPD(testErr, DoTestSequenceL());
	if(KErrNone != testErr)
		{
		SetTestStepResult(ETestSuiteError);
		INFO_PRINTF2(_L("ERROR: internal test suite error [%d]."), testErr);
		}	
			
	return TestStepResult();		
	}
	
