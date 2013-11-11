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

#include <kernel.h>
#include <mmc.h>
#include <locmedia.h>

#include "mmcldd.h"
#include "d_mmcsdif.h"



const TInt KMajorVersionNumber=1;
const TInt KMinorVersionNumber=0;
const TInt KBuildVersionNumber=0;

// global Dfc Que
TDynamicDfcQue* gDfcQ;



DECLARE_STANDARD_LDD()
	{
	return new DLddFactoryMmcCntrlInterface;
	}

/**
 * DLddFactoryMmcCntrlInterface constructor
 *
 * @return N/A
 *
 * @leave N/A
 */
DLddFactoryMmcCntrlInterface::DLddFactoryMmcCntrlInterface()
	{
    iParseMask=KDeviceAllowUnit;  // Pass stack number as unit
	iUnitsMask=0xffffffff;
	iVersion=TVersion(KMajorVersionNumber,KMinorVersionNumber,KBuildVersionNumber);
	}

/**
 * Create logical channel
 *
 * @param DLogicalChannelBase*& aChannel - logical channel
 *
 * @return TInt the error code
 *
 * @leave N/A
 */
TInt DLddFactoryMmcCntrlInterface::Create(DLogicalChannelBase*& aChannel)
	{
	aChannel = new DLddMmcCntrlInterface;
	return aChannel ? KErrNone : KErrNoMemory;
	}

const TInt KDMmcThreadPriority = 27;
_LIT(KDMmcThread,"DMMCSDThread");


/**
 * Install driver
 *
 * @return TInt the error code
 *
 * @leave N/A
 */
TInt DLddFactoryMmcCntrlInterface::Install()
	{
	// Allocate a kernel thread to run the DFC
	TInt r = Kern::DynamicDfcQCreate(gDfcQ, KDMmcThreadPriority, KDMmcThread);

	if (r != KErrNone)
		return r;
	__KTRACE_OPT(KHARDWARE,MMCSDLOG("installing driver name from factory"));
	return SetName(&KLddName);
    }

/**
 * Get the Pc Card Contoller Interface ldd's version info
 *
 * @param TDes8 &aDes - Buffer to contain version info
 *
 * @return N/A
 *
 * @leave N/A
 */
void DLddFactoryMmcCntrlInterface::GetCaps(TDes8 &aDes) const
	{
    TCapsMmcIfV01 b;
	b.version=TVersion(KMajorVersionNumber,KMinorVersionNumber,KBuildVersionNumber);
	aDes.FillZ(aDes.MaxLength());
	aDes.Copy((TUint8 *)&b,Min(aDes.MaxLength(),sizeof(b)));
	}

/**
 * Destructor
 *
 * @return N/A
 *
 * @leave N/A
 */
DLddFactoryMmcCntrlInterface::~DLddFactoryMmcCntrlInterface()
	{
	if (gDfcQ)
		gDfcQ->Destroy();
	}

/**
 * DLddMmcCntrlInterface constructor
 *
 * @return N/A
 *
 * @leave N/A
 */
DLddMmcCntrlInterface::DLddMmcCntrlInterface()
	{
	iClient = &Kern::CurrentThread();
	((DObject*)iClient)->Open();	// can't fail since thread is running
    }

/**
 * Destructor
 *
 * @return N/A
 *
 * @leave	N/A
 */
DLddMmcCntrlInterface::~DLddMmcCntrlInterface()
	{
	Kern::SafeClose((DObject*&)iClient,NULL);
	iAsyncList.ResetAndDestroy();
    }

/**
 * Create driver ojects
 *
 * @param TInt aUnit - Socket ID
 * @param const TDesC8* aInfo - Buffer for logging
 * @param const TVersion& aVer - Current version
 *
 * @return TInt the error code
 *
 * @leave N/A
 */
