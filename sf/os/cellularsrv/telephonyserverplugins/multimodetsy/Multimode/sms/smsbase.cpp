// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// AT Command Class for Sms
// Also contains member functions used by different ATCommands in the GsmTsy.
// 
//

#include "mSLOGGER.H"
#include "ATIO.H"
#include "ATSTD.H"
#include "mSMSSTOR.H"
#include "Matstd.h"
#include "smsbase.H"
#include "smsutil.h"	// for CATSmsUtils

#include <exterror.h>		// for KErrGsmSMS... error codes

// Constants

//
// Macros
//
// This macro is used to help keep un-intereting details outside of the main code
// in this file. The macro uses logging functions and always prefixes writes to the 
// log with the name of the class which is implemented in this file. This macro must
// only be used by code in this file.
#ifdef __LOGDEB__
_LIT8(KLogEntry,"CATSmsCommands::%S\t%S");
#define LOCAL_LOGTEXT(function,text) {_LIT8(F,function);_LIT8(T,text);LOGTEXT3(KLogEntry,&F,&T);}
#else
#define LOCAL_LOGTEXT(function,text)
#endif

//
// CATSmsCommands definitions
//

CATSmsCommands::CATSmsCommands(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit,CPhoneGlobals* aGlobals)
	:CATCommands(aIo,aTelObject,aInit,aGlobals)
/**
 * C++ constructor
 */
 	{
	}

CATSmsCommands::~CATSmsCommands()
/**
 * C++ destructor
 */
 	{}

CATSmsCommands::TRequest CATSmsCommands::RequestATCompleted()
/**
 * This method is a query function which returns the TRequest value for a request which 
 * has completed, otherwise ENone is returned to denote that no request has completed.
 *
 * If this method returns that a request has completed then this method will also
 * reset the iRequestCompleted flag so that this method that the completion of a 
 * request is reported only once. 
 *
 * @return The TRequest value of the request that completed, otherwise ENone
 */
 	{
	LOCAL_LOGTEXT("RequestATCompleted","enter function");
	if(iRequestCompleted)
		{
		const TRequest tmp(iRequest);
		iRequest=ENone;
		iRequestCompleted=EFalse;
		return tmp;
		}

	return ENone;
	}

TBool CATSmsCommands::RequestATActive() const
	{
	LOCAL_LOGTEXT("RequestATActive","enter function");
	return iRequest!=ENone;
	}


void CATSmsCommands::RequestATCommandCancel()
/**
 */
	{
	LOCAL_LOGTEXT("RequestATCommandCancel","enter function");
	if(!RequestATActive())
		return;	// There's nothing to do if we're not active

	// Cancel operations
	LOCAL_LOGTEXT("RequestATCommandCancel","Cancelling...");
	iRequestCancel=ETrue;
	}

void CATSmsCommands::InitializeRequestData(TRequest aReq)
	{
	LOCAL_LOGTEXT("InitializeRequestData","enter function");

	iRequest=aReq;
	iRequestStage=0;	
	iRequestCompleted=EFalse;
	iRequestError=KErrNone;
	iRequestCancel=EFalse;
	}

TInt CATSmsCommands::RequestATCommand(TRequest aReq)
	{
	LOCAL_LOGTEXT("RequestATCommand","enter function");

	// Ensure we are not allready handling a request
	if(RequestATActive())
		{
		LOCAL_LOGTEXT("RequestATCommand","A request is already in progress, returning KErrInUse");
		return KErrInUse;
		}

	//Initialize request data
	InitializeRequestData(aReq);

	// Handle request 
	TInt ret(KErrNone);
	const TEventSource nullSource(TEventSource(-1)); 		// This fiddle is so we can pass a 'null' event source

	switch(aReq)
		{
	case EGetSCAFromPhone:
		ret=GetSCAFromPhone(nullSource);
		break;

	case ESetSCAInPhone:
		ret=SetSCAInPhone(nullSource);	
		break;

	case ESetPhoneToCMTMode:
		ret=SetPhoneToCMTMode(nullSource);
		break;

	case ESetPhoneToCMTIMode:
		ret=SetPhoneToCMTIMode(nullSource);
		break;

	case ENone:		// Must not be caught by default case
		LOCAL_LOGTEXT("RequestATCommand","Request is ENone, so will not do anything");
		break;
	
	default:
		LOCAL_LOGTEXT("RequestATCommand","Unknown request");
		__ASSERT_DEBUG(EFalse,Panic(EATSmsCommandsUnknownRequest));
		ret=KErrNotSupported;
		}

	LOCAL_LOGTEXT("RequestATCommand","exit function");
	return ret;
	}


