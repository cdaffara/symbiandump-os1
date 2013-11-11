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

#include "T_DataSdpElementBuilder.h"

/*@{*/
//Parameters
_LIT(KType,									"type");
_LIT(KSizeDesc,								"sizedesc");
_LIT(KData,									"data");
_LIT(KUint,									"uint");
_LIT(KInt,									"int");
_LIT(KUUID,									"uuid");
_LIT(KBool,									"bool");
_LIT(KString,								"string");
_LIT(KUrl,									"url");
_LIT(KSdpIntBufObj,							"sdpintbufobj");

//Commands
_LIT(KCmdBuildUnknownL,						"BuildUnknownL");
_LIT(KCmdBuildNilL,							"BuildNilL");
_LIT(KCmdBuildUintL,						"BuildUintL");
_LIT(KCmdBuildIntL,							"BuildIntL");
_LIT(KCmdBuildUUIDL,						"BuildUUIDL");
_LIT(KCmdBuildBooleanL,						"BuildBooleanL");
_LIT(KCmdBuildStringL,						"BuildStringL");
_LIT(KCmdBuildDESL,							"BuildDESL");
_LIT(KCmdBuildDEAL,							"BuildDEAL");
_LIT(KCmdStartListL,						"StartListL");
_LIT(KCmdEndListL,							"EndListL");
_LIT(KCmdBuildURLL,							"BuildURLL");
_LIT(KCmdMSEB_ExtensionInterfaceL,			"MSEB_ExtensionInterfaceL");
/*@}*/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_DataSdpElementBuilder::CT_DataSdpElementBuilder(CDataWrapper& aDataWrapper)
:	iDataWrapper(aDataWrapper)
	{
	}

CT_DataSdpElementBuilder::~CT_DataSdpElementBuilder()
	{
	}

/**
 * Process a command read from the ini file
 *
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return					ETrue if the command is processed
 *
 * @leave					System wide error
 */