TInt DLddMmcCntrlInterface::DoCreate(TInt aUnit, const TDesC8* aInfo, const TVersion& aVer)
	{
	if (!Kern::QueryVersionSupported(TVersion(KMajorVersionNumber,KMinorVersionNumber,KBuildVersionNumber),aVer))
		return(KErrNotSupported);

	MMCSDLOG2("DLddMmcCntrlInterface::DoCreate called aInfo(%S)", aInfo);

	// create the socket - there is only a DMMCSocket
	iSocket = static_cast<DMMCSocket*>(DPBusSocket::SocketFromId(aUnit));
	if(!iSocket)
		return(KErrNoMemory);

	//Obtain the appropriate card from the stack
	iStack = static_cast<DMMCStack*>(iSocket->Stack(KStackNo));
	if(iStack == NULL)
		return(KErrNoMemory);

	//Obtain the appropriate card power supply
	iPsu=static_cast<DMMCPsu*>(iSocket->iVcc);
	if (iPsu==NULL)
		return(KErrNoMemory);

	//Obtain the appropriate card  Media change
	iMediaChange=static_cast<DMMCMediaChange*>(iSocket->iMediaChange);
	if (iMediaChange==NULL)
		return(KErrNoMemory);

	SetDfcQ(gDfcQ);
	iMsgQ.Receive();

	iUnit = aUnit;  // save the unit - needed for regitering callbacks for subsequent bus events

    return(KErrNone);
	}

/**
 * Request cancelled callback
 *
 * @param TInt sReqNo - Request Number
 *
 * @return N/A
 *
 * @leave N/A
 */
void DLddMmcCntrlInterface::DoCancel(TInt /*aReqNo*/)
	{
	}

/**
 * Handle kernel side message
 *
 * @param TMessageBase* aMsg - The kernel side message
 *
 * @return N/A
 *
 * @leave N/A
 */
void DLddMmcCntrlInterface::HandleMsg(TMessageBase* aMsg)
    {
    TThreadMessage& m=*(TThreadMessage*)aMsg;
    TInt id=m.iValue;

	if (id==(TInt)ECloseMsg)
		{
		m.Complete(KErrNone, EFalse);
		return;
		}
    else if (id==KMaxTInt)
		{
		// DoCancel
		m.Complete(KErrNone,ETrue);
		return;
		}

    if (id<0)
		{
		// DoRequest
		TRequestStatus* pS=(TRequestStatus*)m.Ptr0();
		TInt r=DoRequest(~id, pS, m.Ptr1(), m.Ptr2());
		if (r!=KErrNone)
	    	Kern::RequestComplete(iClient, pS, r);
		m.Complete(KErrNone,ETrue);
		}
    else
		{
		// DoControl
		TInt r=DoControl(id,m.Ptr0(),m.Ptr1());
		if(r != KErrCompletion)
			{
			m.Complete(r,ETrue);
			}
		}
	}

/**
 * Do a kernel side request
 *
 * @param TInt aFunction - Request Function ID
 * @param TRequestStatus* aStatus - Request Status
 * @param TAny* a1 - Given parameter
 * @param TAny* a2 - Given parameter
 *
 * @return TInt the error code
 *
 * @leave N/A
 */
