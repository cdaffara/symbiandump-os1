// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


//************************************************************************************************************
//Project
#include "LbsLasDataSource.h"
#include "LbsLasBtDataReader.h"
#include "LbsLasCommon.h"


static const TInt KMaxConnectErrors = 4;

_LIT(KLbsLasBtDataReader,"LbsLasBtDataReader");

//************************************************************************************************************
//   CLasBtDataReader
//************************************************************************************************************
CLasBtDataReader::CLasBtDataReader(MLasDataSourceObserver* aObserver)
	:
	CActive(CActive::EPriorityStandard),
	iBtConnectState(EStateDeviceUnknown),
	iObserver(aObserver),
	iMonitoring(EFalse)
	{
	CActiveScheduler::Add(this);

	// Under WINS just connect to the hardcoded GPS device
#ifdef __WINS__
	iBtConnectState = EStateDisconnected;
#endif

	}
	

//------------------------------------------------------------------------------------------------------------
CLasBtDataReader* CLasBtDataReader::NewL(MLasDataSourceObserver* aObserver)
	{
	CLasBtDataReader* self = new(ELeave) CLasBtDataReader(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


//------------------------------------------------------------------------------------------------------------
void CLasBtDataReader::ConstructL()
	{
	User::LoadPhysicalDevice(_L("ECDRV"));
	User::LoadPhysicalDevice(_L("EUART1"));
	User::LoadLogicalDevice(_L("ECOMM"));

	TInt err = iSs.Connect();
	User::LeaveIfError(err);

	err = iSs.FindProtocol(_L("RFCOMM"),iBtProtoInfo);
	User::LeaveIfError(err);
	}

//------------------------------------------------------------------------------------------------------------
CLasBtDataReader::~CLasBtDataReader()
   {
	CloseDataSource();

	iBtSock.Close();

	User::FreePhysicalDevice(_L("ECDRV"));
	User::FreePhysicalDevice(_L("EUART1"));
	User::FreeLogicalDevice(_L("ECOMM"));
   }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CLasBtDataReader::DoCancel()
	{
	iBtSock.CancelRecv();
	}

void CLasBtDataReader::RunL()
{
	LASLOG("CLasBtDataReader::RunL - Enter");
	
	switch(iBtConnectState)
	   {
		case EStateDeviceUnknown:
			{
			LASLOG("BTReadData::EStateDeviceUnknown");	
			StartBtSourceSearchL();
			iBtConnectState = EStateSearching;
			SetActive();
			break;
   			}

		case EStateError:
		   {
			LASLOG("BTReadData::EStateError");	
			break;
   		}
   		
		case EStateSearching:
			{
			LASLOG("BTReadData::EStateSearching");

			if (iStatus == KErrNone || iStatus == KErrCancel)
				{
				iBtConnectState = EStateDisconnected;
				CompleteSelf(iStatus.Int());
				}
			else
				{
				iBtConnectState = EStateDeviceUnknown;
				}
			break;
   			}

		case EStateDisconnected:
			{
			LASLOG("BTReadData::EStateDisconnected");	

			TInt err = StartBtSourceQuery();

			if (! err)
				{
				iBtConnectState = EStateQuerying;
				}
			else
				{
				iBtConnectState = EStateDeviceUnknown;
				}
			break;
			}
  
		case EStateQuerying:
			{
			// Log
			LASLOG("BTReadData::EStateQuerying");
			LASLOG2("BTReadData - Connecting on port: %d",iBtDevice.Port());

			// Address and port no. should be known by now. Connect.
			iBtSock.Connect(iBtDevice, iStatus);
			iBtConnectState = EStateConnecting;
			SetActive();
			break;
			}

		case EStateConnecting:
		   {
         LASLOG("BTReadData::EStateConnecting");

		   // Start reading data if connection ok. Retry to connect otherwise.
			if(iStatus == KErrNone)
				{
	         LASLOG("BTReadData - Connected - Going on line");         			
			   iBtConnectState = EStateOnLine;
				iBtSock.RecvOneOrMore(iData, 0, iStatus, iXfrLength);
			   SetActive();
				}
			else
				{
				iErrorCount++;
				if (iErrorCount >= KMaxConnectErrors)
					{
					LASLOG("BTReadData - Too Many Connection Errors");
					iErrorCount = 0;
					iBtConnectState = EStateDeviceUnknown;
					CloseDataSource();
					}
				else
					{
					LASLOG("BTReadData - Connection Error - Lets go round again. Trying to connect");
					iBtSock.Connect(iBtDevice, iStatus);
					SetActive();
					}
				}
		   break;
			}
		
		case EStateOnLine:
		   {
         LASLOG("BTReadData::EStateOnLine");	

			LASLOGDES(iData);
			if (iMonitoring)
				{
				iObserver->HandleData(iData, iStatus.Int());
				}

			if (iStatus == KErrNone)
				{
				LASLOG("BTReadData::EStateOnLine - Got Data");
				iBtSock.RecvOneOrMore(iData, 0, iStatus, iXfrLength);
				SetActive();
				}
			else
				{
				LASLOG("BTReadData::EStateOnLine - Error reading data");	
				iBtConnectState = EStateError;
				}

			break;
   		}
	   }
   }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CLasBtDataReader::StartBtSourceSearchL()
	{
	User::LeaveIfError(iNotifier.Connect());
	
	iNotifier.StartNotifierAndGetResponse(iStatus, KDeviceSelectionNotifierUid, 
									iSelectionFilterPckg, iResultPckg);
	}


void CLasBtDataReader::DoStartBtSourceQueryL()
	{
	// Enumerate records and attributes.
	// Set the port number(in callback function)
   LASLOG("OpenBtSourceL - Checking Record and Attributes of service 0x1101");
   
	iSdpAgent = CSdpAgent::NewL(*this, iBtDevice.BTAddr());

	CSdpSearchPattern* sp = CSdpSearchPattern::NewL();
	sp->AddL(0x1101);

	iSdpAgent->SetRecordFilterL(*sp);
	delete sp;								

	iSdpAgent->NextRecordRequestL();
	}

TInt CLasBtDataReader::StartBtSourceQuery()
	{
	LASLOG("OpenDataSource - OpenBtSource");

	TInt err = iBtSock.Open(iSs,iBtProtoInfo.iAddrFamily,iBtProtoInfo.iSockType,iBtProtoInfo.iProtocol);
	
	if (err)
		{
		return err;
		}
   
   //<<HACK>> Under WINS Just connect to the hardcoded address
#ifdef __WINS__
	iStatus = KErrCancel; 
#endif

   err = GetBtDeviceAddr(iBtDevice);
	
	if (err)
		{
		return err;
		}
	
	TRAP(err, DoStartBtSourceQueryL());

	return err;
	}


TInt CLasBtDataReader::OpenDataSource()
	{
 	LASLOG("OpenDataSource - Enter");

	TInt err = KErrNone;
	switch (iBtConnectState)
		{
		case EStateDeviceUnknown:
			{
			TRAP(err, RunL();)
			break;
			}
		case EStateSearching:
			{
			break;
			}
		case EStateDisconnected:
			{
			TRAP(err, RunL();)
			break;
			}
		case EStateQuerying:
		case EStateConnecting:
		case EStateOnLine:
		case EStateError:
			{
			CloseDataSource();
			TRAP(err, RunL();)
			}
		}

	LASLOG("OpenDataSource - Exit");
	
	return err;
	}

void CLasBtDataReader::CloseDataSource()
	{
	Cancel();

	iNotifier.Close();
	iSs.Close();

	delete iSdpAgent;
	iSdpAgent = NULL;

	iBtConnectState = EStateDisconnected;
	}


void CLasBtDataReader::MonitorDataSource()
	{
	iMonitoring = ETrue;
	}
	
	
// Allows the caller to signal they are no longer interested in receiving data.
// Call does not actual stop the reading from device - data will still be
// consumed from the bluetooth device. However, client will no longer receive
// a callback.
//
void CLasBtDataReader::CancelMonitor()
	{
	iMonitoring = EFalse;
	}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CLasBtDataReader::CompleteSelf(TInt aReason)
   {
   TRequestStatus* pStat = &iStatus;
	
  	User::RequestComplete(pStat, aReason);
  	
   SetActive();
   }


TInt CLasBtDataReader::GetBtDeviceAddr(TRfcommSockAddr& aAddr)
	{
   TBuf<100> tmpBuf;  

	if (iStatus == KErrCancel) //Use a default device
	   {
		LASLOG("Device selection - Using default address");

		TInt64 btAddr = MAKE_TINT64(0x0011, 0x9fb02085);  // Billy's LBS 6610
		//TInt64 btAddr = MAKE_TINT64(0x0002, 0xc7156fdb);  // Emtac GPS
		
		//TInt64 btAddr = MAKE_TINT64(0x0002, 0xc7156fdb);
		//TInt64 btAddr(0x0002, 0xc7156fdb);

		TBTDevAddr defBTAddr(btAddr);

		//TBTDevAddr defBTAddr(0x00081b0584d2);  // Holux GPS
		//TBTDevAddr defBTAddr(0x0002c7156fdb);  // Emtac GPS
		//TBTDevAddr defBTAddr(0x00119fb02085);  // Billy's LBS 6610
		//TBTDevAddr defBTAddr(0x006057d21bf3);  // Jarek's 6600
		//TBTDevAddr defBTAddr(0x0002c7227133);  // GPS
		aAddr.SetBTAddr(defBTAddr);
      return KErrNone;
		}

   if (iStatus == KErrNone) //Use the selected device.
	   {
		// Log
		LASLOG("BT Device selected");
		                   
      tmpBuf=iResultPckg().DeviceName();
      LASLOG2("Device name: %S", &tmpBuf);

      iResultPckg().BDAddr().GetReadable(tmpBuf);
      LASLOG2("Device Addr: %S", &tmpBuf);
		
      LASLOG2("Major Service Class: %d", iResultPckg().DeviceClass().MajorServiceClass());
      LASLOG2("Major Device Class:  %d", iResultPckg().DeviceClass().MajorDeviceClass()); 							 				
      LASLOG2("Minor Device Class:  %d", iResultPckg().DeviceClass().MinorDeviceClass());      
         
 		// Set the Address
      aAddr.SetBTAddr(iResultPckg().BDAddr());
      return KErrNone;
	   }

	return KErrNotFound;
	}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Record enumeration callback function
void CLasBtDataReader::NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount)
   {
   LASLOG2("NextRecordRequestComplete. Records found: %d", aTotalRecordsCount);
 	   
	if ((aError==KErrNone) && (aTotalRecordsCount>0))
		{
		TInt err = KErrNone;
		TRAP(err, iSdpAgent->AttributeRequestL(aHandle,KSdpAttrIdProtocolDescriptorList);)
		if(err)
			User::Panic(KLbsLasBtDataReader,err);
		}
	
	if(aError==KErrEof && iBtDevice.Port()!=0)
	   CompleteSelf(KErrNone);
   }


//------------------------------------------------------------------------------------------------------------
// Attribute enumeration callback function
void CLasBtDataReader::AttributeRequestResult(TSdpServRecordHandle /*aHandle*/, TSdpAttributeID /*aAttrID*/, CSdpAttrValue* aAttrValue)
	{
	TInt err = KErrNone;
	TRAP(err, aAttrValue->AcceptVisitorL(*this);)
	if(err)
		User::Panic(KLbsLasBtDataReader,err);
	
	delete aAttrValue;
	}


//------------------------------------------------------------------------------------------------------------
// Attribute enumeration callback function
void CLasBtDataReader::AttributeRequestComplete(TSdpServRecordHandle /*aHandle*/, TInt aError)
	{
	TInt err = KErrNone;
	if(aError==KErrNone)
		{
		TRAP(err, iSdpAgent->NextRecordRequestL();)
		if(err)
			User::Panic(KLbsLasBtDataReader,err);		
		}
	}


//------------------------------------------------------------------------------------------------------------
// Attribute enumeration callback function
void CLasBtDataReader::VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType)
   {
   if(aType==ETypeUint)
	   {
		if(iBtDevice.Port()<aValue.Uint())
			iBtDevice.SetPort(aValue.Uint());
	   }
   }


//------------------------------------------------------------------------------------------------------------
void CLasBtDataReader::StartListL(CSdpAttrValueList& /*aList*/)
	{}


//------------------------------------------------------------------------------------------------------------
void CLasBtDataReader::EndListL()
	{}

