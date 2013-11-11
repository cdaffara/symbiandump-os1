// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// (U)SAT API implementation - RSat
// 
//

/**
 @file
*/

#include <et_clsvr.h>
#include "etelext.h"

#include "Etelsat.h"
#include "Satptr.h"
#include <satcs.h>

_LIT(KSatToolkit,"TOOLKIT");



EXPORT_C RSat::RSat()
	: RTelSubSessionBase(),
	  iEtelSatPtrHolder(NULL)
/** Default constructor */
	{
	}

EXPORT_C void RSat::ConstructL()
/** Initialises RSat.  called as part of the two-phase construction (typical to Symbian OS). */
	{
	__ASSERT_ALWAYS(iEtelSatPtrHolder==NULL,PanicClient(EEtelPanicHandleNotClosed));
	iEtelSatPtrHolder = CSatPtrHolder::NewL(EMaxNumberSatPtrSlots, EMaxNumberSatCPtrSlots);
	__ASSERT_ALWAYS(iEtelSatPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	}

EXPORT_C void RSat::Destruct()
/** Default destructor */
	{
	delete iEtelSatPtrHolder;
	iEtelSatPtrHolder = NULL;
	}

EXPORT_C TInt RSat::Open(RPhone& aPhone)
/**
 * This method opens the RSat interface.
 *
 * You must have previously opened an interface to the phone (either an RPhone or EMobilePhone).
 * Pass the phone session to RSat::Open().
 *
 * After opening the RSat session:
 *
 * - Send your profile to the UICC (see RSat::GetMeSideSatProfile() )
 * - Request notification to receive proactive commands (see the RSat::Notify.... functions).
 * - Finally, indicate that you are ready to receive proactive commands from the UICC with RSat::UsatClientReadyIndication()
 *
 * @param aPhone A phone session (either an RPhone or EMobilePhone).
 * @see RSat::Close()
 * @capability None
 */
	{
	RSessionBase* session=&aPhone.SessionHandle();
	__ASSERT_ALWAYS(session!=NULL,PanicClient(EEtelPanicNullHandle));
	TInt subSessionHandle=aPhone.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle!=NULL,PanicClient(EEtelPanicNullHandle));

	TRAPD(ret,ConstructL());
	if (ret)
		return ret;
	TPtrC name(KSatToolkit);
	SetSessionHandle(*session);
	ret = CreateSubSession(*session,EEtelOpenFromSubSession,TIpcArgs(&name,TIpcArgs::ENothing,subSessionHandle));
	if (ret)
		Destruct();
	return ret;
	}

EXPORT_C void RSat::Close()
/**
 * This method closes RSat.
 * @see RSat::Open()
 * @capability None
 */
	{
	CloseSubSession(EEtelClose);
	Destruct();
	}


RSat::TSatBase::TSatBase()
	:iExtensionId(KSatVersionNotSet)
/** Default constructor */
	{
	}

EXPORT_C TInt RSat::TSatBase::ExtensionId() const
/**
 * This method returns the version number of the parameter.
 * It will be a value from RSat::TSatVersion.
 * The version number is also called the extension identifier.
 *
 * @return The API version number.
 *
 * @see RSat::TSatVersion
 */
	{
	return iExtensionId;
	}

RSat::TSatPCmdBase::TSatPCmdBase()
	: TSatBase(),
	  iPCmdNumber(0)
/** Default constructor */	  
	{
	}

EXPORT_C TUint8 RSat::TSatPCmdBase::PCmdNumber() const
/**
 * This method returns the value of iPCmdNumber for the associated proactive command class.
 * The PCmd Number is the one originally issued by the UICC card.
 *
 * @return The protected data member iPCmdNumber which specifies the proactive command number.
 */
	{
	return iPCmdNumber;
	}

EXPORT_C TInt RSat::TSatPCmdBase::SetPCmdNumber(TUint8 aNumber)
/**
 * This method sets iPCmdNumber for the associated proactive command class.
 * The PCmd Number is the one originally issued by the UICC card.
 *
 * @param aNumber PCmdNumber associated with the proactive command sent by the UICC card.
 * @return KErrNotSupported returned if aNumber is not within the valid range (1->254 inclusive).
 *         Otherwise, KErrNone returned.
 */
	{
	if ((aNumber<1) || aNumber>254)
		return KErrNotSupported;
	else
		{
		iPCmdNumber=aNumber;
		return KErrNone;
		}
	}

EXPORT_C void RSat::GetIcon(TRequestStatus& aReqStatus, TUint8 aRecordNumber, RSat::TIcon& aIconEf) const
/**
 * A proactive command may be accompanied by an icon to be displayed to the user;
 * see RSat::TIconId.  The command does not contain the icon.  Instead, it
 * contains an icon identifier, RSat::TIconId::iIdentifier. When you want to display an
 * icon, the first step is to pass RSat::TIconId::iIdentifier to
 * RSat::GetIcon() to retrieve the icon's EF record from the UICC.
 *
 * The EF record describes the icon.  The UICC may store several images that
 * represent the same icon.  Images can differ in size and coding scheme. Each is
 * listed and described in the EF record.  Analyse the EF record and choose a
 * suitable image, then call RSat::GetImageInstance() to retrieves
 * the actual image data to display.
 *
 * If none of the images are suitable then see the error conditions described in
 * RSat::TIconId.
 *
 * The EF record is described in ETSI TS 131 102; search for EFimg or "Contents
 * of files at the DFGRAPHICS level"
 *
 * To cancel an outstanding request, call
 * RTelSubSessionBase::CancelAsyncRequest( ESatGetIcon )
 *
 * @param aReqStatus This method is asynchronous.  aReqStatus indicates when the operation has completed.
 * @param aRecordNumber EF record number of the icon to get. This must be a value from RSat::TIconId::iIdentifier.
 * @param aIconEf The TSY fills this with the icon EF record retrieved from the UICC, specified by aRecordNumber.
 * @capability None
 * @see RSat::TIconId
 * @see RSat::GetImageInstance()
 */
 	{
	__ASSERT_ALWAYS(iEtelSatPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iEtelSatPtrHolder->iGetIcon = aRecordNumber;
	TPtr8& ptr1=iEtelSatPtrHolder->Set(ESlot1GetIcon,iEtelSatPtrHolder->iGetIcon);
	TPtr8& ptr2=iEtelSatPtrHolder->Set(ESlot2GetIcon,aIconEf);

	Get(ESatGetIcon,aReqStatus, ptr1, ptr2);
	}


EXPORT_C void RSat::GetImageInstance(TRequestStatus& aReqStatus, const TDesC8& aInstanceInfo, TDes8& aInstance) const
/**
 * This method retrieves an icon's image data from the UICC.  The data is
 * returned in aInstance.
 *
 * For colour images, you will also need to retrieve a Colour LookUp Table (CLUT).
 * Use RSat::GetClut() for this.
 *
 * Before calling this method, you must have called RSat::GetIcon() to retrieve
 * the icon's EF record.
 *
 * aInstanceInfo is a RSat::TInstanceInfoV3Pckg (a packaged
 * RSat::TInstanceInfoV3) containing details of the image you would
 * like to get.  This includes the image's size.  aInstance should be the size of the image.
 *
 * The EF record is described in ETSI TS 131 102; search for EFimg or "Contents
 * of files at the DFGRAPHICS level".  The format of both image data and CLUT
 * is also described in ETSI TS 131 102; see Annex B.
 *
 * To cancel an outstanding request, call
 * RTelSubSessionBase::CancelAsyncRequest( ESatGetImageInstance )
 *
 * @param aReqStatus This method is asynchronous.  aReqStatus indicates when the operation has completed.
 * @param aInstanceInfo Specifies the instance number,
 *        offset and length of the image to retrieve from EF.
 * @param aInstance The TSY fills this with the image instance EF specified by aInstanceInfo, retrieved from the UICC.
 * @capability None
 * @see RSat::TIconId
 * @see RSat::GetIcon()
 * @see RSat::TInstanceInfoV3
 * @see RSat::TInstanceInfoV3Pckg
 */
	{
	SetAndGet(ESatGetImageInstance,aReqStatus, aInstanceInfo, aInstance);
	}


EXPORT_C RSat::TInstanceInfoV2::TInstanceInfoV2()
	: TSatPCmdBase(),
	  iInstanceNumber(0),
	  iOffset(0),
	  iLength(0)
/**
* Default constructor.
*/
	{
	iExtensionId = RSat::KSatV2;
	}

EXPORT_C RSat::TInstanceInfoV3::TInstanceInfoV3()
	: TInstanceInfoV2(),
	  iCoding(KBasic),
	  iImgWidth(0),
	  iImgHeight(0),
	  iFileId(0)
/**
* Default constructor.
*/	  
	{
	iExtensionId = RSat::KSatV3;
	}

EXPORT_C void RSat::GetClut(TRequestStatus& aReqStatus, TUint aInstanceNumber, TUint aOffset, TDes8& aClut) const
/**
 * This method retrieves the CLUT (Colour Lookup Table) for an image instance designated by aInstanceNumber.
 * This is used when displaying icons; see RSat::GetImageInstance().
 *
 * The format of the CLUT is described in ETSI TS 131 102; see Annex B.
 *
 * To cancel an outstanding request, call
 * RTelSubSessionBase::CancelAsyncRequest( ESatGetClut )
 *
 * @param aReqStatus This method is asynchronous.  aReqStatus indicates when the operation has completed.
 * @param aInstanceNumber Specifies which instance of an icon is to be retrieved.
 * @param aOffset Specifies the offset at which is located the clut within the image instance.
 * @param aClut The TSY fills this with the colour look up table specified by aInstance Number, retrieved from the UICC.
 * @capability None
 */
	{
	__ASSERT_ALWAYS(iEtelSatPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iEtelSatPtrHolder->iGetClut.iInstanceNumber= aInstanceNumber;
	iEtelSatPtrHolder->iGetClut.iOffset= aOffset;
	TPtr8& ptr1=iEtelSatPtrHolder->Set(ESlot1GetClut,iEtelSatPtrHolder->iGetClut);

	Get(ESatGetClut,aReqStatus, ptr1, aClut);
	}


EXPORT_C void RSat::TerminalRsp(TRequestStatus& aReqStatus, TPCmd aPCmd, const TDesC8& aRsp) const
/**
 * This method replies to a proactive command.
 * The reply is sent to the UICC.
 * This is a generic method which replies to every proactive command.
 *
 * Each type of proactive command requires a different response. There are classes
 * to implement the responses for the different commands.  To respond to a command, create an instance
 * of the appropriate class, fill in the fields, and then package the object.  Then pass
 * the package and the associated proactive command type to RSat::TerminalRsp().
 *
 * @param aReqStatus This method is asynchronous.  aReqStatus indicates when the operation has completed.
 * @param aPCmd  The type of proactive command that you are responding to.
 * @param aRsp Packaged response object associated with the proactive command type.
 * @capability NetworkControl
 */
	{
	__ASSERT_ALWAYS(iEtelSatPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iEtelSatPtrHolder->iPCmd=aPCmd;
	TPtr8& ptr1=iEtelSatPtrHolder->Set(ESlotPCmd,iEtelSatPtrHolder->iPCmd);

	Set(ESatTerminalRsp,aReqStatus,ptr1,aRsp);
	}


EXPORT_C RSat::TSatProfileV1::TSatProfileV1()
	: TSatBase(),
	  iSatProfileByte1(0),
	  iSatProfileByte2(0),
	  iSatProfileByte3(0),
	  iSatProfileByte4(0),
	  iSatProfileByte5(0),
	  iSatProfileByte6(0),
	  iSatProfileByte7(0),
	  iSatProfileByte8(0),
	  iSatProfileByte9(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	}

EXPORT_C RSat::TSatProfileV2::TSatProfileV2()
	: TSatProfileV1(), iSatProfileByte10(0), iSatProfileByte11(0), iSatProfileByte12(0),
	iSatProfileByte13(0), iNumOfChannels(0), iSatProfileByte14(0), iScreenHeight(0),
	iSatProfileByte15(0), iScreenWidth (0), iSatProfileByte16(0), iWidthReduction(0),
	iSatProfileByte17(0), iSatProfileByte18(0), iTiaEiaProtoVersion(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	}

EXPORT_C RSat::TSatProfileV5::TSatProfileV5()
	: TSatProfileV2(), iSatProfileByte28(0), iSatProfileByte29(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV5;
	}

EXPORT_C RSat::TSatProfileV6::TSatProfileV6()
	: TSatProfileV5(), iSatProfileByte21(0), iSatProfileByte22(0), iSatProfileByte23(0), 
	iSatProfileByte24(0), iSatProfileByte25(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV6;
	}

EXPORT_C TInt RSat::UsatClientReadyIndication() const
/**
 * Indicates that you are ready to receive proactive commands from the UICC.  Do not call this until:
 *
 * - You have sent your profile to the UICC (see RSat::GetMeSideSatProfile() )
 *
 * - You have requested notification to receive proactive commands (see the
 * RSat::Notify.... functions).
 *
 * @return KErrNone
 * @capability NetworkControl
 */
	{
	return Blank(ESatUsatClientReadyIndication);
	}

EXPORT_C void RSat::GetMeSideSatProfile(TRequestStatus& aReqStatus, TDes8& aMeSimSatProfile) const
/**
 * At the beginning of a ETelSat API session, you must tell the UICC what SAT
 * features the phone supports.  Do this after creating and opening an RSat
 * session. This is a three-part process:
 *
 * - Firstly, call RSat::GetMeSideSatProfile(). This returns a profile describing
 * the SAT features that are supported by the phone's hardware and the TSY. These
 * features are not under the ETelSat API control; they happen without your
 * involvement.
 *
 * - Modify the profile to include the SAT features that you support. The profile is
 * a RSat::TSatProfileV1 in Symbian OS v6.1 and v7.0.  Up to v8.1, it is a
 * RSat::TSatProfileV2.  For Symbian OS v9.2, it is a RSat::TSatProfileV5.
 * For Symbian OS v9.4, it is a RSat::TSatProfileV6. In later versions of Symbian OS, check for updated
 * class (RSat::TSatProfileV7 etc).
 *
 * - Finally, send the profile to the UICC by calling RSat::ClientSatProfileIndication()
 *
 * Once you have told the UICC of the profile:
 *
 * - Firstly, request notification of proactive commands with the RSat::Notify... functions.
 *
 * - Finally call RSat::UsatClientReadyIndication() to tell the UICC you are ready for its proactive commands.
 *
 * @see RSat::UsatClientReadyIndication()
 * @see RSat::ClientSatProfileIndication()
 * @see RSat::TSatProfileV1
 * @see RSat::TSatProfileV1Pckg
 * @see RSat::TSatProfileV2
 * @see RSat::TSatProfileV2Pckg
 * @see RSat::TSatProfileV5
 * @see RSat::TSatProfileV5Pckg
 * @see RSat::TSatProfileV6
 * @see RSat::TSatProfileV6Pckg
 *
 * @param aReqStatus This method is asynchronous.  aReqStatus indicates when the operation has completed.
 * @param aMeSimSatProfile The TSY fills this with the phone hardware and TSY's SAT profile, packaged in a RSat::TSatProfileV1Pckg or RSat::TSatProfileV2Pckg
 * @capability None
 */
	{
	Get(ESatGetMeSideSatProfile,aReqStatus,aMeSimSatProfile);
	}


EXPORT_C TInt RSat::ClientSatProfileIndication(const TDesC8& aClientSatProfile) const
/**
 * At the beginning of a ETelSat API session, you must tell the UICC what SAT
 * features the phone supports.  Do this after creating and opening an RSat
 * session. This is a three-part process:
 *
 * - Firstly, call RSat::GetMeSideSatProfile(). This returns a profile describing
 * the SAT features that are supported by the phone's hardware and the TSY. These
 * features are not under the ETelSat API control; they happen without your
 * involvement.
 *
 * - Modify the profile to include the SAT features that you support. The profile is
 * a RSat::TSatProfileV1 in Symbian OS v6.1 and v7.0.  Up to v8.1, it is a
 * RSat::TSatProfileV2.  For Symbian OS v9.2, it is a RSat::TSatProfileV5.
 * For Symbian OS v9.4, it is a RSat::TSatProfileV6. In later versions of Symbian OS, check for updated
 * class (RSat::TSatProfileV7 etc).
 *
 * - Finally, send the profile to the UICC by calling RSat::ClientSatProfileIndication()
 *
 * Once you have told the UICC of the profile:
 *
 * - Firstly, request notification of proactive commands with the RSat::Notify... functions.
 *
 * - Finally call RSat::UsatClientReadyIndication() to tell the UICC you are ready for its proactive commands.
 *
 * @see RSat::UsatClientReadyIndication()
 * @see RSat::GetMeSideSatProfile()
 * @see RSat::TSatProfileV1
 * @see RSat::TSatProfileV1Pckg
 * @see RSat::TSatProfileV2
 * @see RSat::TSatProfileV2Pckg
 * @see RSat::TSatProfileV5
 * @see RSat::TSatProfileV5Pckg
 * @see RSat::TSatProfileV6
 * @see RSat::TSatProfileV6Pckg
 *
 * @param aClientSatProfile  Client SAT profile. A packaged RSat::TSatProfileVx class. See above for which version to use.
 * @capability NetworkControl
 * @capability WriteDeviceData
 * @return KErrNone on success, other system-wide error code otherwise.
 */
	{
	return Set(ESatClientSatProfileIndication,aClientSatProfile);
	}

EXPORT_C RSat::TMenuSelectionV1::TMenuSelectionV1()
	: TSatBase(),
	  iItemId(0x00),
	  iHelp(EHelpRequestNotSet)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	iHelp=EHelpRequestNotSet;
	}

EXPORT_C void RSat::MenuSelection(TRequestStatus& aReqStatus, const TDesC8& aSelection) const
/**
 * Sends a MENU SELECTION envelope command to the UICC.  This is related to a SET
 * UP MENU proactive command.  SET UP MENU asks for a menu to be added to the
 * phone's menu system.  If a user chooses an item from this menu, then inform the
 * UICC of the user's selection with RSat::MenuSelection().
 *
 * @param aReqStatus This method is asynchronous.  aReqStatus indicates when the operation has completed.
 * @param aSelection Describes the menu item that the user selected.
 * Pass a packaged version of the  latest "TMenuSelection" class. For Symbian OS
 * versions up to and including v8.1a and 8.1b use RSat::TMenuSelectionV1Pckg, a
 * packaged RSat::TMenuSelectionV1.  For later versions of Symbian OS, check for an
 * updated API (RSat::TMenuSelectionV2 etc).
 *
 * @capability NetworkControl
 * @capability ReadDeviceData
 *
 * @see RSat::TMenuSelectionV1
 * @see RSat::NotifySetUpMenuPCmd()
 */
	{
	Set(ESatMenuSelection,aReqStatus,aSelection);
	}

EXPORT_C RSat::TCallControlV1::TCallControlV1()
	: TSatBase(),
	  iType(ECcTypeNotSet),
	  iCcp1(),
	  iSubAddress(),
	  iCcp2(),
	  iDcs(0),
	  iTypeOfNumber(ETypeOfNumberNotSet),
	  iNumberPlan(ENumberingPlanNotSet),
	  iBuf(),
	  iResult(EControlResultNotSet),
	  iValidity(EAlphaIdValidityNotSet),
	  iBCRepeatIndicator(EBCAlternateMode)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	iAlphaId.iStatus=EAlphaIdNotSet;
	}

EXPORT_C void RSat::TCallControlV1::SetCallSetUpDetails(const TCallSetUpParams& aParams)
/**
 * This method is used by the TSY to set the call set up details within the TCallControlV1 class
 *
 * This method sets iType to ECcAddress, to specify that the call control has transformed
 * a outstanding SET UP CALL, SEND SS or SEND USSD request into a new call set up request.
 *
 * @param aParams Call set up parameters.
 */
	{
	iType=ECcAddress;
	iCcp1=aParams.iCcp1;
	iSubAddress=aParams.iSubAddress;
	iCcp2=aParams.iCcp2;
	iTypeOfNumber=aParams.iAddress.iTypeOfNumber;
	iNumberPlan=aParams.iAddress.iNumberPlan;
	iBuf=aParams.iAddress.iTelNumber;
	}

EXPORT_C void RSat::TCallControlV1::SetSendSsDetails(const TSsString& aString)
/**
 * This method is used by the TSY to set the SEND SS details within the TCallControlV1 class
 *
 * This method sets iType to ECcString, to specify that the call control has transformed
 * a outstanding SET UP CALL, SEND SS or SEND USSD request into a new SEND SS request.
 *
 * @param aString SS string details.
 */
	{
	iType=ECcSsString;
	iTypeOfNumber=aString.iTypeOfNumber;
	iNumberPlan=aString.iNumberPlan;
	iBuf.Copy(aString.iSsString); //convert 8 bit into 16 bit data
	}

EXPORT_C void RSat::TCallControlV1::SetSendUssdDetails(const TUssdString& aString)
/**
 * This method is used by the TSY to set the SEND USSD details within the TCallControlV1 class
 *
 * This method sets iType to ECcUssdString, to specify that the call control has transformed
 * a outstanding SET UP CALL, SEND SS or SEND USSD request into a new SEND USSD request.
 *
 * @param aString USSD string details.
 */
	{
	iType=ECcUssdString;
	iDcs=aString.iDcs;
	iBuf.Copy(aString.iUssdString); // convert 8 bit into 16 bit data
	}

EXPORT_C void RSat::TCallControlV1::SetCcGeneralResult(TControlResult aResult)
/**
 * This method is used by the TSY to set the general call control result, iResult.
 *
 * @param aResult General call control result.
 */
	{
	iResult=aResult;
	}

EXPORT_C void RSat::TCallControlV1::SetAlphaId(TAlphaIdValidity aValidity, TAlphaId& aAlphaId)
/**
 * This method is used by the TSY to set the alpha identifier.
 *
 * @param aValidity Alpha Identifier validity.
 * @param aAlphaId Alpha Identifier.
 */
	{
	iValidity=aValidity;
	iAlphaId=aAlphaId;
	}

EXPORT_C void RSat::TCallControlV1::GetCallControlType(TCallControlType& aType) const
/**
 * This method is used by the Client to get the call control type.
 *
 * @param aType Call control type  (SET UP CALL, SEND SS or USSD).
 */
	{
	aType=iType;
	}

EXPORT_C void RSat::TCallControlV1::GetCcGeneralResult(TControlResult& aResult)const
/**
 * This method is used by the Client to get the call control general result.
 *
 * @param aResult Call control general result.
 */
	{
	aResult=iResult;
	}

EXPORT_C TInt RSat::TCallControlV1::GetCallSetUpDetails(TCallSetUpParams& aParams) const
/**
 * This method is used by the Client to get the call set up details.
 * If the call control type is not ECcAddress, the client should not use this method,
 * which will return KErrNotSupported
 *
 * @param aParams Call set up details.
 * @return KErrNotSupported returned if the type of call control is not a SET UP CALL.
 *         Otherwise, KErrNone returned.
 */
	{
	if (iType!=ECcAddress)
		return KErrNotSupported;
	else
		{
		aParams.iCcp1=iCcp1;
		aParams.iSubAddress=iSubAddress;
		aParams.iCcp2=iCcp2;
		aParams.iAddress.iTypeOfNumber=iTypeOfNumber;
		aParams.iAddress.iNumberPlan=iNumberPlan;
		aParams.iAddress.iTelNumber=iBuf;
		return KErrNone;
		}
	}

EXPORT_C TInt RSat::TCallControlV1::GetSendSsDetails(TSsString& aString) const
/**
 * This method is used by the Client to get the SEND SS details.
 * If the call control type is not ECcSsString, the client should not use this method
 * which will returns KErrNotSupported.
 *
 * @param aString SS String details.
 * @return KErrNotSupported returned if the type of call control is not a SS String.
 *         Otherwise, KErrNone returned.
 */
	{
	if (iType!=ECcSsString)
		return KErrNotSupported;
	else
		{
		aString.iTypeOfNumber=iTypeOfNumber;
		aString.iNumberPlan=iNumberPlan;
		aString.iSsString.Copy(iBuf); // convert 16bit into 8bit data
		return KErrNone;
		}
	}

EXPORT_C TInt RSat::TCallControlV1::GetSendUssdDetails(TUssdString& aString) const
/**
 * This method is used by the Client to get the SEND USSD details.
 * If the call control type is not ECcUssdString, the client should not use this method
 * which will returns KErrNotSupported.
 *
 * @param aString USSD string details.
 * @return KErrNotSupported returned if the type of call control is not a SEND USSD.
 *         Otherwise, KErrNone returned.
 */
	{
	if (iType!=ECcUssdString)
		return KErrNotSupported;
	else
		{
		aString.iDcs=iDcs;
		aString.iUssdString.Copy(iBuf); // convert 16 bit into 8 bit data
		return KErrNone;
		}
	}

EXPORT_C void RSat::TCallControlV1::GetAlphaId(TAlphaIdValidity& aValidity, TAlphaId& aAlphaId) const
/**
 * This method returns the Alpha Identifier provided by the UICC for user display.
 *
 * @param aValidity Alpha identifier validity.
 * @param aAlphaId Alpha identifier.
 */
	{
	aValidity=iValidity;
	aAlphaId=iAlphaId;
	}

EXPORT_C void RSat::TCallControlV1::SetBCRepeatIndicator(const TBCRepeatIndicator& aBCRepeatIndicator)
/**
 * This method returns the BC Repeat Indicator
 *
 * @param aBCRepeatIndicator Bearer Capability Repeat Indicator
 *
 */
	{
	iBCRepeatIndicator=aBCRepeatIndicator;
	}

EXPORT_C void RSat::TCallControlV1::GetBCRepeatIndicator(TBCRepeatIndicator& aBCRepeatIndicator) const
/**
 * This method returns the Bearer Capability Repeat Indicator
 *
 * @param aBCRepeatIndicator Bearer Capability Repeat Indicator.
 */
	{
	aBCRepeatIndicator = iBCRepeatIndicator;
	}


EXPORT_C RSat::TCallControlV2::TCallControlV2()
	: TCallControlV1(),
	  iCallName(),
	  iNewActionOriginator(EActionOriginatorNotSet)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	}

EXPORT_C void RSat::TCallControlV2::SetCallName(const TName& aName)
/**
 * This method is used by the TSY to set the call name details within the TCallControlV2 class
 *
 * @param aName Call name
 */
	{
	iCallName = aName;
	}

EXPORT_C void RSat::TCallControlV2::SetActionOriginator(const TActionOriginator aOriginator)
/**
 * This method is used by the TSY to set the Call Originator details within the TCallControlV2 class
 *
 * @param aOriginator Call originator
 */
	{
	iNewActionOriginator = aOriginator;
	}

EXPORT_C void RSat::TCallControlV2::GetCallName(TName& aName) const
/**
 * This method returns the call name.
 *
 * @param aName Call Name
 */
	{
	aName = iCallName;
	}

EXPORT_C void RSat::TCallControlV2::GetActionOriginator(TActionOriginator& aOriginator) const
/**
 * This method returns the action originator.
 *
 * @param aOriginator Call originator
 */
	{
	aOriginator = iNewActionOriginator;
	}

EXPORT_C void RSat::NotifyCallControlRequest(TRequestStatus& aReqStatus, TDes8& aCallControlDetails ) const
/**
 * In SAT-enabled phones, the UICC can request control of outgoing calls or activation of PDP Contexts.
 * The phone's TSY passes the call's parameters to the UICC, such as the telephone number, or in the case of PDP activation
 * PDP parameters.  The UICC will allow the call (activation of the context), disallow the call (activation of the context), or it will modify the call's (pdp context activation) parameters.
 *   This happens without the RSat client's involvement.  However, RSat clients can be
 * informed when call control has taken place the by calling
 * RSat::NotifyCallControlRequest().
 *
 * This notification completes when a client requests a call set up or PDP contedxt activation which is
 * modified by the UICC.
 *
 * You may receive this notification following your attempt to send an SMS in
 * response to a SET UP CALL proactive command (call) or OPEN CHANNEL proactive command (PDP context).
 * You may also receive it when another application attempts to set up a call or activate PDP context.
 *
 * Upon completion, for a call, aCallControlDetails contains the details of the new action to
 * be performed, instead of the original call set up; for a PDP context activation, aPdpParameters contain the parameters
 * that shall be utilized instead or original PDP activation request.
 *
 * @param aReqStatus This method is asynchronous.  aReqStatus indicates when the operation has completed.
 * @param aCallControlDetails TCallControlV1 packaged into a descriptor, TCallControlV1Pckg.
 *        starting with version 9.2, in the case of PDP context activation TCallControlV3 packaged into a descriptor, TCallControlV3Pckg.
 * @capability NetworkControl
 * @capability ReadDeviceData
 */
	{
	Get(ESatNotifyCallControlRequest,aReqStatus,aCallControlDetails);
	}