TInt DLddMmcCntrlInterface::DoRequest(TInt aFunction, TRequestStatus* aStatus, TAny* a1, TAny *a2)
	{

	TTCFunctionId fid = TTCFunctionId(aFunction);
	switch(fid.TestType())
		{
		case EMmcTestSocket:
			return SocketDoRequest(aStatus, a1, a2, aFunction);

		case EMmcTestStack:
		case EMmcTestPsu:
		case EMmcTestMediaChange:
	  	case EMmcGeneralTest:
		default:
			break;
		};

    switch(fid.FunctionId())
		{
        case RMMCSDTestControllerInterface::EReadSector:
            {
            // read sector a1 is block number, a2 is client descriptor
            // data will be written back to upon completion
			MMCSDLOG3("RMMCSDTestControllerInterface::EReadSector a1(%d) a2(%x)", (TUint32)a1, a2);
			if(!iCard)
				{
				MMCSDLOG("RMMCSDTestControllerInterface::EReadSector NULL card\n");
				return KErrNotReady;
				}

			DMmcSessionAsyncRequest* request = CreateAsyncSessionRequest(EMmcSesReadBlk, aStatus);

			TUint32 srcAddr=((TUint32)a1)<<KSectorSizeShift;
			TUint readBlLen = 1 << iCard->MaxReadBlLen();
			TUint readBlMask=(readBlLen-1);
			request->iBlkOffet=srcAddr-(srcAddr&(~readBlMask));
			request->iClientDesPtr = a2;
			srcAddr &= (~readBlMask);

			MMCSDLOG("RMMCSDTestControllerInterface::EReadSector #2");
			TMMCArgument da(srcAddr);

			request->Session().SetupCIMReadBlock(da,readBlLen,&(request->iBuf[0]));
			request->Session().Engage();

			MMCSDLOG("RMMCSDTestControllerInterface::EReadSector #3");
			break;
            }
        case RMMCSDTestControllerInterface::EWriteSector:
            {
			MMCSDLOG3("RMMCSDTestControllerInterface::EWriteSector a1(%d) a2(%x) #1", (TUint32)a1, a2);
			if(!iCard)
				{
				MMCSDLOG("RMMCSDTestControllerInterface::EWriteSector NULL card\n");
				return KErrNotReady;
				}

			TUint32 destAddr=((TUint32)a1)<<KSectorSizeShift;
			TUint writeBlLen=1 << iCard->MaxWriteBlLen();
			
			DMmcSessionAsyncRequest* request = CreateAsyncSessionRequest(EMmcSesWriteBlk, aStatus);
			
			TUint writeBlMask=(writeBlLen-1);
			request->iBlkOffet=destAddr-(destAddr&(~writeBlMask));
			TPtr8* srcDes = (TPtr8*)a2;
			TPtr8 ptr(&(request->iBuf[request->iBlkOffet]),KSectorSizeInBytes,KSectorSizeInBytes);
			TInt r = Kern::ThreadDesRead(iClient, srcDes, ptr, 0, KChunkShiftBy0);
			if(r != KErrNone)
				{
				return(r);
				}

			destAddr&=(~writeBlMask);
			request->Session().SetupCIMWriteBlock(TMMCArgument(destAddr),writeBlLen,&(request->iBuf[0]));
			request->Session().Engage();

			MMCSDLOG("RMMCSDTestControllerInterface::EWriteSector #4");
			break;
			}
        }
	return(KErrNone);
	}

/**
 * Do a kernel side control
 *
 * @param TInt aFunction - Request Function ID
 * @param TAny* a1 - Given parameter
 * @param TAny* a2 - Given parameter
 *
 * @return TInt the error code
 *
 * @leave N/A
 */