TBool CT_DataSdpElementBuilder::DoCommandL(MSdpElementBuilder& aElementBuilder, const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret = ETrue;

	if ( aCommand==KCmdBuildUnknownL )
		{
		DoCmdBuildUnknownL(aElementBuilder, aSection);
		}
	else if ( aCommand==KCmdBuildNilL )
		{
		DoCmdBuildNilL(aElementBuilder);
		}
	else if ( aCommand==KCmdBuildUintL )
		{
		DoCmdBuildUintL(aElementBuilder, aSection);
		}
	else if ( aCommand==KCmdBuildIntL )
		{
		DoCmdBuildIntL(aElementBuilder, aSection);
		}
	else if ( aCommand==KCmdBuildUUIDL )
		{
		DoCmdBuildUUIDL(aElementBuilder, aSection);
		}
	else if ( aCommand==KCmdBuildBooleanL )
		{
		DoCmdBuildBooleanL(aElementBuilder, aSection);
		}
	else if ( aCommand==KCmdBuildStringL )
		{
		DoCmdBuildStringL(aElementBuilder, aSection);
		}
	else if ( aCommand==KCmdBuildDESL )
		{
		DoCmdBuildDESL(aElementBuilder);
		}
	else if ( aCommand==KCmdBuildDEAL )
		{
		DoCmdBuildDEAL(aElementBuilder);
		}
	else if ( aCommand==KCmdStartListL )
		{
		DoCmdStartListL(aElementBuilder);
		}
	else if ( aCommand==KCmdEndListL )
		{
		DoCmdEndListL(aElementBuilder);
		}
	else if ( aCommand==KCmdBuildURLL )
		{
		DoCmdBuildURLL(aElementBuilder, aSection);
		}
	else if ( aCommand==KCmdMSEB_ExtensionInterfaceL )
		{
		DoCmdMSEB_ExtensionInterfaceL(aElementBuilder);
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}

/**
Test MSdpElementBuilder::BuildUnknownL()
*/
void CT_DataSdpElementBuilder::DoCmdBuildUnknownL(MSdpElementBuilder& aElementBuilder, const TDesC& aSection)
	{
	TInt 								theValue;
	if( iDataWrapper.GetIntFromConfig(aSection, KType(), theValue) )
		{
		TUint8 type = theValue;

		if( iDataWrapper.GetIntFromConfig(aSection, KSizeDesc(), theValue) )
			{
			TUint8 sizeDesc = theValue;

			TPtrC	theString;
			if( iDataWrapper.GetStringFromConfig(aSection, KData(), theString) )
				{
				HBufC8*	theString8 = HBufC8::NewLC(theString.Length());
				theString8->Des().Copy(theString);
				TPtrC8	stringPtr=theString8->Des();
				TRAPD(err, aElementBuilder.BuildUnknownL(type, sizeDesc, stringPtr));
				if(err != KErrNone)
					{
					iDataWrapper.ERR_PRINTF2(_L("MSdpElementBuilder BuildUnknownL failed with error %d"), err);
					iDataWrapper.SetError(err);
					}
				CleanupStack::PopAndDestroy(theString8);
				}
			else
				{
				iDataWrapper.ERR_PRINTF2(_L("Missing parameter %S"), &KData());
				iDataWrapper.SetBlockResult(EFail);
				}
			}
		else 
			{
			iDataWrapper.ERR_PRINTF2(_L("Missing parameter %S"), &KData());
			iDataWrapper.SetBlockResult(EFail);
			}
		}
	else
		{
		iDataWrapper.ERR_PRINTF2(_L("Missing parameter %S"), &KData());
		iDataWrapper.SetBlockResult(EFail);
		}
	}


/**
Test MSdpElementBuilder::BuildNilL()
*/
void CT_DataSdpElementBuilder::DoCmdBuildNilL(MSdpElementBuilder& aElementBuilder)
	{
	iDataWrapper.INFO_PRINTF1(_L("MSdpElementBuilder BuildNilL Call"));

	TRAPD(err, aElementBuilder.BuildNilL());
	if(err != KErrNone)
		{
		iDataWrapper.ERR_PRINTF2(_L("MSdpElementBuilder BuildNilL failed with error %d"), err);
		iDataWrapper.SetError(err);
		}
	}


/**
Test MSdpElementBuilder::BuildUintL()
*/
void CT_DataSdpElementBuilder::DoCmdBuildUintL(MSdpElementBuilder& aElementBuilder, const TDesC& aSection)
	{
	iDataWrapper.INFO_PRINTF1(_L("MSdpElementBuilder BuildUintL Call"));

	TPtrC 								theString;
	TPtrC								sdpIntBufName;
	TBool								foundParameter=EFalse;	
	if( iDataWrapper.GetStringFromConfig(aSection, KUint(), theString) )
		{
		foundParameter=ETrue;			
		HBufC8* theString8 = HBufC8::NewLC(theString.Length());
		theString8->Des().Copy(theString);
		TPtrC8	stringPtr = theString8->Des();
		TRAPD(err, aElementBuilder.BuildUintL(stringPtr));
		if(err != KErrNone)
			{
			iDataWrapper.ERR_PRINTF2(_L("MSdpElementBuilder BuildUintL failed with error %d"), err);
			iDataWrapper.SetError(err);
			}
		CleanupStack::PopAndDestroy(theString8);
		}
		
	if ( iDataWrapper.GetStringFromConfig(aSection, KSdpIntBufObj(), sdpIntBufName) )
		{
		foundParameter=ETrue;
		TAny* object=iDataWrapper.GetDataObjectL(sdpIntBufName);
		TSdpIntBuf<TUint8>*	sdpIntBuf = static_cast<TSdpIntBuf<TUint8>*>(object);
		TSdpIntBuf<TUint8>	sdpIntObj = *sdpIntBuf;
		const TUint8 *tmp=&sdpIntObj[0];			
		TPtrC8 bufferPtr(tmp, sdpIntObj.Length());
		TRAPD(err, aElementBuilder.BuildUintL(bufferPtr));
	    if ( err!=KErrNone )
			{
			iDataWrapper.ERR_PRINTF2(_L("MSdpElementBuilder BuildUintL failed with error %d"), err);
			iDataWrapper.SetError(err);
			}		
		}
		
	if (!foundParameter)
		{
		iDataWrapper.ERR_PRINTF2(_L("Missing parameter %S"), &KSdpIntBufObj());
		iDataWrapper.SetBlockResult(EFail);
		}
	}


/**
Test MSdpElementBuilder::BuildIntL()
*/
void CT_DataSdpElementBuilder::DoCmdBuildIntL(MSdpElementBuilder& aElementBuilder, const TDesC& aSection)
	{
	iDataWrapper.INFO_PRINTF1(_L("MSdpElementBuilder BuildIntL Call"));

	TPtrC 								theString;
	TPtrC								sdpIntBufName;
	TBool								foundParameter=EFalse;	
	if( iDataWrapper.GetStringFromConfig(aSection, KInt(), theString) )
		{
		foundParameter=ETrue;			
		HBufC8* theString8 = HBufC8::NewLC(theString.Length());
		theString8->Des().Copy(theString);
		TPtrC8	stringPtr = theString8->Des();
		TRAPD(err, aElementBuilder.BuildIntL(stringPtr));
		if(err != KErrNone)
			{
			iDataWrapper.ERR_PRINTF2(_L("MSdpElementBuilder BuildIntL failed with error %d"), err);
			iDataWrapper.SetError(err);
			}
		CleanupStack::PopAndDestroy(theString8);
		}
		
	if ( iDataWrapper.GetStringFromConfig(aSection, KSdpIntBufObj(), sdpIntBufName) )
		{
		foundParameter=ETrue;
		TAny* object=iDataWrapper.GetDataObjectL(sdpIntBufName);
		TSdpIntBuf<TUint8>*	sdpIntBuf = static_cast<TSdpIntBuf<TUint8>*>(object);
		TSdpIntBuf<TUint8>	sdpIntObj = *sdpIntBuf;
		const TUint8 *tmp=&sdpIntObj[0];			
		TPtrC8 bufferPtr(tmp, sdpIntObj.Length());
		TRAPD(err, aElementBuilder.BuildIntL(bufferPtr));
	    if ( err!=KErrNone )
			{
			iDataWrapper.ERR_PRINTF2(_L("MSdpElementBuilder BuildIntL failed with error %d"), err);
			iDataWrapper.SetError(err);
			}		
		}
		
	if (!foundParameter)
		{
		iDataWrapper.ERR_PRINTF2(_L("Missing parameter %S"), &KSdpIntBufObj());
		iDataWrapper.SetBlockResult(EFail);
		}
	}


/**
Test MSdpElementBuilder::BuildUUIDL()
*/
void CT_DataSdpElementBuilder::DoCmdBuildUUIDL(MSdpElementBuilder& aElementBuilder, const TDesC& aSection)
	{
	iDataWrapper.INFO_PRINTF1(_L("MSdpElementBuilder BuildUUIDL Call"));

	TInt								theValue;
	if( iDataWrapper.GetIntFromConfig(aSection, KUUID(), theValue) )
		{
		TUUID	theUUID(theValue);

		TRAPD(err, aElementBuilder.BuildUUIDL(theUUID));
		if(err != KErrNone)
			{
			iDataWrapper.ERR_PRINTF2(_L("MSdpElementBuilder BuildUUIDL failed with error %d"), err);
			iDataWrapper.SetError(err);
			}
		}
	else
		{
		iDataWrapper.ERR_PRINTF2(_L("Missing parameter %S"), &KUUID());
		iDataWrapper.SetBlockResult(EFail);
		}
	}


/**
Test MSdpElementBuilder::BuildBooleanL()
*/
void CT_DataSdpElementBuilder::DoCmdBuildBooleanL(MSdpElementBuilder& aElementBuilder, const TDesC& aSection)
	{
	iDataWrapper.INFO_PRINTF1(_L("MSdpElementBuilder BuildBooleanL Call"));

	TBool 								boolValue = 0;
	if( iDataWrapper.GetBoolFromConfig(aSection, KBool(), boolValue) )
		{
		TRAPD(err, aElementBuilder.BuildBooleanL(boolValue));
		if(err != KErrNone)
			{
			iDataWrapper.ERR_PRINTF2(_L("MSdpElementBuilder BuildBooleanL failed with error %d"), err);
			iDataWrapper.SetError(err);
			}
		}
	else
		{
		iDataWrapper.ERR_PRINTF2(_L("Missing parameter %S"), &KBool());
		iDataWrapper.SetBlockResult(EFail);
		}
	}


/**
Test MSdpElementBuilder::BuildStringL()
*/
void CT_DataSdpElementBuilder::DoCmdBuildStringL(MSdpElementBuilder& aElementBuilder, const TDesC& aSection)
	{
	iDataWrapper.INFO_PRINTF1(_L("MSdpElementBuilder BuildStringL Call"));

	TPtrC 								theString;
	if( iDataWrapper.GetStringFromConfig(aSection, KString(), theString) )
		{
		HBufC8*	theString8=HBufC8::NewLC(theString.Length());
		theString8->Des().Copy(theString);
		TPtrC8 stringPtr = theString8->Des();
		TRAPD(err, aElementBuilder.BuildStringL(stringPtr));
		if(err != KErrNone)
			{
			iDataWrapper.ERR_PRINTF2(_L("MSdpElementBuilder BuildStringL failed with error %d"), err);
			iDataWrapper.SetError(err);
			}
		CleanupStack::PopAndDestroy(theString8);
		}
	else
		{
		iDataWrapper.ERR_PRINTF2(_L("Missing parameter %S"), &KString());
		iDataWrapper.SetBlockResult(EFail);
		}
	}

/**
Test MSdpElementBuilder::BuildDESL()
*/
void CT_DataSdpElementBuilder::DoCmdBuildDESL(MSdpElementBuilder& aElementBuilder)
	{
	iDataWrapper.INFO_PRINTF1(_L("MSdpElementBuilder BuildDESL Call"));

	TRAPD(err, aElementBuilder.BuildDESL());
	if(err != KErrNone)
		{
		iDataWrapper.ERR_PRINTF2(_L("MSdpElementBuilder BuildDESL failed with error %d"), err);
		iDataWrapper.SetError(err);
		}
	}


/**
Test MSdpElementBuilder::BuildDEAL()
*/
void CT_DataSdpElementBuilder::DoCmdBuildDEAL(MSdpElementBuilder& aElementBuilder)
	{
	iDataWrapper.INFO_PRINTF1(_L("MSdpElementBuilder BuildDEAL Call"));

	TRAPD(err, aElementBuilder.BuildDEAL());
	if(err != KErrNone)
		{
		iDataWrapper.ERR_PRINTF2(_L("MSdpElementBuilder BuildDEAL failed with error %d"), err);
		iDataWrapper.SetError(err);
		}
	}


/**
Test MSdpElementBuilder::StartListL()
*/
void CT_DataSdpElementBuilder::DoCmdStartListL(MSdpElementBuilder& aElementBuilder)
	{
	iDataWrapper.INFO_PRINTF1(_L("MSdpElementBuilder StartListL Call"));

	TRAPD(err, aElementBuilder.StartListL());
	if(err != KErrNone)
		{
		iDataWrapper.ERR_PRINTF2(_L("MSdpElementBuilder StartListL failed with error %d"), err);
		iDataWrapper.SetError(err);
		}
	}


/**
Test MSdpElementBuilder::EndListL()
*/
void CT_DataSdpElementBuilder::DoCmdEndListL(MSdpElementBuilder& aElementBuilder)
	{
	iDataWrapper.INFO_PRINTF1(_L("MSdpElementBuilder EndListL Call"));

	TRAPD(err, aElementBuilder.EndListL());
	if(err != KErrNone)
		{
		iDataWrapper.ERR_PRINTF2(_L("MSdpElementBuilder EndListL failed with error %d"), err);
		iDataWrapper.SetError(err);
		}
	}


/**
Test MSdpElementBuilder::BuildURLL()
*/
void CT_DataSdpElementBuilder::DoCmdBuildURLL(MSdpElementBuilder& aElementBuilder, const TDesC& aSection)
	{
	iDataWrapper.INFO_PRINTF1(_L("MSdpElementBuilder BuildURLL Call"));

	TPtrC 								theString;
	if( iDataWrapper.GetStringFromConfig(aSection, KUrl(), theString) )
		{
		HBufC8*	theString8=HBufC8::NewLC(theString.Length());
		theString8->Des().Copy(theString);
		TPtrC8	stringPtr = theString8->Des();
		TRAPD(err, aElementBuilder.BuildURLL(stringPtr));
		if(err != KErrNone)
			{
			iDataWrapper.ERR_PRINTF2(_L("MSdpElementBuilder BuildURLL failed with error %d"), err);
			iDataWrapper.SetError(err);
			}
		CleanupStack::PopAndDestroy(theString8);
		}
	else
		{
		iDataWrapper.ERR_PRINTF2(_L("Missing parameter %S"), &KUrl());
		iDataWrapper.SetBlockResult(EFail);
		}
	}


/**
Test MSdpElementBuilder::MSEB_ExtensionInterfaceL()
*/
void CT_DataSdpElementBuilder::DoCmdMSEB_ExtensionInterfaceL(MSdpElementBuilder& aElementBuilder)
	{
	iDataWrapper.INFO_PRINTF1(_L("MSdpElementBuilder MSEB_ExtensionInterfaceL Call"));

	void* tmpVoid;
	TRAPD(err, aElementBuilder.MSEB_ExtensionInterfaceL(KNullUid, tmpVoid));
	if(err != KErrNone)
		{
		iDataWrapper.ERR_PRINTF2(_L("MSdpElementBuilder MSEB_ExtensionInterfaceL failed with error %d"), err);
		iDataWrapper.SetError(err);
		}
	}
