/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#include "t_raccessoryconnectiondata.h"

/*@{*/
//LIT's for reading params from ini file
_LIT(KRAccessoryServer, "RAccessoryServer");
_LIT( KDeviceTypeCap, "DeviceTypeCap" );
/*@}*/


/*@{*/
//LIT's for commands
_LIT( KCmdCreateAccessorySubSession, "CreateSubSession");
_LIT( KCmdCloseAccessorySubSession, "CloseSubSession");
_LIT( KCmdGetAccessoryConnectionStatus, "GetAccessoryConnectionStatus");
_LIT( KCmdUtilityVerifyDeviceTypeCaps, "VerifyDeviceTypeCaps");
/*@}*/



/**
 * Two phase constructor
 *
 * @leave	system wide error
 */
CT_RAccessoryConnectionData* CT_RAccessoryConnectionData::NewL()
	{
	CT_RAccessoryConnectionData* ret=new (ELeave) CT_RAccessoryConnectionData();
	return ret;
	}

/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
TAny* CT_RAccessoryConnectionData::GetObject()
	{
	return NULL;
	}


/**
 * Process a command read from the ini file
 *
 * @param aCommand	The command to process
 * @param aSection	The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return	ETrue if the command is processed
 *
 * @leave System wide error
 */
TBool CT_RAccessoryConnectionData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
{
	TBool	ret = ETrue;

	if ( aCommand == KCmdCreateAccessorySubSession)
		{
		DoCmdCreateSubSession(aSection);
		}
	
	else if ( aCommand == KCmdCloseAccessorySubSession)
		{
		DoCmdCloseSubSession();
		}
	
	else if ( aCommand == KCmdGetAccessoryConnectionStatus)
		{
		DoCmdGetAccessoryConnectionStatus();
		}
	else if ( aCommand == KCmdUtilityVerifyDeviceTypeCaps)
		{
		DoCmdUtilityVerifyDeviceTypeCaps(aSection);
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}

/**
 * Creates a subsession with accessory Server
 * @return err Symbian error code. KErrNone if successful
 */
void CT_RAccessoryConnectionData::DoCmdCreateSubSession(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_RAccessoryConnectionData::DoCmdCreateSubSession"));
	TPtrC	rAccessoryServName;
	
	if ( !GetStringFromConfig(aSection, KRAccessoryServer(), rAccessoryServName) )
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRAccessoryServer);
		SetBlockResult(EFail);
		}
	else
		{
		RAccessoryServer* accServer=static_cast<RAccessoryServer*>(GetDataObjectL(rAccessoryServName));
		// The test verdict, set by the actual test function
		TInt err( KErrNone );
		err = iConnection.CreateSubSession( *accServer );

		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Failed to create subsession [%d]"), err);
			SetError(err);
			}
		}
	INFO_PRINTF1(_L("*END*CT_RAccessoryConnectionData::DoCmdCreateSubSession"));
	}


/**
 * Closes a subsession with accessory Server
 * @return err Symbian error code. KErrNone if successful
 */
void CT_RAccessoryConnectionData::DoCmdCloseSubSession()
	{
	INFO_PRINTF1(_L("*START*CT_RAccessoryConnectionData::DoCmdCloseSubSession"));
	TInt err( KErrNone );
	err = iConnection.CloseSubSession();

	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Failed to close subsession [%d]"), err);
		SetError(err);
		}
	INFO_PRINTF1(_L("*END*CT_RAccessoryConnectionData::DoCmdCloseSubSession"));
	}


/**
 * Fills a generic ID array with all currently connected accessories
 * @return err Symbian error code. 
 */
void CT_RAccessoryConnectionData::DoCmdGetAccessoryConnectionStatus()
	{
	INFO_PRINTF1(_L("*START*CT_RAccessoryConnectionData::DoCmdGetAccessoryConnectionStatus"));
	iGenericIDarray = TAccPolGenericIDArray();	
	// Get combined accessory connection status, i.e. all currently
	// connected accessories.
	TInt err = iConnection.GetAccessoryConnectionStatus( iGenericIDarray );
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Failed to get accessory connection status [%d]"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("Accessory iGenericIDarray contains %d items"), iGenericIDarray.Count());

		for(TInt i = 0; i < iGenericIDarray.Count(); i++)
			{
			const TAccPolGenericID id = iGenericIDarray.GetGenericIDL(i);
			TPtrC modelId = id.HWModelID();
			TUint32 appProtocolCaps = id.ApplicationProtocolCaps();
			TUint64 hwdeviceId = id.HWDeviceID();
			TUint64 deviceAddress = id.DeviceAddress();
			INFO_PRINTF2( _L("HW Model ID: %S"), &modelId );
			INFO_PRINTF2( _L("Application protocol capabilities: %i"), appProtocolCaps );
			INFO_PRINTF2( _L("HW device ID: %Lu"), hwdeviceId );
			INFO_PRINTF2( _L("Device address: %Lu"), deviceAddress );
			
			TUint32 deviceTypeCaps = id.DeviceTypeCaps();
			LogDeviceTypeCapabilities( deviceTypeCaps );
			}
		}
	INFO_PRINTF1(_L("*END*CT_RAccessoryConnectionData::DoCmdGetAccessoryConnectionStatus"));
	}


