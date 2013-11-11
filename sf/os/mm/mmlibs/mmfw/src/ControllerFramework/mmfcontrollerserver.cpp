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
//

#include <mmf/common/mmfcontroller.h>
#include <mmf/server/mmffile.h>
#include <mmf/common/mmfdrmcustomcommands.h>
#include <mmf/common/mmfstandardcustomcommands.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfcontrollerextendeddata.h>
#include <mmf/common/mmfcustomcommandparsermanager.h>
#endif

inline void ExtendedDataTlsCleanup(TAny* /*aPtr*/)
	{
	delete static_cast<CMMFControllerExtendedData*>(Dll::Tls());
	Dll::SetTls(NULL);
	}

CMMFController* CMMFController::NewL(TUid aControllerUid, MAsyncEventHandler& aEventHandler, TThreadId aClientTid)
	{
	Dll::SetTls(CMMFController::CreateExtendedDataL(aClientTid));
	CleanupStack::PushL(TCleanupItem(&ExtendedDataTlsCleanup, NULL));
	CMMFController* s = REINTERPRET_CAST(CMMFController*,REComSession::CreateImplementationL(aControllerUid,_FOFF(CMMFController,iDtor_ID_Key)));
	CleanupStack::PushL(s);
	s->ConstructL(aEventHandler, aClientTid);
	CleanupStack::Pop(s);
	CleanupStack::PopAndDestroy();	// TCleanupItem(ExtendedDataTlsCleanup)
	return s;
	}

void CMMFController::ConstructL(MAsyncEventHandler& aEventHandler, TThreadId aClientTid)
	{
	iAsyncEventHandler = &aEventHandler;
	
	// If extended data hasn't been initialized, GetExtendedDataL will initialize it and
	// add it into iMMFObjectContainer
#ifdef _DEBUG
	CMMFControllerExtendedData* extendData =
#endif
		GetExtendedDataL();
#ifdef _DEBUG
	ASSERT(extendData->ClientThreadId() == aClientTid);
#else
	(void)aClientTid;	// remove compilation error
#endif
	}