TInt CATSmsCommands::SetSCAInPhone(const TEventSource& aEventSource)
/**
 * Sets phone's default SCA to be that which is stored in iRequestSCA.
 * IMPORTANT: If this method returns an error then EventSignal will complete our request.
 * @return Standard KErr... codes
 */
	{
	LOCAL_LOGTEXT("SetSCAInPhone","enter function");
	__ASSERT_DEBUG(iRequest==ESetSCAInPhone,Panic(EATSmsCommandsWrongRequest));

	TInt ret(KErrNone);
	switch(iRequestStage)
		{
	case EStage0:
		if(iRequestCancel)		// Handle request cancellation, if needed
			{
			LOCAL_LOGTEXT("SetSCAInPhone","Request has been cancelled");
			ret=KErrCancel;
			}
		else
			{
			// Set the SCA to use as the default in the phone's memory
			iTxBuffer.Copy(KSmsSetSCCommand);
			iTxBuffer.Append(KDblQuoteChar);
			const TInt count(iRequestSCA.iTelNumber.Length());
			for(TInt i=0;i<count;++i)
				{
				if(CATSmsUtils::IsAddressChar(TChar(iRequestSCA.iTelNumber[i])))
					iTxBuffer.Append(TChar(iRequestSCA.iTelNumber[i]));
				}
			iTxBuffer.Append(KDblQuoteChar);
			iTxBuffer.Append(KCommaChar);

			TUint val;
			CATSmsUtils::GetTypeOfAddressInDecimal(iRequestSCA,val);		
			iTxBuffer.AppendNum(val,EDecimal);

			iTxBuffer.Append(KCarriageReturn);
			iIo->Write(this,iTxBuffer);
			iIo->SetTimeOut(this,KATWriteTimeout);
			}
		break;
		
	case EStage1:
		HandleWriteCompletion(aEventSource);
		break;
	
	case EStage2:
		// If we did not get an error
		if(HandleResponseCompletion(aEventSource)==KErrNone)	
			CompleteRequest();
		else
			{
			if(iRequestCancel)		// Handle request cancellation, if needed
				{
				LOCAL_LOGTEXT("SetSCAInPhone","Request has been cancelled");
				ret=KErrCancel;
				}
			else
				{
				// An error occurred
				// Try to set SCA again, but this time don't use <tosca>
				// The Motorola Timeport needs this behaviour
				iTxBuffer.Copy(KSmsSetSCCommand);
				iTxBuffer.Append(KDblQuoteChar);

				// Prefix phone number with a '+' if it's international
				if(iRequestSCA.iTypeOfNumber==RMobilePhone::EInternationalNumber)
					iTxBuffer.Append(KPlusChar);

				const TInt count(iRequestSCA.iTelNumber.Length());
				for(TInt i=0;i<count;++i)
					{
					if(CATSmsUtils::IsAddressChar(TChar(iRequestSCA.iTelNumber[i])))
						iTxBuffer.Append(TChar(iRequestSCA.iTelNumber[i]));
					}
				iTxBuffer.Append(KDblQuoteChar);
				iTxBuffer.Append(KCarriageReturn);
				iIo->Write(this,iTxBuffer);
				iIo->SetTimeOut(this,KATWriteTimeout);
				}
			}
		break;

	case EStage3:
		HandleWriteCompletion(aEventSource);
		break;
	
	case EStage4:
		if((ret=HandleResponseCompletion(aEventSource))==KErrNone)
			{
			// We've finished the request
			CompleteRequest();
			}
		break;

	default:
		LOCAL_LOGTEXT("SetSCAInPhone","Unknown request stage");
		__ASSERT_DEBUG(EFalse,Panic(EATSmsCommandsUnknownRequestStage));
		ret=KErrGeneral;
		}

	++iRequestStage;		// Increment request stage for next iteration

	return ret;
	}