/**
 * Checks that all the expected device type capabilites are set.
 * @param	aExpectedDeviceTypeCaps		TUint value indicating expected device type capabilities
 * @return 	err Symbian error code.
 */
void CT_RAccessoryConnectionData::DoCmdUtilityVerifyDeviceTypeCaps( const TTEFSectionName& aSection )
	{
	INFO_PRINTF1(_L("*START*CT_RAccessoryConnectionData::DoCmdVerifyDeviceTypeCaps"));
	TUint expectedDeviceTypeCaps;
	
	if(!GetUintFromConfig(aSection,KDeviceTypeCap, expectedDeviceTypeCaps))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDeviceTypeCap);
		SetBlockResult(EFail);
		}
	else
		{
		// Initial error code applies if no accessories are connected
		// -> It is assumed that at least one accessory is connected when
		// DoCmdVerifyDeviceTypeCaps is called
		TInt err( KErrNotFound );

		for(TInt i = 0; i < iGenericIDarray.Count(); i++ )
			{
			const TAccPolGenericID id = iGenericIDarray.GetGenericIDL(i);
			TUint32 deviceTypeCaps = id.DeviceTypeCaps();
			LogDeviceTypeCapabilities( deviceTypeCaps );

			// Check that all the expected type capabilities are set
			if( ( expectedDeviceTypeCaps & deviceTypeCaps ) == expectedDeviceTypeCaps )
				{
				err = KErrNone;
				}
			}

		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Failed to verify device type caps [%d]"), err);
			SetError(err);
			}
		}
	
	INFO_PRINTF1(_L("*END*CT_RAccessoryConnectionData::DoCmdVerifyDeviceTypeCaps"));
	}


/**
 * Prints the Device Type Capabilities
 * @param aDeviceTypeCaps	The device type capabilites
 */
void CT_RAccessoryConnectionData::LogDeviceTypeCapabilities( const TUint aDeviceTypeCaps )
	{
	INFO_PRINTF1(_L("*START*CT_RAccessoryConnectionData::LogDeviceTypeCapabilities"));
	INFO_PRINTF2( _L("Device type capabilities: %i"), aDeviceTypeCaps );

	if( aDeviceTypeCaps & KDTHeadset )
		{
		INFO_PRINTF1(_L("Device type capability KDTHeadset"));
		}
	else if( aDeviceTypeCaps & KDTCarKit )
		{
		INFO_PRINTF1(_L("Device type capability KDTCarKit"));
		}
	else if( aDeviceTypeCaps & KDTOffice )
		{
		INFO_PRINTF1(_L("Device type capability KDTOffice"));
		}
	else if( aDeviceTypeCaps & KDTMessaging )
		{
		INFO_PRINTF1(_L("Device type capability KDTMessaging"));
		}
	else if( aDeviceTypeCaps & KDTImaging )
		{
		INFO_PRINTF1(_L("Device type capability KDTImaging"));
		}
	else if( aDeviceTypeCaps & KDTLoopset )
		{
		INFO_PRINTF1(_L("Device type capability KDTLoopset"));
		}
	else if( aDeviceTypeCaps & KDTDataCable )
		{
		INFO_PRINTF1(_L("Device type capability KDTDataCable"));
		}
	else if( aDeviceTypeCaps & KDTCharger )
		{
		INFO_PRINTF1(_L("Device type capability KDTCharger"));
		}
	else if( aDeviceTypeCaps & KDTTTY )
		{
		INFO_PRINTF1(_L("Device type capability KDTTTY"));
		}
	else if( aDeviceTypeCaps & KDTAVDevice )
		{
		INFO_PRINTF1(_L("Device type capability KDTAVDevice"));
		}
	else
		{
		ERR_PRINTF1(_L("Device type capability Not Found"));
		}
	INFO_PRINTF1(_L("*END*CT_RAccessoryConnectionData::LogDeviceTypeCapabilities"));
	}
