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


#include "t_rpositionserverdata.h"

/**
 * Literals for names or values of scriptable parameters and default values.
 */

/**
 * UID of a position module. Uid for "GPS", "A-GPS" or "NB".
 * May also be KParameterValueDefault.
 */
_LIT( KModuleId, "ModuleId" );
/**
 * Allowed device status. One of _TDeviceStatus values.
 * May also be KParameterValueAny.
 */
_LIT( KDeviceStatus, "DeviceStatus" );

/**
 * Allowed device status. One of _TDataQualityStatus values.
 * May also be KParameterValueAny.
 */
_LIT( KDataQualityStatus, "DataQualityStatus" );
/**
 * A parameter value that means that the parameter is not checked against
 * any reference value.
 */
_LIT( KParameterValueAny, "Any" );

/** A parameter value that means that the system default should be used.*/
_LIT( KParameterValueDefault, "Default" );

/** A parameter value that means that no value should be used.*/
_LIT( KParameterValueNone, "None" );

/*@{*/
//Command literals 
_LIT(KCmdConnect, 							"Connect");
_LIT(KCmdClose,	 							"Close");
_LIT(KCmdGetModuleInfoByIndex,				"GetModuleInfoByIndex");
_LIT(KCmdGetModuleStatus,					"GetModuleStatus");
/*@}*/

/**
 * Two phase constructor
 *
 * @leave	system wide error
 */
CT_RPositionServerData* CT_RPositionServerData::NewL()
	{
	CT_RPositionServerData* ret = new (ELeave) CT_RPositionServerData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

/**
 * Private constructor. First phase construction
 */
CT_RPositionServerData::CT_RPositionServerData()
	:
	iPositionServer()
	{
	}

/**
 * Second phase construction
 * @internalComponent
 * @return	N/A
 * @pre		None
 * @post	None
 * @leave	system wide error
 */
void CT_RPositionServerData::ConstructL()
	{
	}

/**
 * Public destructor
 */
CT_RPositionServerData::~CT_RPositionServerData()
	{
	DestroyData();
	}

/**
 * Return a pointer to the object that the data wraps
 *
 * @return	NULL
 */
TAny* CT_RPositionServerData::GetObject()
	{
	return &iPositionServer;
	}

/**
 * Helper Destructor
 */
void CT_RPositionServerData::DestroyData()
	{	
	}

/**
 * Process a command read from the Ini file
 * @param aCommand 		   -	The command to process
 * @param aSection		   -	The section get from the *.ini file of the project T_Wlan
 * @param aAsyncErrorIndex -	Command index dor async calls to returns errors to
 * @return TBool		   -    ETrue if the command is process
 * @leave				   -	system wide error
 */
TBool CT_RPositionServerData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool ret = ETrue;
	
	if (aCommand == KCmdConnect)
		{
		DoCmdConnect();
		}
	else if (aCommand == KCmdGetModuleInfoByIndex)
		{
		DoCmdGetModuleInfoByIndex();
		}
	else if (aCommand == KCmdGetModuleStatus)
		{
		DoCmdGetModuleStatus(aSection);
		}
	else if (aCommand == KCmdClose)
		{
		DoCmdClose();
		}
	else
		{
		ERR_PRINTF1(_L("Unknown command."));
		ret=EFalse;
		}
	return ret;
	}

/**
 * This method is used to test opening a session with the position server
 * (RPositionServer::Connect()).
 */
void CT_RPositionServerData::DoCmdConnect()
	{
	INFO_PRINTF1(_L("*START*CT_RPositionerData::DoCmdConnectPositionServer"));
	TInt err = iPositionServer.Connect();
	if (err!=KErrNone)
		{
		ERR_PRINTF2(_L("Could not connect to position server. Error: %d"), err);
		SetError(err);
		}
	INFO_PRINTF1(_L("*END*CT_RPositionerData::DoCmdConnectPositionServer"));
	}

/**
 * This method is used to test closing the client handle to position server
 * (RPositionServer::Close()).
 */
void CT_RPositionServerData::DoCmdClose()
	{
	INFO_PRINTF1(_L("*START*CT_RPositionerData::DoCmdClosePositionServer"));
	iPositionServer.Close();
	INFO_PRINTF1(_L("*END*CT_RPositionerData::DoCmdClosePositionServer"));
	}

/**
 * This method is used to test getting the details of the specified module
 * (RPositionServer::GetModuleInfoByIndex()).
 */