TInt CATSmsCommands::GetSCAFromPhone(const TEventSource& aEventSource)
/**
 * Gets phone's default SCA from phone and saves it in iRequestSCA.
 * IMPORTANT: If this method returns an error then EventSignal will complete our request.
 * @return Standard KErr... codes
 */
	{
	LOCAL_LOGTEXT("GetSCAFromPhone","enter function");
	__ASSERT_DEBUG(iRequest==EGetSCAFromPhone,Panic(EATSmsCommandsWrongRequest));
	
	TInt ret(KErrNone);
	switch(iRequestStage)
		{
	case EStage0:
		if(iRequestCancel)		// Handle request cancellation, if needed
			{
			LOCAL_LOGTEXT("GetSCAFromPhone","Request has been cancelled");
			ret=KErrCancel;
			}
		else
			{	
			// Initialize data
			iRequestSCA.iTypeOfNumber=RMobilePhone::EUnknownNumber;
			iRequestSCA.iNumberPlan=RMobilePhone::EUnknownNumberingPlan;
			iRequestSCA.iTelNumber.Zero();
	
			// Send SCA request to phone 
			iTxBuffer.Format(KServiceCentreQueryCommand);
			iIo->Write(this,iTxBuffer);
			iIo->SetTimeOut(this,KATWriteTimeout);
			}
		break;
		
	case EStage1:
		HandleWriteCompletion(aEventSource);
		break;
	
	case EStage2:
		ret=HandleResponseCompletion(aEventSource);
		if(ret!=KErrNone)
			break;

		// Did the phone have an SCA in default memory
		ret=ParseRxResultsForCSCAResponse(iRequestSCA);
		if(ret==KErrNone && iRequestSCA.iTelNumber.Length()>0)
			{
			// We've finished the request
			CompleteRequest();
			ret=KErrNone;
			}
		else
			{
			if(iRequestCancel)		// Handle request cancellation, if needed
				{
				LOCAL_LOGTEXT("GetSCAFromPhone","Request has been cancelled");
				ret=KErrCancel;
				}
			else
				{
				// Check if the phone has a SCA after doing a memory refresh
				iTxBuffer.Format(KServiceCentreQueryCommandWithCRES);
				iIo->Write(this,iTxBuffer);
				iIo->SetTimeOut(this,KATWriteTimeout);
				}
			}
		break;

	case EStage3:
		HandleWriteCompletion(aEventSource);
		break;

	case EStage4:
		ret=HandleResponseCompletion(aEventSource);
		if(ret!=KErrNone)
			break;

		// Did the phone have an SCA in default memory, if so initialise iRequestSCA
		ret=ParseRxResultsForCSCAResponse(iRequestSCA);
		if(ret==KErrNone)
			{
			// We've finished the request
			CompleteRequest();
			ret=KErrNone;
			}
		else
			{
			// We do not have an SCA and have exhausted all possibilities ;-(
			LOCAL_LOGTEXT("GetSCAFromPhone","Have not been able to find an SCA to use");

			//
			// We have to complete with KErrNone so that the second part of the GetSMSP 
			// retrieval IPC is called. If we do not do this the MSMSMESS file will get
			// it's iGetSMSPList array out of sync.	
			CompleteRequest();
			ret=KErrNone;
			}
		break;

	default:
		LOCAL_LOGTEXT("GetSCAFromPhone","Unknown request stage");
		__ASSERT_DEBUG(EFalse,Panic(EATSmsCommandsUnknownRequestStage));
		ret=KErrGeneral;
		}
			
	++iRequestStage;		// Increment request stage for next iteration

	return ret;
	}