EXPORT_C RSat::TMoSmControlV1::TMoSmControlV1()
	: TSatBase(),
	  iResult(EControlResultNotSet)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	iAlphaId.iStatus=EAlphaIdNotSet;
	iRpAddress.iTypeOfNumber=ETypeOfNumberNotSet;
	iRpAddress.iNumberPlan=ENumberingPlanNotSet;
	iTpAddress.iTypeOfNumber=ETypeOfNumberNotSet;
	iTpAddress.iNumberPlan=ENumberingPlanNotSet;

	}

EXPORT_C void RSat::NotifyMoSmControlRequest(TRequestStatus& aReqStatus,TDes8& aMoSmDetails) const
/**
 * In SAT-enabled phones on GSM/WCDMA networks, the UICC can control who SMS
 * messages are sent to.  The phone's TSY passes the UICC the SMS's parameters,
 * such as the telephone number of the recipient.  The UICC will allow the
 * message, disallow the message, or it modify the delivery parameters.    This
 * happens without the RSat client's involvement.  However, RSat clients can be
 * informed when SMS control has taken place the by calling
 * RSat::NotifyMoSmControlRequest().
 *
 * This notification completes when a client tries to send an SMS which is then
 * modified by SMS control.
 *
 * You may receive this notification following your attempt to send an SMS in
 * response to a SEND SHORT MESSAGE proactive command.  You may also receive it
 * when another application attempts to send an SMS.
 *
 * Upon completion, aMoSmDetails contains the details of the new SMS operation to
 * be performed instead of the original request.
 *
 * @param aReqStatus This method is asynchronous.  aReqStatus indicates when the operation has completed.
 * @param aMoSmDetails TMoSmControlV1 packaged into a descriptor, TMoSmControlV1Pckg.
 * @capability NetworkControl
 * @capability ReadDeviceData
 */
	{
	Get(ESatNotifyMoSmControlRequest,aReqStatus,aMoSmDetails);
	}

EXPORT_C RSat::TTimerExpirationV3::TTimerExpirationV3()
	: TSatBase()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV3;
	}

EXPORT_C void RSat::NotifyTimerExpiration(TRequestStatus& aReqStatus,TDes8& aTimerExpiration) const
/**
 * The phone provides timers for the UICC.  Using the TIMER MANAGEMENT proactive
 * command, the UICC can start, stop and read the current value of a timer.
 * Receive these commands with RSat::NotifyTimerMgmtPCmd(), then start, stop or
 * read a timer as requested.  Timers are started with a particular value.   When
 * this counts down to zero, send a TIMER EXPIRATION envelope command to the UICC
 * with RSat::NotifyTimerExpiration().
 *
 * Each TIMER MANAGEMENT proactive command includes an ID that identifiers the
 * timer.  You must manage up to 8 timer for the UICC.  When a timer expires, pass
 * the ID of the timer to the UICC in aTimerExpiration.
 *
 * Support for this command is only available from v7.0s of Symbian OS onwards.
 *
 * @see RSat::NotifyTimerMgmtPCmd()
 * @see RSat::TTimerExpirationV3
 *
 * @param aReqStatus This method is asynchronous.  aReqStatus indicates when the operation has completed.
 * @param aTimerExpiration Information about the timer that has expired.  A RSat::TTimerExpirationV3Pckg, a packaged RSat::TTimerExpirationV3.
 * @capability None
 */
 	{
	Get(ESatNotifyTimerExpiration, aReqStatus,aTimerExpiration);
	}

EXPORT_C RSat::TSmsPpDownloadV3::TSmsPpDownloadV3()
	: TSatBase(),
		iPdu()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV3;
	}

EXPORT_C void RSat::NotifySmsPpDownload(TRequestStatus& aReqStatus,TDes8& aSmsPpDownload) const
/**
 * This notification completes when a client (not necessarily the SAT client)
 * requests a SMS-PP data download. Upon completion the envelope command details will
 * be returned in a RSar::TSmsPpDownloadV3Pckg, a packaged version of the RSat::TSmsPpDownloadV3 class in the aSmsPpDownload.
 *
 * @param aReqStatus TRequestStatus result code after the asynchronous call completes
 * @param aSmsPpDownload RSat::TSmsPpDownloadV3 packaged in a RSat::TSmsPpDownloadV3Pckg
 * @capability NetworkControl
 * @capability ReadUserData
 */
 	{
	Get(ESatNotifySmsPpDownload, aReqStatus,aSmsPpDownload);
	}

EXPORT_C RSat::TCbDownloadV3::TCbDownloadV3()
	: TSatBase(),
		iPdu()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV3;
	}

EXPORT_C void RSat::NotifyCbDownload(TRequestStatus& aReqStatus,TDes8& aCbDownload) const
/**
 * This notification completes when a client (not necessarily the SAT client)
 * requests a Cell Broadcast data download. Upon completion the envelope command
 * details will be returned in a RSat::TCbDownloadV3Pckg, a packaged version of the RSat::TCbDownloadV3 class in the aCbDownload.
 *
 * @param aReqStatus TRequestStatus result code after the asynchronous call completes
 * @param aCbDownload An RSat:: TCbDownloadV3 packaged in a RSat::TCbDownloadV3Pckg
 * @capability NetworkControl
 * @capability ReadUserData
 */
 	{
	Get(ESatNotifyCbDownload, aReqStatus,aCbDownload);
	}

EXPORT_C RSat::TEventDownloadBaseV2::TEventDownloadBaseV2()
	: TSatBase(),
	iDataAvailable(EFalse)
/**
* Default constructor.
*/
	{
	iExtensionId   = KSatV2;
	}

EXPORT_C RSat::TCardReaderStatusEventV2::TCardReaderStatusEventV2()
	: TEventDownloadBaseV2(),
	  iCardReaderStatus(0)
/**
* Default constructor.
*/
	{
	iDataAvailable = ETrue;
	}

EXPORT_C RSat::TLanguageSelectionEventV2::TLanguageSelectionEventV2()
	: TEventDownloadBaseV2(),
	  iLanguage(0)
/**
* Default constructor.
*/
	{
	iDataAvailable = ETrue;
	}

EXPORT_C RSat::TChannelStatusEventV2::TChannelStatusEventV2()
	: TEventDownloadBaseV2(),
	iStatus()
/**
* Default constructor.
*/
	{
	iDataAvailable = ETrue;
	}

EXPORT_C RSat::TBrowserTerminationEventV2::TBrowserTerminationEventV2()
	: TEventDownloadBaseV2(),
	  iCause(EUnknownCause)
/**
* Default constructor.
*/
	{
	iDataAvailable = ETrue;
	}

EXPORT_C RSat::TDataAvailableEventV2::TDataAvailableEventV2()
	: TEventDownloadBaseV2(),
	  iStatus(),
	  iLength(0)
/**
* Default constructor.
*/
	{
	iDataAvailable = ETrue;
	}

EXPORT_C RSat::TLocalConnectionEventV2::TLocalConnectionEventV2()
	: TEventDownloadBaseV2()
/**
* Default constructor.
*/
	{
	iDataAvailable = ETrue;
	iRecord.iBearerId=ELocalBearerIdNotSet;
	iRecord.iServiceId=0;
	iAddress.iCoding=ECodingNotSet;
	}

EXPORT_C RSat::TDisplayParamsEventV2::TDisplayParamsEventV2()
	: TEventDownloadBaseV2(),
	  iSatProfileByte14(0),
	  iScreenHeight(0),
	  iSatProfileByte15(0),
	  iScreenWidth(0),
	  iSatProfileByte16(0),
	  iWidthReduction(0)
/**
* Default constructor.
*/
	{
	iDataAvailable    = ETrue;
	}

EXPORT_C RSat::TNetworkSearchModeChangeEventV6::TNetworkSearchModeChangeEventV6()
	: TEventDownloadBaseV2(), iNetworkSearchMode(EUnknown)
/**
* Default constructor.
*/
	{
	iDataAvailable = ETrue;
	}

EXPORT_C RSat::TBrowsingStatusEventV6::TBrowsingStatusEventV6()
	: TEventDownloadBaseV2(),iBrowserStatus(0)
/**
* Default constructor.
*/
	{
	iDataAvailable = ETrue;
	}

EXPORT_C RSat::TFramesInformationChangedEventV6::TFramesInformationChangedEventV6()
	: TEventDownloadBaseV2()
/**
* Default constructor.
*/
	{
	iDataAvailable = ETrue;
	iFramesInformation.iFrameId=0;
	}
			
EXPORT_C void RSat::EventDownload(TRequestStatus& aReqStatus, TEventList aSingleEvent, const TDesC8& aEventInfo) const
/**
 * The SET UP EVENT LIST proactive command gives the phone a list of events that may occur
 * to the phone.  The event list may include a change of access technology, user
 * action, browser termination and more.   You must inform the UICC when an event
 * on the list occurs by calling RSat::EventDownload().  If the event is not on the
 * list then you do not need to notify the UICC.
 *
 * See RSat::NotifySetUpEventListPCmd() for more information.
 *
 * @param aReqStatus This method is asynchronous.  aReqStatus indicates when the operation has completed.
 * @param aSingleEvent Name of the event that has occurred.
 * @param aEventInfo Additional information available about aSingleEvent.
 *
 * @capability NetworkControl
 * @capability WriteDeviceData
 *
 * @see RSat::NotifySetUpEventListPCmd()
 * @see RSat::TEventList
 */
	{
	__ASSERT_ALWAYS(iEtelSatPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iEtelSatPtrHolder->iSingleEvent=aSingleEvent;
	TPtrC8& ptr1=iEtelSatPtrHolder->SetC(ESlotSingleEvent,iEtelSatPtrHolder->iSingleEvent);

	Set(ESatEventDownload,aReqStatus,ptr1,aEventInfo);
	}


EXPORT_C RSat::TDisplayTextV1::TDisplayTextV1()
	: TSatPCmdBase(),
	  iPriority(EDisplayPriorityNotSet),
	  iClearScreenTrigger(EClearScreenTriggerNotSet),
	  iText(),
	  iImmediateRsp(EImmediateRspNotSet)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	iIconId.iQualifier=EIconQualifierNotSet;
	iIconId.iIdentifier=0;
	}

EXPORT_C RSat::TDisplayTextV2::TDisplayTextV2()
	: TDisplayTextV1()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	iDuration.iTimeUnit=ETimeUnitNotSet;
	iDuration.iNumOfUnits=0;
	}

EXPORT_C RSat::TDisplayTextV5::TDisplayTextV5()
	: TDisplayTextV2()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV5;
	iTextAttribute.iStatus = ETextAttributeNotSet;
	}

EXPORT_C RSat::TDisplayTextV6::TDisplayTextV6()
	: TDisplayTextV5(),iFrameId(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV6;
	}
	
EXPORT_C RSat::TDisplayTextRspV1::TDisplayTextRspV1()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iAdditionalInfo()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	}

EXPORT_C void RSat::NotifyDisplayTextPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a DISPLAY TEXT proactive command is sent
 * by the UICC.  When the request completes, the command is placed into aPCmd and the
 * aReqStatus is completed.  Unpack the command to examine its contents.
 *
 * The DISPLAY TEXT command instructs the terminal to display a text message,
 * and/or an icon.  The command is described fully in ETSI TS 102 223.  Briefly,
 * though, the command includes:
 *
 * - Text to display - RSat::TDisplayTextV1::iText
 * - Icon to display - RSat::TDisplayTextV1::iIconId
 * - Priority of icon/text - RSat::TDisplayTextV1::iPriority
 * - Determines whether you respond to the UICC straight away or when the text/icon is removed
 *     from the screen  - RSat::TDisplayTextV1::iImmediateRsp
 * - Determines whether the text should be removed from the screen after a delay or after user
 *     action. - RSat::TDisplayTextV1::iClearScreenTrigger
 * - Length of delay to display text - RSat::TDisplayTextV2::iDuration (Symbian OS v7.0s and later.)
 * - Text attributes of the text string to display - RSat::TDisplayTextV5::iTextAttribute (Symbian OS v9.2 or later)
 *
 * Once you have dealt with the command, you must give your response to the
 * DISPLAY TEXT command.  In Symbian OS versions up to and including 8.1a & 8.1b,
 * pass RSat::TerminalRsp() a RSat::TDisplayTextRspV1Pckg (a packaged
 * RSat::TDisplayTextRspV1).  For later versions of Symbian OS, check for an updated API
 * (RSat::TDisplayTextRspV2 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.  aPCmd must be a packaged version of the  latest "TDisplayText" class.
 * For Symbian OS 7.0s, 8.0 and 8.1 use RSat::TDisplayTextV2Pckg, a packaged
 * RSat::TDisplayTextV2. Use RSat::TDisplayTextV1 with older versions of
 * Symbian OS.  For Symbian OS version 9.2, use RSat::TDisplayTextV5Pckg.
 * For Symbian OS version 9.4, use RSat::TDisplayTextV6Pckg.
 * For later versions of Symbian OS, check for an updated API
 * (RSat::TDisplayTextV7 etc).
 *
 * @see RSat::TDisplayTextV1
 * @see RSat::TDisplayTextV2
 * @see RSat::TDisplayTextV5
 * @see RSat::TDisplayTextV6
 * @see RSat::TDisplayTextRspV1
 *
 * @capability ReadDeviceData
 */
	{
	Get(ESatNotifyDisplayTextPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TGetInkeyV1::TGetInkeyV1()
	: TSatPCmdBase(),
	  iRspFormat(EGetInkeyRspFormatNotSet),
	  iText(),
	  iHelp(EHelpNotSet)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	iIconId.iQualifier=EIconQualifierNotSet;
	iIconId.iIdentifier=0;
	}

EXPORT_C RSat::TGetInkeyV2::TGetInkeyV2()
	: TGetInkeyV1(),
	  iMode(EGetInkeyRspModeNotSet)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	iDuration.iTimeUnit=ETimeUnitNotSet;
	iDuration.iNumOfUnits=0;
	}

EXPORT_C RSat::TGetInkeyV5::TGetInkeyV5()
	: TGetInkeyV2()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV5;
	iTextAttribute.iStatus = ETextAttributeNotSet;
	}

EXPORT_C RSat::TGetInkeyV6::TGetInkeyV6()
	: TGetInkeyV5(),iFrameId(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV6;
	}
	
EXPORT_C RSat::TGetInkeyRspV1::TGetInkeyRspV1()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iRspFormat(EGetInkeyRspFormatNotSet),
	  iAdditionalInfo()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	}

EXPORT_C RSat::TGetInkeyRspV2::TGetInkeyRspV2()
	: TGetInkeyRspV1()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	iDuration.iTimeUnit=ETimeUnitNotSet;
	iDuration.iNumOfUnits=0;
	}

EXPORT_C void RSat::NotifyGetInkeyPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a GET INKEY proactive command is sent by
 * the UICC.  When the request completes, the command is placed into aPCmd and the
 * aReqStatus is completed.  Unpack the command to
 * examine its contents.
 *
 * The GET INKEY command instructs you to display text and/or an icon and to
 * expect the user to enter a single character. The command states the required
 * response:
 *
 * - Get a digit (0-9, *, #, and +) from the user.
 * - Get a character from the user from the SMS alphabet.
 * - Get a character from the user from the UCS2 alphabet.
 * - Get a Yes or No decision from the user.
 *
 * The response entered by the user shall be passed back to the UICC.  The command
 * is described fully in ETSI TS 102 223.  Briefly, though, the command includes:
 *
 * - Text to display - RSat::TGetInkeyV1::iText
 * - Icon to display - RSat::TGetInkeyV1::iIconId
 * - Response required - RSat::TGetInkeyV1::iRspFormat
 * - Determines whether help is available - RSat::TGetInkeyV1::iHelp
 * - Determines how long should you wait for the user to respond. -
 *     RSat::TGetInkeyV2::iDuration (Symbian OS v7.0s and later.)
 * - Determines whether the user can alter/confirm their choice. -
 *     RSat::TGetInkeyV2::iMode (Symbian OS v7.0s and later.)
 * - Text attributes of text contained in iText - RSat::TGetInkeyV5::iTextAttribute (Symbian OS v9.2 or later)
 *
 * Once you have dealt with the command, you must give your response to the GET
 * INKEY command.  For Symbian OS v6.1 or v7.0 pass RSat::TerminalRsp() a
 * RSat::TGetInkeyRspV1Pckg (a packaged TGetInkeyRspV1).  For versions up to and
 * included v8.1a and v8.1b use RSat::TGetInkeyRspV2Pckg (a packaged
 * RSat::TGetInkeyRspV2).   For later versions of Symbian OS, check for an updated API
 * (RSat::TGetInkeyRspV3 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.  aPCmd must be a packaged version of the  latest "TGetInkey" class.
 * For Symbian OS 7.0s, 8.0 and 8.1 use RSat::TGetInkeyV2Pckg, a packaged
 * RSat::TGetInkeyV2. For Symbian OS version 9.2, use RSat::TDisplayTextV5Pckg.
 * Use RSat::TGetInkeyV1 with older versions of Symbian OS.  For Symbian OS version 9.4, use 
 * RSat::TDisplayTextV6Pckg.a packaged RSat::TGetInkeyV6. For later versions of Symbian OS, check 
 * for an updated API(RSat::TGetInkeyV7 etc).
 *
 * @see RSat::TGetInkeyV1
 * @see RSat::TGetInkeyV2
 * @see RSat::TGetInkeyV5
 * @see RSat::TGetInkeyV6
 * @see RSat::TGetInkeyRspV1
 * @see RSat::TGetInkeyRspV2
 *
 * @capability None
 */
	{
	Get(ESatNotifyGetInkeyPCmd,aReqStatus,aPCmd);
	}


EXPORT_C RSat::TGetInputV1::TGetInputV1()
	: TSatPCmdBase(),
	  iInputDisplayOption(EInputDisplayOptionNotSet),
	  iRspFormat(EGetInputRspFormatNotSet),
	  iText(),
	  iDefaultText(),
	  iHelp(EHelpNotSet)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	iIconId.iQualifier=EIconQualifierNotSet;
	iIconId.iIdentifier=0;
	iRspLength.iMinRspLength=0;
	iRspLength.iMaxRspLength=0;
	}

EXPORT_C RSat::TGetInputV5::TGetInputV5()
	: TGetInputV1()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV5;
	iTextAttribute.iStatus=ETextAttributeNotSet;
	}

EXPORT_C RSat::TGetInputV6::TGetInputV6()
	: TGetInputV5(),iFrameId(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV6;
	}
	
EXPORT_C RSat::TGetInputRspV1::TGetInputRspV1()
	: TSatPCmdBase(),
	iGeneralResult(KPCmdResultNotSet),
	iInfoType(0),
	iRspFormat(EGetInputRspFormatNotSet),
	iAdditionalInfo()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	}

EXPORT_C void RSat::NotifyGetInputPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a GET INPUT proactive command is sent
 * by the UICC.  When the request completes, the command is placed into aPCmd and the
 * aReqStatus is competed.  Unpack the command to examine its contents.
 *
 * The GET INPUT command instructs you to display text and/or an icon and to
 * expect the user to enter a string. Pass the string back to the UICC.  The
 * command is described fully in ETSI TS 102 223.  Briefly, though, the command
 * includes:
 *
 * - Text to display - RSat::TGetInkeyV1::iText
 * - Icon to display - RSat::TGetInkeyV1::iIconId
 * - Determines wether help is available - RSat::TGetInkeyV1::iHelp
 * - The required string format: digits only (0-9, *, # and +), characters from
 *     the SMS default alphabet, or characters from the UCS2 alphabet -
 *     RSat::TGetInputV1::iRspFormat
 * - The maximum and minimum length of string - RSat::TGetInputV1::iRspLength
 * - Determines whether the string can contain digits or the characters from
 *     SMS default alphabet.  Also determines whether they are in a
 *     packed or unpacked format. - RSat::TGetInputV1::iRspFormat
 * - Determines whether the user can see the characters they enter. RSat::TGetInputV1::iInputDisplayOption.
 * - A default string can be provided.  This field determines whether the
 *     user can edit this, or can only delete it and replace it with their own text -
 *     RSat::TGetInputV1::iDefaultText
 * - Text attributes of text contained in iText - RSat::TGetInputV5::iTextAttribute (Symbian OS v9.2 or later)
 *
 * Once you have dealt with the command, you must give your response to the GET
 * INPUT command.  For Symbian OS versions up to and including  v8.1a and v8.1b
 * pass RSat::TerminalRsp() a RSat::TGetInputRspV1Pckg (a packaged
 * RSat::TGetInputRspV1).  For later versions of Symbian OS, check for an updated API
 * (RSat::TGetInputRspV2 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.  aPCmd must be a packaged version of the  latest "TGetInput" class.
 * For Symbian OS versions up to and including v8.1a and 8.1b use
 * RSat::TGetInkeyV1Pckg, a packaged RSat::TGetInkeyV1.  
 * For Symbian OS version 9.2, use RSat::TGetInkeyV5Pckg.  
 * For Symbian OS version 9.4, use RSat::TGetInkeyV6Pckg.
 * For later versions of Symbian OS, check for an updated API 
 * (RSat::TGetInkeyV7 etc).
 *
 * @see RSat::TGetInputV1
 * @see RSat::TGetInputV5
 * @see RSat::TGetInputV6
 * @see RSat::TGetInputRspV1
 *
 * @capability None
 */
	{
	Get(ESatNotifyGetInputPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TPlayToneV1::TPlayToneV1()
	: TSatPCmdBase(),
	  iTone(KToneNotSet)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	iDuration.iTimeUnit=ETimeUnitNotSet;
	iDuration.iNumOfUnits=0;
	iAlphaId.iStatus=EAlphaIdNotSet;
	}

EXPORT_C RSat::TPlayToneV2::TPlayToneV2()
	: TPlayToneV1()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	iIconId.iQualifier=EIconQualifierNotSet;
	iIconId.iIdentifier=0;
	}

EXPORT_C RSat::TPlayToneV5::TPlayToneV5()
	: TPlayToneV2()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV5;
	iTextAttribute.iStatus = ETextAttributeNotSet;
	}

EXPORT_C RSat::TPlayToneV6::TPlayToneV6()
	: TPlayToneV5(),iFrameId(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV6;
	}
	
EXPORT_C RSat::TPlayToneRspV1::TPlayToneRspV1()
	: TSatPCmdBase(),
	iGeneralResult(KPCmdResultNotSet),
	iInfoType(0),
	iAdditionalInfo()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	}