void CT_RPositionServerData::DoCmdGetModuleInfoByIndex()
	{
	INFO_PRINTF1(_L("*START*CT_RPositionServerData::DoCmdGetModuleInfoByIndex"));
	TUint numModules;
	TInt err = iPositionServer.GetNumModules(numModules);
	if (err!=KErrNone)
		{
		ERR_PRINTF2(_L("Could not get number of module ID's. Error: %d"), err);
		SetError(err);
		}
	for (TInt i=0;i<numModules;i++)
		{
		TPositionModuleInfo modInfo;
		err = iPositionServer.GetModuleInfoByIndex(i, modInfo);
		if(err!=KErrNone)
			{
			ERR_PRINTF3(_L("Could not get module info at index %d. Error: %d"), i, err);
			SetError(err);
			}
		INFO_PRINTF3(_L("Given position [%d] module id:   %d"), i, modInfo.ModuleId());
		TBuf<50> moduleName;
		modInfo.GetModuleName(moduleName);
		INFO_PRINTF3(_L("Given position [%d] module name: %S"), i, &moduleName);
		}
	INFO_PRINTF1(_L("*END*CT_RPositionServerData::DoCmdGetModuleInfoByIndex"));
	}

void CT_RPositionServerData::DoCmdGetModuleStatus(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_RPositionServerData::DoCmdGetModuleStatus"));
	TBool dataOk=ETrue;

	TPtrC16 moduleIdPtr;
	if (!GetStringFromConfig(aSection, KModuleId, moduleIdPtr))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KModuleId);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(dataOk)
		{
		if (moduleIdPtr.Compare(KParameterValueDefault)==0)
			{
			INFO_PRINTF1(_L("Getting default module ID."));
			TInt err = iPositionServer.GetDefaultModuleId(iPositionModuleId);
			if (err!=KErrNone)
				{
				ERR_PRINTF2(_L("Could not set requestor. Error: %d"), err);
				SetError(err);
				}
			}
		else if (moduleIdPtr.Compare(KParameterValueNone)==0)
			{
			INFO_PRINTF1(_L("No module ID will be used."));
			}
		else
			{
			TInt id;
			if (!GetIntFromConfig(aSection, KModuleId, id))
				{
				ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KModuleId);
				SetBlockResult(EFail);
				dataOk = EFalse;
				}
			if(dataOk)
				{
				INFO_PRINTF2(_L("Using module ID: %d"),id);
				iPositionModuleId.iUid = id;
				}
			}
		}
	
	TPtrC deviceStatusPtr;
	if (!GetStringFromConfig(aSection, KDeviceStatus, deviceStatusPtr))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDeviceStatus);
		SetBlockResult(EFail);
		dataOk=EFalse;
		}
	TPtrC dataQualityStatusPtr;
	if (!GetStringFromConfig(aSection, KDataQualityStatus, dataQualityStatusPtr))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDataQualityStatus);
		SetBlockResult(EFail);
		dataOk=EFalse;
		}

	
	if(dataOk)
		{
		// Verify position module status if needed
		if (deviceStatusPtr.Compare(KParameterValueAny) != 0 ||
			dataQualityStatusPtr.Compare(KParameterValueAny) != 0)
			{
			INFO_PRINTF1(_L("Checking device status"));
			
			TPositionModuleStatus posModuleStatus;
			TInt err = iPositionServer.GetModuleStatus(posModuleStatus, iPositionModuleId);
			if (err!=KErrNone)
				{
				ERR_PRINTF2(_L("Could not get module status. Error: %d"), err);
				SetError(err);
				}

			// Verify device status if needed
			if (deviceStatusPtr.Compare(KParameterValueAny) != 0)
				{
				TInt deviceStatus;
				if (!GetIntFromConfig(aSection, KDeviceStatus, deviceStatus))
					{
					ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDeviceStatus);
					SetBlockResult(EFail);
					dataOk=EFalse;
					}
				if(dataOk)
					{
					if (deviceStatus != posModuleStatus.DeviceStatus())
						{
						ERR_PRINTF1(_L("Actual device status:"));
						switch (posModuleStatus.DeviceStatus())
							{
							case TPositionModuleStatus::EDeviceUnknown:
								INFO_PRINTF1(_L("EDeviceUnknown"));
								break;
							case TPositionModuleStatus::EDeviceError:
								INFO_PRINTF1(_L("EDeviceError"));
								break;
							case TPositionModuleStatus::EDeviceDisabled:
								INFO_PRINTF1(_L("EDeviceDisabled"));
								break;
							case TPositionModuleStatus::EDeviceInactive:
								INFO_PRINTF1(_L("EDeviceInactive"));
								break;
							case TPositionModuleStatus::EDeviceInitialising:
								INFO_PRINTF1(_L("EDeviceInitialising"));
								break;
							case TPositionModuleStatus::EDeviceStandBy:
								INFO_PRINTF1(_L("EDeviceStandBy"));
								break;
							case TPositionModuleStatus::EDeviceReady:
								INFO_PRINTF1(_L("EDeviceReady"));
								break;
							case TPositionModuleStatus::EDeviceActive:
								INFO_PRINTF1(_L("EDeviceActive"));
								break;
							default:
								INFO_PRINTF1(_L("Unknown request"));
							}
						ERR_PRINTF1(_L("Expected device status:"));
						switch (deviceStatus)
							{
							case TPositionModuleStatus::EDeviceUnknown:
								INFO_PRINTF1(_L("EDeviceUnknown"));
								break;
							case TPositionModuleStatus::EDeviceError:
								INFO_PRINTF1(_L("EDeviceError"));
								break;
							case TPositionModuleStatus::EDeviceDisabled:
								INFO_PRINTF1(_L("EDeviceDisabled"));
								break;
							case TPositionModuleStatus::EDeviceInactive:
								INFO_PRINTF1(_L("EDeviceInactive"));
								break;
							case TPositionModuleStatus::EDeviceInitialising:
								INFO_PRINTF1(_L("EDeviceInitialising"));
								break;
							case TPositionModuleStatus::EDeviceStandBy:
								INFO_PRINTF1(_L("EDeviceStandBy"));
								break;
							case TPositionModuleStatus::EDeviceReady:
								INFO_PRINTF1(_L("EDeviceReady"));
								break;
							case TPositionModuleStatus::EDeviceActive:
								INFO_PRINTF1(_L("EDeviceActive"));
								break;
							default:
								INFO_PRINTF2(_L("Unknown request: %d"),deviceStatus);
							}
						ERR_PRINTF1(_L("Actual device status is not equal to the expected value."));
						SetBlockResult(EFail);
						}
					}
				}
			// Verify data quality status if needed:
			if (dataQualityStatusPtr.Compare( KParameterValueAny ) != 0)
				{
				TInt dataQualityStatus;
				if (!GetIntFromConfig(aSection, KDataQualityStatus, dataQualityStatus))
					{
					ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDataQualityStatus);
					SetBlockResult(EFail);
					dataOk=EFalse;
					}
				if(dataOk)
					{
					if (dataQualityStatus != posModuleStatus.DataQualityStatus())
						{
						ERR_PRINTF1(_L("Actual data quality status:"));
						switch (posModuleStatus.DataQualityStatus())
							{
							case TPositionModuleStatus::EDataQualityUnknown:
								INFO_PRINTF1(_L("EDataQualityUnknown"));
								break;
							case TPositionModuleStatus::EDataQualityLoss:
								INFO_PRINTF1(_L("EDataQualityLoss"));
								break;
							case TPositionModuleStatus::EDataQualityPartial:
								INFO_PRINTF1(_L("EDataQualityPartial"));
								break;
							case TPositionModuleStatus::EDataQualityNormal:
								INFO_PRINTF1(_L("EDataQualityNormal"));
								break;
							default:
								INFO_PRINTF1(_L("Unknown request"));
							}
						ERR_PRINTF1(_L("Expected data quality status:"));
						switch (dataQualityStatus)
							{
							case TPositionModuleStatus::EDataQualityUnknown:
								INFO_PRINTF1(_L("EDataQualityUnknown"));
								break;
							case TPositionModuleStatus::EDataQualityLoss:
								INFO_PRINTF1(_L("EDataQualityLoss"));
								break;
							case TPositionModuleStatus::EDataQualityPartial:
								INFO_PRINTF1(_L("EDataQualityPartial"));
								break;
							case TPositionModuleStatus::EDataQualityNormal:
								INFO_PRINTF1(_L("EDataQualityNormal"));
								break;
							default:
								INFO_PRINTF2(_L("Unknown request: %d"), dataQualityStatus);
							}
						ERR_PRINTF1(_L("Actual data quality status is not equal to the expected value."));
						SetBlockResult(EFail);
						}
					}
				}
			}
		else
			{
			INFO_PRINTF1(_L("No need to check device status"));
			}
		}
	INFO_PRINTF1(_L("*END*CT_RPositionServerData::DoCmdGetModuleStatus"));
	}