TInt CATSmsCommands::SetPhoneToCMTMode(const TEventSource& aEventSource)
/**
 * Sets the phone into CMT mode by using 'AT+CNMI=x,2'
 * IMPORTANT: If this method returns an error then EventSignal will complete our request.
 * @return Standard KErr... codes
 */
	{
	LOCAL_LOGTEXT("SetPhoneToCMTMode","enter function");
	__ASSERT_DEBUG(iRequest==ESetPhoneToCMTMode,Panic(EATSmsCommandsWrongRequest));
	
	TInt ret(KErrNone);
	switch(iRequestStage)
		{
	case EStage0:
		if(iRequestCancel)		// Handle request cancellation, if needed
			{
			LOCAL_LOGTEXT("SetPhoneToCMTMode","Request has been cancelled");
			ret=KErrCancel;
			}
		else
			{	
			// Send 'AT+CNMI=?' to find out phones capabilities
			iTxBuffer.Format(KGetCNMIConfigCommand);
			iIo->Write(this,iTxBuffer);
			iIo->SetTimeOut(this,KATWriteTimeout);
			}
		break;
		
	case EStage1:
		HandleWriteCompletion(aEventSource);
		break;
	
	case EStage2:
		{						// Curly brackets used to scope use of cnmiFirstValue
		//
		// If the phone returns ERROR then ignore the error and assume the 
		// CNMI first parameter should be 2	
		TInt cnmiFirstValue(ECnmiMode2);
		if(HandleResponseCompletion(aEventSource)==KErrNone)
			{
			// Parse the response from the phone
			TRAP(ret,ParseCNMIFirstCapabilityL(cnmiFirstValue));
			ret=KErrNone;		// Ignore errors from ParseCNMIFirstCapabilityL call
			}
				
		// Send 'AT+CNMI=x,2' to phone to set it to CMT mode
		iTxBuffer.Format(KSmsSetCMTMode,cnmiFirstValue);
		iIo->Write(this,iTxBuffer);
		iIo->SetTimeOut(this,KATWriteTimeout);
		}
		break;

	case EStage3:
		HandleWriteCompletion(aEventSource);
		break;

	case EStage4:
		ret=HandleResponseCompletion(aEventSource);
		if(ret==KErrNone)
			{
			// We have completed our request
			CompleteRequest();
			}
		break;

	default:
		LOCAL_LOGTEXT("SetPhoneToCMTMode","Unknown request stage");
		__ASSERT_DEBUG(EFalse,Panic(EATSmsCommandsUnknownRequestStage));
		ret=KErrGeneral;
		}
			
	++iRequestStage;		// Increment request stage for next iteration

	return ret;
	}


TInt CATSmsCommands::SetPhoneToCMTIMode(const TEventSource& aEventSource)
/**
 * Sets the phone into CMTI mode by using 'AT+CNMI=x,1'
 * IMPORTANT: If this method returns an error then EventSignal will complete our request.
 * @return Standard KErr... codes
 */
	{
	LOCAL_LOGTEXT("SetPhoneToCMTIMode","enter function");
	__ASSERT_DEBUG(iRequest==ESetPhoneToCMTIMode,Panic(EATSmsCommandsWrongRequest));
	
	TInt ret(KErrNone);
	switch(iRequestStage)
		{
	case EStage0:
		if(iRequestCancel)		// Handle request cancellation, if needed
			{
			LOCAL_LOGTEXT("SetPhoneToCMTIMode","Request has been cancelled");
			ret=KErrCancel;
			}
		else
			{	
			// Send 'AT+CNMI=?' to find out phones capabilities
			iTxBuffer.Format(KGetCNMIConfigCommand);
			iIo->Write(this,iTxBuffer);
			iIo->SetTimeOut(this,KATWriteTimeout);
			}
		break;
		
	case EStage1:
		HandleWriteCompletion(aEventSource);
		break;
	
	case EStage2:
		{						// Curly brackets used to scope use of cnmiFirstValue
		//
		// If the phone returned ERROR then ignore the error and assume the 
		// CNMI first parameter should be 2	
		TInt cnmiFirstValue(2);
		if(HandleResponseCompletion(aEventSource)==KErrNone)
			{
			// Parse the response from the phone
			TRAP(ret,ParseCNMIFirstCapabilityL(cnmiFirstValue));
			ret=KErrNone;		// Ignore errors from ParseCNMIFirstCapabilityL call
			}
				
		// Send 'AT+CNMI=x,1' to phone to set it to CMTI mode
		iTxBuffer.Format(KSmsSetCMTIMode,cnmiFirstValue);
		iIo->Write(this,iTxBuffer);
		iIo->SetTimeOut(this,KATWriteTimeout);
		}
		break;

	case EStage3:
		HandleWriteCompletion(aEventSource);
		break;

	case EStage4:
		ret=HandleResponseCompletion(aEventSource);
		if(ret==KErrNone)
			{
			// We have completed our request
			CompleteRequest();
			}
		break;

	default:
		LOCAL_LOGTEXT("SetPhoneToCMTIMode","Unknown request stage");
		__ASSERT_DEBUG(EFalse,Panic(EATSmsCommandsUnknownRequestStage));
		ret=KErrGeneral;
		}
			
	++iRequestStage;		// Increment request stage for next iteration

	return ret;
	}