TInt DLddMmcCntrlInterface::DoControl(TInt aFunction,TAny* a1,TAny* a2)
	{
	TInt r = KErrNotSupported;
	TTCFunctionId fid = TTCFunctionId(aFunction);
	MMCSDLOG4("DLddMmcCntrlInterface::DoControl aFunction(%d) \
			fid.FunctionId()(%d) fid.TestType(%d)"
			, aFunction, fid.FunctionId(), fid.TestType());

	switch(fid.TestType())
		{
		case EMmcTestStack:
			return StackDoControl(aFunction, a1, a2);
		case EMmcTestSocket:
			return SocketDoControl(aFunction, a1, a2);
      	case EMmcTestPsu:
			return PsuDoControl(aFunction, a1, a2);
		case EMmcTestMediaChange:
			return MediaChangeDoControl(aFunction, a1, a2);
		case EMmcGeneralTest:
		default:
			break;
		};

	switch (fid.FunctionId())
		{
      	case RMMCSDTestControllerInterface::EReadCSD:
      		{
			iCSD = iCard->CSD();
				{				
				TCSDInfo csdInfo(iCSD);
	    		r = Kern::ThreadRawWrite(iClient, a1, &csdInfo, sizeof(csdInfo));
				}
      		}
	  		break;
	  		  		
      	case RMMCSDTestControllerInterface::EReadExtCSD:
      		{
			iExtendedCSD = iCard->ExtendedCSD();
			TExtendedCSDInfo extendedCSDInfo(iExtendedCSD);
    		r = Kern::ThreadRawWrite(iClient, a1, &extendedCSDInfo, sizeof(extendedCSDInfo));
      		}
	  		break;
	  		
		case RMMCSDTestControllerInterface::ECardInfo:
			{
			TMMCCardInfo cardInfo;
			cardInfo.SetCardInfo(*iCard);
			r = Kern::ThreadRawWrite(iClient, a1, &cardInfo, sizeof(cardInfo));
			}
			break;
			
		case RMMCSDTestControllerInterface::ECard:
      		{
			r = Kern::ThreadRawWrite(iClient, a1, &iCard, sizeof(iCard));
      		}
	  		break;
		  		
		case RMMCSDTestControllerInterface::ECardIsPresent:
			{
			TBool isPresent = iCard->IsPresent();
			r = Kern::ThreadRawWrite(iClient, a1, &isPresent, sizeof(isPresent));
			}
			break;
		  		
		case RMMCSDTestControllerInterface::ECardIsReady:
			{
			TBool isReady = iCard->IsReady();
			r = Kern::ThreadRawWrite(iClient, a1, &isReady, sizeof(isReady));
			}
			break;
		  		
		case RMMCSDTestControllerInterface::ECardIsLocked:
			{
			TBool isLocked = iCard->IsLocked();
			r = Kern::ThreadRawWrite(iClient, a1, &isLocked, sizeof(isLocked));
			}
			break;
		  		
       default:
        	{
        	MMCSDLOG2("DLddMmcCntrlInterface::DoControl - Invalid FunctionId()(%d)", fid.FunctionId());        	
        	}
			break;
		}
		
		
	return (r);
	}

/**
 * Check if a request status exists
 *
 * @param TRequestStatus* aStatus - Request Status
 *
 * @return TBool A request status exists
 *
 * @leave N/A
 */
TBool DLddMmcCntrlInterface::RequestStatusExists(TRequestStatus* aStatus)
	{

	// check whether this request status already exists in the async request list
	for(int i=0;i<iAsyncList.Count();i++)
		{
		MMCSDLOG3("Checking index(%d) iAsyncList[i]->Status(%x)", i, iAsyncList[i]->Status());

		if(iAsyncList[i]->Status() == aStatus)
			{
			MMCSDLOG3("WaRNING - adding duplicate TRequestStatus index[%d] aStatus(%x)", i, aStatus);
			return ETrue;
			}
		}

	return EFalse;
	}

/**
 * Create Asynchronous Session Request
 *
 * @param TMmcAsyncCmd aFunctionId - Request Function ID
 * @param TRequestStatus* aStatus - Request Status
 *
 * @return DMmcSessionAsyncRequest* Poniter to the created session request object
 *
 * @leave N/A
 */
DMmcSessionAsyncRequest* DLddMmcCntrlInterface::CreateAsyncSessionRequest(TMmcAsyncCmd aFunctionId, TRequestStatus* aStatus)
	{

	MMCSDLOG3("DLddMmcCntrlInterface::CreateAsyncRequest aStatus(%x) num requests in list (%d)",
			aStatus, iAsyncList.Count());
	
	// create an async request & append to async request list	
	DMmcSessionAsyncRequest* request = new  DMmcSessionAsyncRequest(aFunctionId, aStatus, *this, gDfcQ, *iStack, *iCard);	
	iAsyncList.Append(request);		
	MMCSDLOG4("DLddMmcCntrlInterface::CreateAsyncSessionRequest created OK Status(%x) functionID(%d) aFunctionId(%d)",
			request->Status(), request->FunctionId(), aFunctionId);
	return request;
	}

/**
 * Create Asynchronous Bus Event Request
 *
 * @param TMmcAsyncCmd aFunctionId - Request Function ID
 * @param TRequestStatus* aStatus - Request Status
 * @param TInt aUnit - Socket ID
 *
 * @return DBusEventRequest* Poniter to the created bus event request object
 *
 * @leave N/A
 */