EXPORT_C CMMFController::~CMMFController()
	{
	delete iCustomCommandParserManager;
	delete iMMFObjectContainer;
	delete iMetaDataBuffer;
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

EXPORT_C TInt CMMFController::DoSendEventToClient(const TMMFEvent& aEvent)
	{
	return iAsyncEventHandler->SendEventToClient(aEvent);
	}

EXPORT_C CMMFObjectContainer& CMMFController::MMFObjectContainerL()
	{
	// Construct iMMFObjectContainer if we haven't already.
	if (!iMMFObjectContainer)
		iMMFObjectContainer = new(ELeave) CMMFObjectContainer;
	return *iMMFObjectContainer;
	}

EXPORT_C void CMMFController::AddCustomCommandParserL(CMMFCustomCommandParserBase& aParser)
	{
	// Construct iCustomCommandParserManager if we haven't already
	if (!iCustomCommandParserManager)
		iCustomCommandParserManager = CMMFCustomCommandParserManager::NewL();
	iCustomCommandParserManager->AddCustomCommandParserL(aParser);
	}


EXPORT_C void CMMFController::HandleRequestL(TMMFMessage& aMessage)
	{
	// If the message has the constant controller destination handle, pass the message to
	// the controller plugin to handle.
	// Otherwise, the message must be for one of the MMFObjects.
	if (aMessage.Destination().DestinationHandle() == KMMFObjectHandleController)
		{
		// If the message has an interface ID for this controller, handle it here.
		// Otherwise, pass it to the controller plugin to handle as a custom command.
		if (aMessage.Destination().InterfaceId() == KUidInterfaceMMFController)
			{
			TBool complete = ETrue;
			switch (aMessage.Function())
				{
			case EMMFControllerAddDataSource:
				complete = DoAddDataSourceL(aMessage);
				break;
			case EMMFControllerAddDataSink:
				complete = DoAddDataSinkL(aMessage);
				break;
			case EMMFControllerRemoveDataSource:
				complete = DoRemoveDataSourceL(aMessage);
				break;
			case EMMFControllerRemoveDataSink:
				complete = DoRemoveDataSinkL(aMessage);
				break;
			case EMMFControllerReset:
				complete = DoResetL(aMessage);
				break;
			case EMMFControllerPrime:
				complete = DoPrimeL(aMessage);
				break;
			case EMMFControllerPlay:
				complete = DoPlayL(aMessage);
				break;
			case EMMFControllerPause:
				complete = DoPauseL(aMessage);
				break;
			case EMMFControllerStop:
				complete = DoStopL(aMessage);
				break;
			case EMMFControllerGetPosition:
				complete = DoGetPositionL(aMessage);
				break;
			case EMMFControllerSetPosition:
				complete = DoSetPositionL(aMessage);
				break;
			case EMMFControllerGetDuration:
				complete = DoGetDurationL(aMessage);
				break;
			case EMMFControllerGetNumberOfMetaDataEntries:
				complete = DoGetNumberOfMetaDataEntriesL(aMessage);
				break;
			case EMMFControllerGetSizeOfMetaDataEntry:
				complete = DoGetSizeOfMetaDataEntryL(aMessage);
				break;
			case EMMFControllerGetMetaDataEntry:
				complete = DoGetMetaDataEntryL(aMessage);
				break;
			case EMMFControllerSetPrioritySettings:
				complete = DoSetPrioritySettingsL(aMessage);
				break;
			case EMMFControllerCancelAddDataSource:
				complete = ETrue;//Nothing to cancel since AddDataSource is synchronous server-side
				break;
			case EMMFControllerCancelAddDataSink:
				complete = ETrue;//Nothing to cancel since AddDataSink is synchronous server-side
				break;
			case EMMFControllerAddFileHandleDataSource:
				complete = DoAddFileHandleDataSourceL(aMessage);
				break;
			case EMMFControllerAddFileHandleDataSink:
				complete = DoAddFileHandleDataSinkL(aMessage);
				break;
			case EMMFControllerSourceSinkInitDataPreload:
				complete = DoPreloadSourceSinkInitDataL(aMessage);
				break;
			case EMMFControllerAddFileHandleDataSourceWithInitData:
				complete = DoAddFileHandleDataSourceWithInitDataL(aMessage);
				break;
			case EMMFControllerAddFileHandleDataSinkWithInitData:
				complete = DoAddFileHandleDataSinkWithInitDataL(aMessage);
				break;
			default:
				User::Leave(KErrNotSupported);
				break;
				}
			if (complete)
				aMessage.Complete(KErrNone);
			}
		else
			{
			// Must be a custom command
			DoCustomCommand(aMessage);
			}
		}
	else
		{
		// Message for one of the MMF Objects
		CMMFObject* object = NULL;
		User::LeaveIfError(MMFObjectContainerL().FindMMFObject(aMessage.Destination(), object));
		object->HandleRequest(aMessage);
		}

	}

TBool CMMFController::DoAddDataSourceL(TMMFMessage& aMessage)
	{
	// Get the UID of the source from the client
	TMMFUidPckg uidPckg;
	aMessage.ReadData1FromClientL(uidPckg);

	// Get the size of the init data and create a buffer to hold it
	TInt desLength = aMessage.SizeOfData2FromClient();
	// Leaving here in order to prevent a panic in the NewLC if the value is negative
	User::LeaveIfError(desLength); 
	HBufC8* buf = HBufC8::NewLC(desLength);
	TPtr8 ptr = buf->Des();
	aMessage.ReadData2FromClientL(ptr);

	// Create the source
	MDataSource* source = MDataSource::NewSourceL(uidPckg(), *buf);
	CleanupStack::PopAndDestroy(buf);//buf
	AddMDataSourceAndRepondClientL(source, aMessage);
	
	return ETrue;
	}
	
	
TBool CMMFController::DoAddDataSinkL(TMMFMessage& aMessage)
	{
	// Get the UID of the sink from the client
	TMMFUidPckg uidPckg;
	aMessage.ReadData1FromClientL(uidPckg);

	// Get the size of the init data and create a buffer to hold it
	TInt desLength = aMessage.SizeOfData2FromClient();
	// Leaving here in order to prevent a panic in the NewLC if the value is negative
	User::LeaveIfError(desLength);
	HBufC8* buf = HBufC8::NewLC(desLength);
	TPtr8 ptr = buf->Des();
	aMessage.ReadData2FromClientL(ptr);

	// Create the sink
	MDataSink* sink = MDataSink::NewSinkL(uidPckg(), *buf);
	CleanupStack::PopAndDestroy(buf);//buf
	AddMDataSinkAndRepondClientL(sink, aMessage);
	
	return ETrue;
	}
	
TBool CMMFController::DoRemoveDataSourceL(TMMFMessage& aMessage)
	{
	TMMFMessageDestinationPckg handleInfo;
	aMessage.ReadData1FromClientL(handleInfo);
	
	// Find the correct source
	CMMFObject* object = NULL;
	CMMFDataSourceHolder* holder = NULL;
	User::LeaveIfError(MMFObjectContainerL().FindMMFObject(handleInfo(), object));
	// Cast the object found if possible..
	if (object->Handle().InterfaceId() == KUidInterfaceMMFDataSourceHolder)
		holder = STATIC_CAST(CMMFDataSourceHolder*, object);
	else
		User::Leave(KErrBadHandle);

	// Try to remove the data source from the plugin
	RemoveDataSourceL(holder->DataSource());

	// If we're here, the removal worked so remove the source from the object array and delete it
	MMFObjectContainerL().RemoveAndDestroyMMFObject(*holder);
	return ETrue;
	}

TBool CMMFController::DoRemoveDataSinkL(TMMFMessage& aMessage)
	{
	TMMFMessageDestinationPckg handleInfo;
	aMessage.ReadData1FromClientL(handleInfo);
	
	// Find the correct source
	CMMFObject* object = NULL;
	CMMFDataSinkHolder* holder = NULL;
	User::LeaveIfError(MMFObjectContainerL().FindMMFObject(handleInfo(), object));
	// Cast the object found if possible..
	if (object->Handle().InterfaceId() == KUidInterfaceMMFDataSinkHolder)
		holder = STATIC_CAST(CMMFDataSinkHolder*, object);
	else
		User::Leave(KErrBadHandle);

	// Try to remove the data sink from the plugin
	RemoveDataSinkL(holder->DataSink());

	// If we're here, the removal worked so remove the source from the object array and delete it
	MMFObjectContainerL().RemoveAndDestroyMMFObject(*holder);
	return ETrue;
	}

TBool CMMFController::DoResetL(TMMFMessage& /*aMessage*/)
	{
	ResetL();
	// Controller extended data must be kept until the death of controller
	const RPointerArray<CMMFObject>& objects = iMMFObjectContainer->MMFObjects();
	for (TInt i = objects.Count()-1; i >= 0; i--)
		{
		CMMFObject* object = objects[i];
		if (object->Handle().InterfaceId() != KUidMMFControllerExtendedDataHolder)
			{
			iMMFObjectContainer->RemoveAndDestroyMMFObject(*object);
			}
		}
	return ETrue;
	}

TBool CMMFController::DoPrimeL(TMMFMessage& aMessage)
	{
	PrimeL(aMessage);
	return EFalse;
	}


EXPORT_C void CMMFController::PrimeL(TMMFMessage& aMessage)
	{
	PrimeL();
	aMessage.Complete(KErrNone);
	}


TBool CMMFController::DoPlayL(TMMFMessage& aMessage)
	{
	PlayL(aMessage);
	return EFalse;
	}

EXPORT_C void CMMFController::PlayL(TMMFMessage& aMessage)
	{
	PlayL();
	aMessage.Complete(KErrNone);
	}



TBool CMMFController::DoPauseL(TMMFMessage& aMessage)
	{
	PauseL(aMessage);
	return EFalse;
	}

EXPORT_C void CMMFController::PauseL(TMMFMessage& aMessage)
	{
	PauseL();
	aMessage.Complete(KErrNone);
	}


TBool CMMFController::DoStopL(TMMFMessage& aMessage)
	{
	StopL(aMessage);
	return EFalse;
	}

EXPORT_C void CMMFController::StopL(TMMFMessage& aMessage)
	{
	StopL();
	aMessage.Complete(KErrNone);
	}



TBool CMMFController::DoGetPositionL(TMMFMessage& aMessage)
	{
	TMMFTimeIntervalMicroSecondsPckg pckg(PositionL());
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFController::DoSetPositionL(TMMFMessage& aMessage)
	{
	TMMFTimeIntervalMicroSecondsPckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	SetPositionL(pckg());
	return ETrue;
	}

TBool CMMFController::DoGetDurationL(TMMFMessage& aMessage)
	{
	TMMFTimeIntervalMicroSecondsPckg pckg(DurationL());
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFController::DoSetPrioritySettingsL(TMMFMessage& aMessage)
	{
	TMMFPrioritySettingsPckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	SetPrioritySettings(pckg());
	return ETrue;
	}

void CMMFController::DoCustomCommand(TMMFMessage& aMessage)
	{
	// First, try giving the message to the custom command parser manager...
	TBool handled = EFalse;
	if (iCustomCommandParserManager)
		{
		if (aMessage.Destination().InterfaceId() == KUidInterfaceMMFDRMControl ||
			aMessage.Destination().InterfaceId() == KUidInterfaceMMFVideoDRMExt)
			{
			TBool secureDrmMode = EFalse;
			TRAPD(err, secureDrmMode = GetExtendedDataL()->SecureDrmMode());
			if (err)
				{
				aMessage.Complete(err);
				handled = ETrue;
				}
			else if (secureDrmMode)
				{
				aMessage.Complete(KErrPermissionDenied);
				handled = ETrue;
				}
			}
		
		if (!handled)
			{
			handled = iCustomCommandParserManager->HandleRequest(aMessage);
			}
		}
		
	// If the ccp manager couldn't handle the message, pass directly to the 
	// controller plugin as a custom command.
	if (!handled)
		CustomCommand(aMessage);
	}

TBool CMMFController::DoGetNumberOfMetaDataEntriesL(TMMFMessage& aMessage)
	{
	TInt numberOfEntries;
	GetNumberOfMetaDataEntriesL(numberOfEntries);
	TPckgBuf<TInt> pckg(numberOfEntries);
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFController::DoGetSizeOfMetaDataEntryL(TMMFMessage& aMessage)
	{
	// Get the index of the required entry
	TPckgBuf<TInt> pckg;
	aMessage.ReadData1FromClientL(pckg);
	// Get the entry
	CMMFMetaDataEntry* entry = GetMetaDataEntryL(pckg());
	CleanupStack::PushL(entry);
	// Delete any existing buffer
	delete iMetaDataBuffer;
	iMetaDataBuffer = NULL;
	// Create a buffer to hold the externalised entry
	iMetaDataBuffer = CBufFlat::NewL(32);
	RBufWriteStream s;
	s.Open(*iMetaDataBuffer);
	CleanupClosePushL(s);
	entry->ExternalizeL(s);
	CleanupStack::PopAndDestroy(2);//s, entry
	// Write the size of the externalised data back to the client
	pckg() = iMetaDataBuffer->Ptr(0).Length();
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFController::DoGetMetaDataEntryL(TMMFMessage& aMessage)
	{
	// We should have already prepared the buffer
	if (!iMetaDataBuffer)
		User::Leave(KErrNotReady);
	aMessage.WriteDataToClientL(iMetaDataBuffer->Ptr(0));
	return ETrue;
	}

TBool CMMFController::DoPreloadSourceSinkInitDataL(TMMFMessage& aMessage)
	{
	GetExtendedDataL()->ResetSourceSinkInitData();
	
	// Get the size of the init data and create a buffer to hold it
	TInt desLength = aMessage.SizeOfData1FromClient();
	HBufC8* sourceSinkInitData = HBufC8::NewLC(desLength);
	TPtr8 ptr = sourceSinkInitData->Des();
	aMessage.ReadData1FromClientL(ptr);
	
	GetExtendedDataL()->SetSourceSinkInitData(sourceSinkInitData);
	CleanupStack::Pop(sourceSinkInitData);
	return ETrue;
	}

TBool CMMFController::DoAddFileHandleDataSourceWithInitDataL(TMMFMessage& aMessage)
	{
	if (GetExtendedDataL()->SourceSinkInitData() == NULL)
		{
		User::Leave(KErrNotReady);
		}
	HBufC8* initData = GetExtendedDataL()->SourceSinkInitData()->AllocLC();
	
	// Get the client file handle and replace 
	RFile file;
	aMessage.AdoptFileHandleFromClientL(1,2, file);
	CleanupClosePushL(file);
	TPtr8 initDataPtr(initData->Des());
	ReplaceFileHandleInInitDataL(&file, initDataPtr);
	
	// Create the source
	MDataSource* source = MDataSource::NewSourceL(KUidMmfFileSource, *initData);
	CleanupStack::PopAndDestroy(2, initData);	//file, initData
	AddMDataSourceAndRepondClientL(source, aMessage);
	
	GetExtendedDataL()->ResetSourceSinkInitData();
	return ETrue;
	}

TBool CMMFController::DoAddFileHandleDataSinkWithInitDataL(TMMFMessage& aMessage)
	{
	if (GetExtendedDataL()->SourceSinkInitData() == NULL)
		{
		User::Leave(KErrNotReady);
		}
	HBufC8* initData = GetExtendedDataL()->SourceSinkInitData()->AllocLC();
	
	// Get the client file handle and replace
	RFile file;
	aMessage.AdoptFileHandleFromClientL(1,2, file);
	CleanupClosePushL(file);
	TPtr8 initDataPtr(initData->Des());
	ReplaceFileHandleInInitDataL(&file, initDataPtr);
	
	// Create the sink
	MDataSink* sink = MDataSink::NewSinkL(KUidMmfFileSink, *initData);
	CleanupStack::PopAndDestroy(2, initData);	//file, initData
	AddMDataSinkAndRepondClientL(sink, aMessage);
	
	GetExtendedDataL()->ResetSourceSinkInitData();
	return ETrue;
	}
	
void CMMFController::ReplaceFileHandleInInitDataL(RFile* aFile, TDes8& aInitData)
	{
	// aInitData should have the second 4bytes data (first 4bytes represent TUid) containing 
	// a client RFile pointer.
	// This RFile pointer may be invalid due to the fact that client and server threads
	// may reside in different processes, and therefore, different memory space.
	// In this context, we would rather trust the file handle retrieve from the call
	// AdoptFileHandleFromClientL, and replace the RFile pointer with a more reliable one
	TPtr8 filePtrStart = aInitData.MidTPtr(sizeof(TUid));
	RDesWriteStream writeStream;
	writeStream.Open(filePtrStart);
	CleanupClosePushL(writeStream);
	TPckgBuf<RFile*> filePtr(aFile);
	writeStream.WriteL(filePtr);
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(&writeStream);
	}

void CMMFController::AddMDataSourceAndRepondClientL(MDataSource* aSource, TMMFMessage& aMessage)
	{
	CleanupDeletePushL(aSource);	
	CMMFDataSourceHolder* holder = new(ELeave) CMMFDataSourceHolder(*aSource);
	CleanupStack::Pop(aSource);	//aSource (since now owned by holder)
	
	CleanupStack::PushL(holder);
	// Append holder to array of MMFObjects
	User::LeaveIfError(MMFObjectContainerL().AddMMFObject(*holder));
	CleanupStack::Pop(holder);//holder

	// Write source handle info back to client
	TMMFMessageDestination handleInfo(holder->Handle());
	TMMFMessageDestinationPckg handlePckg(handleInfo);
	TInt error = aMessage.WriteDataToClient(handlePckg);

	// Add source to plugin
	if (!error)
		TRAP(error, AddDataSourceL(*aSource));

	if (error)
		{
		// Source not accepted by plugin or we couldn't write handle info back to client, 
		// so delete it and return error to client
		MMFObjectContainerL().RemoveAndDestroyMMFObject(*holder);
		User::Leave(error);
		}	
	}

void CMMFController::AddMDataSinkAndRepondClientL(MDataSink* aSink, TMMFMessage& aMessage)
	{
	CleanupDeletePushL(aSink);
	CMMFDataSinkHolder* holder = new(ELeave) CMMFDataSinkHolder(*aSink);
	CleanupStack::Pop(aSink);	//aSink (since now owned by holder)
	
	CleanupStack::PushL(holder);
	// Append sink to array
	User::LeaveIfError(MMFObjectContainerL().AddMMFObject(*holder));
	CleanupStack::Pop(holder);//holder

	// Write sink handle info back to client
	TMMFMessageDestination handleInfo(holder->Handle());
	TMMFMessageDestinationPckg handlePckg(handleInfo);
	TInt error = aMessage.WriteDataToClient(handlePckg);

	// Add sink to plugin
	if (!error)
		TRAP(error, AddDataSinkL(*aSink));

	if (error)
		{
		// Sink not accepted by plugin or we couldn't write handle info back to client, 
		// so delete it and return error to client
		MMFObjectContainerL().RemoveAndDestroyMMFObject(*holder);
		User::Leave(error);
		}
	}

CMMFControllerExtendedData* CMMFController::GetExtendedDataL()
	{
	CMMFControllerExtendedData* data = NULL;
	
	// iMMFObjectContainer and CMMFControllerExtendedData must be constructed 
	// in CMMFController constructor. Only 1 CMMFControllerExtendedData object can
	// be found in iMMFObjectContainer.		
	const RPointerArray<CMMFObject>& objects = MMFObjectContainerL().MMFObjects();
	for (TInt i = 0; i < objects.Count(); i++)
		{
		if (objects[i]->Handle().InterfaceId() == KUidMMFControllerExtendedDataHolder)
			{
			data = static_cast<CMMFControllerExtendedData*>(objects[i]);
			break;
			}
		}
	if (data == NULL)
		{
		// In this case, extended data has not been constructed and added into iMMFObjectContainer.
		// This is a situation where GetExtendedDataL is being indirectly called in the controller
		// initialization process (i.e. CMMFController::NewL() or constructor of licensee's controller).
		// Extended data should have a copy stored in Dll::Tls already at this point.
		const CMMFControllerExtendedData* dataCopy = static_cast<CMMFControllerExtendedData*>(Dll::Tls());
		data = CreateExtendedDataL(dataCopy->ClientThreadId());
		CleanupStack::PushL(data);
		User::LeaveIfError(MMFObjectContainerL().AddMMFObject(*data));
		CleanupStack::Pop(data);
		}
	return data;
	}

CMMFControllerExtendedData* CMMFController::CreateExtendedDataL(TThreadId aClientTid)
	{
	// In secureDRMMode, SecureDRM server and user are running in
	// different processes.
	TProcessId clientProcessId;
	RThread clientThread;
	CleanupClosePushL(clientThread);
	User::LeaveIfError(clientThread.Open(aClientTid));
	
	RProcess clientProcess;
	CleanupClosePushL(clientProcess);
	User::LeaveIfError(clientThread.Process(clientProcess));
	clientProcessId = clientProcess.Id();
	CleanupStack::PopAndDestroy(2, &clientThread);	// clientProcess, clientThread
	
	RProcess thisProcess;
	TBool secureDrmMode = clientProcessId != thisProcess.Id();
	thisProcess.Close();
	
	CMMFControllerExtendedData* dataSet = new(ELeave) CMMFControllerExtendedData();
	CleanupStack::PushL(dataSet);
	dataSet->SetClientThreadId(aClientTid);
	dataSet->SetSecureDrmMode(secureDrmMode);
	CleanupStack::Pop(dataSet);
	
	return dataSet;
	}


EXPORT_C CMMFCustomCommandParserManager* CMMFCustomCommandParserManager::NewL()
	{
	return new(ELeave) CMMFCustomCommandParserManager;
	}

CMMFCustomCommandParserManager::CMMFCustomCommandParserManager()
	{
	}

EXPORT_C CMMFCustomCommandParserManager::~CMMFCustomCommandParserManager()
	{
	iParsers.ResetAndDestroy();
	iParsers.Close();
	}

EXPORT_C TBool CMMFCustomCommandParserManager::HandleRequest(TMMFMessage& aMessage)
	{
	TBool handledRequest = EFalse;
	for (TInt i=0; i<iParsers.Count(); i++)
		{
		CMMFCustomCommandParserBase* c = iParsers[i];
		if (c->InterfaceId() == aMessage.Destination().InterfaceId())
			{
			c->HandleRequest(aMessage);
			handledRequest = ETrue;
			break;
			}
		}
	return handledRequest;
	}

EXPORT_C void CMMFCustomCommandParserManager::AddCustomCommandParserL(CMMFCustomCommandParserBase& aParser)
	{
	User::LeaveIfError(iParsers.Append(&aParser));
	}


TBool CMMFController::DoAddFileHandleDataSourceL(TMMFMessage& aMessage)
	{
	RFile file;
	aMessage.AdoptFileHandleFromClientL(1,2, file);
	CleanupClosePushL(file);
	TMMFFileHandleConfig fileConfig;
	fileConfig().iFile = &file;
	// Create the source
	MDataSource* source = MDataSource::NewSourceL(KUidMmfFileSource, fileConfig);
	CleanupStack::PopAndDestroy(&file);
	CleanupDeletePushL(source);	
	CMMFDataSourceHolder* holder = new(ELeave) CMMFDataSourceHolder(*source);
	CleanupStack::Pop(source);//source (since now owned by holder)

	CleanupStack::PushL(holder);
	// Append holder to array of MMFObjects
	User::LeaveIfError(MMFObjectContainerL().AddMMFObject(*holder));
	CleanupStack::Pop(holder);//holder

	// Write source handle info back to client
	TMMFMessageDestination handleInfo(holder->Handle());
	TMMFMessageDestinationPckg handlePckg(handleInfo);
	TInt error = aMessage.WriteDataToClient(handlePckg);

	// Add source to plugin
	if (!error)
		TRAP(error, AddDataSourceL(*source));

	if (error)
		{
		// Source not accepted by plugin or we couldn't write handle info back to client, 
		// so delete it and return error to client
		MMFObjectContainerL().RemoveAndDestroyMMFObject(*holder);
		User::Leave(error);
		}
	
	return ETrue;
	}


TBool CMMFController::DoAddFileHandleDataSinkL(TMMFMessage& aMessage)
	{
	RFile file;
	aMessage.AdoptFileHandleFromClientL(1,2, file);
	CleanupClosePushL(file);
	TMMFFileHandleConfig fileConfig;
	fileConfig().iFile = &file;
	// Create the sink
	MDataSink* sink = MDataSink::NewSinkL(KUidMmfFileSink, fileConfig);
	CleanupStack::PopAndDestroy(&file); // close our handle to the file
	CleanupDeletePushL(sink);
	CMMFDataSinkHolder* holder = new(ELeave) CMMFDataSinkHolder(*sink);
	CleanupStack::Pop(sink);//sink (since now owned by holder)

	CleanupStack::PushL(holder);
	// Append sink to array
	User::LeaveIfError(MMFObjectContainerL().AddMMFObject(*holder));
	CleanupStack::Pop(holder);//holder


	// Write sink handle info back to client
	TMMFMessageDestination handleInfo(holder->Handle());
	TMMFMessageDestinationPckg handlePckg(handleInfo);
	TInt error = aMessage.WriteDataToClient(handlePckg);

	// Add sink to plugin
	if (!error)
		TRAP(error, AddDataSinkL(*sink));

	if (error)
		{
		// Sink not accepted by plugin or we couldn't write handle info back to client, 
		// so delete it and return error to client
		MMFObjectContainerL().RemoveAndDestroyMMFObject(*holder);
		User::Leave(error);
		}
	
	return ETrue;
	}

EXPORT_C TThreadId CMMFController::ClientThreadIdL()
	{
	return GetExtendedDataL()->ClientThreadId();
	}

EXPORT_C TBool CMMFController::IsSecureDrmModeL()
	{
	return GetExtendedDataL()->SecureDrmMode();
	}