void CATSmsCommands::CompleteRequest(TInt aError)
/**
 * Common code for local AT request functions to complete a request from a derived class.
 * In the header file smsbase.h the default value for aError ir KErrNone.
 */
 	{
	LOCAL_LOGTEXT("CompleteRequest","enter function");
	iRequestStage=0;	
	iRequestCompleted=ETrue;
	iRequestError=aError;
	}

void CATSmsCommands::HandleWriteCompletion(TEventSource aSource)
/**
 * Common code for handling a write event from the CATIO class when communicating
 * with the modem.
 */
	{
	LOCAL_LOGTEXT("HandleWriteCompletion","enter function");
	__ASSERT_DEBUG(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
	iIo->WriteAndTimerCancel(this);
	AddCmsErrorExpectString();
	StandardWriteCompletionHandler(aSource,KATResponseTimeout);
	}


TInt CATSmsCommands::HandleResponseCompletion(TEventSource aSource,TBool aValidateExpectString)
/**
 * Common code for handling a read event from the CATIO class when communicating
 * with the modem. Does some parsing of response to see if 'OK' or 'ERROR' was responded.
 */
	{
	LOCAL_LOGTEXT("HandleResponseCompletion","enter function");

	//
	// The below is an ASSERT ALWAYS just so that we do not get a WINSCW UREL warning.
	// The below only needs to be a ASSERT_DEBUG
	__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
	iIo->WriteAndTimerCancel(this);

	TInt ret(KErrNone);
	if(aValidateExpectString)
		ret=ValidateExpectString();		// This will only check for 'OK' or 'ERROR'

	if(ret!=KErrNone)
		{
		//
		// If we received some kind of error, see if the CMS ERROR stuff can provide
		// a more meanigful error code
		ret=ConvertCMSErrorToKErr(CMSErrorValue());
		}

	RemoveCmsErrorExpectString();
	RemoveStdExpectStrings();
	return ret;
	}

void CATSmsCommands::EventSignal(TEventSource aEventSource)
/**
 * Handles events for local AT commands
 * Will complete request if a handler returns anything other than KErrNone
 */
 	{
	LOCAL_LOGTEXT("EventSignal","enter function");
	TInt error(KErrNone);
	// Check if a timeout occurred	
	if(aEventSource==ETimeOutCompletion)
		{
		LOCAL_LOGTEXT("EventSignal","Timeout event has occurred");
		iIo->RemoveExpectStrings(this);
		iIo->WriteAndTimerCancel(this);
		error=KErrTimedOut;
		}
	else
		{
		// Dispatch event to appropriate request handler
		switch(iRequest)
			{
		case EGetSCAFromPhone:
			error=GetSCAFromPhone(aEventSource);
			break;

		case ESetSCAInPhone:
			error=SetSCAInPhone(aEventSource);
			break;

		case ESetPhoneToCMTMode:
			error=SetPhoneToCMTMode(aEventSource);
			break;

		case ESetPhoneToCMTIMode:
			error=SetPhoneToCMTIMode(aEventSource);
			break;

		case ENone:		// Must not be caught by default case	
			break;
		default:
			LOCAL_LOGTEXT("EventSignal","Unknown request");
			__ASSERT_DEBUG(EFalse,Panic(EATSmsCommandsUnknownRequest));
			}
		}

	if(error!=KErrNone)
		CompleteRequest(error);
	}


TBool CATSmsCommands::ComparePrefMem(const TStorageType& aName) const
	{
	LOCAL_LOGTEXT("ComparePrefMem","enter function");
	return iPhoneGlobals->iPhonePrefMem==aName;
	}

void CATSmsCommands::SetCurrentPrefMem(const TStorageType& aStorageName)
	{
	LOCAL_LOGTEXT("SetCurrentPrefMem","enter function");
	iTxBuffer.Format(KSetPrefMemCommandWithString,&aStorageName,&aStorageName); // Setting both the read and write memory to one and the same store(ME or SM).
	iIo->Write(this,iTxBuffer);
	iIo->SetTimeOut(this,KATWriteTimeout);
	}



TInt CATSmsCommands::ParseRxResultsForCSCAResponse(RMobilePhone::TMobileAddress& aTelNumber)
/**
 * Analyse rx results for a CSCA response and attempt to parse into provided tel.
 * The actual work is done in DoParseRxResultsForCSCAResponseL. Since this method
 * can't leave and ParseBufferLC() has to be called and ParseBufferLC() puts something
 * on the cleanup stack, the "Do" method had to be added.
 * @return Standard KErr... values
 */
	{
	LOCAL_LOGTEXT("ParseRxResultsForCSCAResponse","enter function");
	TInt ret(KErrNone);
	TRAPD(leaveCode,ret = DoParseRxResultsForCSCAResponseL(aTelNumber));
	if(leaveCode)
		return leaveCode;
	else
		return ret;
	}


TInt CATSmsCommands::DoParseRxResultsForCSCAResponseL(RMobilePhone::TMobileAddress& aTelNumber)
/**
 * Analyse rx results for a CSCA response and attempt to parse into provided tel
 * number object
 * @return Standard KErr... values
 */
	{
	LOCAL_LOGTEXT("DoParseRxResultsForCSCAResponseL","enter function");
	TInt ret(KErrNotFound);

	// Parse buffer
	ParseBufferLC();
	TDblQueIter<CATParamListEntry> iter(iRxResults);

	// Extract tokens from parser
	CATParamListEntry* cscaEntry=iter++;
	
	if((cscaEntry)&&(cscaEntry->iResultPtr == KCSCAResponseString))
		{
		CATParamListEntry* telNumberEntry=iter++;
		if (telNumberEntry)
			{
			//
			// Parse tokens into a TMobileAddress
			//
			// Type of number field <tosca> of +CSCA is optional so we have to accomodate for that.
			// If <tosca> was missing then we subsitute it with a dummy '000' <tosca>.
			_LIT8(KDummyNumberTypeEntry,"000");
			CATParamListEntry* numberTypeEntry=iter++;		
			if (!numberTypeEntry)
				ret=CATSmsUtils::CopyAddressStringToAddressStruct(telNumberEntry->iResultPtr,KDummyNumberTypeEntry,aTelNumber);
			else
				ret=CATSmsUtils::CopyAddressStringToAddressStruct(telNumberEntry->iResultPtr,numberTypeEntry->iResultPtr,aTelNumber);
			}
		}

	CleanupStack::PopAndDestroy();		// ParseBufferLC
	return ret;
	}


TInt CATSmsCommands::CMSErrorValue()
/*
 * Returns the error code decoded from the response string .eg '+CMS ERROR: xxx'.
 * This functions considers '+CMS ERROR: 500' and 'ERROR' as the error 500.
 * @return if there was an error the ETSI or KErr error value
 * @return if there was not an error then KErrNone
  */
 	{
	LOCAL_LOGTEXT("CMSErrorValue","enter function");
	// Locate error string
	iBuffer.Set(iIo->Buffer());
	TInt pos=iBuffer.FindF(KERRORResponseString);
	if (pos==KErrNotFound)
		return KErrNone;		// If we can't find the 'ERROR' string then that means there was not an error ;-)
	
	// Locate error value
	// (ie. read in all digits form the first found to the end of the string)
	const TInt length=iBuffer.Length();
	pos+=KCMGSResponseStringLength;
	while(pos<length && !(TChar(iBuffer[pos]).IsDigit()))
		pos++;
	
	TInt ret=500;		// 500 is the error value for 'undefined'
	if(pos<length) 	// Only decode error value if we found some digits
		{
		TLex8 lex(iBuffer.Mid(pos));
		TUint16 val;
		if(lex.Val(val,EDecimal)==KErrNone)
			ret=val;
		}
	
	LOGTEXT2(_L8("CATSmsCommands::CmsErrorValue\terror = %d"),ret);
	return ret;
	}


TInt CATSmsCommands::ConvertCMSErrorToKErr(const TInt& aCmsError) const 
	{
	LOCAL_LOGTEXT("ConvertCMSErrorToKErr","enter function");
	if(aCmsError==0)
		return KErrNone;

	return KErrGsmSmsBase-aCmsError;
	}


void CATSmsCommands::ProcessCapsElementL(TDblQueIter<CATParamListEntry>& aIter,TInt32& aCapsMask)
//
// Process a single element, i.e. a range or a bracketed list
//
	{
	TInt r=KErrNone;
_LIT8(KOpenBracket,  "(");
_LIT8(KCloseBracket, ")");
_LIT8(KDashChar,	 "-");

	CATParamListEntry* entry=aIter;
	if(!entry)
		return;

	aIter++;

	TBool listPresent=EFalse;					// Is there a list here?
	if(entry->iResultPtr==KOpenBracket)
		{
		listPresent=ETrue;
		entry=aIter++;
		if(!entry)
			return;
		}

// Process the element
	do
		{
// It might be a range, signified by a '-'
		TInt pos=entry->iResultPtr.Find(KDashChar);
		if(pos!=KErrNotFound)
			{
			TInt32 rangeStart;
			TLex8 rangeStartString(entry->iResultPtr.Left(pos));
			if((r=rangeStartString.Val(rangeStart))!=KErrNone)
				User::Leave(r);		

			TInt32 rangeEnd;
			TInt l;
			if((l=entry->iResultPtr.Length()-pos-1)<=0)
				User::Leave(KErrGeneral);
			TLex8 rangeEndString(entry->iResultPtr.Right(l));
			if((r=rangeEndString.Val(rangeEnd))!=KErrNone)
				User::Leave(r);

			if((rangeStart<0)||(rangeStart>31)||(rangeEnd<0)||(rangeEnd>31))
				User::Leave(KErrGeneral);

			for(TInt i=rangeStart;i<(rangeEnd+1);i++)
				aCapsMask|=(1<<i);
			}
// Or it might be the close bracket ')'
		else if(entry->iResultPtr==KCloseBracket)
			listPresent=EFalse;
// Or it might be a single value
		else
			{
			TInt32 value;
			TLex8 string(entry->iResultPtr);
			if((r=string.Val(value))!=KErrNone)
				User::Leave(r);

			if((value<0)||(value>31))
				User::Leave(KErrGeneral);

			aCapsMask|=(1<<value);
			}
		entry=aIter++;
		}
	while((listPresent)&&(entry!=NULL));
	aIter--;
	}


void CATSmsCommands::ParseCNMIFirstCapabilityL(TInt& aHighestValue)
	{
	ParseBufferLC(ETrue);
	TDblQueIter<CATParamListEntry> iter(iRxResults);
	CATParamListEntry* entry=iter++;

	if((entry==NULL)||(entry->iResultPtr!=KCNMIResponseString))
		{
		LOGTEXT(_L8("Error - Cannot find CNMI: string"));
		User::Leave(KErrNotFound);
		}

	TInt32 bitmap(0);
	ProcessCapsElementL(iter,bitmap);

	aHighestValue=ECnmiMode0;		// bitmap&0x01 denotes highest value of 0

	if(bitmap&KCapsCnmiMode2) 
		aHighestValue=ECnmiMode2;
	else if(bitmap&KCapsCnmiMode1)  // Mode 1 only set if mode 2 not supported
		aHighestValue=ECnmiMode1;
	else if(bitmap&KCapsCnmiMode3)  // Only set to mode 3 if this is the only mode supported
		aHighestValue=ECnmiMode3;

	CleanupStack::PopAndDestroy();		// ParseBufferLC object
	}