DBusEventRequest* DLddMmcCntrlInterface::CreateAsyncBusEventRequest(TMmcAsyncCmd aFunctionId, TRequestStatus* aStatus, TInt aUnit)
	{
	MMCSDLOG3("DLddMmcCntrlInterface::CreateAsyncBusEventRequest aStatus(%x) num requests in list (%d)",
			aStatus, iAsyncList.Count());
	
	MMCSDLOG4("gDfcQ(%x), iStack(%x), iCard(%x)", gDfcQ, iStack, iCard);

	// create an async request & append toS async request list
	DBusEventRequest* request = new  DBusEventRequest(aFunctionId, aStatus, *this, aUnit, gDfcQ, *iStack, *iCard);
	iAsyncList.Append(request);
	MMCSDLOG4("DLddMmcCntrlInterface::CreateAsyncBusEventRequest created OK Status(%x) functionID(%d) aFunctionId(%d)",
			request->Status(), request->FunctionId(), aFunctionId);
	return request;

	}

/**
 * Clears an asynchronous Request from list
 *
 * @param DLddAsyncRequest* aRequest - Pointer to the request
 *
 * @return N/A
 *
 * @leave N/A
 */
void DLddMmcCntrlInterface::ClearRequest(DLddAsyncRequest* aRequest)
	{
	if(!aRequest)
		{
		MMCSDLOG("ERROR IN DLddMmcCntrlInterface::ClearRequest NULL request");
		return;
		}

	TInt i = iAsyncList.Find(aRequest);
	if(i >= 0)
		{
		iAsyncList.Remove(i);
		}
	else
		{
		MMCSDLOG2("ERROR IN DLddMmcCntrlInterface::ClearRequest err(%d)", i);
		}

	delete aRequest;

	}

/**
 * Callback when request has completed
 *
 * @param DLddAsyncRequest& aRequest - The completed request
 *
 * @return N/A
 *
 * @leave N/A
 */