EXPORT_C void RSat::NotifyPlayTonePCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a PLAY TONE proactive command is sent
 * by the UICC.  When the request completes, the command is placed into aPCmd and the
 * aReqStatus is competed.  Unpack the command to examine its contents.
 *
 * The PLAY TONE command instructs you to play a tone.   The
 * command is described fully in ETSI TS 102 223.  Briefly, though, the command
 * includes:
 *
 * - The tone to play - RSat::TPlayToneV1::iTone.
 * - The duration of the tone - RSat::TPlayToneV1::iDuration.
 * - An alpha identifier containing text to display - RSat::TPlayToneV1::iAlphaId.
 * - An icon to display - RSat::TPlayToneV2::iIconId (Symbian OS v7.0s and later.)
 * - Text attributes of text contained in iAlphaId - RSat::TPlayToneV5::iTextAttribute (Symbian OS v9.2 or later)
 *
 * See the description of the PLAY TONE command in ETSI TS 102 223 for the
 * circumstances under which the alpha identifier's text should be displayed.
 * Display the icon as described in RSat::TIconId.
 *
 * Once you have dealt with the command, you must give your response to the PLAY
 * TONE command.  For Symbian OS v6.1 or v7.0 pass RSat::TerminalRsp() a
 * RSat::TPlayToneRspV1Pckg (a packaged TPlayToneRspV1).  For versions up to and
 * including v8.1a and v8.1b use RSat::TPlayToneRspV2Pckg (a packaged
 * RSat::TPlayToneRspV2).   
 * For later versions of Symbian OS, check for an updated API
 * (RSat::TPlayToneRspV3 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.  aPCmd must be a packaged version of the  latest "TPlayTone" class.
 * For Symbian OS versions up to and including v8.1a and 8.1b use
 * RSat::TPlayToneV2Pckg, a packaged RSat::TPlayToneV2.  
 * For Symbian OS version 9.2, use RSat::TPlayToneV5Pckg.
 * For Symbian OS version 9.4, use RSat::TPlayToneV6Pckg.
 * For later versions of Symbian OS, check for an updated API 
 * (RSat::TPlayToneV7 etc).
 *
 * @see RSat::TPlayToneV1
 * @see RSat::TPlayToneV2
 * @see RSat::TPlayToneV5
 * @see RSat::TPlayToneV6
 * @see RSat::TPlayToneRspV1
 *
 * @capability None
 */
	{
	Get(ESatNotifyPlayTonePCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TMenuItemBaseV1::TMenuItemBaseV1()
	: TSatPCmdBase(),
	iNumberOfItems(0),
	iMenuContent(0),
	iBuf()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	}

EXPORT_C RSat::TMenuItemBaseV5::TMenuItemBaseV5()
	: TMenuItemBaseV1()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV5;
	}

EXPORT_C TBool RSat::TMenuItemBaseV1::IsRemoveMenu() const
/**
 * This method returns ETrue if the SET UP MENU
 * proactive command specifies that the existing SAT menu should be removed.
 *
 * This method returns EFalse if the SET UP MENU
 * proactive command specifies a new SAT menu to be added to the phone's menu system.
 *
 * RSat::TMenuItemBaseV1 is used to contain both SET UP MENU and SELECT ITEM
 * proactive commands.  However, only call this method when handling a SET UP MENU command.
 *
 * @return ETrue if the existing SAT menu should be removed from the phone menu system.
 * @return EFalse if a new SAT menu should be inserted in the phone menu system.
 */
	{
	if(iBuf.Length()==0)
		return ETrue;
	else
		return EFalse;
	}

EXPORT_C TInt RSat::TMenuItemBaseV1::MenuContent() const
/**
 * This method describes the menu's content by returning four flags.
 * All menus contain a number of items which may also have an
 * associated text attribute.
 * Items are indicated by the RSat::KItems flag.
 * The presence of text attribute information for these items is
 * indicated by the RSat::KItemTextAttributes flag.
 *
 * In addition, every item in a menu may be accompanied by an icon
 * (indicated by RSat::KItemIconIds) and/or a "next action indicator"
 * (RSat::KItemNextActions).
 *
 * In a menu, either ALL menu items will have an icon, or NONE of
 * them will.
 * Similarly, all menu items in a menu will also have text
 * attribute information or none of them will.
 * Also, all menu items in a menu will have a "next action indicator"
 * or none of them will.
 *
 * @return A combination of the flags RSat::KItems, RSat::KItemTextAttributes, RSat::KItemNextActions and RSat::KItemIconIds.
 */
	{
	return iMenuContent;
	}

EXPORT_C TUint RSat::TMenuItemBaseV1::NumberOfItems() const
/**
 * This method returns the number of items in the menu.
 *
 * @return Protected data member, iNumberOfItems.
 */
	{
	return iNumberOfItems;
	}

TInt RSat::TMenuItemBaseV1::FindItemInBuffer(TUint aIndex, TInt& aOffset) const
/**
 * This method finds the position of the item designated by aIndex in iBuf (which
 * contains the menu item list)
 * The position of the item is returned in aOffset.
 *
 * Actions and icons are optional, so the buffer could only be made of items or items&actions
 * or items&icons or items&actions&icons.
 *
 * In all of the above mentioned menu structures, the items themselves can consist of
 * only the item text string or the item text string and associated text attribute
 * information. If an item with text attributes is added to the menu, then all subsequent
 * items must also contain text attribute information.
 *
 * The structure of the buffer is as follows:
 *
 * <pre>
 * |-------------|--------|------|--
 * | item header | itemid | item | item header
 * |-------------|--------|------|--
 *
 * |-------------|--------|------|---------------|--------|--
 * | item header | itemid | item | action header | action | item header
 * |-------------|--------|------|---------------|--------|--
 *
 * |-------------|--------|------|----------------|---------|--
 * | item header | itemid | item | icon ID header | icon ID | item header
 * |-------------|--------|------|----------------|---------|--
 *
 * |-------------|--------|------|---------------|--------|----------------|---------|--
 * | item header | itemid | item | action header | action | icon ID header | icon ID | item header
 * |-------------|--------|------|---------------|--------|----------------|---------|--
 *
 * In Symbian OS v9.2, support for these structures was added:
 *
 * |---------------------------------|--------|---------------------|------|-------
 * | item with text attribute header | itemid | item text attribute | item | item with text attribute header
 * |---------------------------------|--------|---------------------|------|-------
 *
 * |---------------------------------|--------|---------------------|------|---------------|--------|--
 * | item with text attribute header | itemid | item text attribute | item | action header | action | item with text attribute header
 * |---------------------------------|--------|---------------------|------|---------------|--------|--
 *
 * |---------------------------------|--------|---------------------|------|----------------|---------|--
 * | item with text attribute header | itemid | item text attribute | item | icon ID header | icon ID | item with text attribute header
 * |---------------------------------|--------|---------------------|------|----------------|---------|--
 *
 * |---------------------------------|--------|---------------------|------|---------------|--------|----------------|---------|--
 * | item with text attribute header | itemid | item text attribute | item | action header | action | icon ID header | icon ID | item with text attribute header
 * |---------------------------------|--------|---------------------|------|---------------|--------|----------------|---------|--
 *
 * </pre>
 *
 * The header is two bytes:
 *		- the first byte is a tag corresponding to the associated object (item, action or icon).
 *		- the second byte is the length of the associated object.
 *
 * @param aIndex Index of the item to be found in the buffer.
 * @param aOffset Position of the item in iBuf (offset from the start of iBuf).
 * @return KErrCorrupt returned if the buffer is corrupted & the item can't be found.
 *         Otherwise, KErrNone returned
 */
	{
	aOffset=0;
	for (TUint i=1;i<aIndex;i++)
		{
		// All the header bits are in the upper byte of the TBuf16 only so
		// compare the upper byte with item header.
		if(((iBuf[aOffset]&KUpperByte)!=KItemHeader) &&
			((iBuf[aOffset]&KUpperByte)!=KItemWithTextAttributeHeader))
			{
			return KErrCorrupt;
			}
		else
			{
			// The length is in the lower byte of the item header.
			// This length does NOT include the header length.
			// Jump to next header.
			aOffset+=KHeaderLength+(iBuf[aOffset]&KLowerByte);

			// Next header can be an action, jump past it, if so.
			if((iBuf[aOffset]&KUpperByte)==KActionHeader)
				{
				aOffset=aOffset+KHeaderLength+KActionLength;
				}

			// Next header can be an icon, jump past it, if so.
			if ((iBuf[aOffset]&KUpperByte)==KIconIdHeader)
				{
				aOffset=aOffset+KHeaderLength+KIconIdLength;
				}
			}
		}
	return KErrNone;
	}


EXPORT_C TInt RSat::TMenuItemBaseV1::GetItem(TUint aIndex, TItem& aItem) const
/**
 * This method returns the menu item, designated by aIndex.
 *
 * @param aIndex Index of the menu item. Index range starts at 1.
 *        RSat::TMenuItemBaseV1::NumberOfItems() returns the total
 *        number of items.
 * @param aItem Item from the menu, designated by aIndex.
 * @return KErrNotFound returned if the menu contains no items or aIndex is out of range.
 *         KErrCorrupt returned if the buffer is corrupted and the item can't be found.
 */
	{
	// Check if aIndex is in the valid range.
	// Also check that iBuf is not empty, in which case there are no menu items stored.
	if((iBuf.Length()==0) || (aIndex<1) || (aIndex>iNumberOfItems))
		{
		return KErrNotFound;
		}
	else
		{
		TInt offset;
		// FindItemInBuffer modifies offset to return the position of the requested
		// menu item within iBuf.
		if (FindItemInBuffer(aIndex, offset)==KErrCorrupt)
			{
			return KErrCorrupt;
			}

		// The offset should be pointing at one of the item headers now.
		if (((iBuf[offset]&KUpperByte)!=KItemHeader) &&
			(iBuf[offset]&KUpperByte)!=KItemWithTextAttributeHeader)
			{
			return KErrCorrupt;
			}
		else
			{
			// The item ID is a TUint8, which has been stored in a unicode buffer.
			// Before casting it back to a TUint8 we check that the upper byte
			// is zero.
			// If it is different from 0, then there has been a data corruption
			// since only headers have any content in the upper byte.
			if((iBuf[offset+KHeaderLength]&KUpperByte)!=0)
				{
				return KErrCorrupt;
				}
			else
				{
				TUint8 textAttributeOffset = 0;
				if ((iBuf[offset]&KUpperByte)==KItemWithTextAttributeHeader)
					{
					textAttributeOffset = KTextAttributeBufSize;
					}

				aItem.iItemId=static_cast<TUint8>(iBuf[offset+KHeaderLength]);
				aItem.iItemString=iBuf.Mid(offset+KHeaderLength+KItemIdLength+textAttributeOffset,
					((iBuf[offset]&KLowerByte)-KItemIdLength-textAttributeOffset));
				return KErrNone;
				}
			}
		}
	}


EXPORT_C TInt RSat::TMenuItemBaseV1::GetItem(TUint aIndex, TItem& aItem, TInt& aAction) const
/**
 * This method is an overloaded method of RSat::TMenuItemBaseV1::GetItem(TUint aIndex, TItem& aItem).
 * It returns the menu item's "next action indicator" (if available) as well as the menu item itself.
 * RSat::TMenuItemBaseV1::MenuContent() tells you whether a menu's items are
 * accompanied by "next action indicators".
 *
 * @param aIndex Index of the menu item. Index range starts at 1.
 *        RSat::TMenuItemBaseV1::NumberOfItems() returns the total
 *        number of items.
 * @param aItem Item from the menu, designated by aIndex.
 * @param aAction Action associated with the item.
 * @return KErrNotFound returned if the menu contains no items or aIndex is out of range.
 *         KErrCorrupt returned if the buffer is corrupted and the item can't be found.
 */
	{
	// Check if aIndex is in the valid range.
	// Also check that iBuf is not empty, in which case there are no menu items stored.
	if((iBuf.Length()==0)||(aIndex<1)||(aIndex>iNumberOfItems))
		{
		return KErrNotFound;
		}
	else
		{
		TInt offset;
		// FindItemInBuffer modifies offset to return the position of the requested
		// menu item within iBuf.
		if (FindItemInBuffer(aIndex, offset)==KErrCorrupt)
			{
			return KErrCorrupt;
			}

		if (((iBuf[offset]&KUpperByte)!=KItemHeader) &&
			(iBuf[offset]&KUpperByte)!=KItemWithTextAttributeHeader)
			{
			return KErrCorrupt;
			}
		else
			{
			if((iBuf[offset+KHeaderLength]&KUpperByte)!=0)
				{
				return KErrCorrupt;
				}
			else
				{
				TUint8 textAttributeOffset = 0;
				if ((iBuf[offset]&KUpperByte)==KItemWithTextAttributeHeader)
					{
					textAttributeOffset = KTextAttributeBufSize;
					}

				aItem.iItemId=static_cast<TUint8>(iBuf[offset+KHeaderLength]);
				aItem.iItemString=iBuf.Mid(offset+KHeaderLength+KItemIdLength+textAttributeOffset,
					((iBuf[offset]&KLowerByte)-KItemIdLength-textAttributeOffset));
				// Set the offset past the item data, should be pointing to
				// action header now.
				offset+=KHeaderLength+(iBuf[offset]&KLowerByte);
				if ((offset+KActionLength)>iBuf.Length())
					{
					// There is no item next action associated with the requested item
					// and offset has reached the end of iBuf.
					aAction=KErrNotFound;
					}
				else
					{
					if((iBuf[offset]&KUpperByte)!=KActionHeader)
						{
						// There is no item next action associated with the requested item
						aAction=KErrNotFound;
						}
					else
						{
						aAction=iBuf[offset+KHeaderLength];
						}
					}
				return KErrNone;
				}
			}
		}
	}

EXPORT_C TInt RSat::TMenuItemBaseV1::GetItemIcon(TUint aIndex, TItem& aItem, TInt& aIconId) const
/**
 * This method is an overloaded method of RSat::TMenuItemBaseV1::GetItem(TUint aIndex, TItem& aItem).
 * It returns the menu item's icon identifier  (if available) as well as the menu item itself.
 * RSat::TMenuItemBaseV1::MenuContent() tells you whether a menu's items are
 * accompanied by icon identifier.
 *
 * @param aIndex Index of the menu item. Index range starts at 1.
 *        RSat::TMenuItemBaseV1::NumberOfItems() returns the total
 *        number of items.
 * @param aItem Item from the menu, designated by aIndex.
 * @param aIconId Icon associated with the menu item.
 * @return KErrNotFound returned if the menu contains no items or aIndex is out of range.
 *         KErrCorrupt returned if the buffer is corrupted and the item can't be found.
 */
	{
	if((iBuf.Length()==0) || (aIndex<1) || (aIndex>iNumberOfItems))
		{
		return KErrNotFound;
		}
	else
		{
		TInt offset;
		// FindItemInBuffer modifies offset to return the position of the requested
		// menu item within iBuf.
		if (FindItemInBuffer(aIndex,offset)==KErrCorrupt)
			{
			return KErrCorrupt;
			}

        if (((iBuf[offset]&KUpperByte)!=KItemHeader) &&
			(iBuf[offset]&KUpperByte)!=KItemWithTextAttributeHeader)
			{
			return KErrCorrupt;
			}
		else
			{
			// Only headers should have the upper byte set, so there shouldn't
			// be anything in the upper byte after the header.
			if((iBuf[offset+KHeaderLength]&KUpperByte)!=0)
				{
				return KErrCorrupt;
				}
			else
				{
				TUint8 textAttributeOffset = 0;
				if ((iBuf[offset]&KUpperByte)==KItemWithTextAttributeHeader)
					{
					textAttributeOffset = KTextAttributeBufSize;
					}

				aItem.iItemId=static_cast<TUint8>(iBuf[offset+KHeaderLength]);
				aItem.iItemString=iBuf.Mid(offset+KHeaderLength+KItemIdLength+textAttributeOffset,
					((iBuf[offset]&KLowerByte)-KItemIdLength-textAttributeOffset));
				offset+=KHeaderLength+(iBuf[offset]&KLowerByte);
				if ((offset+KActionLength)>iBuf.Length())
					{
					// There is no item icon ID associated with the requested item
					// and offset has reached the end of iBuf.
					aIconId=KErrNotFound;
					}
				else
					{
					if ((iBuf[offset]&KUpperByte)==KActionHeader)
						{
						offset+=KHeaderLength+(iBuf[offset]&KLowerByte);
						}
					if ((offset+KIconIdLength)>iBuf.Length())
						{
						aIconId=KErrNotFound;
						}
					else
						{
						if((iBuf[offset]&KUpperByte)!=KIconIdHeader)
							{
							// There is no item icon ID associated with the requested item
							aIconId=KErrNotFound;
							}
						else
							{
							aIconId=iBuf[offset+KHeaderLength];
							}
						}
					}
				return KErrNone;
				}
			}
		}
	}

EXPORT_C TInt RSat::TMenuItemBaseV1::GetItem(TUint aIndex, TItem& aItem, TInt& aAction, TInt& aIconId) const
/**
 * This method is an overloaded method of RSat::TMenuItemBaseV1::GetItem(TUint aIndex, TItem& aItem).
 * It returns the menu item's "next action indicator" and icon identifier
 * (if they are available) as well as the requested menu item.
 * RSat::TMenuItemBaseV1::MenuContent() tells you whether a menu's items are
 * accompanied by icon identifiers and "next action indicators".
 *
 * @param aIndex Index of the menu item. Index range starts at 1.
 *        RSat::TMenuItemBaseV1::NumberOfItems() returns the total
 *        number of items.
 * @param aItem Item from the menu, designated by aIndex.
 * @param aAction Action associated with the item.
 * @param aIconId Icon associated with the menu item.
 * @return KErrNotFound returned if the menu contains no items or aIndex is out of range.
 *         KErrCorrupt returned if the buffer is corrupted and the item can't be found.
 */
	{
	if((iBuf.Length()==0) || (aIndex<1) || (aIndex>iNumberOfItems))
		{
		return KErrNotFound;
		}
	else
		{
		TInt offset;
		// FindItemInBuffer modifies offset to return the position of the requested
		// menu item within iBuf.
		if (FindItemInBuffer(aIndex,offset)==KErrCorrupt)
			{
			return KErrCorrupt;
			}

		if (((iBuf[offset]&KUpperByte)!=KItemHeader) &&
			(iBuf[offset]&KUpperByte)!=KItemWithTextAttributeHeader)
			{
			return KErrCorrupt;
			}
		else
			{
			if((iBuf[offset+KHeaderLength]&KUpperByte)!=0)
				{
				return KErrCorrupt;
				}
			else
				{
				TUint8 textAttributeOffset = 0;
				if ((iBuf[offset]&KUpperByte)==KItemWithTextAttributeHeader)
					{
					textAttributeOffset = KTextAttributeBufSize;
					}

				aItem.iItemId=static_cast<TUint8>(iBuf[offset+KHeaderLength]);
				aItem.iItemString=iBuf.Mid(offset+KHeaderLength+KItemIdLength+textAttributeOffset,
									((iBuf[offset]&KLowerByte)-KItemIdLength-textAttributeOffset));
				// Make offset point past the item string
				offset+=KHeaderLength+(iBuf[offset]&KLowerByte);

				if ((offset+KActionLength)>iBuf.Length())
					{
					aAction=KErrNotFound;
					aIconId=KErrNotFound;
					}
				else
					{
					if((iBuf[offset]&KUpperByte)!=KActionHeader)
						{
						aAction=KErrNotFound;
						}
					else
						{
						aAction=iBuf[offset+KHeaderLength];
						offset+=KHeaderLength+KActionLength;
						}
					if ((offset+KIconIdLength)>iBuf.Length())
						{
						aIconId=KErrNotFound;
						}
					else
						{
						if ((iBuf[offset]&KUpperByte)!=KIconIdHeader)
							aIconId=KErrNotFound;
						else
							aIconId=iBuf[offset+KHeaderLength];
						}
					}
				return KErrNone;
				}
			}
		}
	}

EXPORT_C TInt RSat::TMenuItemBaseV1::AddItem(const TItem& aItem)
/**
 * This method adds a menu item to iBuf, which contains the menu item list.
 * It is not needed by RSat clients, only the TSY.
 *
 * @param aItem Item to be added to iBuf.
 * @return KErrNoMemory returned if there is not
 *         enough space left in iBuf to stored aItem.
 *         Otherwise KErrNone returned.
 */
	{
	TInt header=0;

	if ((iBuf.Length()!=0)&&(iMenuContent != KItems))
		{
		return KErrNotSupported;
		}
	else if ((iBuf.Length()+KHeaderLength+KItemIdLength+aItem.iItemString.Length())>iBuf.MaxLength())
		{
		// There is not enough space left in iBuf to add an item
		return KErrNoMemory;
		}
	else
		{
		// If the menu is empty or only contains items (with no text attributes)
		// and there is enough space for the next item, a new item can be added.
		header = KItemHeader|(aItem.iItemString.Length()+KItemIdLength);
		iBuf.Append(header);
		iBuf.Append(aItem.iItemId);
		iBuf.Append(aItem.iItemString);
		iMenuContent = KItems;
		iNumberOfItems++;
		return KErrNone;
		}
	}

EXPORT_C TInt RSat::TMenuItemBaseV1::AddItem(const TItem& aItem, TUint aAction)
/**
 * This method is an overloaded method of AddItem(TItem aItem)
 * It adds the menu item next action as well as the specified menu item.
 * It is not needed by RSat clients, only the TSY.
 *
 * @param aItem Item to be added to iBuf.
 * @param aAction Action associated with an item that is to be added to iBuf.
 * @return KErrNoMemory returned if there is not
 *         enough space left in iBuf to stored aItem and an Action.
 *         Otherwise, KErrNone returned.
 */
	{
	TInt header=0;

	if ((iBuf.Length()!=0)&&(iMenuContent!=(KItems|KItemNextActions)))
		{
		return KErrNotSupported;
		}
	else if ((iBuf.Length()+KHeaderLength+KItemIdLength+aItem.iItemString.Length()
		+KHeaderLength+KActionLength)>iBuf.MaxLength())
		{
		// There is not enough space left in iBuf to add an item and its associated
		// next action
		return KErrNoMemory;
		}
	else
		{
		header = KItemHeader|(aItem.iItemString.Length()+KItemIdLength);
		iBuf.Append(header);
		iBuf.Append(aItem.iItemId);
		iBuf.Append(aItem.iItemString);
		header = KActionHeader|KActionLength;
		iBuf.Append(header);
		iBuf.Append(aAction);
		iMenuContent=KItems|KItemNextActions;
		iNumberOfItems++;
		return KErrNone;
		}
	}

EXPORT_C TInt RSat::TMenuItemBaseV1::AddItemIcon(const TItem& aItem, TUint aIconId)
/**
 * This method is an overloaded method of AddItem(TItem aItem)
 * It adds the menu item icon ID as well as the specified menu item.
 * It is not needed by RSat clients, only the TSY.
 *
 * @param aItem Item to be added to iBuf.
 * @param aIconId Icon identifier, associated with aItem, to be added to iBuf.
 * @return KErrNoMemory returned if there is not
 *         enough space left in iBuf to store aItem and aIconId.
 *         Otherwise, KErrNone returned.
 */
	{
	TInt header=0;

	if ((iBuf.Length()!=0)&&(iMenuContent!=(KItems|KItemIconIds)))
		{
		return KErrNotSupported;
		}
	else if ((iBuf.Length()+KHeaderLength+KItemIdLength+aItem.iItemString.Length()
		+KHeaderLength+KIconIdLength)>iBuf.MaxLength())
		{
		// There is not enough space left in iBuf to add an item and its associated
		// icon identifier.
		return KErrNoMemory;
		}
	else
		{
		header=KItemHeader|(aItem.iItemString.Length()+KItemIdLength);
		iBuf.Append(header);
		iBuf.Append(aItem.iItemId);
		iBuf.Append(aItem.iItemString);
		header=KIconIdHeader|KIconIdLength;
		iBuf.Append(header);
		iBuf.Append(aIconId);
		iMenuContent=KItems|KItemIconIds;
		iNumberOfItems++;
		return KErrNone;
		}
	}

EXPORT_C TInt RSat::TMenuItemBaseV1::AddItem(const TItem& aItem, TUint aAction, TUint aIconId)
/**
 * This method is an overloaded method of AddItem(TItem aItem)
 * It adds the menu item next action and icon ID as well as the specified menu item
 * It is not needed by RSat clients, only the TSY.
 *
 * @param aItem Item to be added to iBuf.
 * @param aAction Action associated with an item that is to be added to iBuf.
 * @param aIconId Icon identifier, associated with aItem, to be added to iBuf.
 * @return KErrNoMemory returned if there is not
 *         enough space left in iBuf to store aItem and aIconId.
 *         Otherwise, KErrNone returned.
 */
	{
	TInt header=0;
	if ((iBuf.Length()!=0)&&(iMenuContent!=(KItems|KItemNextActions|KItemIconIds)))
		{
		return KErrNotSupported;
		}
	else if ((iBuf.Length()+3*(KHeaderLength)+KItemIdLength+aItem.iItemString.Length()
		+KActionLength+KIconIdLength)>iBuf.MaxLength())
		{
		// There is not enough space left in iBuf to add an item and its associated
		// icon identifier and next action.
		return KErrNoMemory;
		}
	else
		{
		header = KItemHeader|(aItem.iItemString.Length()+KItemIdLength);
		iBuf.Append(header);
		iBuf.Append(aItem.iItemId);
		iBuf.Append(aItem.iItemString);
		header = KActionHeader|KActionLength;
		iBuf.Append(header);
		iBuf.Append(aAction);
		header = KIconIdHeader|KIconIdLength;
		iBuf.Append(header);
		iBuf.Append(aIconId);
		iMenuContent=KItems|KItemNextActions|KItemIconIds;
		iNumberOfItems++;
		return KErrNone;
		}
	}

EXPORT_C void RSat::TMenuItemBaseV1::Reset()
/**
 * This method sets the iBuf length and content to zero.
 * It is not needed by RSat clients, only the TSY.
 */
	{
	iNumberOfItems=0;
	iMenuContent=0;
	iBuf.SetLength(0);
	}

EXPORT_C TInt RSat::TMenuItemBaseV5::GetItem (TUint aIndex, TItem& aItem, TTextAttribute& aTextAttribute) const
/**
 * This method is an overloaded method of RSat::TMenuItemBaseV1::GetItem(TUint aIndex, TItem& aItem).
 * It returns the menu item's text attribute
 * (if available) as well as the requested menu item.
 * RSat::TMenuItemBaseV1::MenuContent() tells you whether a menu's items are
 * accompanied by text attributes.
 *
 * @param aIndex Index of the menu item. Index range starts at 1.
 *        RSat::TMenuItemBaseV1::NumberOfItems() returns the total
 *        number of items.
 * @param aItem Item from the menu, designated by aIndex.
 * @param aTextAttribute Text attribute of the item at aIndex in the menu.
 * @return KErrNotFound returned if the menu contains no items or aIndex is out of range.
 *         KErrCorrupt returned if the buffer is corrupted and the item can't be found.
 */
	{
	// Check if aIndex is in the valid range.
	// Also check that iBuf is not empty, in which case there are no menu items stored.
	if((iBuf.Length()==0) || (aIndex<1) || (aIndex>iNumberOfItems))
		{
		return KErrNotFound;
		}
	else
		{
		TInt offset;
		// FindItemInBuffer modifies offset to return the position of the requested
		// menu item within iBuf.
		if (FindItemInBuffer(aIndex, offset)==KErrCorrupt)
			{
			return KErrCorrupt;
			}

		// The offset should be pointing at one of the item headers now.
		if (((iBuf[offset]&KUpperByte)!=KItemHeader) &&
			(iBuf[offset]&KUpperByte)!=KItemWithTextAttributeHeader)
			{
			return KErrCorrupt;
			}
		else
			{
			// The item ID is a TUint8, which has been stored in a unicode buffer.
			// Before casting it back to a TUint8 we check that the upper byte
			// is zero.
			// If it is different from 0, then there has been a data corruption
			// since only headers have any content in the upper byte.
			if((iBuf[offset+KHeaderLength]&KUpperByte)!=0)
				{
				return KErrCorrupt;
				}
			else
				{
				TUint8 textAttributeOffset = 0;
				if ((iBuf[offset]&KUpperByte)==KItemWithTextAttributeHeader)
					{
					textAttributeOffset = KTextAttributeBufSize;
					aTextAttribute.iStatus = ETextAttributeProvided;
					aTextAttribute.iTextAttributeData.SetLength(KTextAttributeBufSize);
					for (TUint8 i = 0; i < KTextAttributeBufSize; i++)
						{
						aTextAttribute.iTextAttributeData[i] = iBuf[offset+KHeaderLength+KItemIdLength+i];
						}
					}
				else
					{
					aTextAttribute.iStatus = ETextAttributeNotPresent;
					}
				aItem.iItemId=static_cast<TUint8>(iBuf[offset+KHeaderLength]);
				aItem.iItemString=iBuf.Mid(offset+KHeaderLength+KItemIdLength+textAttributeOffset,
					((iBuf[offset]&KLowerByte)-KItemIdLength-textAttributeOffset));

				return KErrNone;
				}
			}
		}
	}

EXPORT_C TInt RSat::TMenuItemBaseV5::GetItem (TUint aIndex, TItem& aItem, TTextAttribute& aTextAttribute, TInt& aAction) const
/**
 * This method is an overloaded method of RSat::TMenuItemBaseV1::GetItem(TUint aIndex, TItem& aItem).
 * It returns the menu item's text attribute
 * (if available) as well as the requested menu item.
 * RSat::TMenuItemBaseV1::MenuContent() tells you whether a menu's items are
 * accompanied by text attributes and item next actions.
 *
 * @param aIndex Index of the menu item. Index range starts at 1.
 *        RSat::TMenuItemBaseV1::NumberOfItems() returns the total
 *        number of items.
 * @param aItem Item from the menu, designated by aIndex.
 * @param aTextAttribute Text attribute of the item at aIndex in the menu.
 * @param aAction Action associated with the item.
 * @return KErrNotFound returned if the menu contains no items or aIndex is out of range.
 *         KErrCorrupt returned if the buffer is corrupted and the item can't be found.
 */
	{
	// Check if aIndex is in the valid range.
	// Also check that iBuf is not empty, in which case there are no menu items stored.
	if((iBuf.Length()==0) || (aIndex<1) || (aIndex>iNumberOfItems))
		{
		return KErrNotFound;
		}
	else
		{
		TInt offset;
		// FindItemInBuffer modifies offset to return the position of the requested
		// menu item within iBuf.
		if (FindItemInBuffer(aIndex, offset)==KErrCorrupt)
			{
			return KErrCorrupt;
			}

		// The offset should be pointing at one of the item headers now.
		if (((iBuf[offset]&KUpperByte)!=KItemHeader) &&
			(iBuf[offset]&KUpperByte)!=KItemWithTextAttributeHeader)
			{
			return KErrCorrupt;
			}
		else
			{
			// The item ID is a TUint8, which has been stored in a unicode buffer.
			// Before casting it back to a TUint8 we check that the upper byte
			// is zero.
			// If it is different from 0, then there has been a data corruption
			// since only headers have any content in the upper byte.
			if((iBuf[offset+KHeaderLength]&KUpperByte)!=0)
				{
				return KErrCorrupt;
				}
			else
				{
				TUint8 textAttributeOffset = 0;
				if ((iBuf[offset]&KUpperByte)==KItemWithTextAttributeHeader)
					{
					textAttributeOffset = KTextAttributeBufSize;
					aTextAttribute.iStatus = ETextAttributeProvided;
					aTextAttribute.iTextAttributeData.SetLength(KTextAttributeBufSize);
					for (TUint8 i = 0; i < KTextAttributeBufSize; i++)
						{
						aTextAttribute.iTextAttributeData[i] = iBuf[offset+KHeaderLength+KItemIdLength+i];
						}
					}
				else
					{
					aTextAttribute.iStatus = ETextAttributeNotPresent;
					}
				aItem.iItemId=static_cast<TUint8>(iBuf[offset+KHeaderLength]);
				aItem.iItemString=iBuf.Mid(offset+KHeaderLength+KItemIdLength+textAttributeOffset,
					((iBuf[offset]&KLowerByte)-KItemIdLength-textAttributeOffset));

				// Make offset point past the item string
				offset+=KHeaderLength+(iBuf[offset]&KLowerByte);

				if ((offset+KActionLength)>iBuf.Length())
					{
					aAction=KErrNotFound;
					}
				else
					{
					if((iBuf[offset]&KUpperByte)!=KActionHeader)
						{
						aAction=KErrNotFound;
						}
					else
						{
						aAction=iBuf[offset+KHeaderLength];
						}
					}

				return KErrNone;
				}
			}
		}
	}

EXPORT_C TInt RSat::TMenuItemBaseV5::GetItemIcon (TUint aIndex, TItem& aItem, TTextAttribute& aTextAttribute, TInt& aIconId) const
/**
 * This method is an overloaded method of RSat::TMenuItemBaseV1::GetItem(TUint aIndex, TItem& aItem).
 * It returns the menu item's text attribute
 * (if available) as well as the requested menu item.
 * RSat::TMenuItemBaseV1::MenuContent() tells you whether a menu's items are
 * accompanied by text attributes and item next actions.
 *
 * @param aIndex Index of the menu item. Index range starts at 1.
 *        RSat::TMenuItemBaseV1::NumberOfItems() returns the total
 *        number of items.
 * @param aItem Item from the menu, designated by aIndex.
 * @param aTextAttribute Text attribute of the item at aIndex in the menu.
 * @param aAction Action associated with the item.
 * @return KErrNotFound returned if the menu contains no items or aIndex is out of range.
 *         KErrCorrupt returned if the buffer is corrupted and the item can't be found.
 */
	{
	if((iBuf.Length()==0) || (aIndex<1) || (aIndex>iNumberOfItems))
		{
		return KErrNotFound;
		}
	else
		{
		TInt offset;
		// FindItemInBuffer modifies offset to return the position of the requested
		// menu item within iBuf.
		if (FindItemInBuffer(aIndex,offset)==KErrCorrupt)
			{
			return KErrCorrupt;
			}

        if (((iBuf[offset]&KUpperByte)!=KItemHeader) &&
			(iBuf[offset]&KUpperByte)!=KItemWithTextAttributeHeader)
			{
			return KErrCorrupt;
			}
		else
			{
			// Only headers should have the upper byte set, so there shouldn't
			// be anything in the upper byte after the header.
			if((iBuf[offset+KHeaderLength]&KUpperByte)!=0)
				{
				return KErrCorrupt;
				}
			else
				{
				TUint8 textAttributeOffset = 0;
				if ((iBuf[offset]&KUpperByte)==KItemWithTextAttributeHeader)
					{
					textAttributeOffset = KTextAttributeBufSize;
					aTextAttribute.iStatus = ETextAttributeProvided;
					aTextAttribute.iTextAttributeData.SetLength(KTextAttributeBufSize);
					for (TUint8 i = 0; i < KTextAttributeBufSize; i++)
						{
						aTextAttribute.iTextAttributeData[i] = iBuf[offset+KHeaderLength+KItemIdLength+i];
						}
					}
				else
					{
					aTextAttribute.iStatus = ETextAttributeNotPresent;
					}

				aItem.iItemId=static_cast<TUint8>(iBuf[offset+KHeaderLength]);
				aItem.iItemString=iBuf.Mid(offset+KHeaderLength+KItemIdLength+textAttributeOffset,
					((iBuf[offset]&KLowerByte)-KItemIdLength-textAttributeOffset));

				offset+=KHeaderLength+(iBuf[offset]&KLowerByte);
				if ((offset+KActionLength)>iBuf.Length())
					{
					// There is no item icon ID associated with the requested item
					// and offset has reached the end of iBuf.
					aIconId=KErrNotFound;
					}
				else
					{
					if ((iBuf[offset]&KUpperByte)==KActionHeader)
						{
						offset+=KHeaderLength+(iBuf[offset]&KLowerByte);
						}
 					if ((offset+KIconIdLength)>iBuf.Length())
						{
						aIconId=KErrNotFound;
						}
					else
						{
						if((iBuf[offset]&KUpperByte)!=KIconIdHeader)
							{
							// There is no item icon ID associated with the requested item
							aIconId=KErrNotFound;
							}
						else
							{
							aIconId=iBuf[offset+KHeaderLength];
							}
						}
					}
				return KErrNone;
				}
			}
		}
	}

EXPORT_C TInt RSat::TMenuItemBaseV5::GetItem (TUint aIndex, TItem& aItem, TTextAttribute& aTextAttribute, TInt& aAction, TInt& aIconId) const
/**
 * This method is an overloaded method of RSat::TMenuItemBaseV1::GetItem(TUint aIndex, TItem& aItem).
 * It returns the menu item's text attribute, item next action and icon if
 * they are available as well as the requested menu item.
 * RSat::TMenuItemBaseV1::MenuContent() tells you whether a menu's items are
 * accompanied by text attributes, item next actions, and icons.
 *
 * @param aIndex Index of the menu item. Index range starts at 1.
 *        RSat::TMenuItemBaseV1::NumberOfItems() returns the total
 *        number of items.
 * @param aItem Item from the menu, designated by aIndex.
 * @param aTextAttribute Text attribute of the item at aIndex in the menu.
 * @param aIcon Icon associated with the item.
 * @param aAction Action associated with the item.
 * @return KErrNotFound returned if the menu contains no items or aIndex is out of range.
 *         KErrCorrupt returned if the buffer is corrupted and the item can't be found.
 *
 */
	{
	if((iBuf.Length()==0) || (aIndex<1) || (aIndex>iNumberOfItems))
		{
		return KErrNotFound;
		}
	else
		{
		TInt offset;
		// FindItemInBuffer modifies offset to return the position of the requested
		// menu item within iBuf.
		if (FindItemInBuffer(aIndex,offset)==KErrCorrupt)
			{
			return KErrCorrupt;
			}

		if (((iBuf[offset]&KUpperByte)!=KItemHeader) &&
			(iBuf[offset]&KUpperByte)!=KItemWithTextAttributeHeader)
			{
			return KErrCorrupt;
			}
		else
			{
			if((iBuf[offset+KHeaderLength]&KUpperByte)!=0)
				{
				return KErrCorrupt;
				}
			else
				{
				// textAttributeOffset indicates the extra 'jump' needed
				// to get to the item string which is four bytes
				// further into the buffer when there is a text
				// attribute present.
				TUint8 textAttributeOffset = 0;
				if ((iBuf[offset]&KUpperByte)==KItemWithTextAttributeHeader)
					{
					textAttributeOffset = KTextAttributeBufSize;
					aTextAttribute.iStatus = ETextAttributeProvided;
					aTextAttribute.iTextAttributeData.SetLength(KTextAttributeBufSize);
					for (TUint8 i = 0; i < KTextAttributeBufSize; i++)
						{
						aTextAttribute.iTextAttributeData[i] = iBuf[offset+KHeaderLength+KItemIdLength+i];
						}
					}
				else
					{
					aTextAttribute.iStatus = ETextAttributeNotPresent;
					}

				aItem.iItemId=static_cast<TUint8>(iBuf[offset+KHeaderLength]);
				aItem.iItemString=iBuf.Mid(offset+KHeaderLength+KItemIdLength+textAttributeOffset,
									((iBuf[offset]&KLowerByte)-KItemIdLength-textAttributeOffset));
				// Make offset point past the item string
				offset+=KHeaderLength+(iBuf[offset]&KLowerByte);

				if ((offset+KActionLength)>iBuf.Length())
					{
					aAction=KErrNotFound;
					aIconId=KErrNotFound;
					}
				else
					{
					if((iBuf[offset]&KUpperByte)!=KActionHeader)
						{
						aAction=KErrNotFound;
						}
					else
						{
						aAction=iBuf[offset+KHeaderLength];
						offset+=KHeaderLength+KActionLength;
						}
					if ((offset+KIconIdLength)>iBuf.Length())
						{
						aIconId=KErrNotFound;
						}
					else
						{
						if ((iBuf[offset]&KUpperByte)!=KIconIdHeader)
							aIconId=KErrNotFound;
						else
							aIconId=iBuf[offset+KHeaderLength];
						}
					}
				return KErrNone;
				}
			}
		}
	}

EXPORT_C TInt RSat::TMenuItemBaseV5::AddItem (const TItem& aItem, const TTextAttribute& aTextAttribute)
/**
 * This method is an overloaded method of AddItem(TItem aItem)
 * It adds a menu item and a text attribute to the menu.
 *
 * It is only possible to add a menu item and text attribute
 * if the menu is either empty or already contains items all
 * with text attributes only. If the existing menu items contain
 * icons or item next actions, then an icon/next action for the
 * menu item must also be specified by adding the item using
 * one of the other overloaded methods of AddItem(TItem aItem).
 * If the existing menu items do not have text attributes
 * specified, then it is not possible to specify a text attribute
 * for the new menu item and one of the other overloaded methods
 * of AddItem(TItem aItem) that does not
 * contain a TTextAttribute parameter must
 * be used.
 *
 * It is not needed by RSat clients, only the TSY.
 *
 * @param aItem Item to be added to iBuf.
 * @param aTextAttribute Item text attribute which applies to aItem to be added to iBuf.
 * @param aIconId Icon identifier, associated with aItem, to be added to iBuf.
 * @return 	KErrNoMemory returned if there is not enough space left in iBuf to store aItem and aIconId.
 *			KErrNotSupported returned if the current menu format does not support items with associated text attribute only.
 *			Otherwise, KErrNone returned.
 */
	{
	TInt header=0;

	if ((iBuf.Length()!=0)&&(iMenuContent != (KItems|KItemTextAttributes)))
		{
		return KErrNotSupported;
		}
	else if ((iBuf.Length()+KHeaderLength+KItemIdLength+KTextAttributeBufSize+aItem.iItemString.Length())>iBuf.MaxLength())
		{
		// There is not enough space left in iBuf to add an item
		return KErrNoMemory;
		}
	else
		{
		// If the menu is empty or only contains items (with no text attributes)
		// and there is enough space for the new item, a new item can be added.
		header = KItemWithTextAttributeHeader|(aItem.iItemString.Length()+KItemIdLength+KTextAttributeBufSize);
		iBuf.Append(header);
		iBuf.Append(aItem.iItemId);
		for (TUint8 i = 0; i < KTextAttributeBufSize; i++)
			{
			iBuf.Append(aTextAttribute.iTextAttributeData[i]);
			}
		iBuf.Append(aItem.iItemString);
		iMenuContent = KItems|KItemTextAttributes;
		iNumberOfItems++;
		return KErrNone;
		}
	}


EXPORT_C TInt RSat::TMenuItemBaseV5::AddItem (const TItem& aItem, const TTextAttribute& aTextAttribute, TUint aAction)
/**
 * This method is an overloaded method of AddItem(TItem aItem)
 * It adds a menu item,a text attribute and an item next action to the menu.
 *
 * It is only possible to add a menu item, text attribute and item next
 * action if the menu is either empty or already contains items ALL
 * with text attributes and item next actions only.
 * If the existing menu items contain icons, all new menu items must also
 * contain icons, therefore, this function cannot
 * be used to add items to the menu. One of the other
 * overloaded methods of AddItem(TItem aItem) which allow an icon to be
 * specified should be used.
 *
 * It is not needed by RSat clients, only the TSY.
 *
 * @param aItem Item to be added to iBuf.
 * @param aTextAttribute Item text attribute which applies to aItem to be added to iBuf.
 * @param aAction Item next action.
 * @return 	KErrNoMemory returned if there is not enough space left in iBuf to store aItem and aIconId.
 *			KErrNotSupported returned if the current menu format does not support items with associated text attribute only.
 *			Otherwise, KErrNone returned.
 */
	{
	TInt header=0;

	if ((iBuf.Length()!=0)&&(iMenuContent!=(KItems|KItemTextAttributes|KItemNextActions)))
		{
		return KErrNotSupported;
		}
	else if ((iBuf.Length()+KHeaderLength+KItemIdLength+KTextAttributeBufSize+
			aItem.iItemString.Length()+KHeaderLength+KActionLength)>iBuf.MaxLength())
		{
		// There is not enough space left in iBuf to add an item and its associated
		// next action
		return KErrNoMemory;
		}
	else
		{
		header = KItemWithTextAttributeHeader|
				(aItem.iItemString.Length()+KItemIdLength+KTextAttributeBufSize);
		iBuf.Append(header);
		iBuf.Append(aItem.iItemId);
		for (TUint8 i = 0; i < KTextAttributeBufSize; i++)
			{
			iBuf.Append(aTextAttribute.iTextAttributeData[i]);
			}
		iBuf.Append(aItem.iItemString);
		header = KActionHeader|KActionLength;
		iBuf.Append(header);
		iBuf.Append(aAction);
		iMenuContent=KItems|KItemNextActions|KItemTextAttributes;
		iNumberOfItems++;
		return KErrNone;
		}
	}

EXPORT_C TInt RSat::TMenuItemBaseV5::AddItemIcon (const TItem& aItem, const TTextAttribute& aTextAttribute, TUint aIconId)
/**
 * This method is an overloaded method of
 * RSat::TMenuItemBaseV1::AddItemIcon(TItem aItem)
 * It adds a menu item,a text attribute and an item next action to the menu.
 *
 * It is only possible to add a menu item, text attribute and item icon
 * if the menu is either empty or already contains items ALL
 * with text attributes and item icons only.
 * If the existing menu items contain item next actions, all new menu items
 * must also contain icons, therefore, this function cannot
 * be used to add items to the menu. One of the other
 * overloaded methods of AddItem(TItem aItem) which allow an item next
 * action to be specified should be used instead.
 *
 * It is not needed by RSat clients, only the TSY.
 *
 * @param aItem Item to be added to iBuf.
 * @param aTextAttribute Item text attribute which applies to aItem to be added to iBuf.
 * @param aIcon Item icon to be added to iBuf.
 * @return 	KErrNoMemory returned if there is not enough space left in iBuf to store aItem and aIconId.
 *			KErrNotSupported returned if the current menu format does not support items with associated text attribute only.
 *			Otherwise, KErrNone returned.
 */
	{
	TInt header=0;

	if ((iBuf.Length()!=0)&&(iMenuContent!=(KItems|KItemTextAttributes|KItemIconIds)))
		{
		return KErrNotSupported;
		}
	else if ((iBuf.Length()+KHeaderLength+KItemIdLength+KTextAttributeBufSize+aItem.iItemString.Length()
		+KHeaderLength+KIconIdLength)>iBuf.MaxLength())
		{
		// There is not enough space left in iBuf to add an item and its associated
		// icon identifier.
		return KErrNoMemory;
		}
	else
		{
		header=KItemWithTextAttributeHeader|(aItem.iItemString.Length()+KTextAttributeBufSize+KItemIdLength);
		iBuf.Append(header);
		iBuf.Append(aItem.iItemId);
		for (TUint8 i = 0; i < KTextAttributeBufSize; i++)
			{
			iBuf.Append(aTextAttribute.iTextAttributeData[i]);
			}
		iBuf.Append(aItem.iItemString);
		header=KIconIdHeader|KIconIdLength;
		iBuf.Append(header);
		iBuf.Append(aIconId);
		iMenuContent=KItems|KItemIconIds|KItemTextAttributes;
		iNumberOfItems++;
		return KErrNone;
		}
	}

EXPORT_C TInt RSat::TMenuItemBaseV5::AddItem (const TItem& aItem, const TTextAttribute& aTextAttribute, TUint aAction, TUint aIconId)
/**
 * This method is an overloaded method of AddItem(TItem aItem)
 * It adds a menu item,a text attribute and an item next action
 * and icon to the menu.
 *
 * It is only possible to add a menu item with all the associated
 * information if the menu is either empty or already contains
 * items ALL of which also contain text attributes, item icons,
 * and item next actions.
 * If the existing menu items do not contain all of these,
 * this function cannot be used to add items to the menu.
 * One of the other overloaded methods of AddItem(TItem aItem)
 * which allow an item next action to be specified should be
 * used instead.
 *
 * It is not needed by RSat clients, only the TSY.
 *
 * @param aItem Item to be added to iBuf.
 * @param aTextAttribute Item text attribute which applies to aItem to be added to iBuf.
 * @param aIcon Item icon to be added to iBuf.
 * @param aAction Item next action to be added to iBuf.
 * @return 	KErrNoMemory returned if there is not enough space left in iBuf to store aItem and aIconId.
 *			KErrNotSupported returned if the current menu format does not support items with associated text attribute only.
 *			Otherwise, KErrNone returned.
 */
	{
	TInt header=0;
	if ((iBuf.Length()!=0)&&(iMenuContent!=(KItems|KItemNextActions|KItemIconIds|KItemTextAttributes)))
		{
		return KErrNotSupported;
		}
	else if ((iBuf.Length()+3*(KHeaderLength)+KItemIdLength+KTextAttributeBufSize+aItem.iItemString.Length()
		+KActionLength+KIconIdLength)>iBuf.MaxLength())
		{
		// There is not enough space left in iBuf to add an item and its associated
		// icon identifier and next action.
		return KErrNoMemory;
		}
	else
		{
		header = KItemWithTextAttributeHeader|(aItem.iItemString.Length()+KTextAttributeBufSize+KItemIdLength);
		iBuf.Append(header);
		iBuf.Append(aItem.iItemId);
		for (TUint8 i = 0; i < KTextAttributeBufSize; i++)
			{
			iBuf.Append(aTextAttribute.iTextAttributeData[i]);
			}
		iBuf.Append(aItem.iItemString);
		header = KActionHeader|KActionLength;
		iBuf.Append(header);
		iBuf.Append(aAction);
		header = KIconIdHeader|KIconIdLength;
		iBuf.Append(header);
		iBuf.Append(aIconId);
		iMenuContent=KItems|KItemNextActions|KItemIconIds|KItemTextAttributes;
		iNumberOfItems++;
		return KErrNone;
		}
	}

EXPORT_C RSat::TSetUpMenuV1::TSetUpMenuV1()
	: TMenuItemBaseV1(),
	  iHelp(EHelpNotSet),
	  iIconListQualifier(EIconQualifierNotSet)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	iAlphaId.iStatus=EAlphaIdNotSet;
	iIconId.iQualifier=EIconQualifierNotSet;
	iIconId.iIdentifier=0;
	}

EXPORT_C RSat::TSetUpMenuV2::TSetUpMenuV2()
	:TSetUpMenuV1(),
	iPreference(ESelectionPreferenceNotSet)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	}

EXPORT_C RSat::TSetUpMenuV5::TSetUpMenuV5()
	:TMenuItemBaseV5()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV5;
	iAlphaId.iStatus=EAlphaIdNotSet;
	iHelp=EHelpNotSet;
	iIconId.iQualifier=EIconQualifierNotSet;
	iIconId.iIdentifier=0;
	iIconListQualifier=EIconQualifierNotSet;
	iPreference=ESelectionPreferenceNotSet;
	iTextAttribute.iStatus=ETextAttributeNotSet;
	}

EXPORT_C RSat::TSetUpMenuRspV1::TSetUpMenuRspV1()
	: TSatPCmdBase(),
	iGeneralResult(KPCmdResultNotSet),
	iInfoType(0),
	iAdditionalInfo()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	}

EXPORT_C void RSat::NotifySetUpMenuPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a SET UP MENU proactive command is sent
 * by the UICC.  When the request completes, the command is placed into aPCmd and the
 * aReqStatus is competed.  Unpack the command to examine its contents.
 *
 * The SET UP MENU command provides a set of menu items to be integrated with the
 * phone's menu system. The user can choose one of these menu items at their own
 * discretion.  The command is described fully in ETSI TS 102 223.  Briefly,
 * though, the command includes:
 *
 * - The items in the menu, in a RSat::TMenuItemBaseV1
 * - A title for the menu in an alpha identifier - RSat::TSetUpMenuV1::iAlphaId
 * - An icon that represents the menu - RSat::TSetUpMenuV1::iIconId
 * - Determines whether help is available - RSat::TSetUpMenuV1::iHelp
 * - Determines whether soft key is used with the menu - RSat::TSetUpMenuV2::iPreference (Symbian OS v7.0s and later.)
 * - Text attributes of text contained in iAlphaId - RSat::TSetUpMenuV5::iTextAttribute (Symbian OS v9.2 or later)
 *
 * Once you have integrated the menu (but before the user has made a selection),
 * you must give your response to the SET UP MENU command.  For Symbian OS
 * versions up to and including  v8.1a and v8.1b pass RSat::TerminalRsp() a
 * RSat::TSetUpMenuRspV1Pckg (a packaged TGetInputRspV1).  For later versions of
 * Symbian OS, check for an updated API (RSat::TSetUpMenuRspV2 etc).
 *
 * If the user chooses a menu item then inform the UICC with
 * RSat::MenuSelection().  This includes the identifier of the chosen menu item.
 *
 * You may receive a SET UP MENU command with no menu!  This means that the menu
 * from the previous SET UP MENU command should be removed; see
 * RSat::TMenuItemBaseV1::IsRemoveMenu().
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.      Use a packaged version of the latest "TSetUpMenu" class. For
 * Symbian OS 7.0s, 8.0 and 8.1 use RSat::TSetUpMenuV2Pckg, a packaged
 * RSat::TSetUpMenuV2. For Symbian OS version 9.2, use RSat::TSetUpMenuV5.
 * Use RSat::TSetUpMenuV1 with some older versions of Symbian
 * OS.  For later versions of Symbian OS, check for an updated API
 * (RSat::TSetUpMenuV6 etc).
 *
 * @see RSat::TSetUpMenuV1
 * @see RSat::TSetUpMenuV2
 * @see RSat::TSetUpMenuV5
 * @see RSat::TMenuItemBaseV1
 * @see RSat::TMenuItemBaseV5
 * @see RSat::TSetUpMenuRspV1
 * @see RSat::MenuSelection()
 * @capability None
 */
	{
	Get(ESatNotifySetUpMenuPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TSelectItemV1::TSelectItemV1()
	: TMenuItemBaseV1(),
	  iPresentationType(EPresentationTypeNotSet),
	  iHelp(EHelpNotSet),
	  iDefaultItemId(0),
	  iIconListQualifier(EIconQualifierNotSet)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	iAlphaId.iStatus=EAlphaIdNotSet;
	iIconId.iQualifier=EIconQualifierNotSet;
	iIconId.iIdentifier=0;
	}

EXPORT_C RSat::TSelectItemV2::TSelectItemV2()
	: TSelectItemV1(),
	  iPreference(ESelectionPreferenceNotSet)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	}

EXPORT_C RSat::TSelectItemV5::TSelectItemV5()
	: TMenuItemBaseV5()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV5;
	iPresentationType=EPresentationTypeNotSet;
	iAlphaId.iStatus=EAlphaIdNotSet;
	iHelp=EHelpNotSet;
	iDefaultItemId=0;
	iIconId.iQualifier=EIconQualifierNotSet;
	iIconId.iIdentifier=0;
	iIconListQualifier=EIconQualifierNotSet;
	iPreference=ESelectionPreferenceNotSet;
	iTextAttribute.iStatus=ETextAttributeNotSet;
	}

EXPORT_C RSat::TSelectItemV6::TSelectItemV6()
	: TSelectItemV5(),iFrameId(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV6;
	}
	
EXPORT_C RSat::TSelectItemRspV1::TSelectItemRspV1()
	: TSatPCmdBase(),
	iGeneralResult(KPCmdResultNotSet),
	iInfoType(0),
	iAdditionalInfo()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	}

EXPORT_C void RSat::NotifySelectItemPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a SELECT ITEM proactive command is sent
 * by the UICC.  When the request completes, the command is placed into aPCmd and the
 * aReqStatus is competed.  Unpack the command to examine its contents.
 *
 * The SELECT ITEM command provides a set of menu items to be shown to the user.
 * The user can choose one of these menu items. The command is described fully in
 * ETSI TS 102 223.  Briefly, though, the command includes:
 *
 * - The items in the menu, in a RSat::TMenuItemBaseV1
 * - A title for the menu in an alpha identifier - RSat::TSelectItemV1::iAlphaId
 * - An icon that represents the menu - RSat::TSelectItemV1::iIconId
 * - A default menu selection -  RSat::TSelectItemV1::iDefaultItemId
 * - Determines whether help is available - RSat::TSelectItemV1::iHelp
 * - Determines whether soft key is used with the menu - RSat::TSelectItemV2::iPreference (Symbian OS v7.0s and later.)
 * - Text attributes of text contained in iAlphaId - RSat::TSelectItemV5::iTextAttribute (Symbian OS v9.2 or later)
 *
 * Once you have integrated the menu (but before the user has made a selection),
 * you must give your response to the SELECT ITEM command.  For Symbian OS
 * versions up to and including  v8.1a and v8.1b pass RSat::TerminalRsp() a
 * RSat::TSelectItemRspV1Pckg (a packaged TGetInputRspV1).  For later versions of
 * Symbian OS, check for an updated API (RSat::TSelectItemRspV2 etc).
 *
 * If the user chooses a menu item then inform the UICC with
 * RSat::MenuSelection().  This includes the identifier of the chosen menu item.
 *
 * You may receive a SELECT ITEM command with no menu!  This means that the menu
 * from the previous SELECT ITEM command should be removed; see
 * RSat::TMenuItemBaseV1::IsRemoveMenu().
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.  Use a packaged version of the latest "TSelectItem" class. For
 * Symbian OS 7.0s, 8.0 and 8.1 use RSat::TSelectItemV2Pckg, a packaged
 * RSat::TSelectItemV2. 
 * For Symbian OS version 9.2, use RSat::TDisplayTextV5Pckg.
 * For Symbian OS version 9.4, use RSat::TDisplayTextV6Pckg.
 * Use RSat::TSelectItemV1 with some older versions of Symbian
 * OS.  For later versions of Symbian OS, check for an updated API
 * (RSat::TSelectItemV7 etc).
 *
 * @see RSat::TSelectItemV1
 * @see RSat::TSelectItemV2
 * @see RSat::TSelectItemV5
 * @see RSat::TSelectItemV6
 * @see RSat::TMenuItemBaseV1
 * @see RSat::TMenuItemBaseV5
 * @see RSat::TSelectItemRspV1
 * @see RSat::MenuSelection()
 * @capability None
 */
       {
	Get(ESatNotifySelectItemPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TSendSmV1::TSendSmV1()
	: TSatPCmdBase(),
	  iSmsTpdu()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	iAlphaId.iStatus=EAlphaIdNotSet;
	iAddress.iTypeOfNumber=ETypeOfNumberNotSet;
	iAddress.iNumberPlan=ENumberingPlanNotSet;
	iIconId.iQualifier=EIconQualifierNotSet;
	iIconId.iIdentifier=0;
	}

EXPORT_C RSat::TSendSmV5::TSendSmV5()
	: TSendSmV1()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV5;
	iTextAttribute.iStatus = ETextAttributeNotSet;
	}

EXPORT_C RSat::TSendSmV6::TSendSmV6()
	: TSendSmV5(),iFrameId(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV6;
	}
	
EXPORT_C RSat::TSendSmRspV1::TSendSmRspV1()
	: TSatPCmdBase(),
	iGeneralResult(KPCmdResultNotSet),
	iInfoType(0),
	iAdditionalInfo()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	}

EXPORT_C void RSat::NotifySendSmPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a SEND SHORT MESSAGE  proactive command is sent
 * by the UICC.  When the request completes, the command is placed into aPCmd and the
 * aReqStatus is competed.  Unpack the command to examine its contents.
 *
 * The SEND SHORT MESSAGE command asks the phone to send an SMS
 * message. Return the response to this request to the UICC.
 *
 * The command is described in ETSI TS 102 223, with additional information for
 * GSM/WCDMA networks in 3GPP TS 31.111, and for CDMA networks in 3GPP2 C.S0035-0.
 * The command contains:
 *
 * - The address of the message recipient - RSat::TSendSmV1::iAddress.
 * - The SMS message itself - RSat::TSendSmV1::iSmsTpdu.
 * - An alpha identifier containing text to display - RSat::TSendSmV1::iAlphaId.
 * - An icon to display - RSat::TSendSmV1::iIconId
 * - Text attributes of text contained in iAlphaId - RSat::TSendSmV5::iTextAttribute (Symbian OS v9.2 or later) 
 *
 * Display the icon as described in RSat::TIconId. Display the text in the alpha
 * identifier, if there is any.  If the alpha identifier is not present in the
 * proactive command then you can display whatever you like while sending the
 * message.
 *
 * However, if the alpha identifier is present in the proactive command but it is an
 * empty string then do not display anything: just send the message without letting the
 * user know.  To help with this, send the message by calling
 * RSat::SendMessageNoLogging() instead of the usual
 * RMobileSmsMessaging::SendMessage().  This avoids the message being logged.
 *
 * Return the result of the SMS or an error code to the UICC.  For Symbian
 * OS versions up to and including v8.1a and 8.1b pass RSat::TerminalRsp() a
 * RSat::TSendSsRspV1Pckg (a packaged TSendSsRspV1).   For later versions of
 * Symbian OS, check for an updated API (RSat::TSendSsRspV2 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.  aPCmd must be a packaged version of the  latest "TSendSm" class.
 * For Symbian OS versions up to and including v8.1a and 8.1b use
 * RSat::TSendSmV1Pckg, a packaged RSat::TSendSmV1.  
 * For Symbian OS version 9.2, use RSat::TSendSmV5Pckg.
 * For Symbian OS version 9.4, use RSat::TSendSmV6Pckg.
 * For later versions of Symbian OS, check for an updated API (RSat::TSendSmV7 etc).
 *
 * @see RSat::TSendSmV1
 * @see RSat::TSendSmV5
 * @see RSat::TSendSmV6
 * @see RSat::TSendSmRspV1
 *
 * @capability ReadDeviceData
 */
	{
	Get(ESatNotifySendSmPCmd,aReqStatus,aPCmd);
	}


EXPORT_C RSat::TSendSsV1::TSendSsV1(): TSatPCmdBase()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	iAlphaId.iStatus=EAlphaIdNotSet;
	iSsString.iTypeOfNumber=ETypeOfNumberNotSet;
	iSsString.iNumberPlan=ENumberingPlanNotSet;
	iIconId.iQualifier=EIconQualifierNotSet;
	iIconId.iIdentifier=0;
	}

EXPORT_C RSat::TSendSsV6::TSendSsV6()
	: TSendSsV1()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV6;
	iTextAttribute.iStatus =ETextAttributeNotSet;
	}
	
EXPORT_C RSat::TSendSsRspV1::TSendSsRspV1()
	: TSatPCmdBase(),
	iGeneralResult(KPCmdResultNotSet),
	iInfoType(0),
	iAdditionalInfo(),
	iGeneralResult2(KPCmdResultNotSet),
	iInfoType2(0),
	iAdditionalInfo2(),
	iCcRequestedAction()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	}

EXPORT_C RSat::TSendSsRspV2::TSendSsRspV2()
	: TSendSsRspV1(),
	  iUssdString()
/**
* Default constructor.
*/
	{
	iUssdString.iDcs=0;
	iExtensionId=KSatV2;
	}