void DLddMmcCntrlInterface::Notify(DLddAsyncRequest& aRequest)  // callback from DLddAsyncRequest
	{
	MMCSDLOG("DLddMmcCntrlInterface::Notify called");
	TRequestStatus* status =  aRequest.Status();
	switch (aRequest.FunctionId())
		{
		case DLddMmcCntrlInterface::EMmcSesReadBlk:
			{
			DMmcSessionAsyncRequest& request = (DMmcSessionAsyncRequest&) aRequest;
			MMCSDLOG("DLddMmcCntrlInterface::ReadBlockComplete");
			TInt r = KErrNone;
			if (request.iClientDesPtr) // have we supplied descriptor to hold returned data
				{
				TPtr8 ptr(&(request.iBuf[request.iBlkOffet]), KSectorSizeInBytes, KSectorSizeInBytes);
				TPtrC8* srcDes = (TPtrC8*)request.iClientDesPtr;
				r = Kern::ThreadDesWrite(iClient, srcDes, ptr, 0, iClient);
				if(r != KErrNone)
					{
					MMCSDLOG2("ERROR DLddMmcCntrlInterface::ReadBlockComplete Kern::ThreadDesWrite returned r(%d)", r);
					}
				}
			else
				{
				MMCSDLOG("ERROR DLddMmcCntrlInterface::ReadExtCSDComplete - No pointer provided to hold returned data");
				Kern::PanicCurrentThread(_L("PBUS-MMC"), 0);				
				}
			Kern::RequestComplete(iClient, status, (r == KErrNone) ? aRequest.Error() : r);
			}
			break;

		case DLddMmcCntrlInterface::EMmcSesWriteBlk:
			{
			MMCSDLOG("DLddMmcCntrlInterface::WriteBlockComplete)");
			TRequestStatus* status =  aRequest.Status();
			Kern::RequestComplete(iClient, status, aRequest.Error());
			}
			break;

		case DLddMmcCntrlInterface::EMmcSesReadCSD:
			{
			DMmcSessionAsyncRequest& request = (DMmcSessionAsyncRequest&) aRequest;
			MMCSDLOG("DLddMmcCntrlInterface::ReadCSDComplete");
			TInt r = KErrNone;
			if (request.iClientDesPtr) // have we supplied pointer to hold returned data
				{
					{
					TCSDInfo csdInfo(iCSD);
		    		r = Kern::ThreadRawWrite(iClient, request.iClientDesPtr, &csdInfo, sizeof(csdInfo));
					}
				if(r != KErrNone)
					{
					MMCSDLOG2("ERROR DLddMmcCntrlInterface::ReadCSDComplete Kern::ThreadRawWrite returned r(%d)", r);
					}
				}
			else
				{
				MMCSDLOG("ERROR DLddMmcCntrlInterface::ReadExtCSDComplete - No pointer provided to hold returned data");
				Kern::PanicCurrentThread(_L("PBUS-MMC"), 0);				
				}
			Kern::RequestComplete(iClient, status, (r == KErrNone) ? aRequest.Error() : r);
			}
			break;

		case DLddMmcCntrlInterface::EMmcSesReadExtCSD:
			{
			DMmcSessionAsyncRequest& request = (DMmcSessionAsyncRequest&) aRequest;
			MMCSDLOG("DLddMmcCntrlInterface::ReadExtCSDComplete");
			TInt r = KErrNone;
			if (request.iClientDesPtr) // have we supplied pointer to hold returned data
				{
				TExtendedCSDInfo extendedCSDInfo(iExtendedCSD);
	    		r = Kern::ThreadRawWrite(iClient, request.iClientDesPtr, &extendedCSDInfo, sizeof(extendedCSDInfo));
				if(r != KErrNone)
					{
					MMCSDLOG2("ERROR DLddMmcCntrlInterface::ReadExtCSDComplete Kern::ThreadRawWrite returned r(%d)", r);
					}
				}
			else
				{
				MMCSDLOG("ERROR DLddMmcCntrlInterface::ReadExtCSDComplete - No pointer provided to hold returned data");
				Kern::PanicCurrentThread(_L("PBUS-MMC"), 0);				
				}
			Kern::RequestComplete(iClient, status, (r == KErrNone) ? aRequest.Error() : r);
			}
			break;
			
		case DLddMmcCntrlInterface::EMMcSocketPowerUp:
			{
			DBusEventRequest& request = (DBusEventRequest&) aRequest;
			TInt retCode = KErrNone;
			
			MMCSDLOG2("DLddMmcCntrlInterface::EMMcSocketPowerUp ret code = %d", aRequest.Error());
			MMCSDLOG2("DLddMmcCntrlInterface::EMMcSocketPowerUp request.BusState() = %d", request.BusState());
			MMCSDLOG2("DLddMmcCntrlInterface::EMMcSocketPowerUp request.BusError() = %d", request.BusError());
			
			switch(aRequest.Error())
				{
				case TPBusCallBack::EPBusStateChange:
					{
					TPBusState newState = request.BusState();
					retCode = request.BusError();

					if (retCode == KErrNone)
						{
						switch(newState)
							{
							case EPBusCardAbsent:	retCode = KErrNotFound;		break;			
							case EPBusPsuFault:		retCode = KErrBadPower;		break;
							case EPBusOff:
							case EPBusOn:
							case EPBusPowerUpPending:
							case EPBusPoweringUp:
							default:	
								break;
							}
						}

					break;
					}
				}
			
	   		Kern::RequestComplete(iClient, status, retCode);
			}
			break;

		default:
			MMCSDLOG("#######################################################################");
			MMCSDLOG("## UNHANDLED END DFC DLddAsyncRequest DLddMmcCntrlInterface::Notify  ##");
			MMCSDLOG2("## aRequest.FunctionId(%d)  ##", aRequest.FunctionId());
			MMCSDLOG("#######################################################################");
			Kern::PanicCurrentThread(_L("PBUS-MMC"), 0);
			break;
		};

	ClearRequest(&aRequest);

	}