EXPORT_C void RSat::NotifySendSsPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a SEND SS proactive command is sent
 * by the UICC.  When the request completes, the command is placed into aPCmd and the
 * aReqStatus is competed.  Unpack the command to examine its contents.
 *
 * This command is only supported on GSM/WCDMA networks.  It can be ignored on
 * CDMA networks.
 *
 * The SEND SS command asks the phone to send a supplementary service request to
 * the network.  Return the response to this request to the UICC.
 *
 * The command is described fully in 3GPP TS 31.111 but does not appear in ETSI TS
 * 102 223.  It contains:
 *
 * - The supplementary services string - RSat::TSendSsV1::iSsString.
 * - An alpha identifier containing text to display - RSat::TSendSsV1::iAlphaId.
 * - An icon to display - RSat::TSendSsV1::iIconId
 *
 * See the description of the SEND SS command in 3GPP TS 31.111 for the
 * circumstances under which the alpha identifier's text should be displayed.
 * Display the icon as described in RSat::TIconId.
 *
 * Return the result of the SS request or an error code to the UICC.  For Symbian
 * OS versions up to and including v8.1a and 8.1b pass RSat::TerminalRsp() a
 * RSat::TSendSsRspV1Pckg (a packaged TSendSsRspV1).   For later versions of
 * Symbian OS, check for an updated API (RSat::TSendSsRspV2 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.  aPCmd must be a packaged version of the  latest "TSendSs" class.
 * For Symbian OS versions up to and including v8.1a and 8.1b use
 * RSat::TSendSsV1Pckg, a packaged RSat::TSendSsV1.  
 * For Symbian OS version 9.4, use RSat::TSendSmV6Pckg.
 * For later versions of Symbian OS, check for an updated API (RSat::TSendSsV7 etc).
 *
 * @see RSat::TSendSsV1
 * @see RSat::TSendSsV6
 * @see RSat::TSendSsRspV1
 *
 * @capability ReadDeviceData
 */
	{
	Get(ESatNotifySendSsPCmd,aReqStatus,aPCmd);
	}


EXPORT_C RSat::TSendUssdV1::TSendUssdV1()
	: TSatPCmdBase()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	iAlphaId.iStatus=EAlphaIdNotSet;
	iUssdString.iDcs=0;
	iIconId.iQualifier=EIconQualifierNotSet;
	iIconId.iIdentifier=0;
	}

EXPORT_C RSat::TSendUssdV6::TSendUssdV6()
	: TSendUssdV1()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV6;
	iTextAttribute.iStatus = ETextAttributeNotSet;
	}
	
EXPORT_C RSat::TSendUssdRspV1::TSendUssdRspV1()
	: TSatPCmdBase(),
	iGeneralResult(KPCmdResultNotSet),
	iInfoType(0),
	iAdditionalInfo(),
	iGeneralResult2(KPCmdResultNotSet),
	iInfoType2(0),
	iAdditionalInfo2(),
	iCcRequestedAction()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	iUssdString.iDcs=0;
	}

EXPORT_C void RSat::NotifySendUssdPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a SEND USSD proactive command is sent
 * by the UICC.  When the request completes, the command is placed into aPCmd and the
 * aReqStatus is competed.  Unpack the command to examine its contents.
 *
 * This command is only supported on GSM/WCDMA networks.  It can be ignored on
 * CDMA networks.
 *
 * The SEND USSD command asks the phone to send an Unstructured Supplementary
 * Service Data request to the network.  Return the response to this request to
 * the UICC.
 *
 * The command is described fully in 3GPP TS 31.111. Note that it does not appear in ETSI TS
 * 102 223.  It contains:
 *
 * - The Unstructured Supplementary Service Data (USSD) string - RSat::TSendUssdV1::iUssdString.
 * - An alpha identifier containing text to display - RSat::TSendUssdV1::iAlphaId.
 * - An icon to display - RSat::TSendUssdV1::iIconId
 *
 * See the description of the SEND USSD command in 3GPP TS 31.111 for the
 * circumstances under which the alpha identifier's text should be displayed.
 * Display the icon as described in RSat::TIconId.
 *
 * Return the result of the USSD request or an error code to the UICC.  For Symbian
 * OS versions up to and including v8.1a and 8.1b pass RSat::TerminalRsp() a
 * RSat::TSendUssdRspV1Pckg (a packaged TSendUssdRspV1).   For later versions of
 * Symbian OS, check for an updated API (RSat::TSendUssdRspV2 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.  aPCmd must be a packaged version of the  latest "TSendUssd" class.
 * For Symbian OS versions up to and including v8.1a and 8.1b use
 * RSat::TSendUssdV1Pckg, a packaged RSat::TSendUssdV1. 
 * For Symbian OS versions 9.4 and above use
 * RSat::TSendUssdV6Pckg, a packaged RSat::TSendUssdV6. For later versions of
 * Symbian OS, check for an updated API (RSat::TSendUssdV7 etc).
 *
 * @see RSat::TSendUssdV1
 * @see RSat::TSendUssdV6
 * @see RSat::TSendUssdRspV1
 *
 * @capability ReadDeviceData
 */
	{
	Get(ESatNotifySendUssdPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TSetUpCallV1::TSetUpCallV1()
	: TSatPCmdBase(),
	  iType(ESetUpCallTypeNotSet),
	  iCapabilityConfigParams(),
	  iSubAddress()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	iDuration.iTimeUnit=ETimeUnitNotSet;
	iDuration.iNumOfUnits=0;
	iAlphaIdConfirmationPhase.iStatus=EAlphaIdNotSet;
	iAlphaIdCallSetUpPhase.iStatus=EAlphaIdNotSet;
	iAddress.iTypeOfNumber=ETypeOfNumberNotSet;
	iAddress.iNumberPlan=ENumberingPlanNotSet;
	iIconIdConfirmationPhase.iQualifier=EIconQualifierNotSet;
	iIconIdConfirmationPhase.iIdentifier=0;
	iIconIdCallSetUpPhase.iQualifier=EIconQualifierNotSet;
	iIconIdCallSetUpPhase.iIdentifier=0;
	}

EXPORT_C RSat::TSetUpCallV5::TSetUpCallV5()
	: TSetUpCallV1()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV5;
	iTextAttributeConfirmationPhase.iStatus = ETextAttributeNotSet;
	iTextAttributeCallSetUpPhase.iStatus = ETextAttributeNotSet;
	}

EXPORT_C RSat::TSetUpCallV6::TSetUpCallV6()
	: TSetUpCallV5(),iFrameId(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV6;
	}
	
EXPORT_C RSat::TSetUpCallRspV1::TSetUpCallRspV1()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iAdditionalInfo(),
  	  iGeneralResult2(KPCmdResultNotSet),
	  iInfoType2(0),
	  iAdditionalInfo2(),
	  iCcRequestedAction()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	}

EXPORT_C RSat::TSetUpCallRspV2::TSetUpCallRspV2()
	: TSetUpCallRspV1(),
	  iUssdString()
/**
* Default constructor.
*/
	{
	iUssdString.iDcs=0;
	iExtensionId=KSatV2;
	}

EXPORT_C void RSat::NotifySetUpCallPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a SET UP CALL proactive command is sent
 * by the UICC.  When the request completes, the command is placed into aPCmd and the
 * aReqStatus is competed.  Unpack the command to examine its contents.
 *
 * The SET UP CALL command asks you to connect a phone call.   The
 * command is described fully in ETSI TS 102 223.  Briefly, though, the command
 * includes:
 *
 * - The number to call - RSat::TSetUpCallV1::iAddress
 * - The sub-address of the remote party - RSat::TSetUpCallV1::iSubAddress
 * - Capability configuration parameters - RSat::TSetUpCallV1::iCapabilityConfigParams
 * - The procedure to follow if there is already a call in progress - RSat::TSetUpCallV1::iType
 * - Determines whether you re-dial if initial attempts to make the call fail - RSat::TSetUpCallV1::iType
 * - Detemines how long you should you try to re-dial - RSat::TSetUpCallV1::iDuration
 * - An icon and some text for the confirmation phase - RSat::TSetUpCallV1::iAlphaIdConfirmationPhase and RSat::TSetUpCallV1::iIconIdConfirmationPhase
 * - An icon and some text for the 'call set up' phase - RSat::TSetUpCallV1::iAlphaIdCallSetUpPhase and RSat::TSetUpCallV1::iIconIdCallSetUpPhase
 * - Text attribute information for RSat::TSetUpCallV1::iAlphaIdConfirmationPhase and 
 * RSat::TSetUpCallV1::iAlphaIdCallSetUpPhase stored in 
 * RSat::TSetUpCallV5::iTextAttributeCallSetUpPhase and 
 * RSat::TSetUpCallV5::iTextAttributeConfirmationPhase respectively. (Symbian OS v9.2 and later)
 *
 * After receiving the command, first look at RSat::TSetUpCallV1::iType to decide
 * whether to make the call.  Then you need to ask the user's permission to call.  This is
 * the confirmation phase.  Display the confirmation alpha ID and icon as
 * described in ETSI TS 102 223.
 *
 * If the user gives permission, you can dial the number and attempt to connect
 * the call.  This is the 'call set up' phase. Display the call setup alpha ID and
 * icon as described in ETSI TS 102 223.
 *
 * If the call could not be connected, look at RSat::TSetUpCallV1::iType to see
 * whether you should try to dial again.  Do not re-dial for longer than
 * RSat::TSetUpCallV1::iDuration.
 *
 * Once the call has connected, or if the call could not be made and you have
 * finished re-dialling, you must give your response to the SET UP CALL command.
 * For Symbian OS v6.1 or v7.0 pass RSat::TerminalRsp() a
 * RSat::TSetUpCallRspV1Pckg (a packaged TSetUpCallRspV1).  For versions up to and
 * including v8.1a and v8.1b use RSat::TSetUpCallRspV2Pckg (a packaged
 * RSat::TSetUpCallRspV2).  For later versions of Symbian OS, check for an
 * updated API (RSat::TSetUpCallRspV3 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.  aPCmd must be a packaged version of the  latest "TSetUpCall" class.
 * For Symbian OS versions up to and including v8.1a and 8.1b use
 * RSat::TSetUpCallV1Pckg, a packaged RSat::TSetUpCallV1.  
 * For Symbian OS version 9.2, use RSat::TSetUpCallV5Pckg.
 * For Symbian OS version 9.4, use RSat::TSetUpCallV6Pckg.
 * For later versions of Symbian OS, check for an updated API 
 * (RSat::TSetUpCallV7 etc).
 *
 * @see RSat::TSetUpCallV1
 * @see RSat::TSetUpCallV5
 * @see RSat::TSetUpCallV6
 * @see RSat::TSetUpCallRspV1
 * @see RSat::TSetUpCallRspV2
 *
 * @capability ReadUserData
 */
	{
	Get(ESatNotifySetUpCallPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TRefreshV1::TRefreshV1()
	: TSatPCmdBase(),
	  iType(ERefreshTypeNotSet),
	  iFileList()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	}

EXPORT_C RSat::TRefreshV2::TRefreshV2()
	: TRefreshV1(),
	  iAid()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	}

EXPORT_C RSat::TRefreshRspV1::TRefreshRspV1()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iAdditionalInfo()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	}

EXPORT_C void RSat::NotifyRefreshPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a REFRESH proactive command is sent
 * by the UICC.  When the request completes, the command is placed into aPCmd and the
 * aReqStatus is competed.  Unpack the command to examine its contents.
 *
 * The REFRESH command is described fully in ETSI TS 102 223.  It tells you three
 * main things:
 *
 * - It can indicate that one or more of the files on the UICC have changed.
 * - It can request that the UICC has been reset.
 * - It can request that one of the applications running on the UICC be refreshed.
 *
 * RSat::TRefreshV1::iType indicates any or all of these things.  If one or more
 * of the data files on the UICC have changed, a list of the changed files will be
 * given in RSat::TRefreshV1::iFileList.  This will be a series of identifiers
 * from RSat::TElementaryFiles.  If an application needs to be refreshed, its ID
 * is in RSat::TRefreshV2::iAid.
 *
 * Any application that deals with UICC data may want to receive notification when
 * files change. For instance, the address book application may want to know if
 * the address book data changes. Hence the procedure for handling REFRESH
 * commands is more complicated than other proactive commands.  The following
 * scheme is necessary to allow for multiple clients to process a REFRESH
 * notification:
 *
 * Your phone's main SAT application decides when other applications receive
 * REFRESH commands.  If the main SAT application is in the middle of editing UICC
 * data, it can delay the REFRESH notification to other applications until it
 * has finished editing:
 *
 * - To request notification of a REFRESH proactive command, the main SAT
 * application calls both RSat::NotifyRefreshRequired() and
 * RSat::NotifyRefreshPCmd(), while other applications only call
 * RSat::NotifyRefreshPCmd().
 *
 * - When the UICC sends a REFRESH command, the RSat::NotifyRefreshRequired()
 * notification completes.  This tells the main SAT application that a REFRESH
 * command is pending.  This gives the main SAT application a chance to tidy up.
 *
 * - Once the main SAT application is ready, it calls RSat::RefreshAllowed().
 * This tells the TSY that it can complete any outstanding
 * RSat::NotifyRefreshPCmd() requests.
 *
 * Once you have dealt with a REFRESH command, you must give your response to the
 * UICC.   For Symbian OS versions up to and included v8.1a and v8.1b pass a
 * RSat::TRefreshRspV2Pckg (a packaged RSat::TRefreshRspV2) to RSat::TerminalRsp().
 * For later versions of Symbian OS, check for an updated API (RSat::TRefreshRspV3
 * etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command. For Symbian OS 7.0s, 8.0 and 8.1 use
 * RSat::TRefreshV2Pckg, a packaged RSat::TRefreshV2. Use RSat::TRefreshV1 with
 * older versions of Symbian OS.  For later versions of Symbian OS, check for an
 * updated API (RSat::TRefreshV3 etc).
 *
 * @see RSat::TRefreshV1
 * @see RSat::TRefreshV2
 * @see RSat::TRefreshRspV1
 * @see RSat::NotifyRefreshRequired()
 * @see RSat::RefreshAllowed()
 *
 * @capability None
 */
	{
	Get(ESatNotifyRefreshPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TSetUpEventListV1::TSetUpEventListV1()
	: TSatPCmdBase(),
	  iType(ESetUpEventListTypeNotSet),
	  iEvents(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	}

EXPORT_C RSat::TSetUpEventListRspV1::TSetUpEventListRspV1()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iAdditionalInfo()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	}

EXPORT_C void RSat::NotifySetUpEventListPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a SET UP EVENT LIST proactive command is sent
 * by the UICC.  When the request completes, the command is placed into aPCmd and the
 * aReqStatus is competed.  Unpack the command to examine its contents.
 *
 * The SET UP EVENT LIST command gives the phone a list of events about which
 * the UICC is interested in receiving notification.  The event list may include
 * such things as a change of access technology, user
 * action, browser termination and more.   You must inform the UICC when an event
 * on the list occurs by calling RSat::EventDownload().  If the event is not on the
 * list then you do not need to notify the UICC.
 *
 * RSat::TSetUpEventListV1::iEvents field contains
 * the list of events in the form of flags from RSat::TEventList.   When a flagged
 * event occurs, notify the UICC by passing RSat::EventDownload() the appropriate
 * flag. Many events require you to supply information about the event.
 *
 * The events, their flags and any additional information required by the UICC are
 * shown below.  Most events are described in the "Envelope Commands (Event
 * Download)" section of ETSI TS 102 223. 3GPP TS 31.111 contains additional
 * detail for certain events on GSM/WCDMA networks.
 *
 * - MT(Mobile Terminated) Call Event - RSat::KMTCall flag. Notify the UICC when the
 * phone receives a call setup message.   Return the transaction number, address
 * and subaddress as defined in ETSI TS 102 223 (and 3GPP TS 31.111 on GSM/WCDMA
 * networks.)  This event is often handled by the TSY, in which case RSat clients can ignore it.
 *
 * - Call Connected Event - RSat::KCallConnected flag. Notify the UICC when the
 * phone receives or sends a call connect message for mobile-terminated calls.
 * Return the transaction number as defined in as defined in ETSI TS 102 223 (and
 * 3GPP TS 31.111 on GSM/WCDMA networks.)  This event is often handled by the TSY,
 * in which case RSat clients can ignore it.
 *
 * - Call Disconnected Event - RSat::KCallDisconnected flag. Notify the UICC
 * when a call is disconnected.  Return the transaction number and cause code as
 * defined in ETSI TS 102 223 (and 3GPP TS 31.111 on GSM/WCDMA networks.)
 *  This event is often handled by the TSY, in which case RSat clients can ignore it.
 *
 * - Location Status Event - RSat::KLocationStatus flag. Notify the UICC when
 * the phone has changed location.   This only happens when the phone enters the
 * idle state.  Return the location information and location status as defined in
 * as defined in ETSI TS 102 223 (and 3GPP TS 31.111 on GSM/WCDMA networks.)
 * This event is often handled by the TSY, in which case RSat clients can ignore
 * it.
 *
 * - User Activity Event - RSat::KUserActivity flag. Notify the UICC when you
 * detect some user activity (e.g. a key-press, removal of key-lock).   There is
 * no additional information to be returned to the UICC. Unlike the majority of
 * other events, you should only respond to the first instance of user activity.
 * Ignore subsequent user activity unless the UICC sends another SET UP EVENT LIST
 * command listing the user activity event.
 *
 * - Idle Screen Available Event - RSat::KIdleScreenAvailable flag. Notify the
 * UICC when the phone next enters a state where it is willing to display text
 * that is not urgent.  More specifically, the phone will accept rather than
 * reject a DISPLAY TEXT command of normal priority. Unlike the majority of other
 * events, you should only respond to the first "Idle screen available event".
 * Ignore subsequent events unless the UICC sends another SET UP EVENT LIST command
 * listing the "Idle screen available event".
 *
 * - Card Reader Status Event - RSat::KCardReaderStatus flag. Notify the UICC
 * when a card reader becomes available or unavailable, and when a card is
 * inserted or removed.  Return the new status.  This is a
 * RSat::TCardReaderStatusEventV2 containing the "Card Reader Status" object
 * defined in ETSI TS 102 223's object definitions.
 *
 * - Language Selection Event - RSat::KLanguageSelection flag. Notify the UICC
 * when the user selects a new language.  Return the new language in a
 * RSat::TLanguageSelectionEventV2.
 *
 * - Browser Termination Event - RSat::KBrowserTermination flag. Notify the UICC
 * when the browser is terminated either by the user action or by an error.
 * Return the reason for termination in a RSat::TBrowserTerminationEventV2.
 *
 * - Data Available Event - RSat::KDataAvailable flag. Notify the UICC when the
 * phone receives data.  Return status of the channel
 * that received the data in a RSat::TDataAvailableEventV2.  Also return the
 * length of data received.
 *
 * - Channel status event - RSat::KChannelStatus flag. Notify the UICC when a
 * link enters an error condition.  Return status of
 * the channel in a RSat::TChannelStatusEventV2.
 *
 * - Access Technology Change Event - RSat::KAccessTechnologyChange flag.
 * Notify the UICC when the phone detects a change in its current access
 * technology.
 *
 * - Display Parameters Changed Event - RSat::KDisplayParamsChanges flag. Notify
 * the UICC when the phone's screen is resized.  Return the new screen parameters
 * in a RSat::TDisplayParamsEventV2.
 *
 * - Local Connection Event - RSat::KLocalConnection flag.  Notify the UICC when
 * the phone receives an incoming connection request on a local bearer using a
 * service previously declared by the UICC.
 * Return information about the new connection in a RSat::TLocalConnectionEventV2.
 *
 * As an alternative to the above, the SET UP EVENT LIST command may cancel the
 * previous command.  It does this by setting the command's
 * RSat::TSetUpEventListV1::iType to RSat::ERemoveExistingEventList instead of
 * RSat::EUpdateEventList.
 *
 * You must give your response to the command after you receive it.  In Symbian OS
 * versions up to and including 8.1a & 8.1b, pass RSat::TerminalRsp() a
 * RSat::TSetUpEventListRspV1Pckg (a packaged TSetUpEventListRspV1).  For later
 * versions of Symbian OS, check for an updated API (RSat::TSetUpEventListRspV2
 * etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.  aPCmd must be a packaged version of the latest "TSetUpEventList"
 * class. For Symbian OS versions up to and including 8.1a and 8,1b, use
 * RSat::TSetUpEventListV1Pckg, a packaged RSat::TSetUpEventListV1.  For later
 * versions of Symbian OS, check for an updated API (RSat::TSetUpEventListV2 etc).
 *
 * @see RSat::TSetUpEventListV1
 * @see RSat::TSetUpEventListRspV1
 * @see RSat::TEventList
 * @see RSat::TEventDownloadBaseV2
 * @see RSat::TCardReaderStatusEventV2
 * @see RSat::TLanguageSelectionEventV2
 * @see RSat::TBrowserTerminationEventV2
 * @see RSat::TDataAvailableEventV2
 * @see RSat::TChannelStatusEventV2
 * @see RSat::TDisplayParamsEventV2
 * @see RSat::TLocalConnectionEventV2
 *
 * @capability None
 */
	{
	Get(ESatNotifySetUpEventListPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TSetUpIdleModeTextV1::TSetUpIdleModeTextV1()
	: TSatPCmdBase(),
	  iType(ESetUpIdleModeTextTypeNotSet),
	  iText()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	iIconId.iQualifier=EIconQualifierNotSet;
	iIconId.iIdentifier=0;
	iCodingScheme=E7bitPacked;
	}

EXPORT_C RSat::TSetUpIdleModeTextV5::TSetUpIdleModeTextV5()
	: TSetUpIdleModeTextV1()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV5;
	iTextAttribute.iStatus = ETextAttributeNotSet;
	}

EXPORT_C RSat::TSetUpIdleModeTextV6::TSetUpIdleModeTextV6()
	: TSetUpIdleModeTextV5(),iFrameId(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV6;
	}
	
EXPORT_C RSat::TSetUpIdleModeTextRspV1::TSetUpIdleModeTextRspV1()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iAdditionalInfo()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	}

EXPORT_C void RSat::NotifySetUpIdleModeTextPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a SET UP IDLE MODE TEXT proactive command is sent
 * by the UICC.  When the request completes, the command is placed into aPCmd and the
 * aReqStatus is competed.  Unpack the command to examine its contents.
 *
 * The SET UP IDLE MODE TEXT command provides text (and optionally an icon) to be
 * displayed when the phone is idle.  The presentation style is not defined by
 * this command; that is left to the phone designer. The command is described
 * fully in ETSI TS 102 223.
 *
 * Two types of command can be sent by the UICC:
 *
 * - If RSat::TSetUpIdleModeTextV1::iType is RSat::EUpdateIdleModeText then the
 * command contains idle mode text in RSat::TSetUpIdleModeTextV1::iText and,
 * optionally, an icon to display in RSat::TSetUpIdleModeTextV1::iIconId. It also
 * contains the scheme in which iText has been encoded (7-bit packed, 7-bit unpacked
 * or 16-bit UCS-2 Unicode). In Symbian OS v9.2, optional text attribute information 
 * may be present in RSat::TSetUpIdleModeTextV5::iTextAttribute.
 *
 * - If RSat::TSetUpIdleModeTextV1::iType is RSat::ERemoveExistingIdleModeText
 * then the idle mode text and icon from a previous SET UP IDLE MODE TEXT command
 * should no longer be used.  The command does not contain text or an icon.
 *
 * Once you have dealt with the command, you must give your response to the SET UP
 * IDLE MODE TEXT command.  For Symbian OS versions up to and including  v8.1a and
 * v8.1b pass RSat::TerminalRsp() a RSat::TSetUpIdleModeTextRspV1Pckg (a packaged
 * RSat::TSetUpIdleModeTextRspV1).  For later versions of Symbian OS, check for an updated API
 * (RSat::TSetUpIdleModeTextRspV2 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.  aPCmd must be a packaged version of the  latest "TSetUpIdleModeText" class.
 * For Symbian OS versions up to and including v8.1a and 8.1b use
 * RSat::TSetUpIdleModeTextV1Pckg, a packaged RSat::TSetUpIdleModeTextV1.
 * For Symbian OS version 9.2, use RSat::TSetUpIdleModeTextV5Pckg.  
 * For Symbian OS version 9.4, use RSat::TSetUpIdleModeTextV6Pckg. 
 * For later versions of Symbian OS, check for an updated API (RSat::TSetUpIdleModeTextV7 etc).
 *
 * @see RSat::TSetUpIdleModeTextV1
 * @see RSat::TSetUpIdleModeTextV5
 * @see RSat::TSetUpIdleModeTextV6
 * @see RSat::TSetUpIdleModeTextRspV1
 *
 * @capability ReadDeviceData
 */
	{
	Get(ESatNotifySetUpIdleModeTextPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TSendDtmfV1::TSendDtmfV1()
	: TSatPCmdBase(),
	  iDtmfString()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	iAlphaId.iStatus=EAlphaIdNotSet;
	iIconId.iQualifier=EIconQualifierNotSet;
	iIconId.iIdentifier=0;
	}

EXPORT_C RSat::TSendDtmfV5::TSendDtmfV5()
	: TSendDtmfV1()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV5;
	iTextAttribute.iStatus = ETextAttributeNotSet;
	}

EXPORT_C RSat::TSendDtmfV6::TSendDtmfV6()
	: TSendDtmfV5(),iFrameId(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV6;
	}
	
EXPORT_C RSat::TSendDtmfRspV1::TSendDtmfRspV1()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iAdditionalInfo()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV1;
	}

EXPORT_C void RSat::NotifySendDtmfPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a SEND DTMF proactive command is sent
 * by the UICC.  When the request completes, the command is placed into aPCmd and the
 * aReqStatus is competed.  Unpack the command to examine its contents.
 *
 * The SEND DTMF command asks the phone to send a DTMF string.  A call must be in
 * progress to send DTMF tones. This command is independent of sending DTMF within
 * the call set up (as defined in the SET UP CALL command) and therefore can be
 * used at any time during a call.
 *
 * The command is described fully in ETSI TS 102 223.  It contains:
 *
 * - The DTMF string - RSat::TSendDtmfV1::iDtmfString.
 * - An alpha identifier containing text to display - RSat::TSendDtmfV1::iAlphaId.
 * - An icon to display - RSat::TSendDtmfV1::iIconId
 * - Text attributes of text contained in iAlphaId - RSat::TSendDtmfV5::iTextAttribute 
 * (Symbian OS v9.2 or later)
 *
 * See the description of the SEND DTMF command in ETSI TS 102 223 for the
 * circumstances under which the alpha identifier's text should be displayed.
 * Display the icon as described in RSat::TIconId.
 *
 * Once you have dealt with the command, you must give your response to the SEND
 * DTMF command.  For Symbian OS v6.1 or v7.0 pass RSat::TerminalRsp() a
 * RSat::TSendDtmfRspV1Pckg (a packaged TSendDtmfRspV1).  For versions 7.0s, v8.1a and
 * v8.1b use RSat::TSendDtmfRspV2Pckg (a packaged TSendDtmfRspV2).   For later versions
 * of Symbian OS, check for an updated API (RSat::TSendDtmfRspV3 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.  aPCmd must be a packaged version of the  latest "TSendDtmf" class.
 * For Symbian OS versions up to and including v8.1a and 8.1b use
 * RSat::TSendDtmfV1Pckg, a packaged RSat::TSendDtmfV1.  
 * For Symbian OS version 9.2, use RSat::TSendDtmfV5.
 * For Symbian OS version 9.4, use RSat::TSendDtmfV6.
 * For later versions of Symbian OS, check for an updated API (RSat::TSendDtmfV7 etc).
 *
 * @see RSat::TSendDtmfV1
 * @see RSat::TSendDtmfV5
 * @see RSat::TSendDtmfV6
 * @see RSat::TSendDtmfRspV1
 *
 * @capability ReadDeviceData
 */
	{
	Get(ESatNotifySendDtmfPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TPerformCardApduV2::TPerformCardApduV2()
	: TSatPCmdBase(),
	  iDestination(KDeviceIdNotSet),
	  iCApdu()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	}

EXPORT_C RSat::TPerformCardApduRspV2::TPerformCardApduRspV2()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iAdditionalInfo()
	{
	iExtensionId=KSatV2;
	}

EXPORT_C void RSat::NotifyPerformCardApduPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a PERFORM CARD APDU proactive command is sent
 * by the UICC.  When the request completes, the command is placed into aPCmd and the
 * aReqStatus is competed.  Unpack the command to examine its contents.
 *
 * The PERFORM CARD APDU command asks the phone to send an APDU command to the
 * card specified in the command. The command is described fully in ETSI TS 102
 * 223.  Symbian OS support for this command is available from v7.0s onwards.
 *
 * The command includes:
 *
 * - The APDU command - RSat::TPerformCardApduV2::iCApdu
 * - The device that the APDU command is for - RSat::TPerformCardApduV2::iDestination
 *
 * Once you have dealt with the APDU command, you must give your response to the
 * PERFORM CARD APDU command.   This should include the APDU command results, if
 * there are any. For Symbian OS versions from v7.0s to v8.1a and v8.1b, pass
 * RSat::TerminalRsp() a RSat::TPerformCardApduRspV2Pckg (a packaged
 * RSat::TPerformCardApduRspV2). For later versions of Symbian OS, check for an updated API
 * (RSat::TPerformCardApduRspV3 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.      Use a packaged version of the latest "TPerformCardApdu" class. For
 * Symbian OS 7.0s, 8.0 and 8.1 use RSat::TPerformCardApduV2Pckg, a packaged
 * RSat::TPerformCardApduV2. For later versions of Symbian OS, check for an updated API
 * (RSat::TPerformCardApduV3 etc).
 *
 * @see RSat::TPerformCardApduV2
 * @see RSat::TPerformCardApduRspV2
 *
 * @capability ReadDeviceData
 */
	{
	Get(ESatNotifyPerformCardApduPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TPowerOffCardV2::TPowerOffCardV2()
	: TSatPCmdBase(),
	  iDestination(KDeviceIdNotSet)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	}

EXPORT_C RSat::TPowerOffCardRspV2::TPowerOffCardRspV2()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iAdditionalInfo()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	}

EXPORT_C void RSat::NotifyPowerOffCardPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a POWER OFF CARD proactive command is sent
 * by the UICC.  When the request completes, the command is placed into aPCmd and the
 * aReqStatus is competed.  Unpack the command to examine its contents.
 *
 * The POWER OFF CARD command closes the session with the card specified in the
 * command. The command is described fully in ETSI TS 102 223.  Symbian OS support
 * for this command is available from v7.0s onwards.
 *
 * Once you have tried to close the session, you must give your response to the
 * POWER OFF CARD command.   This should include the APDU command results, if
 * there are any. For Symbian OS versions from v7.0s to v8.1a and v8.1b, pass
 * RSat::TerminalRsp() a RSat::TPowerOffCardRspV2Pckg (a packaged
 * RSat::TPowerOffCardRspV2). For later versions of Symbian OS, check for an updated API
 * (RSat::TPowerOffCardRspV3 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.      Use a packaged version of the latest "TPowerOffCard" class. For
 * Symbian OS 7.0s, 8.0 and 8.1 use RSat::TPowerOffCardV2Pckg, a packaged
 * RSat::TPowerOffCardV2. For later versions of Symbian OS, check for an updated API
 * (RSat::TPowerOffCardV3 etc).
 *
 * @see RSat::TPowerOffCardV2
 * @see RSat::TPowerOffCardRspV2
 *
 * @capability None
 */
	{
	Get(ESatNotifyPowerOffCardPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TPowerOnCardV2::TPowerOnCardV2()
	: TSatPCmdBase(),
	  iDestination(KDeviceIdNotSet)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	}

EXPORT_C RSat::TPowerOnCardRspV2::TPowerOnCardRspV2()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iAdditionalInfo()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	}

EXPORT_C void RSat::NotifyPowerOnCardPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a POWER ON CARD proactive command is sent
 * by the UICC.  When the request completes, the command is placed into aPCmd and the
 * aReqStatus is competed.  Unpack the command to examine its contents.
 *
 * The POWER ON CARD command requests that you start a session with the card
 * specified in the command. The command is described fully in ETSI TS 102 223.
 * Symbian OS support for this command is available from v7.0s onwards.
 *
 * Once you have tried to start the session, you must give your response to the
 * POWER ON CARD command.   This should include the APDU command results, if
 * there are any. For Symbian OS versions from v7.0s to v8.1a and v8.1b, pass
 * RSat::TerminalRsp() a RSat::TPowerOnCardRspV2Pckg (a packaged
 * RSat::TPowerOnCardRspV2). For later versions of Symbian OS, check for an updated API
 * (RSat::TPowerOnCardRspV3 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.      Use a packaged version of the latest "TPowerOnCard" class. For
 * Symbian OS 7.0s, 8.0 and 8.1 use RSat::TPowerOnCardV2Pckg, a packaged
 * RSat::TPowerOnCardV2. For later versions of Symbian OS, check for an updated API
 * (RSat::TPowerOnCardV3 etc).
 *
 * @see RSat::TPowerOnCardV2
 * @see RSat::TPowerOnCardRspV2
 *
 * @capability None
 */
       {
	Get(ESatNotifyPowerOnCardPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TGetReaderStatusV2::TGetReaderStatusV2()
	: TSatPCmdBase(),
	  iDestination(KDeviceIdNotSet),
	  iMode(ECardReaderModeNotSet)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	}

EXPORT_C RSat::TGetReaderStatusRspV2::TGetReaderStatusRspV2()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iAdditionalInfo(),
	  iMode(ECardReaderModeNotSet)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	}

EXPORT_C void RSat::NotifyGetReaderStatusPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a GET READER STATUS proactive command is sent
 * by the UICC.  When the request completes, the command is placed into aPCmd and the
 * aReqStatus is competed.  Unpack the command to examine its contents.
 *
 * The GET READER STATUS command requests the status of the card reader specified
 * in the command. The command is described fully in ETSI TS 102 223. Symbian OS
 * support for this command is available from v7.0s onwards.
 *
 * The command can request two types of information:
 *
 * - If RSat::TGetReaderStatusV2::iMode is RSat::ECardReaderStatus then return the card's status.
 * - If RSat::TGetReaderStatusV2::iMode is RSat::ECardReaderIdentifier then return the card's identifier.
 *
 * Once you have tried to get the status, you must give your response to the
 * GET READER STATUS command.   This should include the APDU command results, if
 * there are any. For Symbian OS versions from v7.0s to v8.1a and v8.1b, pass
 * RSat::TerminalRsp() a RSat::TGetReaderStatusRspV2Pckg (a packaged
 * RSat::TGetReaderStatusRspV2). For later versions of Symbian OS, check for an updated API
 * (RSat::TGetReaderStatusRspV3 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.      Use a packaged version of the latest "TGetReaderStatus" class. For
 * Symbian OS 7.0s, 8.0 and 8.1 use RSat::TGetReaderStatusV2Pckg, a packaged
 * RSat::TGetReaderStatusV2. For later versions of Symbian OS, check for an updated API
 * (RSat::TGetReaderStatusV3 etc).
 *
 * @see RSat::TGetReaderStatusV2
 * @see RSat::TGetReaderStatusRspV2
 *
 * @capability None
 */
	{
	Get(ESatNotifyGetReaderStatusPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TRunAtCommandV2::TRunAtCommandV2()
	: TSatPCmdBase(),
	  iAtCommand()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	iAlphaId.iStatus=EAlphaIdNotSet;
	iIconId.iQualifier=EIconQualifierNotSet;
	iIconId.iIdentifier=0;
	}

EXPORT_C RSat::TRunAtCommandV5::TRunAtCommandV5()
	: TRunAtCommandV2()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV5;
	iTextAttribute.iStatus = ETextAttributeNotSet;
	}

EXPORT_C RSat::TRunAtCommandV6::TRunAtCommandV6()
	: TRunAtCommandV5(),iFrameId(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV6;
	}
	
EXPORT_C RSat::TRunAtCommandRspV2::TRunAtCommandRspV2()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iAdditionalInfo()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	}

EXPORT_C void RSat::NotifyRunAtCommandPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a RUN AT COMMAND proactive command is sent
 * by the UICC.  When the request completes, the command is placed into aPCmd and the
 * aReqStatus is competed.  Unpack the command to examine its contents.
 *
 * The RUN AT COMMAND command asks the phone to perform the AT command provided.
 * The command is described fully in ETSI TS 102 223 and 3GPP TS 31.111.  Symbian
 * OS support for this command is available from v7.0s onwards.
 *
 * The command includes:
 *
 * - The AT command string - RSat::TRunAtCommandV2::iAtCommand
 * - An optional alpha identifier - RSat::TRunAtCommandV2::iAlphaId
 * - An optional icon -- RSat::TRunAtCommandV2::iIconId.
 * - Text attributes of text contained in iAlphaId - RSat::TRunAtCommandV5::iTextAttribute 
 * (Symbian OS v9.2 or later)
 *
 * See the description of the RUN AT COMMAND command in ETSI TS 102 223 for the
 * circumstances under which the alpha identifier's text should be displayed.
 * Display the icon as described in RSat::TIconId.
 *
 * Once you have dealt with the command, you must give your response to the RUN AT
 * COMMAND command.   This should include the AT response string, if there is one.
 * For Symbian OS versions from v7.0s to v8.1a and v8.1b, pass RSat::TerminalRsp()
 * a RSat::TRunAtCommandV2RspPckg (a packaged TRunAtCommandRspV2). For later versions of
 * Symbian OS, check for an updated API (RSat::TRunAtCommandRspV3 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.    Use a packaged version of the latest "TRunAtCommand" class. For
 * Symbian OS 7.0s, 8.0 and 8.1 use RSat::TRunAtCommandV2Pckg, a packaged
 * RSat::TRunAtCommandV2.  For Symbian OS version 9.2, use RSat::TRunAtCommandV5Pckg.
 * For Symbian OS version 9.4, use RSat::TRunAtCommandV6Pckg.
 * For later versions of Symbian OS, check for an updated API
 * (RSat::TRunAtCommandV7 etc).
 *
 * @see RSat::TRunAtCommandV2
 * @see RSat::TRunAtCommandV5
 * @see RSat::TRunAtCommandV6
 * @see RSat::TRunAtCommandRspV2
 *
 * @capability ReadDeviceData
 */
	{
	Get(ESatNotifyRunAtCommandPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TLanguageNotificationV2::TLanguageNotificationV2()
	: TSatPCmdBase(),
	  iNotificationType(ENotificationTypeNotSet),
	  iLanguage(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	}

EXPORT_C RSat::TLanguageNotificationRspV2::TLanguageNotificationRspV2()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iAdditionalInfo()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	}

EXPORT_C void RSat::NotifyLanguageNotificationPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a LANGUAGE NOTIFICATION proactive command is sent
 * by the UICC.  When the request completes, the command is placed into aPCmd and the
 * aReqStatus is competed.  Unpack the command to examine its contents.
 *
 * The LANGUAGE NOTIFICATION command tells the phone the language in which any
 * text strings from the UICC are written.  This includes text within proactive
 * commands or envelope command responses. The command is described fully in ETSI
 * TS 102 223 and 3GPP TS 31.111.  Symbian OS support for this command is
 * available from v7.0s onwards.
 *
 * The command contains a flag, RSat::TLanguageNotificationV2::iNotificationType,
 * that describes the command's purpose:
 *
 * - If iNotificationType is set to RSat::ESpecificLangNotification then
 * RSat::TLanguageNotificationV2::iLanguage contains the language in use by the
 * UICC.
 *
 * - If iNotificationType is set to RSat::ENonSpecificLangNotification then the
 * UICC is not using any specific language. This has the effect of cancelling
 * previous specific LANGUAGE NOTIFICATION commands.
 *
 * Once you have dealt with the command, you must give your response to the
 * LANGUAGE NOTIFICATION command.   For Symbian OS versions from v7.0s to v8.1a
 * and v8.1b, pass RSat::TerminalRsp() a RSat::TLanguageNotificationRspV2Pckg (a
 * packaged TLanguageNotificationRspV2). For later versions of Symbian OS, check
 * for an updated API (RSat::TLanguageNotificationRspV3 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.      Use a packaged version of the latest "TRunAtCommand" class. For
 * Symbian OS 7.0s, 8.0 and 8.1 use RSat::TRunAtCommandV2Pckg, a packaged
 * RSat::TRunAtCommandV2. For later versions of Symbian OS, check for an updated API
 * (RSat::TRunAtCommandV3 etc).
 *
 * @see RSat::TRunAtCommandV2
 * @see RSat::TRunAtCommandRspV2
 *
 * @capability None
 */
	{
	Get(ESatNotifyLanguageNotificationPCmd,aReqStatus,aPCmd);
	}

EXPORT_C void RSat::GetProvisioningRefFile(TRequestStatus& aReqStatus,
										   const TProvisioningFileRef& aFileRef,
										   TDes8& aFile) const
/**
 * This method retrieves the Provisioning Reference File designated by aFileRef.
 * Call this when handling a LAUNCH BROWSER proactive command.
 *
 * @param aReqStatus This method is asynchronous.  aReqStatus indicates when the operation has completed.
 * @param aFileRef Specifies which provisioning file is to be retrieved.
 * @param aFile File, specified by aInstance Number, retrieved from the UICC.
 * @capability ReadDeviceData
 *
 * @see RSat::NotifyLaunchBrowserPCmd()
 */
	{
	__ASSERT_ALWAYS(iEtelSatPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iEtelSatPtrHolder->iGetProvisioningRefFile = aFileRef;
	TPtrC8& ptr1=iEtelSatPtrHolder->SetC(ESlot1GetProvisioningRefFile,iEtelSatPtrHolder->iGetProvisioningRefFile);

	SetAndGet(ESatGetProvisioningRefFile,aReqStatus, ptr1, aFile);
	}

EXPORT_C RSat::TLaunchBrowserV2::TLaunchBrowserV2()
	: TSatPCmdBase(),
	  iBrowserSel(EBrowserSelectionNotSet),
	  iBrowserId(EBrowserIdNotSet),
	  iUrl(),
	  iBearerList(),
	  iText(),
      iFileRef(),
	  iNumOfFiles(0)
/**
* Default constructor.
*/
	{
	iExtensionId = KSatV2;
	iOffset[0]   = 0;
	iLength[0]   = 0;
	iAlphaId.iStatus=EAlphaIdNotSet;
	iIconId.iQualifier=EIconQualifierNotSet;
	iIconId.iIdentifier=0;
	}

EXPORT_C TInt RSat::TLaunchBrowserV2::AddFileRef(const TProvisioningFileRef& aFileRef)
/**
 * This method adds a Provisioning Reference File designated by aFileRef
 * @param aFileRef File ref to add
 * @return An error or KErrNone
 */
	{
	if (iNumOfFiles >= RSat::KFileRefMaxSize)
		return KErrOverflow;
	else
		{
		if ((aFileRef.Length() + iFileRef.Length()) >= iFileRef.MaxLength())
			return KErrOverflow;

		iOffset[iNumOfFiles] = (TUint8)iFileRef.Length();	// Current length gives offset
		iLength[iNumOfFiles] = (TUint8)aFileRef.Length();	// Store length of new entry
		iFileRef.Append(aFileRef);							// Store object in iFileRef
		++iNumOfFiles;

		return KErrNone;
		}
	}

EXPORT_C TInt RSat::TLaunchBrowserV2::GetFileRef(TUint aIndex, TProvisioningFileRef& aFileRef) const
/**
 * This method retrieves the Provisioning Reference File designated by aIndex
 * @return An error or KErrNone
 * @param aIndex Index of file ref to retrieve
 * @param aFileRef Specifies which provisioning file is to be retrieved.
 */
	{
	TInt index = (TInt)aIndex - 1;	// External index is 1..max, internal is 0..max-1  (cast required to convert unsigned->signed)

	if (index < 0)
		return KErrArgument;
	else if (index >= RSat::KFileRefMaxSize || index >= iNumOfFiles)
		return KErrNotFound;
	else
		aFileRef = iFileRef.Mid(iOffset[index], iLength[index]);

	return KErrNone;
	}

EXPORT_C void RSat::TLaunchBrowserV2::ResetFileRef()
/*
* Resets the buffer storing the provisioning file references.
*/
	{
	iNumOfFiles = 0;
	iFileRef.SetLength(0);
	}

EXPORT_C TUint RSat::TLaunchBrowserV2::NumberOfFileRef() const
/**
* Returns the number of provisioning file references.
*
* @return Returns the number of provisioning file references.
*/
	{
	return iNumOfFiles;
	}

EXPORT_C RSat::TLaunchBrowserV5::TLaunchBrowserV5()
	: TLaunchBrowserV2()
/**
* Default constructor.
*/
	{
	iExtensionId = KSatV5;
	iTextAttribute.iStatus = ETextAttributeNotSet;
	}

EXPORT_C RSat::TLaunchBrowserV6::TLaunchBrowserV6()
	: TLaunchBrowserV5(),iFrameId(0)
/**
* Default constructor.
*/
	{
	iExtensionId = KSatV6;
	}
	
EXPORT_C RSat::TLaunchBrowserRspV2::TLaunchBrowserRspV2()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iAdditionalInfo()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	}

EXPORT_C void RSat::NotifyLaunchBrowserPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a LAUNCH BROWSER proactive command is sent
 * by the UICC.  When the request completes, the command is placed into aPCmd and the
 * aReqStatus is competed.  Unpack the command to examine its contents.
 *
 * The LAUNCH BROWSER command requests that the phone browses to the supplied URL.
 * The command is described fully in ETSI TS 102 223 and 3GPP TS 31.111.  Symbian
 * OS support for this command is available from v7.0s onwards.
 *
 * Handling the command is a four-stage process:
 *
 * Step one is to ask the user for their permission to launch the browser.  The LAUNCH
 * BROWSER command provides two parameters for this: an alpha identifier
 * (RSat::TLaunchBrowserV2::iAlphaId) and an icon
 * (RSat::TLaunchBrowserV2::iIconId).  You can display these to help the user make a choice.
 *
 * Step two: examine the rest of the command and decide whether you can proceed.
 * The command includes:
 *
 * - A description of the browser to use - RSat::TLaunchBrowserV2::iBrowserSel
 * - The URL - RSat::TLaunchBrowserV2::iUrl
 * - A list of bearers allowed in order of priority - RSat::TLaunchBrowserV2::iBearerList
 * - Name/identity of the Gateway/Proxy - RSat::TLaunchBrowserV2::iText
 * - A number of reference files - see RSat::TLaunchBrowserV2.
 * - Text attributes of text contained in iText - RSat::TLaunchBrowserV5::iTextAttribute 
 * (Symbian OS v9.2 or later)
 *
 * Step three is to tell the UICC your intentions.  You must do this before
 * launching the browser.  For Symbian OS versions from 7.0s to v8.1a and
 * v8.1b pass RSat::TerminalRsp() a RSat::TLaunchBrowserRspV2Pckg (a packaged
 * RSat::TLaunchBrowserRspV2).  For later versions of Symbian OS, check for an updated API
 * (RSat::TLaunchBrowserRspV3 etc).
 *
 * Finally, you can launch the browser.
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command. Use a packaged version of the latest "TLaunchBrowser" class. For
 * Symbian OS 7.0s, 8.0 and 8.1 use RSat::TLaunchBrowserV2Pckg, a packaged
 * RSat::TLaunchBrowserV2.  For Symbian OS version 9.2, use RSat::TLaunchBrowserV5Pckg.
 * For Symbian OS version 9.4, use RSat::TLaunchBrowserV6Pckg.
 * For later versions of Symbian OS, check for an updated API
 * (RSat::TLaunchBrowserV7 etc).
 *
 * @see RSat::TLaunchBrowserV2
 * @see RSat::TLaunchBrowserV5
 * @see RSat::TLaunchBrowserV6
 * @see RSat::TLaunchBrowserRspV2
 * @see RSat::GetProvisioningRefFile()
 *
 * @capability ReadDeviceData
 */
	{
	Get(ESatNotifyLaunchBrowserPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TOpenChannelBaseV2::TOpenChannelBaseV2()
	: TSatPCmdBase(),
	  iPCmdType(EPCmdTypeNotSet),
	  iLinkEst(ELinkEstablishmentNotSet),
	  iReconnectionMode(EReconnectionModeNotSet),
	  iBufferSize(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	iAlphaId.iStatus=EAlphaIdNotSet;
	iIconId.iQualifier=EIconQualifierNotSet;
	iIconId.iIdentifier=0;
	iBearer.iType=EBearerTypeNotSet;
	iDestinationAddress.iType=EAddressNotSet;
	iSimMeInterface.iTransportProto=EProtocolNotSet;
	iSimMeInterface.iPrtNumber=0;
	}

EXPORT_C RSat::TOpenCsChannelV2::TOpenCsChannelV2()
	: TOpenChannelBaseV2(),
	  iSubAddress(),
	  iUserLogin(),
	  iUserPassword()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	iDuration1.iTimeUnit=ETimeUnitNotSet;
	iDuration1.iNumOfUnits=0;
	iDuration2.iTimeUnit=ETimeUnitNotSet;
	iDuration2.iNumOfUnits=0;
	iAddress.iTypeOfNumber=ETypeOfNumberNotSet;
	iAddress.iNumberPlan=ENumberingPlanNotSet;
	iLocalAddress.iType=EAddressNotSet;
	}

EXPORT_C RSat::TOpenCsChannelV5::TOpenCsChannelV5()
	: TOpenCsChannelV2()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV5;
	iTextAttribute.iStatus = ETextAttributeNotSet;
	}

EXPORT_C RSat::TOpenCsChannelV6::TOpenCsChannelV6()
	: TOpenCsChannelV5(),iFrameId(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV6;
	}
	
EXPORT_C RSat::TOpenGprsChannelV2::TOpenGprsChannelV2()
	: TOpenChannelBaseV2(),
	  iAccessName()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	iLocalAddress.iType=EAddressNotSet;
	}

EXPORT_C RSat::TOpenGprsChannelV4::TOpenGprsChannelV4()
	: TOpenGprsChannelV2(),
	  iUserLogin(),
	  iUserPassword()
/**
* Default constructor.
*/
	{
	iExtensionId = KSatV4;
	}

EXPORT_C RSat::TOpenGprsChannelV5::TOpenGprsChannelV5()
	: TOpenGprsChannelV4()
/**
* Default constructor.
*/
	{
	iExtensionId = KSatV5;
	iTextAttribute.iStatus = ETextAttributeNotSet;
	}

EXPORT_C RSat::TOpenGprsChannelV6::TOpenGprsChannelV6()
	: TOpenGprsChannelV5(),iFrameId(0)
/**
* Default constructor.
*/
	{
	iExtensionId = KSatV6;
	}
	
EXPORT_C RSat::TOpenLocalLinksChannelV2::TOpenLocalLinksChannelV2()
	: TOpenChannelBaseV2(),
	  iUserPassword()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	iDuration1.iTimeUnit=ETimeUnitNotSet;
	iDuration1.iNumOfUnits=0;
	iDuration2.iTimeUnit=ETimeUnitNotSet;
	iDuration2.iNumOfUnits=0;
	iRemoteAddress.iCoding=ECodingNotSet;
	}

EXPORT_C RSat::TOpenLocalLinksChannelV5::TOpenLocalLinksChannelV5()
	: TOpenLocalLinksChannelV2()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV5;
	iTextAttribute.iStatus = ETextAttributeNotSet;
	}

EXPORT_C RSat::TOpenLocalLinksChannelV6::TOpenLocalLinksChannelV6()
	: TOpenLocalLinksChannelV5(), iFrameId(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV6;
	}

EXPORT_C RSat::TOpenUiccServerModeChannelV7::TOpenUiccServerModeChannelV7()
	: TOpenChannelBaseV2(), iFrameId(0)
/**
 * Default constructor.
 */
	{
	iExtensionId=KSatV7;
	iBearer.iType = EBearerTypeNotSet;
	iSimMeInterface.iTransportProto = EProtocolNotSet;
	iTextAttribute.iStatus = ETextAttributeNotSet;
	}

EXPORT_C RSat::TOpenChannelRspV2::TOpenChannelRspV2()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iAdditionalInfo(),
	  iBearer(),
	  iBufferSize(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	}

EXPORT_C void RSat::NotifyOpenChannelPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when an OPEN CHANNEL proactive command is sent
 * by the UICC.  When the request completes, the command is placed into aPCmd and the
 * aReqStatus is competed.  Unpack the command to examine its contents.
 *
 * The OPEN CHANNEL command asks the phone to open a channel.  You must inform the
 * UICC whether you were successful.  If so, subsequent proactive commands ask
 * the phone to read or write data to the channel, to get the channel's status
 * and to close the channel.
 *
 * The command is described fully in ETSI TS 102 223 and 3GPP TS 31.111.  The
 * documents also describe these related commands:
 *
 * - CLOSE CHANNEL
 * - SEND DATA
 * - RECEIVE DATA
 * - GET CHANNEL STATUS
 *
 * The OPEN COMMAND defines the channel that should be opened.  Typical examples
 * are a channel to an IP address, an Internet domain name, a local serial
 * connection etc.
 *
 * The command is returned in a class that inherits from RSat::TOpenChannelBaseV2.
 * When you receive an OPEN CHANNEL command, look at its
 * RSat::TOpenChannelBaseV2::iPCmdType field.  This determines the type of bearer
 * to use for the channel.  There are three types:
 *
 * - Circuit switched.  iPCmdType will be RSat::ECsBearer.
 *     - The command is contained in a RSat::TOpenCsChannelV2 (or derived class).
 * - Packet switched.  iPCmdType will be RSat::EGprsBearer.
 *     - The command is contained in a RSat::TOpenGprsChannelV2 (or derived class).
 * - Local (such as infra-red and Bluetooth.)  iPCmdType will be RSat::ELocalLinksBearer.
 *     - The command is contained in a RSat::TOpenLocalLinksChannelV2 (or derived class).
 * - UICC server mode
 *     - The command is contained in a RSat::TOpenUiccServerModeChannelV7 (or derived class).
 *
 * The description of the "TOpen..." classes describes their contents and
 * how to set up the channel.
 *
 * If you successfully open a channel then you must assign it a channel number
 * from 1 to 7. You can have up to seven channels open at once, each opened with a
 * separate OPEN CHANNEL command.  You must inform the UICC of your chosen channel
 * number. Subsequent CLOSE CHANNEL, SEND DATA, RECEIVE DATA and GET CHANNEL
 * STATUS commands from the UICC will include a channel number from those you have
 * returned to the UICC.
 *
 * If you do not successfully open a channel then return an error code as
 * recommended in ETSI TS 102 223 and 3GPP TS 31.111.
 *
 * Whether you are successful or unsuccessful, respond to the OPEN CHANNEL command
 * as follows: for Symbian OS versions from 7.0s to v8.1a and v8.1b pass
 * RSat::TerminalRsp() a RSat::TOpenChannelRspV2Pckg (a packaged
 * RSat::TOpenChannelRspV2).
 * For later versions of Symbian OS, check for an updated
 * API (RSat::TOpenChannelRspV3 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received.
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command. Pass in a RSat::TOpenChannelBaseV2Pckg.  When the request
 * completes, class will be changed to a RSat::TOpenCsChannelV2Pckg for Symbian OS 7.0s, 8.0 and 8.1.
 * For Symbian OS 9.2, class will be changed to a RSat::TOpenCsChannelV5Pckg.
 * For Symbian OS 9.4, class will be changed to a RSat::TOpenCsChannelV6Pckg.
 * or for Symbian OS 7.0s, 8.0 and 8.1, class will be changed to a RSat::TOpenGprsChannelV2Pckg.
 * For Symbian OS 9.2, class will be changed to a RSat::TOpenGprsChannelV5Pckg.
 * For Symbian OS 9.4, class will be changed to a RSat::TOpenGprsChannelV6Pckg.
 * or for Symbian OS 7.0s, 8.0 and 8.1, class will be changed to a RSat::TOpenLocalLinksChannelV2Pckg.
 * For Symbian OS 9.2, class will be changed to a RSat::TOpenLocalLinksChannelV5Pckg. 
 * For Symbian OS 9.4, class will be changed to a RSat::TOpenLocalLinksChannelV6Pckg.
 * and RSat::TOpenUiccServerModeChannelV7Pckg has been introduced to support UICC Server Mode.
 * Look at RSat::TOpenChannelBaseV2::iPCmdType to find out which determines the type of bearer to
 * use for the channel.
 * For later versions of Symbian OS, check for an updated version of APIs
 * (RSat::TOpenCsChannelV7 or RSat::TOpenGprsChannelV6 or 
 * RSat::TOpenLocalLinksChannelV7 or TOpenUiccServerModeChannelV8 etc).
 *
 * @see RSat::TOpenChannelBaseV2
 * @see RSat::TOpenCsChannelV2
 * @see RSat::TOpenGprsChannelV2
 * @see RSat::TOpenCsChannelV5
 * @see RSat::TOpenGprsChannelV5 
 * @see RSat::TOpenLocalLinksChannelV2
 * @see RSat::TOpenLocalLinksChannelV5
 * @see RSat::TOpenCsChannelV6
 * @see RSat::TOpenGprsChannelV6
 * @see RSat::TOpenLocalLinksChannelV6
 * @see RSat::TOpenUiccServerModeChannelV7
 * @see RSat::TOpenChannelRspV2
 * @see RSat::NotifyCloseChannelPCmd()
 * @see RSat::NotifySendDataPCmd()
 * @see RSat::NotifyReceiveDataPCmd()
 * @see RSat::NotifyGetChannelStatusPCmd()
 *
 * @capability ReadDeviceData
 */
	{
	Get(ESatNotifyOpenChannelPCmd,aReqStatus,aPCmd);
	}

EXPORT_C void RSat::GetOpenChannelPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method can be used to retrieve more details about the channel after
 * the channel is opened. PCmd type should be set to RSat::ECsBearer,
 * RSat::EGprsBearer, RSat::ELocalLinksBearer or RSat::EUiccServerMode (or another
 * enum defined in RSat::TOpenChannelType.
 *
 * @param aPCmd Pass in a RSat::TOpenCsChannelV2Pckg, RSat::TOpenGprsChannelV2Pckg,
 * RSat::TOpenLocalLinksChannelV2Pckg, RSat::TOpenUiccServerModeChannelV7 or 
 * derived class as appropriate.
 *
 * @see RSat::TOpenChannelBaseV2
 * @see RSat::TOpenCsChannelV2
 * @see RSat::TOpenGprsChannelV2
 * @see RSat::TOpenLocalLinksChannelV2
 * @see RSat::TOpenUiccServerModeChannelV7
 * @see RSat::NotifyOpenChannelPCmd
 *
 * @capability ReadDeviceData
 */
	{
	Get(ESatGetOpenChannelPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TCloseChannelV2::TCloseChannelV2()
	: TSatPCmdBase(),
	  iDestination(KDeviceIdNotSet)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	iAlphaId.iStatus=EAlphaIdNotSet;
	iIconId.iQualifier=EIconQualifierNotSet;
	iIconId.iIdentifier=0;
	}

EXPORT_C RSat::TCloseChannelV5::TCloseChannelV5()
	: TCloseChannelV2()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV5;
	iTextAttribute.iStatus = ETextAttributeNotSet;
	}

EXPORT_C RSat::TCloseChannelV6::TCloseChannelV6()
	: TCloseChannelV5(),iFrameId(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV6;
	}

EXPORT_C RSat::TCloseChannelRspV2::TCloseChannelRspV2()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iAdditionalInfo()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	}

EXPORT_C void RSat::NotifyCloseChannelPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a CLOSE CHANNEL proactive command is
 * sent by the UICC.  When the request completes, the command is placed into aPCmd
 * and the aReqStatus is completed.  Unpack the command
 * to examine its contents.
 *
 * The CLOSE CHANNEL command requests that the phone closes a channel previously
 * opened with the OPEN CHANNEL proactive command. The command is described fully
 * in ETSI TS 102 223 and 3GPP TS 31.111.
 *
 * Symbian OS support for this command is available from v7.0s onwards.
 *
 * The command includes:
 *
 * - a channel identifier  that tells you which channel to close - RSat::TCloseChannelV2::iDestination.
 * - An alpha identifier containing text to display - RSat::TCloseChannelV2::iAlphaId.
 * - An icon to display - RSat::TCloseChannelV2::iIconId
 * - Text attributes of text contained in iAlphaId - RSat::TCloseChannelV5::iTextAttribute 
 * (Symbian OS v9.2 or later)
 *
 * When you open a channel following an OPEN CHANNEL command, you chose a channel
 * identifier -  a number from 1 to 7.  When you inform the UICC that you opened a
 * channel successfully, you tell the UICC which number you have chosen to
 * represent the channel.   The UICC includes this number in subsequent CLOSE
 * CHANNEL commands so that you know which channel to close.
 *
 * See the description of the CLOSE CHANNEL command in ETSI TS 102 223 for the
 * circumstances under which the alpha identifier's text should be displayed.
 * Display the icon as described in RSat::TIconId.
 *
 * Once you have attempted to close the channel, give your response to the UICC.
 * For Symbian OS versions from 7.0s to v8.1a and v8.1b pass RSat::TerminalRsp() a
 * RSat::TCloseChannelRspV2Pckg (a packaged TCloseChannelRspV2).  For later
 * versions of Symbian OS, check for an updated API (RSat::TCloseChannelRspV3
 * etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.      Use a packaged version of the latest "TCloseChannel" class. For
 * Symbian OS 7.0s, 8.0 and 8.1 use RSat::TCloseChannelV2Pckg, a packaged
 * RSat::TCloseChannelV2.  For Symbian OS version 9.2, use RSat::TCloseChannelV5Pckg.
 * For Symbian OS version 9.4, use RSat::TCloseChannelV6Pckg.
 * For later versions of Symbian OS, check for an updated API
 * (RSat::TCloseChannelV7 etc).
 *
 * @see RSat::TCloseChannelV2
 * @see RSat::TCloseChannelV5
 * @see RSat::TCloseChannelV6
 * @see RSat::TCloseChannelRspV2
 * @see RSat::NotifyOpenChannelPCmd()
 * @see RSat::NotifySendDataPCmd()
 * @see RSat::NotifyReceiveDataPCmd()
 * @see RSat::NotifyGetChannelStatusPCmd()
 *
 * @capability None
 */
	{
	Get(ESatNotifyCloseChannelPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TReceiveDataV2::TReceiveDataV2()
	: TSatPCmdBase(),
	  iDestination(KDeviceIdNotSet),
	  iChannelDataLength(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	iAlphaId.iStatus=EAlphaIdNotSet;
	iIconId.iQualifier=EIconQualifierNotSet;
	iIconId.iIdentifier=0;
	}

EXPORT_C RSat::TReceiveDataV5::TReceiveDataV5()
	: TReceiveDataV2()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV5;
	iTextAttribute.iStatus = ETextAttributeNotSet;
	}

EXPORT_C RSat::TReceiveDataV6::TReceiveDataV6()
	: TReceiveDataV5(),iFrameId(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV6;
	}
	
EXPORT_C RSat::TReceiveDataRspV2::TReceiveDataRspV2()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iAdditionalInfo(),
	  iChannelDataLength(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	}

EXPORT_C void RSat::NotifyReceiveDataPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a RECEIVE DATA proactive command is
 * sent by the UICC.  When the request completes, the command is placed into aPCmd
 * and the aReqStatus is completed.  Unpack the command
 * to examine its contents.
 *
 * The RECEIVE DATA command asks the phone for data from  a channel opened with
 * the OPEN CHANNEL proactive command. Both of these commands are described fully
 * in ETSI TS 102 223 and 3GPP TS 31.111.
 *
 * Symbian OS support for this command is available from v7.0s onwards.
 *
 * The command includes:
 *
 * - A channel identifier  that tells you which channel to receive data from - RSat::TReceiveDataV2::iDestination.
 * - The length of data to receive - RSat::TReceiveDataV2::iChannelDataLength.
 * - An alpha identifier containing text to display - RSat::TReceiveDataV2::iAlphaId.
 * - An icon to display - RSat::TReceiveDataV2::iIconId
 * - Text attributes of text contained in iAlphaId - RSat::TReceiveDataV5::iTextAttribute 
 * (Symbian OS v9.2 or later)
 *
 * When you open a channel following an OPEN CHANNEL command, you chose a channel
 * identifier -  a number from 1 to 7.  When you inform the UICC that you opened a
 * channel successfully, you tell the UICC which number you have chosen to
 * represent the channel.   The UICC includes this number in subsequent RECEIVE
 * DATA commands so that you know which channel to use.
 *
 * See the description of the RECEIVE DATA command in ETSI TS 102 223 for the
 * circumstances under which the alpha identifier's text should be displayed.
 * Display the icon as described in RSat::TIconId.
 *
 * You need to give the data or an error to the UICC. For Symbian OS versions from
 * 7.0s to v8.1a and v8.1b pass RSat::TerminalRsp() a RSat::TReceiveDataRspV2Pckg
 * (a packaged TReceiveDataRspV2).  For later versions of Symbian OS, check for an
 * updated API (RSat::TReceiveDataRspV3 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.   Use a packaged version of the latest "TReceiveData" class. For
 * Symbian OS 7.0s, 8.0 and 8.1 use RSat::TReceiveDataV2Pckg, a packaged
 * RSat::TReceiveDataV2. For Symbian OS version 9.2, use RSat::TReceiveDataV5Pckg.
 * For Symbian OS version 9.4, use RSat::TReceiveDataV6Pckg.
 * For later versions of Symbian OS, check for an updated API
 * (RSat::TReceiveDataV7 etc).
 *
 * @see RSat::TReceiveDataV2
 * @see RSat::TReceiveDataV5 
 * @see RSat::TReceiveDataV6 
 * @see RSat::TReceiveDataRspV2
 * @see RSat::NotifyOpenChannelPCmd()
 * @see RSat::NotifySendDataPCmd()
 * @see RSat::NotifyCloseChannelPCmd()
 * @see RSat::NotifyGetChannelStatusPCmd()
 *
 * @capability None
 */
	{
	Get(ESatNotifyReceiveDataPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TSendDataV2::TSendDataV2()
	: TSatPCmdBase(),
	  iDestination(KDeviceIdNotSet),
	  iMode(ESendDataModeNotSet),
	  iChannelData()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	iAlphaId.iStatus=EAlphaIdNotSet;
	iIconId.iQualifier=EIconQualifierNotSet;
	iIconId.iIdentifier=0;
	}

EXPORT_C RSat::TSendDataV5::TSendDataV5()
	: TSendDataV2()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV5;
	iTextAttribute.iStatus = ETextAttributeNotSet;
	}

EXPORT_C RSat::TSendDataV6::TSendDataV6()
	: TSendDataV5(),iFrameId(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV6;
	}
	
EXPORT_C RSat::TSendDataRspV2::TSendDataRspV2()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iAdditionalInfo(),
	  iChannelDataLength(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	}

EXPORT_C void RSat::NotifySendDataPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a SEND DATA proactive command is
 * sent by the UICC.  When the request completes, the command is placed into aPCmd
 * and the aReqStatus is completed.  Unpack the command
 * to examine its contents.
 *
 * The SEND DATA command asks the phone to send data on a channel opened with the
 * OPEN CHANNEL proactive command. Both of these commands are described fully in
 * ETSI TS 102 223 and 3GPP TS 31.111.
 *
 * Symbian OS support for this command is available from v7.0s onwards.
 *
 * The command includes:
 *
 * - A channel identifier  that tells you which channel to send data on - RSat::TSendDataV2::iDestination.
 * - A mode indicator:  determines whether the SEND DATA command's data is sent immediately, or
 *     the data from a series of SEND DATA commands is buffered by the phone
 *     and sent at whatever rate the phone considers best RSat::TSendDataV2::iMode
 * - The length of data to send - RSat::TSendDataV2::iChannelDataLength.
 * - An alpha identifier containing text to display - RSat::TSendDataV2::iAlphaId.
 * - An icon to display - RSat::TSendDataV2::iIconId
 * - Text attributes of text contained in iAlphaId - RSat::TSendDataV5::iTextAttribute 
 * (Symbian OS v9.2 or later) 
 *
 * When you open a channel following an OPEN CHANNEL command, you chose a channel
 * identifier -  a number from 1 to 7.  When you inform the UICC that you opened a
 * channel successfully, you tell the UICC which number you have chosen to
 * represent the channel.   The UICC includes this number in subsequent SEND
 * DATA commands so that you know which channel to use.
 *
 * See the description of the SEND DATA command in ETSI TS 102 223 for the
 * circumstances under which the alpha identifier's text should be displayed.
 * Display the icon as described in RSat::TIconId.
 *
 * You need to give a response to the UICC. For Symbian OS versions from
 * 7.0s to v8.1a and v8.1b pass RSat::TerminalRsp() a RSat::TSendDataRspV2Pckg
 * (a packaged TSendDataRspV2).  For later versions of Symbian OS, check for an
 * updated API (RSat::TSendDataRspV3 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.      Use a packaged version of the latest "TSendData" class. For
 * Symbian OS 7.0s, 8.0 and 8.1 use RSat::TSendDataV2Pckg, a packaged
 * RSat::TSendDataV2.  For Symbian OS version 9.2, use RSat::TSendDataV5Pckg.
 * For Symbian OS version 9.4, use RSat::TSendDataV6Pckg.
 * For later versions of Symbian OS, check for an updated API
 * (RSat::TSendDataV7 etc).
 *
 * @see RSat::TSendDataV2
 * @see RSat::TSendDataV5
 * @see RSat::TSendDataV6
 * @see RSat::TSendDataRspV2
 * @see RSat::NotifyOpenChannelPCmd()
 * @see RSat::NotifyReceiveDataPCmd()
 * @see RSat::NotifyCloseChannelPCmd()
 * @see RSat::NotifyGetChannelStatusPCmd()
 *
 * @capability ReadDeviceData
 */
	{
	Get(ESatNotifySendDataPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TGetChannelStatusV2::TGetChannelStatusV2()
	: TSatPCmdBase()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	}

EXPORT_C RSat::TGetChannelStatusRspV2::TGetChannelStatusRspV2()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iAdditionalInfo()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	iGeneralResult=KPCmdResultNotSet;
	iInfoType=0;
	}

EXPORT_C void RSat::NotifyGetChannelStatusPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a GET CHANNEL STATUS proactive command is
 * sent by the UICC.  When the request completes, the command is placed into aPCmd
 * and the aReqStatus is completed.  Unpack the command
 * to examine its contents.
 *
 * The GET CHANNEL STATUS command asks the phone to return the status of a channel
 * opened with the OPEN CHANNEL proactive command. Both of these commands are
 * described fully in ETSI TS 102 223 and 3GPP TS 31.111.
 *
 * When you open a channel following an OPEN CHANNEL command, you chose a channel
 * identifier -  a number from 1 to 7.  When you inform the UICC that you opened a
 * channel successfully, you tell the UICC which number you have chosen to
 * represent the channel.   The UICC includes this number in subsequent GET CHANNEL STATUS
 *  commands so that you know which channel's status to return.
 *
 * Symbian OS support for this command is available from v7.0s onwards.
 *
 * You need to return the channel status or an error to the UICC. For Symbian OS
 * versions from 7.0s to v8.1a and v8.1b pass RSat::TerminalRsp() a
 * RSat::TGetChannelStatusRspV2Pckg (a packaged TGetChannelStatusRspV2).  For later versions of
 * Symbian OS, check for an updated API (RSat::TGetChannelStatusRspV3 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received.
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.      Use a packaged version of the latest "TGetChannelStatus" class. For
 * Symbian OS 7.0s, 8.0 and 8.1 use RSat::TGetChannelStatusV2Pckg, a packaged
 * RSat::TGetChannelStatusV2. For later versions of Symbian OS, check for an updated API
 * (RSat::TGetChannelStatusV3 etc).
 *
 * @see RSat::TGetChannelStatusV2
 * @see RSat::TGetChannelStatusRspV2
 * @see RSat::NotifyOpenChannelPCmd()
 * @see RSat::NotifyReceiveDataPCmd()
 * @see RSat::NotifySendDataPCmd()
 * @see RSat::NotifyCloseChannelPCmd()
 *
 * @capability None
 */
	{
	Get(ESatNotifyGetChannelStatusPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TServiceSearchV2::TServiceSearchV2()
	: TSatPCmdBase()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	iAlphaId.iStatus=EAlphaIdNotSet;
	iIconId.iQualifier=EIconQualifierNotSet;
	iIconId.iIdentifier=0;
	iSearch.iBearerId=ELocalBearerIdNotSet;
	iFilter.iBearerId=ELocalBearerIdNotSet;
	}

EXPORT_C RSat::TServiceSearchV5::TServiceSearchV5()
	: TServiceSearchV2()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV5;
	iTextAttribute.iStatus = ETextAttributeNotSet;
	}

EXPORT_C RSat::TServiceSearchV6::TServiceSearchV6()
	: TServiceSearchV5(),iFrameId(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV6;
	}
	
EXPORT_C RSat::TServiceSearchRspV2::TServiceSearchRspV2()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iAdditionalInfo()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	}

EXPORT_C void RSat::NotifyServiceSearchPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a SERVICE SEARCH proactive command is
 * sent by the UICC.  When the request completes, the command is placed into aPCmd
 * and the aReqStatus is completed.  Unpack the command
 * to examine its contents.
 *
 * The command is described fully in ETSI TS 102 223 and 3GPP TS 31.111.
 *
 * Symbian OS support for this command is available from v7.0s onwards.
 *
 * You need to give a response to command back to the UICC. For Symbian OS
 * versions from 7.0s to v8.1a and v8.1b pass RSat::TerminalRsp() a
 * RSat::TServiceSearchRspV2Pckg (a packaged TServiceSearchRspV2).  For later versions of
 * Symbian OS, check for an updated API (RSat::TServiceSearchRspV3 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.      Use a packaged version of the latest "TServiceSearch" class. For
 * Symbian OS 7.0s, 8.0 and 8.1 use RSat::TServiceSearchV2Pckg, a packaged
 * RSat::TServiceSearchV2. For Symbian OS version 9.2, use RSat::TServiceSearchV5Pckg.
 * For Symbian OS version 9.4, use RSat::TServiceSearchV6Pckg.
 * For later versions of Symbian OS, check for an updated API
 * (RSat::TServiceSearchV7 etc).
 *
 * @see RSat::TServiceSearchV2
 * @see RSat::TServiceSearchV5
 * @see RSat::TServiceSearchV6
 * @see RSat::TServiceSearchRspV2
 * @see RSat::NotifyGetServiceInfoPCmd()
 * @see RSat::NotifyDeclareServicePCmd()
 *
 * @capability ReadDeviceData
 */
	{
	Get(ESatNotifyServiceSearchPCmd,aReqStatus,aPCmd);
	}


EXPORT_C RSat::TGetServiceInfoV2::TGetServiceInfoV2()
	: TSatPCmdBase()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	iAlphaId.iStatus=EAlphaIdNotSet;
	iIconId.iQualifier=EIconQualifierNotSet;
	iIconId.iIdentifier=0;
	iAttributeInfo.iBearerId=ELocalBearerIdNotSet;
	}

EXPORT_C RSat::TGetServiceInfoV5::TGetServiceInfoV5()
	: TGetServiceInfoV2()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV5;
	iTextAttribute.iStatus = ETextAttributeNotSet;
	}

EXPORT_C RSat::TGetServiceInfoV6::TGetServiceInfoV6()
	: TGetServiceInfoV5(),iFrameId(0)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV6;
	}
	
EXPORT_C RSat::TGetServiceInfoRspV2::TGetServiceInfoRspV2()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iAdditionalInfo()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	iRecordInfo.iBearerId=ELocalBearerIdNotSet;
	iRecordInfo.iServiceId=0;
	}

EXPORT_C void RSat::NotifyGetServiceInfoPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a GET SERVICE INFO proactive command is
 * sent by the UICC.  When the request completes, the command is placed into aPCmd
 * and the aReqStatus is completed.  Unpack the command
 * to examine its contents.
 *
 * The command is described fully in ETSI TS 102 223 and 3GPP TS 31.111.
 *
 * Symbian OS support for this command is available from v7.0s onwards.
 *
 * You need to give a response to command back to the UICC. For Symbian OS
 * versions from 7.0s to v8.1a and v8.1b pass RSat::TerminalRsp() a
 * RSat::TGetServiceInfoRspV2Pckg (a packaged TGetServiceInfoRspV2).  For later versions of
 * Symbian OS, check for an updated API (RSat::TGetServiceInfoRspV3 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.  Use a packaged version of the latest "TGetServiceInfo" class. For
 * Symbian OS 7.0s, 8.0 and 8.1 use RSat::TGetServiceInfoV2Pckg, a packaged
 * RSat::TGetServiceInfoV2. For Symbian OS version 9.2, use RSat::TGetServiceInfoV5Pckg.
 * For Symbian OS version 9.4, use RSat::TGetServiceInfoV6Pckg.
 * For later versions of Symbian OS, check for an updated API
 * (RSat::TGetServiceInfoV7 etc).
 *
 * @see RSat::TGetServiceInfoV2
 * @see RSat::TGetServiceInfoV5
 * @see RSat::TGetServiceInfoV6
 * @see RSat::TGetServiceInfoRspV2
 * @see RSat::NotifyServiceSearchPCmd()
 * @see RSat::NotifyDeclareServicePCmd()
 *
 * @capability ReadDeviceData
 */
	{
	Get(ESatNotifyGetServiceInfoPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TDeclareServiceV2::TDeclareServiceV2()
	: TSatPCmdBase(),
	  iType(EDeclarationTypeNotSet)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	iServiceRecord.iBearerId=ELocalBearerIdNotSet;
	iServiceRecord.iServiceId=0;
	iInterface.iTransportProto=EProtocolNotSet;
	iInterface.iPrtNumber=0;
	}

EXPORT_C RSat::TDeclareServiceRspV2::TDeclareServiceRspV2()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iAdditionalInfo()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV2;
	}

EXPORT_C void RSat::NotifyDeclareServicePCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a DECLARE SERVICE proactive command is
 * sent by the UICC.  When the request completes, the command is placed into aPCmd
 * and the aReqStatus is completed.  Unpack the command
 * to examine its contents.
 *
 * The command is described fully in ETSI TS 102 223 and 3GPP TS 31.111.
 *
 * Symbian OS support for this command is available from v7.0s onwards.
 *
 * You need to give a response to command back to the UICC. For Symbian OS
 * versions from 7.0s to v8.1a and v8.1b pass RSat::TerminalRsp() a
 * RSat::TDeclareServiceRspV2Pckg (a packaged TDeclareServiceRspV2).  For later versions of
 * Symbian OS, check for an updated API (RSat::TDeclareServiceRspV3 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.      Use a packaged version of the latest "TDeclareService" class. For
 * Symbian OS 7.0s, 8.0 and 8.1 use RSat::TDeclareServiceV2Pckg, a packaged
 * RSat::TDeclareServiceV2. For later versions of Symbian OS, check for an updated API
 * (RSat::TDeclareServiceV3 etc).
 *
 * @see RSat::TDeclareServiceV2
 * @see RSat::TDeclareServiceRspV2
 * @see RSat::NotifyGetServiceInfoPCmd()
 * @see RSat::NotifyDeclareServicePCmd()
 *
 * @capability ReadDeviceData
 */
	{
	Get(ESatNotifyDeclareServicePCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TTimerMgmtV3::TTimerMgmtV3()
	: TSatPCmdBase()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV3;
	}

EXPORT_C RSat::TTimerMgmtRspV3::TTimerMgmtRspV3()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iAdditionalInfo()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV3;
	}

EXPORT_C void RSat::NotifyTimerMgmtPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a TIMER MANAGEMENT proactive
 * command is sent by the UICC.  When the request completes, the command is placed
 * into aPCmd and the aReqStatus is completed.  Unpack
 * the command to examine its contents.
 *
 * Support for this command is only available from v7.0s of Symbian OS onwards.
 *
 * The phone provides timers for the UICC.  Using proactive commands, the UICC
 * can start, stop and read the current value of timers.  Timers are started with a
 * particular value.  When this counts down to zero, send a Timer Expiration
 * envelope command to the UICC; see RSat::NotifyTimerExpiration(). You must
 * manage up to 8 timers for the UICC.
 *
 * The command is described fully in ETSI TS 102 223, 3GPP2 C.S0035-0 and 3GPP TS
 * 31.111. Briefly, though, TIMER MANAGEMENT commands are of three
 * types:
 *
 * - Start a timer at the specified value - RSat::TTimerMgmtV3::iOperation is
 *     RSat::EStartTimer.  The timer counts down from the value in
 *     RSat::TTimerMgmtV3::iTimerValue
 * - Stop a timer and return the timer's value when it was stopped -
 *     RSat::TTimerMgmtV3::iOperation is RSat::EDeactivateTimer.
 * - Return a timer's value to the UICC without stopping it -
 *     RSat::TTimerMgmtV3::iOperation is RSat::EGetTimerValue.
 *
 * Each command includes a timer identifier, RSat::TTimerMgmtV3::iTimerId, so that
 * you know which timer to use.
 *
 * After you have tried to stop, start or read a timer, respond to the UICC as
 * follows:  In Symbian OS versions 7.0s, 8.1a & 8.1b, pass RSat::TerminalRsp() a
 * RSat::TTimerMgmtRspV3Pckg (a packaged TTimerMgmtRspV3).  For later versions of
 * Symbian OS, check for an updated API (RSat::TTimerMgmtRspV4 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.  aPCmd must be a packaged version of the  latest "TTimerMgmt" class.
 * For Symbian OS versions up to and including v8.1a and 8.1b use
 * RSat::TTimerMgmtV3Pckg, a packaged RSat::TTimerMgmtV3.  For later versions of
 * Symbian OS, check for an updated API (RSat::TTimerMgmtV4 etc).
 *
 * @see RSat::TTimerMgmtV3
 * @see RSat::TTimerMgmtRspV3
 * @see RSat::NotifyTimerExpiration()
 *
 * @capability None
 */
	{
	Get(ESatNotifyTimerMgmtPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TLocalInfoV3::TLocalInfoV3()
	: TSatPCmdBase()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV3;
	}

EXPORT_C RSat::TLocalInfoV6::TLocalInfoV6()
	: TLocalInfoV3(),iUtranMeasurementQualifier(EUtranMeasurementQualifierNotSet)
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV6;
	}
	
EXPORT_C RSat::TLocalInfoRspV3::TLocalInfoRspV3()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iAdditionalInfo()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV3;
	}

EXPORT_C void RSat::NotifyLocalInfoPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a PROVIDE LOCAL INFORMATION proactive
 * command is sent by the UICC.  When the request completes, the command is placed
 * into aPCmd and the aReqStatus is completed.  Unpack
 * the command to examine its contents.
 *
 * Support for this command is only available from v7.0s of Symbian OS onwards.
 *
 * The PROVIDE LOCAL INFORMATION command instructs the phone to send local
 * information to the UICC.  The command is described fully in ETSI TS 102 223,
 * 3GPP2 C.S0035-0 and 3GPP TS 31.111. Briefly, though, the command includes:
 *
 * - The type of information required - RSat::TLocalInfoV3::iInfoType
 * - The device for which the information is required - RSat::TLocalInfoV3::iDevideId
 * (note the spelling mistake in the member variable's name).
 *
 * The type of information depends upon the network on which the phone is running.
 * For GSM/WCDMA networks, read 3GPP TS 31.111; for CDMA networks read 3GPP2
 * C.S0035-0.  ETSI TS 102 223 only provides generic information about the PROVIDE
 * LOCAL INFORMATION command.
 *
 * Provide the information (or an error code) in the format described in the
 * specifications.  In Symbian OS versions up to and including 8.1a & 8.1b, pass
 * RSat::TerminalRsp() a RSat::TLocalInfoRspV3Pckg (a packaged
 * RSat::TLocalInfoRspV3).  For later versions of Symbian OS, check for an updated API
 * (RSat::TLocalInfoRspV4 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.  aPCmd must be a packaged version of the  latest "TLocalInfo" class.
 * For Symbian OS versions up to and including v8.1a and 8.1b use
 * RSat::TLocalInfoV3Pckg, a packaged RSat::TLocalInfoV3.  
 * For Symbian OS version 9.4, use RSat::TLocalInfoV6.
 * For later versions of Symbian OS, check for an updated API (RSat::TLocalInfoV7 etc).
 *
 * @see RSat::TLocalInfoV3
 * @see RSat::TLocalInfoV6
 * @see RSat::TLocalInfoRspV3
 *
 * @capability ReadDeviceData
 */
	{
	Get(ESatNotifyLocalInfoPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TPollingIntervalV3::TPollingIntervalV3()
	: TSatPCmdBase()
/**
* Default constructor.
*/{
	iExtensionId=KSatV3;
	}

EXPORT_C RSat::TPollingIntervalRspV3::TPollingIntervalRspV3()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iAdditionalInfo()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV3;
	}

EXPORT_C void RSat::NotifyPollingIntervalPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a POLL INTERVAL proactive
 * command is sent by the UICC.  When the request completes, the command is placed
 * into aPCmd and the aReqStatus is completed.  Unpack
 * the command to examine its contents.
 *
 * Support for this command is only available from v7.0s of Symbian OS onwards.
 *
 * The POLL INTERVAL command tells the phone how often the UICC would like to be
 * polled by the phone.  Each time it is polled, it can pass proactive commands to the phone.
 * The command is described fully in ETSI TS 102 223.  The command includes:
 *
 * - The requested polling interval - RSat::TPollingIntervalV3::iIntv.
 * - The device for which the polling is required - RSat::TPollingIntervalV3::iDestination.
 *
 * It is up to the phone to choose the polling interval; the value in the
 * proactive command is only a request.  It should choose the closest acceptable
 * value.  The phone should tell the UICC what interval it has chosen to use as follows:
 *
 * In Symbian OS versions up to and including 8.1a & 8.1b, pass
 * RSat::TerminalRsp() a RSat::TPollingIntervalRspV3Pckg (a packaged
 * RSat::TPollingIntervalRspV3).  For later versions of Symbian OS, check for an updated API
 * (RSat::TPollingIntervalRspV4 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.  aPCmd must be a packaged version of the  latest "TPollingInterval" class.
 * For Symbian OS versions from v7.0s to  v8.1a and 8.1b use
 * RSat::TPollingIntervalV3Pckg, a packaged RSat::TPollingIntervalV3.  For later versions of
 * Symbian OS, check for an updated API (RSat::TPollingIntervalV4 etc).
 *
 * @see RSat::TPollingIntervalV3
 * @see RSat::TPollingIntervalRspV3
 *
 * @capability None
 */
	{
	Get(ESatNotifyPollingIntervalPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TPollingOffV3::TPollingOffV3()
	: TSatPCmdBase()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV3;
	}

EXPORT_C RSat::TPollingOffRspV3::TPollingOffRspV3()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),
	  iInfoType(0),
	  iAdditionalInfo()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV3;
	}

EXPORT_C void RSat::NotifyPollingOffPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This method requests notification when a POLLING OFF proactive
 * command is sent by the UICC.  When the request completes, the command is placed
 * into aPCmd and the aReqStatus is completed.  Unpack
 * the command to examine its contents.
 *
 * Support for this command is only available from v7.0s of Symbian OS onwards.
 *
 * The POLLING OFF command tells the phone not to poll the UICC for proactive
 * commands any more. The command is described fully in ETSI TS 102 223.  The
 * command includes the device for which polling should be turned off -
 * RSat::TPollingOffV3::iDestination.
 *
 * Once you have dealt with the command, you must give your response to the UICC.
 * In Symbian OS versions up to and including 8.1a & 8.1b, pass
 * RSat::TerminalRsp() a RSat::TPollingOffRspV3Pckg (a packaged
 * RSat::TPollingOffRspV3).  For later versions of Symbian OS, check for an updated
 * API (RSat::TPollingOffRspV4 etc).
 *
 * @param aReqStatus The aReqStatus is completed when the proactive command is received
 * @param aPCmd When the request completes, this buffer will contain the proactive
 * command.  aPCmd must be a packaged version of the  latest "TPollingOff" class.
 * For Symbian OS versions from v7.0s to v8.1a and 8.1b use
 * RSat::TPollingOffV3Pckg, a packaged RSat::TPollingOffV3.  For later versions of
 * Symbian OS, check for an updated API (RSat::TPollingOffV4 etc).
 *
 * @see RSat::TPollingOffV3
 * @see RSat::TPollingOffRspV3
 *
 * @capability None
 */
	{
	Get(ESatNotifyPollingOffPCmd,aReqStatus,aPCmd);
	}

EXPORT_C RSat::TSatSmsV1::TSatSmsV1()
	: TSatPCmdBase(),
	  iBuf(0)
/**
* Default constructor.
*/
	{
	}

EXPORT_C void RSat::SendMessageNoLogging(TRequestStatus& aReqStatus, const TDesC8& aMsg, TUint16& aMsgRef) const
/**
 * This method sends a SAT SMS without it being logged.
 *
 * See RSat::NotifySendSmPCmd() for more information.
 *
 * @param aReqStatus This method is asynchronous.  aReqStatus indicates when the operation has completed.
 * @param aMsg TSatSmsV1 packaged into a descriptor, TSatSmsV1Pckg.
 * @capability NetworkServices
 * @capability WriteDeviceData
 */
	{
	__ASSERT_ALWAYS(iEtelSatPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	TPtr8& ptr1=iEtelSatPtrHolder->Set(ESlotMsgRef,aMsgRef);

	SetAndGet(ESatSendMessageNoLogging,aReqStatus,aMsg,ptr1);
	}

EXPORT_C void RSat::NotifyProactiveSimSessionStart(TRequestStatus& aReqStatus) const
/**
 * This function is no longer needed when using the ETelSat API.
 * @capability None
 */
	{
	Blank(ESatNotifyProactiveSimSessionStart,aReqStatus);
	}

EXPORT_C void RSat::NotifyProactiveSimSessionEnd(TRequestStatus& aReqStatus) const
/**
 * This function is no longer needed when using the ETelSat API.
 * @capability None
 */
	{
	Blank(ESatNotifyProactiveSimSessionEnd,aReqStatus);
	}

EXPORT_C void RSat::NotifyTsyStateUpdated(TRequestStatus& aReqStatus, TPCmd& aPCmd) const
/**
 * This notification completes when the TSY has finished its handling of a proactive command.
 * This should be used for proactive commands that are handled by the TSY
 * and not by the ETelSat client.  It informs the ETelSat client when the
 * TSY has completed the required actions of the command.
 *
 * @param aReqStatus This method is asynchronous.  aReqStatus indicates when the operation has completed.
 * @param aPCmd Upon completion, aPcmd contains an RSat::TPCmd indicating the proactive command that has completed.
 * @capability None
 */
 	{
 	__ASSERT_ALWAYS(iEtelSatPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	TPtr8& ptr1=iEtelSatPtrHolder->Set(ESlot1NotifyTsyStateUpdated,aPCmd);

 	Get(ESatNotifyTsyStateUpdated,aReqStatus,ptr1);
 	}

EXPORT_C void RSat::NotifyRefreshRequired(TRequestStatus& aReqStatus) const
/**
 * This notification completes when SAT engine indicates that it can complete the required refresh.
 * This is phase 1 of a two phase refresh cycle with RSat::NotifyRefreshPCmd() being phase 2. The TSY should inform
 * the UICC via terminal response should refresh not be possible at this time. This is intended to be used
 * only by the SAT engine.
 *
 * See RSat::NotifyRefreshPCmd() for more information.
 *
 * @return aReqStatus TRequestStatus result code after the asynchronous call completes
 * @capability None
 */
 	{
 	Blank(ESatNotifyRefreshRequired, aReqStatus);
 	}

EXPORT_C void RSat::NotifyRefreshRequired(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * This notification completes when SAT Engine indicates that it can complete the required refresh.
 * This is phase 1 on a two phase refresh cycle with NotifyRefreshPCmd being phase 2. The TSY should inform
 * the UICC via terminal response should refresh not be possible at this time. This is intended to be used
 * only by the SAT Engine.
 *
 * @return aReqStatus TRequestStatus result code after the asynchronous call completes
 * @param aPCmd TRefreshV1 packaged into a descriptor, TRefreshV1Pckg.
 * @capability None
 */
 	{
	Get(ESatNotifyRefreshRequiredParam,aReqStatus,aPCmd);
 	}

EXPORT_C void RSat::RefreshAllowed(TRequestStatus& aReqStatus, const TDesC8& aRefreshAllowedRsp) const
/**
 * This method allows client to report whether refresh can be carried out. This is intended to be used
 * only by the SAT engine.
 *
 * See RSat::NotifyRefreshPCmd() for more information.
 *
 * @param aReqStatus This method is asynchronous.  aReqStatus indicates when the operation has completed.
 * @param aRefreshAllowedRsp Indication that successful refresh can be carried out or nature of error if not
 * @capability WriteDeviceData
 */
 	{
	Set(ESatRefreshAllowed, aReqStatus, aRefreshAllowedRsp);
 	}

EXPORT_C RSat::TCallControlV5::TCallControlV5()
	: TCallControlV2()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV5;
	}

EXPORT_C void RSat::TCallControlV5::SetPdpParameters(const TPdpParameters& aPdpParams)
/**
 * This method is used by the TSY to set the PDP parameters within the TCallControlV5 class
 *
 * This method sets iType to ECcPDPParameters, to specify that the call control has transformed
 * a outstanding PDP Context Activate request to a new one.
 *
 * @param aPdpParameters PDP parameters.
 */
	{
	iType=ECcPDPParameters;
	iPdpParameters=aPdpParams;
	}

EXPORT_C void RSat::TCallControlV5::GetPdpParameters(TPdpParameters& aPdpParams) const
/**
 * This method is used by the Client to get the PDP Context activation parameters.
 * If the call control type is not ECcPdpParameters, the client should not use this method.
 *
 * @param aPdpParams PDP context parameters.
 */
	{
	aPdpParams=iPdpParameters;
	}

EXPORT_C RSat::TCallControlV6::TCallControlV6()
: TCallControlV5()
/**
* Default constructor.
*/
	{
	iExtensionId=KSatV6;
	}

EXPORT_C void RSat::TCallControlV6::SetCallParamOrigin (TCallParamOrigin aCallParamOrigin)
/**
 * This method is used by the TSY to set the call originator 
 * within the TCallControlV6 class.
 * 
 * The TSY should set this to the same value as that in the 
 * incoming call parameters.
 * 
 * @param aCallParamOrigin The originator of the call.
 * 
 * @see RSat::TCallControlV6::iCallParamOrigin
 */ 
	{
	iCallParamOrigin = aCallParamOrigin;
	}

EXPORT_C void RSat::TCallControlV6::GetCallParamOrigin (TCallParamOrigin& aCallParamOrigin) const
/**
 * Retrieves the call parameter origin.
 * 
 * @param aCallParamOrigin Is populated with the call originator when
 * the function returns.
 */ 
	{
	aCallParamOrigin = iCallParamOrigin;
	}

EXPORT_C RSat::TRetrieveMultimediaMessageV6::TRetrieveMultimediaMessageV6()
	: TSatPCmdBase(),
      iDestination(KDeviceIdNotSet)
/**
* Default constructor
*/
 	{
	iExtensionId=KSatV6;
	iAlphaId.iStatus=EAlphaIdNotSet;
	iIconId.iIdentifier=0;
	iTextAttribute.iStatus=ETextAttributeNotSet;	
	}

EXPORT_C void RSat::NotifyRetrieveMultimediaMsgPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * The RETRIEVE MULTIMEDIA MESSAGE command tells the phone to retrieve a Multimedia file from Network.
 *
 * The command is described fully in 3GPP TS 31.111. Note that it does not appear in ETSI TS
 * 102 223.  It contains:  
 * - An alpha identifier containing text to display - RSat::TRetrieveMultimediaMessageV6::iAlphaId.
 * - An icon to display - RSat::TRetrieveMultimediaMessageV6::iIconId 
 * - Text attributes of text contained in iAlphaId - RSat::TRetrieveMultimediaMessageV6::iTextAttribute 
 * - Location in the UICC where the file needed to be stored - RSat::TFileList
 * - It contains the information,which refers to the location of the content of the multimedia file to be 
 *   retrieved - RSat::TRetrieveMultimediaMessageV6::iMultimediaMessageRef
 * - This contains the Data Object tag to be used when the MM Content is stored in the referenced 
 *   BER-TLV file - RSat::TRetrieveMultimediaMessageV6::iMultimediaContentId
 * - Multimedia Message Identifier is the identifier of the Multimedia Message within the MMS Reception File,
 *   It is mandatory when the file supports multiple MMs - RSat::TRetrieveMultimediaMessageV6::iMultimediaMessageId
 *
 * This asynchronous request is a notifier that completes when the UICC issues a 
 * RETRIEVE MULTIMEDIA MESSAGE PCmd. Upon completion the proactive command details 
 * will be returned in a packaged version of TRetrieveMultimediaMessageV6 in the 
 * aPCmd parameter.
 *
 * To cancel a previously placed RSat::NotifyRetrieveMultimediaMsgPCmd request, 
 * call RTelSubSessionBase::CancelAsyncRequest(ESatNotifyRetrieveMultimediaMsgPCmd)
 *
 * Once you have dealt with the command, you must give your response to the UICC.
 * In Symbian OS versions beginning from 9.4, pass
 * RSat::TerminalRsp() a RSat::TRetrieveMultimediaMessageRspV6Pckg (a packaged
 * RSat::TRetrieveMultimediaMessageRspV6).  For later versions of Symbian OS, check for an updated
 * API (RSat::TRetrieveMultimediaMessageRspV7 etc).
 *
 * @param aReqStatus TRequestStatus result code after the asynchronous call completes. 
 *  Set to KErrNone on successful completion; another error code otherwise.
 * @param aPCmd a RSat::TRetrieveMultimediaMessageV6, packaged into a RSat::TRetrieveMultimediaMessageV6Pckg. 
 *  Get filled with the details of the proactive command on completion of this notification.
 * @capability ReadDeviceData
 */	
	{
	Get(ESatNotifyRetrieveMultimediaMsgPCmd, aReqStatus, aPCmd);
	}

EXPORT_C RSat::TRetrieveMultimediaMessageRspV6::TRetrieveMultimediaMessageRspV6()
	: TSatPCmdBase(),
  	  iGeneralResult(KPCmdResultNotSet),iInfoType(0),iAdditionalInfo()
/**
* Default constructor
*/	
	{
	iExtensionId=KSatV6;
	}

EXPORT_C RSat::TSubmitMultimediaMessageV6::TSubmitMultimediaMessageV6()
	: TSatPCmdBase(),
	  iDestination(KDeviceIdNotSet)
/**
* Default constructor
*/
	{
	iExtensionId=KSatV6;
	iAlphaId.iStatus=EAlphaIdNotSet;
	iIconId.iIdentifier=0;
	iTextAttribute.iStatus=ETextAttributeNotSet;
	}

EXPORT_C void RSat::NotifySubmitMultimediaMsgPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * The SUBMIT MULTIMEDIA MESSAGE command tells the phone to Submit a Multimedia file to the Network.
 *
 * The command is described fully in 3GPP TS 31.111. Note that it does not appear in ETSI TS
 * 102 223.  It contains:  
 * - An alpha identifier containing text to display - RSat::TSubmitMultimediaMessageV6::iAlphaId.
 * - An icon to display - RSat::TSubmitMultimediaMessageV6::iIconId 
 * - Text attributes of text contained in iAlphaId - RSat::TSubmitMultimediaMessageV6::iTextAttribute 
 * - Location in the UICC from where the file should be retrieved for submission - RSat::TSubmitMultimediaMessageV6::iSubmissionFile
 * - Multimedia Message Identifier is the identifier of the MMs within the MMS Submission Files,
 *   It is mandatory when the file supports multiple MMs - RSat::TSubmitMultimediaMessageV6::iMultimediaMessageId
 * 
 * This asynchronous request is a notifier that completes when the UICC issues a 
 * SUBMIT MULTIMEDIA MESSAGE PCmd. Upon completion the proactive command details 
 * will be returned in a packaged version of TSubmitMultimediaMessageV6 in the 
 * aPCmd parameter.
 *
 * To cancel a previously placed RSat::NotifySubmitMultimediaMsgPCmd request, 
 * call RTelSubSessionBase::CancelAsyncRequest(ESatNotifySubmitMultimediaMsgPCmd)
 *
 * Once you have dealt with the command, you must give your response to the UICC.
 * In Symbian OS versions beginning from 9.4, pass RSat::TerminalRsp() a RSat::TSubmitMultimediaMessageRspV6Pckg (a packaged
 * RSat::TSubmitMultimediaMessageRspV6).  For later versions of Symbian OS, check for an updated
 * API (RSat::TSubmitMultimediaMessageRspV7 etc).
 *
 * @param aReqStatus TRequestStatus result code after the asynchronous call completes
 * @param aPCmd RSat::TSubmitMultimediaMessageV6, packaged into RSat::TSubmitMultimediaMessageV6Pckg
 * @capability ReadDeviceData
 */	
	{
	Get(ESatNotifySubmitMultimediaMsgPCmd, aReqStatus, aPCmd);
	}

EXPORT_C RSat::TSubmitMultimediaMessageRspV6::TSubmitMultimediaMessageRspV6()
	: TSatPCmdBase(),
 	  iGeneralResult(KPCmdResultNotSet),iInfoType(0),iAdditionalInfo()
/**
* Default constructor
*/	
	{
	iExtensionId=KSatV6;
	}

EXPORT_C RSat::TDisplayMultimediaMessageV6::TDisplayMultimediaMessageV6()
	: TSatPCmdBase(),
	  iDestination(KDeviceIdNotSet),
	  iDisplayPriority(EDisplayPriorityNotSet),
	  iClearScreenTrigger(EClearScreenTriggerNotSet),
	  iImmediateRsp(EImmediateRspNotSet)
/**
* Default constructor
*/
	{
	iExtensionId=KSatV6;
	}

EXPORT_C void RSat::NotifyDisplayMultimediaMsgPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * The DISPLAY MULTIMEDIA MESSAGE command tells the phone to Display the Multimedia file to the User.
 *
 * The command is described fully in 3GPP TS 31.111. Note that it does not appear in ETSI TS
 * 102 223.  It contains:  
 * - This determines the text's priority - RSat::TDisplayMultimediaMessageV6::iDisplayPriority.
 * - Determines whether the text should be removed from the screen after a delay or after the 
 *   user's action - RSat::TDisplayMultimediaMessageV6::iClearScreenTrigger 
 * - Location in the UICC from where the file should be retrieved for submission - RSat::TDisplayMultimediaMessageV6::iSubmissionFile
 * - Multimedia Message Identifier is the identifier of the MMs within the MMS Submission Files,
 *   It is mandatory when the file supports multiple MMs - RSat::iMultimediaMessageId
 * - It indicates whether or not the (U)SAT UI client should sustain the display beyond sending the 
 *   Terminal Response - RSat::TDisplayMultimediaMessageV6::iImmediateRsp
 *
 * This asynchronous request is a notifier that completes when the UICC issues a 
 * DISPLAY MULTIMEDIA MESSAGE PCmd. Upon completion the proactive command details 
 * will be returned in a packaged version of TDisplayMultimediaMessageV6 in the 
 * aPCmd parameter.
 *
 * To cancel a previously placed RSat::NotifyDisplayMultimediaMsgPCmd request, 
 * call RTelSubSessionBase::CancelAsyncRequest(ESatNotifyDisplayMultimediaMsgPCmd)
 *
 * Once you have dealt with the command, you must give your response to the UICC.
 * In Symbian OS versions up to and including 9.4, pass RSat::TerminalRsp() a RSat::TDisplayMultimediaMessageRspV6Pckg (a packaged
 * RSat::TDisplayMultimediaMessageRspV6).  For later versions of Symbian OS, check for an updated
 * API (RSat::TDisplayMultimediaMessageRspV7 etc).
 *
 * @return aReqStatus TRequestStatus result code after the asynchronous call completes
 * @param aPCmd RSat::TDisplayMultimediaMessageV6, packaged into RSat::TDisplayMultimediaMessageV6Pckg
 * @capability ReadDeviceData
 */	
	{
	Get(ESatNotifyDisplayMultimediaMsgPCmd, aReqStatus, aPCmd);
	}

EXPORT_C RSat::TDisplayMultimediaMessageRspV6::TDisplayMultimediaMessageRspV6()
	: TSatPCmdBase(),
	  iGeneralResult(KPCmdResultNotSet),iInfoType(0),iAdditionalInfo()
/**
* Default constructor
*/	
	{
	iExtensionId=KSatV6;
	}

EXPORT_C RSat::TSetFramesV6::TSetFramesV6()
	:TSatPCmdBase(),iDestination(KDeviceIdNotSet)
/**
* Default constructor
*/
	{
	iExtensionId=KSatV6;	
	}

EXPORT_C void RSat::NotifySetFramesPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * SET FRAMES command instructs the terminal to divide the terminal screen into multiple, 
 * scrollable rectangular regions called frames inorder to present multiple information at once.
 * It can be applied to entire screen or to an already existing frame, dividing this frame into sub-frames
 *
 * The command is described fully in 3GPP TS 31.111. Note that it does not appear in ETSI TS
 * 102 223.  It contains:  
 * - The indicator tells the terminal whether or not to draw a separator between every 
 *   adjoining frame - RSat::TSetFramesV6::iFrameSeparator
 * - This value is used to indentify the frame, a value of '00' refers to the entire terminal's 
 *   screen.Any other value refers to an existing frame - RSat::TSetFramesV6::iFrameId 
 * - It contains the layout information of the frame - RSat::TSetFramesV6::iFrameLayout
 * - This is optional. It contains an indication of the frame to be used to display information 
 *   in case a Frame Identifier is not included - RSat::TSetFramesV6::iDefaultFrameId
 * 
 * This asynchronous request is a notifier that completes when the UICC issues a 
 * SET FRAMES PCmd. Upon completion the proactive command details 
 * will be returned in a packaged version of TSetFramesV6 in the 
 * aPCmd parameter.
 *
 * To cancel a previously placed RSat::NotifySetFramesPCmd request, 
 * call RTelSubSessionBase::CancelAsyncRequest(ESatNotifySetFramesPCmd)
 *
 * Once you have dealt with the command, you must give your response to the UICC.
 * In Symbian OS versions up to and including 9.4, pass RSat::TerminalRsp() a RSat::TSetFramesRspV6Pckg (a packaged
 * RSat::TSetFramesRspV6).  For later versions of Symbian OS, check for an updated
 * API (RSat::TSetFramesRspV7 etc).
 *
 * @param aReqStatus TRequestStatus result code after the asynchronous call completes
 * @param aPCmd RSat::TSetFramesV6, packaged into RSat::TSetFramesV6Pckg
 * @capability ReadDeviceData
 */
	{
	Get(ESatNotifySetFramesPCmd, aReqStatus, aPCmd);
	}

EXPORT_C RSat::TSetFramesRspV6::TSetFramesRspV6()
	:TSatPCmdBase(),
	 iGeneralResult(KPCmdResultNotSet),iInfoType(0),iAdditionalInfo()
/**
* Default constructor
*/
	{
	iExtensionId=KSatV6;
	iFramesInformation.iFrameId=0;
	}

EXPORT_C RSat::TGetFramesStatusV6::TGetFramesStatusV6()
	:TSatPCmdBase(),iDestination(KDeviceIdNotSet)
/**
* Default constructor
*/	
	{
	iExtensionId=KSatV6;
	}

EXPORT_C void RSat::NotifyGetFramesStatusPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const
/**
 * GET FRAMES STATUS command requests the terminal to return a Frames parameters data object.
 *
 * The command is described fully in 3GPP TS 31.111. Note that it does not appear in ETSI TS
 * 102 223.  It contains:  
 * - Device identification - RSat::TGetFramesStatusV6::iDestination
 * Here,terminal is the destination, It sends the frames information in the terminal response.
 *
 * This asynchronous request is a notifier that completes when the UICC issues a 
 * GET FRAMES STATUS PCmd. Upon completion the proactive command details 
 * will be returned in a packaged version of TGetFramesStatusV6 in the 
 * aPCmd parameter.
 *
 * To cancel a previously placed RSat::NotifyGetFramesStatusPCmd request, 
 * call RTelSubSessionBase::CancelAsyncRequest(ESatNotifyGetFramesStatusPCmd)
 *
 * Once you have dealt with the command, you must give your response to the UICC.
 * In Symbian OS versions up to and including 9.4, pass RSat::TerminalRsp() a RSat::TGetFramesStatusRspV6Pckg (a packaged
 * RSat::TGetFramesStatusRspV6).  For later versions of Symbian OS, check for an updated
 * API (RSat::TGetFramesStatusRspV7 etc).
 *
 * @param aReqStatus TRequestStatus result code after the asynchronous call completes
 * @param aPCmd RSat::TGetFramesStatusV6, packaged into RSat::TGetFramesStatusV6Pckg
 * @capability None
 */
	{
	Get(ESatNotifyGetFramesStatusPCmd, aReqStatus, aPCmd);
	}

EXPORT_C RSat::TGetFramesStatusRspV6::TGetFramesStatusRspV6()
	:TSatPCmdBase(),
	iGeneralResult(KPCmdResultNotSet),iInfoType(0),iAdditionalInfo()
/**
* Default constructor
*/	
	{
	iExtensionId=KSatV6;
	iFramesInformation.iFrameId=0;
	}

EXPORT_C RSat::TMmsNotificationDownloadV6::TMmsNotificationDownloadV6() 
	: TSatBase(), iDeviceId(KDeviceIdNotSet)
/**
* Default constructor
*/
	{
	iExtensionId=KSatV6;
	}

EXPORT_C void RSat::MmsNotificationDownload(TRequestStatus& aReqStatus, const TDesC8& aMmsNotificationDownload) const
/**
 * This method allows the client to pass the MM1_notification.REQ message to the UICC upon 
 * receiving the MMS notification intended for the UICC. The MMS Notification download envelope 
 * command details are passed in the packaged version of TMmsNotificationDownloadV6 in 
 * the aMmsNotificationDownload parameter.
 *
 * The command is described fully in 3GPP TS 31.111. Note that it does not appear in ETSI TS
 * 102 223.  It contains:  
 * - Additional information requested by the network to retrieve a MM is sent 
 * to UICC - RSat::TMmsNotificationDownloadV6::iMMSNotification
 * 
 * - Indicates the last envelope sent to transmit the MMS notification into 
 * the card - RSat::TMmsNotificationDownloadV6::iLastEnvelope
 *
 * To cancel a previously placed RSat::MmsNotificationDownload request, 
 * call RTelSubSessionBase::CancelAsyncRequest(ESatMmsNotificationDownload)
 *
 * @param aReqStatus TRequestStatus result code after the asynchronous call completes
 * @param aPCmd RSat::TMMSNotificationDownloadV6, packaged into RSat::TMMSNotificationDownloadV6Pckg
 * @capability NetworkControl
 * @capability WriteDeviceData
 */
	{
	Set(ESatMmsNotificationDownload, aReqStatus, aMmsNotificationDownload);
	}

EXPORT_C RSat::TMmsTransferStatusV6::TMmsTransferStatusV6()
	:TSatBase(),iDeviceId(KDeviceIdNotSet)
/**
* Default constructor
*/
	{
	iExtensionId=KSatV6;
	}

EXPORT_C void RSat::MmsTransferStatus(TRequestStatus& aReqStatus, const TDesC8& aMmsTransferStatus) const
/**
 * This method allows the client to notify the UICC of the following: 
 *   -	the status of an MMS message submitted to the network.  
 *   -	the UICC storage completion of a retrieved MMS message.
 *
 * The command is described fully in 3GPP TS 31.111. Note that it does not appear in ETSI TS
 * 102 223.  It contains:  
 * - Location of the Submitted/Retrieved Multimedia file on the UICC - RSat::TMmsTransferStatusV6::iMMSTransferFile
 * - Transfer status of the file - RSat::TMmsTransferStatusV6::iMultimediaMessageStatus
 * - Multimedia Identifier is used when the file supports multiple MMs - RSat::TMmsTransferStatusV6::iMultimediaMessageId
 * It completes when the UICC informs the TSY that the download is complete. 
 *
 * To cancel a previously placed RSat::MmsTransferStatus request, 
 * call RTelSubSessionBase::CancelAsyncRequest(ESatMmsTransferStatus)
 *
 * @param aReqStatus TRequestStatus result code after the asynchronous call completes
 * @param aPCmd RSat::TMmsTransferStatusV6, packaged into RSat::TMmsTransferStatusV6Pckg
 * @capability NetworkControl
 * @capability WriteDeviceData
 */
	{
	Set(ESatMmsTransferStatus, aReqStatus, aMmsTransferStatus);
	}

EXPORT_C RSat::TUssdDataDownloadV6::TUssdDataDownloadV6()
	:TSatBase(),iDeviceId(KDeviceIdNotSet)
/**
* Default constructor
*/	
	{
	iExtensionId=KSatV6;
	}


EXPORT_C void RSat::UssdDataDownload(TRequestStatus& aReqStatus, const TDesC8& aDes) const
/**
 * This method allows the client to pass a USSD packet to the UICC upon receiving the USSD 
 * packet intended for the UICC. The USSD Data Download ENVELOPE command details are passed 
 * in the packaged version of TUssdDataDownloadV6 class in the aDes parameter
 * 
 * The command is described fully in 3GPP TS 31.111. Note that it does not appear in ETSI TS
 * 102 223.  It contains: 
 *  - The details of the Ussd Data Download - RSat::TUssdDataDownloadV6::iUssdString
 * 
 * To cancel a previously placed RSat::UssdDataDownload request, 
 * call RTelSubSessionBase::CancelAsyncRequest(ESatUssdDataDownload)
 *
 * @param aReqStatus TRequestStatus result code after the asynchronous call completes
 * @param aUssdDataDownload RSat::TUssdDataDownloadV6 packaged in a RSat::TUssdDataDownloadV6Pckg
 * @capability NetworkControl
 * @capability WriteUserData
 */
 	{
	Set(ESatUssdDataDownload, aReqStatus, aDes);
	}

