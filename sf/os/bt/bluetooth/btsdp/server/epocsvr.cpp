// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/logger.h>
#include <e32svr.h>
#include <e32uid.h>
#include <btsdp.h>
#include "DataEncoder.h"
#include "EncoderVisitor.h"
#include "sdputil.h"
#include "reqhandler.h"
#include "listener.h"
#include "epocsvr.h"
#include "ipcinternals.h"
#include "attrvalueencoded.h"

#include "epocsvr.inl"
#include <f32file.h>	//	For RFs

#ifdef __EPOC32__
#include <c32comm.h>
#endif

#include "SdpServerSecurityPolicy.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_SDP_SERVER);
#endif

#ifdef _DEBUG
PANICCATEGORY("epocsvr");
#endif

GLDEF_D TInt sdp_debug_level = 5;

void Panic(TSdpServerPanics aCode)
	{
	User::Panic(KSdpServerPanicName, aCode);
	}

/**
 The SDP database subsession class has been removed from the SDP server but to 
 maintain BC we still have to deal with subsession requests from the client.
 The constant below is used as the subsession handle for any new subsession
 requests from a client as we no longer maintain a real handle to a subsession.
**/
static const TInt KSdpDatabaseSubSessionHandle = 0x1A2B3C4D;

inline CSdpServerShutdown::CSdpServerShutdown()
	:CTimer(-1)
	{CActiveScheduler::Add(this);}
inline void CSdpServerShutdown::ConstructL()
	{CTimer::ConstructL();}
inline void CSdpServerShutdown::Start()
	{After(KSdpServerShutdownDelay);}

//


void CSdpServerShutdown::RunL()
/**
 Initiate server exit when the timer expires
**/
	{
	LOG_FUNC
	CActiveScheduler::Stop();
	}

CSdpServer::CSdpServer()
	: CPolicyServer(0, KSdpServerPolicy)
	{
	CONNECT_LOGGER
	LOG_FUNC
	}

CSdpServer* CSdpServer::NewLC()
/**
 Create the server object and leave on cleanup stack.
**/
	{
	LOG_STATIC_FUNC
	CSdpServer* self=new(ELeave) CSdpServer();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CSdpServer::ConstructL()
	{
	LOG_FUNC
	StartL(KSdpServerName);
	
	iSdpDatabase = CSdpDatabase::NewL();
	iUuidManager = CSdpUuidManager::NewL(*iSdpDatabase);

	TPckg<TUint> intBuf(0);
	iDbState = CSdpAttrValueUint::NewUintL(intBuf);

	// Create the buffer the correct size for storing a Uint
	TInt encodedSize = TElementEncoder::EncodedSize(iDbState->Type(), iDbState->DataSize());
	iEncodeBuf.CreateL(encodedSize);
	TElementEncoder encoder(iEncodeBuf);

	iEncoderVisitor = CAttrEncoderVisitor::NewL(encoder);
	BuildRecordZeroL(); //Unencoded
	Database()->EncodeDbL(); /*If we get to here, database exists*/

	User::LeaveIfError(iSocketServ.Connect());
	iSdpListener = CSdpListener::NewL(iSocketServ, 4, *(Database()));

	//Ensure that the server will exit even if the 1st client fails to connect
	//Start shutdown timer only after all the heavy duty initialisation has been done. 
	//Otherwise it could fire before any client had a chance to connect to the server
	iShutdown.ConstructL();
	iShutdown.Start();
	}

CSdpServer::~CSdpServer()
	{
	LOG_FUNC
	delete iSdpListener;
	iSocketServ.Close();
	
	delete iRecZero;
	delete iDbState;
	delete iEncoderVisitor;
	iEncodeBuf.Close();

	delete iUuidManager;
	delete iSdpDatabase;
	CLOSE_LOGGER
	}

CSdpDatabase* CSdpServer::Database()
	{
	LOG_FUNC
	return iSdpDatabase;
	}

void CSdpServer::BuildRecordZeroL()
/**
 Record 0 should be used when a server instance is created.
 note that attributes 2, 5 and 0x201 should be updated.
 also note only English, pas de Francais, keine Deutsch, non Espanol
**/
	{
	LOG_FUNC
	ASSERT_DEBUG(iSdpDatabase);
	
	TBuf8<2> attrId;
	TBuf8<4> val;
	
	iRecZero = CSdpServRecord::NewServerSideL(User::Identity());

	// Set Attr 0 (Record handle) to 0
	attrId.FillZ(2);
	val.FillZ(4);
	iRecZero->BuildUintL(attrId)->BuildUintL(val);

	// Set Attr 1 (service class list) to list with UUID = 0x1000
	attrId[0] = 0x00;
	attrId[1] = 0x01;
	iRecZero->BuildUintL(attrId)->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(KServiceDiscoveryServerServiceClassUUID)))
			->EndListL();

	// Set Attr 2 (service record state) to 0
	attrId[0] = 0x00;
	attrId[1] = 0x02;
	val.FillZ(4);
	iRecZero->BuildUintL(attrId)->BuildUintL(val);

	// Set attr 4 (protocol list) to L2CAP, no RFCOMM, no OBEX
	attrId[0] = 0x00;
	attrId[1] = 0x04;
	val.FillZ(4);
	val[3] = 1;
	iRecZero->BuildUintL(attrId)->BuildDESL()
			->StartListL()
				->BuildDESL()
				->StartListL()
					->BuildUUIDL(TUUID(TUint16(0x0100))) // L2CAP
					->EndListL()
					->EndListL();

	// Set Attr 5 (browse group list) to list with one UUID
	// 0x1000 (SDP server class)
	// this should be updated with other service classes when other services are added.
	attrId[0] = 0x00;
	attrId[1] = 5;
	iRecZero->BuildUintL(attrId)->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint32(0x1000)))
			->EndListL();

	// Set Attr 0x006 (language base)
	attrId[0] = 0x00;
	attrId[1] = 0x06;
	TUint16 lang = 0x656e;
	TUint16 coding = 0x006a;
	TUint16 base = 0x0100;

	iRecZero->BuildUintL(attrId)->BuildDESL()
			->StartListL()
				->BuildUintL(TSdpIntBuf<TUint16>(lang)) // english
				->BuildUintL(TSdpIntBuf<TUint16>(coding)) // UTF-8
				->BuildUintL(TSdpIntBuf<TUint16>(base)) // language base
			->EndListL();

	// Set Attr 0x007 (time to live) to 1200 (0x4B0) seconds (20 minutes)
	attrId[0] = 0x00;
	attrId[1] = 0x07;
	val.FillZ(4);
	val[2]=4;
	val[3]=0xb0;
	iRecZero->BuildUintL(attrId)->BuildUintL(val);

	// Set Attr 0x008 (availability) to 0xff - fully available - not in use
	attrId[0] = 0x00;
	attrId[1] = 0x08;
	TBuf8<1> val4;
	val4.FillZ(1);
	val4[0]=0xff;
	iRecZero->BuildUintL(attrId)->BuildUintL(val4);

	// Set Attr 0x100 (default Name) to string
	attrId[0] = 0x01;
	attrId[1] = 0;
	iRecZero->BuildUintL(attrId)->BuildStringL(_L8("SDP Server"));

	// Set Attr 0x101 (def. description) to string
	attrId[0] = 0x01;
	attrId[1] = 1;
	iRecZero->BuildUintL(attrId)->BuildStringL(_L8("Provides local service information for remote devices."));

	// Set Attr 0x102 (def. provider) to Symbian
	attrId[0] = 0x01;
	attrId[1] = 2;
	iRecZero->BuildUintL(attrId)->BuildStringL(_L8("Symbian OS"));

	// Set Attr 0x200 (version number support) to list with 1.0 and 1.1
	iRecZero->BuildUintL(TSdpIntBuf<TSdpAttributeID>(0x200))
			->BuildDESL()
			->StartListL()
				->BuildUintL(TSdpIntBuf<TUint16>(0x0100)) // 1.0
				->BuildUintL(TSdpIntBuf<TUint16>(0x0101)) // 1.1
			->EndListL();

	// Set Attr 0x201 (service database state) to 0
	attrId[0] = 0x02;
	attrId[1] = 0x01;
	val.FillZ(4);
	iRecZero->BuildUintL(attrId)->BuildUintL(val);

	// Add the record into the database
	Database()->AddRecord(iRecZero);
	}

CSession2* CSdpServer::NewSessionL(const TVersion& /*aVersion*/) const
	{
	LOG_FUNC
	User::Leave(KErrNotSupported);
	return 0;
	}

CSession2* CSdpServer::NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const
	{
	LOG_FUNC
	TVersion v(KSdpServerMajorVersionNumber,KSdpServerMinorVersionNumber,KSdpServerBuildVersionNumber);
	if (!User::QueryVersionSupported(v,aVersion))
		{
		User::Leave(KErrNotSupported);
		}
	// make new session
	return new(ELeave) CSdpServSession(aMessage);
	}

void CSdpServer::AddSession()
/**
A new session is being created
Cancel the shutdown timer if it was running
**/
	{
	LOG_FUNC
	++iSessionCount;
	if (iSessionCount > iMaxSessionCount)
		{
		iMaxSessionCount = iSessionCount;
		}
	iShutdown.Cancel();
	}

void CSdpServer::DropSession()
/**
A session is being destroyed
Start the shutdown timer if it is the last session.
**/
	{
	__ASSERT_DEBUG(iSessionCount > 0, PanicServer(ESdpBadState));
	
	if (--iSessionCount==0)
		{
		iShutdown.Start();
		}
	}

void CSdpServer::CheckAllowedL(const RMessage2& aMessage, const CSdpServRecord& aRecord)
	{
	LOG_FUNC
	if (!Allowed(aMessage, aRecord))
		{
		User::Leave(KErrPermissionDenied);
		}
	}

TBool CSdpServer::Allowed(const RMessage2& aMessage, const CSdpServRecord& aRecord)
	{
	LOG_FUNC
	_LIT_SECURITY_POLICY_S0(KSidPolicy, aRecord.ClientUid().iUid);
	
	//if the secure id of the record matches the secure id of the message sender
	if (KSidPolicy().CheckPolicy(aMessage))
		{
		//allowed
		return ETrue;
		}
	
	//not allowed
	return EFalse;
	}

CSdpServRecord* CSdpServer::FindRecordByHandle(const TSdpServRecordHandle aHandle)
/**
Returns a pointer to the first record object found with its 
attribute '0' set to the specified handle.
If no such record object is found, this function return 0 (Null).
**/
	{
	LOG_FUNC
	for(TServRecordIter recIter(Database()->RecordIter()); recIter; recIter++)
		{// Iterate thru records in iDatabase
		if((*recIter).Handle()==aHandle)
			{
			//Destructor of a record removes/destroys all its 
			//attributes and deques it from database list.
			return &(*recIter);
			}
		}
	return 0;
	}

CSdpAttr* CSdpServer::FindAttributeByID(CSdpServRecord& aRecord, const TSdpAttributeID aAttrID)
/**
Returns a pointer to the attribute object found in the 
specified record that has the specified ID.
If no such attribute object is found, this function return 0 (Null).
**/
	{
	LOG_FUNC
	for(TServAttrIter attrIter(aRecord.AttributeIter()); attrIter; attrIter++)
		{// Iterate thru attributes in record
		if((*attrIter).AttributeID()==aAttrID)
			{
			return &(*attrIter);
			}
		}
	return 0;
	}

/**
This function must be called whenever a service record is added or removed.

The SDP server 0 service record has a ServerServiceDatabaseState attribute
which, if present, must be updated when a service record is added or removed.
*/
void CSdpServer::DatabaseStateChange()
	{
	LOG_FUNC
	//increment database state attribute in record zero
	if(iRecZero)
		{
		TUint state = iDbState->Uint();

		TPckg<TUint> stateBuf(0);
		SdpUtil::PutUint(&stateBuf[0], ++state, sizeof(TUint));

		iDbState->SetUintValue(stateBuf);

		// This encodes iDbState into the buffer provided at construction
		iEncodeBuf.SetLength(0);
		TRAPD(err, iEncoderVisitor->EncodeAttributeL(*iDbState));
		// Attribute encoding can only fail if the attribute is of an unknown type 
		// or the supplied buffer is too small.  We have set the length of the
		// buffer to the correct length on creation so that will not fail.
		// We know iDbState is a CSdpAttrValueUint so cannot fail.
		__ASSERT_ALWAYS(!err, PanicServer(ESdpAttributeEncodingFailed));

		CSdpAttr* attr = FindAttributeByID(*iRecZero, KSdpAttrIdSdpServerServiceDatabaseState);
		__ASSERT_ALWAYS(attr->Value().Type() == ETypeEncoded, PanicServer(ESdpStoredAttrValNotEncoded));
		reinterpret_cast<CSdpAttrValueEncoded&>(attr->Value()).SetEncodedValue(iEncodeBuf);

		iRecZero->RecordStateChange();
		iUuidManager->NotifySdpRecordChange();
		}
	}

CSdpServRecord* CSdpServer::CreateServiceRecordL(const RMessage2& aMessage)
/**
Creates a record on the global database. Returns a pointer to the new record.
**/
	{
	LOG_FUNC

	CSdpServRecord* record = CSdpServRecord::NewServerSideL(aMessage.Identity());
	
	CleanupStack::PushL(record);
	
	//Build record handle attribute...
	//..NB if we had created the record using iDatabase.NewRecordL we would not
	//     have to add the handle attribute manually..BUT then to encode the 
	//	   the handle attribute would have required us to dig it out of the
	//	   record first which seems less robust than what is below.
	//	   For 1.1 we might create an EncodeRecordL function for a single record
	//	   in the same way we now have an EncodeDbL function for the whole database.
	//	   This would then allow us to call iDatabase>NewRecordL, followed by 
	//	   record.EncodeRecordL.	
	CSdpAttr* handleAttr = CSdpAttr::NewL(0x00, record);
	record->AddAttribute(handleAttr);
	handleAttr->BuildUintL(TSdpIntBuf<TSdpServRecordHandle>(Database()->NextFreeHandle()));
	CSdpAttrValue* handleAttrValue = &(handleAttr->Value());

	TUint size = TElementEncoder::EncodedSize(handleAttrValue->Type(), 
											  handleAttrValue->DataSize());
	HBufC8* bufEncodedHandle = HBufC8::NewL(size);
	CleanupStack::PushL(bufEncodedHandle);
	TPtr8 ptrEncodedHandle = bufEncodedHandle->Des();

	TElementEncoder ee(ptrEncodedHandle);
	CAttrEncoderVisitor::EncodeAttributeL(ee, *handleAttrValue);
	handleAttr->BuildEncodedL(ptrEncodedHandle); 

	CleanupStack::PopAndDestroy(); //bufEncodedHandle

	//Write record handle back to EPOC client (i.e. user)
	TSdpServRecordHandlePckgBuf pckg(record->Handle());
	pckg() = record->Handle();
	aMessage.WriteL(1, pckg); //ss//

	//Build service class attribute...
 	TInt bufServiceClassLen = aMessage.GetDesLengthL(0); //ss//
 	HBufC8* bufServiceClass = HBufC8::NewLC(bufServiceClassLen);
 	TPtr8 ptrServiceClass = bufServiceClass->Des();
 	aMessage.ReadL(0, ptrServiceClass); //get message data into buf (via ptr) //ss//

	TBuf8<2> attrId;
	attrId.FillZ(2); // Order of following lines important to value in attrId!!!
	attrId[0] = 0x00;
	attrId[1] = 0x01;

	CSdpAttr* attr = CSdpAttr::NewL(0x0001, record);
	record->AddAttribute(attr);
	attr->BuildEncodedL(ptrServiceClass);

	//Add record, now with handle and service class attributes, to database
	Database()->AddRecord(record);
	CleanupStack::PopAndDestroy(); //bufServiceClass
 	CleanupStack::Pop(); //record,
	
	DatabaseStateChange();

	//Return a pointer to the new record
	return record;
	}

CSdpServRecord* CSdpServer::FindAndCheckServiceRecordForDeletion(const RMessage2& aMessage)
/**
Checks it's ok to delete a record on the global database using handle sent in message slot 0.
Returns the pointer of the record that is to be deleted. 
**/
	{
	LOG_FUNC
	
	TSdpServRecordHandlePckgBuf pckg;
 	TRAPD(err, aMessage.ReadL(0, pckg)); //get message data into buf (via ptr)
	if(err)
		{
		// Client has sent us a dodgy descriptor, punish them!
		PanicClient(aMessage, ESdpBadDescriptor);
		return NULL;
		}
	
	//the service record handle should be non-zero
	__ASSERT_DEBUG(pckg() != 0, Panic(ESdpServerDeleteServiceRecordHandleZero));
	
	CSdpServRecord* record = FindRecordByHandle(pckg());
	if(!record)
		{
		PanicClient(aMessage, ESdpNonExistantRecordHandle);
		return NULL;
		}

	TRAP(err, CheckAllowedL(aMessage, *record));
	if(err)
		{
		// Client has been naughty, trying to delete things they shouldn't!
		PanicClient(aMessage, ESdpBadRequest);
		return NULL;
		}

	//Return a pointer to the record to be deleted
	return record;
	}

void CSdpServer::UpdateAttributeL(const RMessage2& aMessage)
/**
Updates/Creates an attribute in a record on the global database using the packaged 
record handle, the packaged attribute id, and the encoded attribute value 
sent in message slots 0, 1, and 2. 
**/
	{
	LOG_FUNC
	
	TSdpAttributeIDPckgBuf idPckg;
	TSdpServRecordHandlePckgBuf handlePckg;
	
 	aMessage.ReadL(0, handlePckg); //get handle into handle package
 	
	//the service record handle should be non-zero
	__ASSERT_DEBUG(handlePckg() != 0, Panic(ESdpServerUpdateAttributeRecordHandleZero));
	
 	aMessage.ReadL(1, idPckg); //get id into id package
 	TInt bufLen = aMessage.GetDesLengthL(2);
 	HBufC8* buf = HBufC8::NewLC(bufLen);
 	TPtr8 ptr = buf->Des();
 	aMessage.ReadL(2, ptr); //get message data into buf (via ptr)
 	
	CSdpServRecord* record = FindRecordByHandle(handlePckg());
	if(!record)
		{
		PanicClient(aMessage, ESdpNonExistantRecordHandle);
		return;
		}
	
	CheckAllowedL(aMessage, *record);
	
	UpdateOrCreateAtributeL(*record, idPckg(), ptr);
	
	iUuidManager->NotifySdpRecordChange();
	
	CleanupStack::PopAndDestroy(); //buf
	}

void CSdpServer::UpdateOrCreateAtributeL(CSdpServRecord& aRecord, TSdpAttributeID aAttrId, const TDesC8& aBuf)
	{
	LOG_FUNC
	CSdpAttr* attr = FindAttributeByID(aRecord, aAttrId);
	if(attr)
		{
		//Deletes old attr value, and replaces with encoded value found in 'ptr'.
		attr->BuildEncodedL(aBuf);	
		}
	else
		{
		TPckg<TUint16> idBuf(0);
		BigEndian::Put16(&idBuf[0], aAttrId);
		static_cast<CSdpAttr*>(aRecord.BuildUintL(idBuf))->BuildEncodedL(aBuf);
		}

	aRecord.RecordStateChange();
	}

void CSdpServer::DeleteAttribute(const RMessage2& aMessage)
/**
Deletes an attribute on the global database using the packaged record and the packaged 
attribute id found in record handle and attribute id sent in message slots 0 and 1. 
**/
	{
	LOG_FUNC
	
	TSdpAttributeIDPckgBuf idPckg;
	TSdpServRecordHandlePckgBuf handlePckg;
	
 	TRAPD(err, aMessage.ReadL(0, handlePckg)); //get handle into handle package
	if(err)
		{
		// Client has sent us a dodgy descriptor, punish them!
		PanicClient(aMessage, ESdpBadDescriptor);
		return;
		}
	
	//the service record handle should be non-zero
	__ASSERT_DEBUG(handlePckg() != 0, Panic(ESdpServerDeleteAttributeRecordHandleZero));
	
 	TRAP(err, aMessage.ReadL(1, idPckg)); //get id into id package
	if(err)
		{
		// Client has sent us a dodgy descriptor, punish them!
		PanicClient(aMessage, ESdpBadDescriptor);
		return;
		}

	CSdpServRecord* record = FindRecordByHandle(handlePckg());
	if(!record)
		{
		PanicClient(aMessage, ESdpNonExistantRecordHandle);
		return;
		}

	TRAP(err, CheckAllowedL(aMessage, *record));
	if(err)
		{
		// Client has been naughty, trying to delete things they shouldn't!
		PanicClient(aMessage, ESdpBadRequest);
		return;
		}
	
	CSdpAttr* attr = FindAttributeByID(*record, idPckg());
	if(!attr)
		{
		return;
		}

	delete attr;

	record->RecordStateChange();

	return;
	}

void CSdpServer::DeleteServiceRecord(CSdpServRecord* aServiceRecord)
/**
Deletes a record on the global database using a pointer to the record. This 
is used to delete records added to a session when the session is closed if 
the records have not been deleted by the client.
**/
	{
	LOG_FUNC
	if (aServiceRecord)
		{
		//Destructor of a record removes/destroys all its 
		//attributes and deques it from database list.
		delete aServiceRecord;
		DatabaseStateChange();
		}
	}

TInt CSdpServer::RunError(TInt aError)
/**
   Handle an error from ServiceL()
   A bad descriptor error implies a badly programmed client, so panic it;
   otherwise report the error to the client
**/
	{
	LOG_FUNC
	LOG1(_L("s\tCSdpServer::RunError(): %d"), aError);
	if (aError==KErrBadDescriptor)
		{
		PanicClient(Message(),ESdpBadDescriptor);
		}
	else
		{
		Message().Complete(aError);
		}
	ReStart();
	return KErrNone;	// handled the error fully
	}

void PanicClient(const RMessage2& aMessage, TSdpClientPanic aPanic)
/**
   Panic the client and complete the message.
   RMessage2::Panic() also completes the message. This is:
   (a) important for efficient cleanup within the kernel
   (b) a problem if the message is completed a second time
**/
	{
	LOG_STATIC_FUNC
	LOG1(_L("s\tPanicClient: Reason = %d"), aPanic);
	aMessage.Panic(KSdpClientPanic,aPanic);
	}

void PanicServer(TSdpServerPanic aPanic)
/**
Panic our own thread
**/
	{
	LOG_STATIC_FUNC
	LOG1(_L("s\tPanicServer: Reason = %d"), aPanic);
	User::Panic(KSdpServerPanic, aPanic);
	}

class CAttrPrintVisitor : public CBase, public MAttributeVisitor
	{
public:
	CAttrPrintVisitor(){}
#ifdef _DEBUG
    void VisitAttributeL(CSdpAttr& aAttribute)
#else
    void VisitAttributeL(CSdpAttr& /*aAttribute*/)
#endif
		{
		Indent();
		LOG1(_L("Attribute ID: 0x%x"), aAttribute.AttributeID());
		}
	void VisitAttributeValueL(CSdpAttrValue & aValue, TSdpElementType aType)
		{
		TBuf16<64> iString;
		switch (aType)
			{
			case ETypeString:
				iString.Copy(aValue.Des());
				LOG1(_L("\"%S\""),&iString);
				break;
			case ETypeDES:
				LOG(_L(" DES"));
				break;
			case ETypeUint:
				LOG1(_L(" UInt:0x%x"), aValue.Uint());
				break;
			case ETypeUUID:
				LOG(_L(" UUID:0x"));
				HexDes(aValue.UUID().ShortestForm());
				break;
			case ETypeEncoded:
				HexDes(aValue.Des());  // simplest
				break;
			default:
				LOG1(_L("type %d"), aType);
			}
		}
    void StartListL(CSdpAttrValueList &/*aList*/)
		{
		++iIndent;
		Indent();
		LOG(_L("{"));
		}
    void EndListL()
		{
		if(iIndent<=0)
			{
			LOG(_L("ERROR! Unmatched EndList!"));
			__DEBUGGER();
			}
		Indent();
		LOG(_L("}"));
		--iIndent;
		}
private:
#ifdef _DEBUG
	void Indent() {for(TInt i=0; i<iIndent;++i)
						LOG(_L("  "));}

	void HexDes(const TDesC8& aDes)
		{
		for (TInt i = 0; i < aDes.Length(); ++i)
			LOG1(_L("%02x"), aDes[i]);
		};
#else
	void Indent() {}
	void HexDes(const TDesC8&) {}
#endif
	TInt iIndent;
	};

void FlogDb(CSdpDatabase& aDb)
	{
	LOG_STATIC_FUNC
	LOG(_L("Printing Database..."));

	for(TServRecordIter recIter(aDb.RecordIter()); recIter; recIter++)
		{// Iterate thru records in Db
		LOG1(_L("...Printing Record 0x%x"), (*recIter).Handle());
		for(TServAttrIter attrIter((*recIter).AttributeIter()); attrIter; attrIter++)
			{// Iterate thru attributes in record
			CAttrPrintVisitor* theVisitor = new CAttrPrintVisitor();
			if (theVisitor)
				{
				TRAP_IGNORE((*attrIter).AcceptVisitorL(*theVisitor));
				delete theVisitor;
				}
			}
		}
	LOG(_L("End Printing Database..."));
	}


	
static void RunServerL(/*TSdpServerStart& aStart*/)

/**
Perform all server initialisation, in particular creation of the
scheduler and server and then run the scheduler
**/
	{
	LOG_STATIC_FUNC

	CActiveScheduler *scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler); //1st Push
	CActiveScheduler::Install(scheduler); 

	// create the server (leave it on the cleanup stack)
	CSdpServer::NewLC();//2nd Push
	//
	
#ifdef __SDPSERVER_NO_PROCESSES__
	RThread::Rendezvous(KErrNone);        // this causes the client's Rendezvous to complete 
#else
	// naming the server thread after the server helps to debug panics
	// ignore the error returned here - it's not critcal
	User::RenameThread(KSdpServerName);

	// Initialisation complete, now signal the client
	RProcess::Rendezvous(KErrNone);        // this causes the client's Rendezvous to complete 
#endif
	
	//
	// Ready to run
	CActiveScheduler::Start();
	//
	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2);
	}


TInt E32Main()
/**
Server process entry-point
Recover the startup parameters and run the server
**/
	{

	__UHEAP_MARK;
#ifdef TEST_OOM //define TEST_OOM in preprocessor definitions in project settings
	__UHEAP_SETFAIL(RHeap::ERandom, 100);	//Qualified - this results in a false positive error from leavescan (... may employ a macro), because the macro name ends with an 'L'.
#endif
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunServerL());
		delete cleanup;
		}
	//
	__UHEAP_MARKEND;
	
	return r;
	}


//=====================================================================
//CSdpServSession
//=====================================================================

CSdpServer& CSdpServSession::Server()
	{
	LOG_FUNC
	return *static_cast<CSdpServer*>(const_cast<CServer2*>(CSession2::Server()));
	}

// constructor - must pass client to CSession2
CSdpServSession::CSdpServSession(const RMessage2& /*aMessage*/)
	{
	LOG_FUNC
	}

void CSdpServSession::CreateL(const CServer2& /*aServer*/)
	{
	LOG_FUNC
	User::Leave(KErrNotSupported); //FIXME - PRE SECURE_API - NEEDED TO AVOID COMPILER WARNING
	}

void CSdpServSession::CreateL()
/**
2nd phase construct for sessions - called by the CServer2 framework
**/
	{
	LOG_FUNC
	//CSession2::CreateL();	// private and does nowt so removed
	//Add session to server first. If anything leaves, it will be removed by the destructor
	Server().AddSession();
	ConstructL();
	}

void CSdpServSession::ConstructL()
	{
	LOG_FUNC
	// try to reopen the acceptor
	Server().Listener().TryRestartL();
	}

CSdpServSession::~CSdpServSession()
	{
	LOG_FUNC
	Server().DropSession();
	
	//Cleanup any records that have been added during this session but not removed
	RemoveSessionRecords();
	}

void CSdpServSession::RemoveSessionRecords()
	{
	LOG_FUNC
	for (TInt index = iSessionRecords.Count()-1; index >= 0; index--)
		{
		Server().DeleteServiceRecord(iSessionRecords[index]);
		}
	iSessionRecords.Close();
	}

void CSdpServSession::ServiceL(const RMessage2& aMessage)
/**
   Handle a client request.
**/
	{
	LOG_FUNC
	switch(aMessage.Function())
		{
	case ESdpCreateAgentSubSession:
		NewSubSessionL(ESdpAgent, aMessage);
		break;
	case ESdpCreateDatabaseSubSession:
		NewSubSessionL(ESdpDatabase, aMessage);
		break;
	case ESdpResourceCountMarkStart:
		ResourceCountMarkStart();
		aMessage.Complete(KErrNone);
		break;
	case ESdpResourceCountMarkEnd:
		ResourceCountMarkEnd(aMessage); //ss//
		aMessage.Complete(KErrNone);
		break;
	case ESdpResourceCount:
		NumResourcesL(aMessage);
		break;
	case ESdpCloseSubSession:
		CloseSubSession(aMessage);
		break;

	case ESdpDatabaseCreateServiceRecord:
		{
		if (aMessage.Int3() != KSdpDatabaseSubSessionHandle)
			{
			PanicClient(aMessage, ESdpBadSubSessionHandle);
			return;
			}
	
		CSdpServRecord* newRecord = NULL;
		TRAPD(err, newRecord = Server().CreateServiceRecordL(aMessage));
		// Inspect iHandle to see if we panicked the client and already completed 
		if (aMessage.Handle())
			{
			if (newRecord)
				{
				// Update our list of records for this session. Ignore error as
				// we wouldn't want to remove the record if an error did occur,
				// which is very unlikely as typically only 1 or 2 records are
				// added per session.
				(void)iSessionRecords.Append(newRecord);
				}
			aMessage.Complete(err);
			}
		return;
		}
	case ESdpDatabaseDeleteServiceRecord:
		{
		if (aMessage.Int3() != KSdpDatabaseSubSessionHandle)
			{
			PanicClient(aMessage, ESdpBadSubSessionHandle);
			return;
			}

		CSdpServRecord* recordToBeDeleted = Server().FindAndCheckServiceRecordForDeletion(aMessage);
		
		// If recordToBeDeleted returned is NULL, PanicClient will have been called so aMessage.Handle() 
		// is zero and we've already completed - see if this is not the case  
		if (aMessage.Handle())
			{
			TInt index = iSessionRecords.Find(recordToBeDeleted); // Find the index of the record before deleting the record
			// Ignore if the record is not found as it may not have been added
			// in the first place.
			if (index >= 0)
				{
				iSessionRecords.Remove(index);
				}
			Server().DeleteServiceRecord(recordToBeDeleted);
			aMessage.Complete(KErrNone);
			}
		return;
		}
	case ESdpDatabaseUpdateAttribute:
		{
		if (aMessage.Int3() != KSdpDatabaseSubSessionHandle)
			{
			PanicClient(aMessage, ESdpBadSubSessionHandle);
			return;
			}

		TRAPD(err, Server().UpdateAttributeL(aMessage));
		// Inspect iHandle to see if we panicked the client and already completed 
		if (aMessage.Handle())
			{
			aMessage.Complete(err);
			}
		return;
		}
	case ESdpDatabaseDeleteAttribute:
		{
		if (aMessage.Int3() != KSdpDatabaseSubSessionHandle)
			{
			PanicClient(aMessage, ESdpBadSubSessionHandle);
			return;
			}

		Server().DeleteAttribute(aMessage);
		// Inspect iHandle to see if we panicked the client and already completed 
		if (aMessage.Handle())
			{
			aMessage.Complete(KErrNone);
			}
		return;
		}
	case ESdpServerDbgMarkHeap:
		{
#ifdef _DEBUG
		LOG(_L("\tmark sdp server heap"));
		__UHEAP_MARK;
#endif // _DEBUG
		aMessage.Complete(KErrNone);
		break;
		}
	case ESdpServerDbgCheckHeap:
		{
#ifdef _DEBUG
		LOG1(_L("\tcheck sdp server heap (expecting %d cells)"), aMessage.Int0());
		__UHEAP_CHECK(aMessage.Int0());
#endif // _DEBUG
		aMessage.Complete(KErrNone);
		break;
		}
	case ESdpServerDbgMarkEnd:
		{
#ifdef _DEBUG
		LOG1(_L("\tmark end sdp server heap (expecting %d cells)"), aMessage.Int0());
		__UHEAP_MARKENDC(aMessage.Int0());
#endif // _DEBUG
		aMessage.Complete(KErrNone);
		break;
		}
	case ESdpServerDbgFailNext:
		{
#ifdef _DEBUG
		LOG1(_L("\tfail next sdp server alloc (simulating failure after %d allocation(s))"), aMessage.Int0());
		if ( aMessage.Int0() == 0 )
			{
			__UHEAP_RESET;
			}
		else
			{
			__UHEAP_FAILNEXT(aMessage.Int0());
			}
#endif // _DEBUG
		aMessage.Complete(KErrNone);
		break;
		}
	default:
		//not handled here so must be for subsession
		PanicClient(aMessage, ESdpBadRequest);
		}
	}

void CSdpServSession::NewSubSessionL(TSubSessionType aType, const RMessage2& aMessage)
/**
   Increment our subsession count and send the SDP subsession handle
   back to the client.
   If we succeed, we complete the message.  If we fail, we leave
   and the server sends back the error for us.
**/
	{
	LOG_FUNC
	if (aType!=ESdpDatabase)
		{
		User::Leave(KErrNotSupported);
		}

	//Write handle to client
	TPckg<TInt> pckg(KSdpDatabaseSubSessionHandle);
	TRAPD(err, aMessage.WriteL(3, pckg)); //ss//
	if(err)
		{
		User::Leave(err);
		}
	iSubSessionCount++;
	aMessage.Complete(KErrNone);
	}

void CSdpServSession::DeleteSubsession(TUint aHandle, const RMessage2& aMessage)
	{
	LOG_FUNC
	//panic client if bad handle
	if ((aHandle != KSdpDatabaseSubSessionHandle) || (iSubSessionCount == 0))
		{
		PanicClient(aMessage, ESdpBadSubSessionRemove);
		return;
		}
	iSubSessionCount--;
	aMessage.Complete(KErrNone);
	}

void CSdpServSession::CloseSubSession(const RMessage2& aMessage)
	{
	LOG_FUNC

	DeleteSubsession((aMessage).Int3(), aMessage); //calls aMessage.Complete(KErrNone)	
	}

void CSdpServSession::NumResourcesL(const RMessage2& aMessage)
	{
	LOG_FUNC
	TPckgBuf<TInt> pckg(CountResources());
	aMessage.WriteL(0, pckg); //ss//
	aMessage.Complete(KErrNone);
	}

TInt CSdpServSession::CountResources()
	{
	LOG_FUNC
	return iSubSessionCount;
	}


//**********************************
// CEirPublisherSdpUuidBase
//**********************************
/**
CEirPublisherSdpUuidBase provides base class for Sdp Uuid Eir Publisher.
**/

CEirPublisherSdpUuidBase::~CEirPublisherSdpUuidBase()
	{
	delete iPublisher;
	}

CEirPublisherSdpUuidBase::CEirPublisherSdpUuidBase(CSdpUuidManager& aSdpUuidManager, TEirTag aTag)
: iParent(aSdpUuidManager), iTag(aTag)
	{
	}

void CEirPublisherSdpUuidBase::ConstructL()
	{
	iPublisher = CEirPublisher::NewL(iTag, *this);
	}

void CEirPublisherSdpUuidBase::UpdateUuids(TInt aLength)
	{
	iPublisher->PublishData(aLength);
	}

//**********************************
// CEirPublisherSdpUuid16
//**********************************
/**
Provides functionality to publish 16 bit UUIDs to EIR.
**/
CEirPublisherSdpUuid16* CEirPublisherSdpUuid16::NewL(CSdpUuidManager& aSdpUuidManager)
	{
	CEirPublisherSdpUuid16* self = new (ELeave) CEirPublisherSdpUuid16(aSdpUuidManager);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CEirPublisherSdpUuid16::CEirPublisherSdpUuid16(CSdpUuidManager& aSdpUuidManager)
:	CEirPublisherSdpUuidBase(aSdpUuidManager, EEirTagSdpUuid16)
	{
	}

CEirPublisherSdpUuid16::~CEirPublisherSdpUuid16()
	{
	delete iExtracted;
	}

// From MEirPublisherNotifier
void CEirPublisherSdpUuid16::MepnSpaceAvailable(TUint aBytesAvailable)
	{
	// Ensure previous memory is freed
	delete iExtracted, iExtracted = NULL;
	TBool partial = EFalse;
	
	if (aBytesAvailable > 0)
		{
		iExtracted = iParent.GetAll16BitUUIDs(aBytesAvailable, partial);
		if(!iExtracted)
			{
			// OOM probably
			iPublisher->SetData(KNullDesC8, EEirDataPartial);
			}
		else if(partial)
			{
			iPublisher->SetData(*iExtracted, EEirDataPartial);
			}
		else
			{
			iPublisher->SetData(*iExtracted, EEirDataComplete);
			}
		}
	}

void CEirPublisherSdpUuid16::MepnSetDataError(TInt /*aResult*/)
	{
	delete iExtracted;
	iExtracted = NULL;
	}


//**********************************
// CEirPublisherSdpUuid128
//**********************************
/**
Provides functionality to publish 128 bit UUIDs to EIR.
**/
CEirPublisherSdpUuid128* CEirPublisherSdpUuid128::NewL(CSdpUuidManager& aSdpUuidManager)
	{
	CEirPublisherSdpUuid128* self = new (ELeave) CEirPublisherSdpUuid128(aSdpUuidManager);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CEirPublisherSdpUuid128::CEirPublisherSdpUuid128(CSdpUuidManager& aSdpUuidManager)
:	CEirPublisherSdpUuidBase(aSdpUuidManager, EEirTagSdpUuid128)
	{
	}

CEirPublisherSdpUuid128::~CEirPublisherSdpUuid128()
	{
	delete iExtracted;
	}

// From MEirPublisherNotifier
void CEirPublisherSdpUuid128::MepnSpaceAvailable(TUint aBytesAvailable)
	{
	// Ensure previous memory is freed
	delete iExtracted, iExtracted = NULL;
	TBool partial = EFalse;
	
	if (aBytesAvailable > 0)
		{
		iExtracted = iParent.GetAll128BitUUIDs(aBytesAvailable, partial);
		if(!iExtracted)
			{
			// OOM probably
			iPublisher->SetData(KNullDesC8, EEirDataPartial);
			}
		else if(partial)
			{
			iPublisher->SetData(*iExtracted, EEirDataPartial);
			}
		else
			{
			iPublisher->SetData(*iExtracted, EEirDataComplete);
			}
		}
	}

void CEirPublisherSdpUuid128::MepnSetDataError(TInt /*aResult*/)
	{
	delete iExtracted;
	iExtracted = NULL;
	}


CSdpUuidManager* CSdpUuidManager::NewL(CSdpDatabase& aSdpDatabase)
	{
	CSdpUuidManager* self = new (ELeave) CSdpUuidManager(aSdpDatabase);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CSdpUuidManager::~CSdpUuidManager()
	{
	i16BitUUIDs.Close();
	i128BitUUIDs.Close();
	
	delete iEirPublisherSdpUuid16;
	delete iEirPublisherSdpUuid128;
	delete iUuidVisitor;
	}

void CSdpUuidManager::NotifySdpRecordChange()
	{
	TServRecordIter recIter(iSdpDatabase.RecordIter());
	CSdpServRecord* rec = NULL;
	CSdpAttr* attr = NULL;
	CSdpAttrValue* serviceClassValue = NULL;
	CSdpAttrValue* browseGroupValue = NULL;
	
	ResetUuids();
	while((rec = recIter++) != NULL)
		{// Iterate thru records in Db
		TServAttrIter attrIter(rec->AttributeIter());
		while((attr = attrIter++) != NULL)
			{
			if(attr->AttributeID()==KSdpAttrIdServiceClassIDList)
				{
				// Find the value of ServiceClassIDList attribute
				serviceClassValue = &(attr->Value());
				}
			if(attr->AttributeID()==KSdpAttrIdBrowseGroupList)
				{
				// Find the value of BrowseGroupList attribute
				browseGroupValue = &(attr->Value());
				}
			}
		if(serviceClassValue && browseGroupValue)
			{
			// Both ServiceClassIDList and BrowseGroupList have value
			iUuidVisitor->GetUuids().Reset();
			TRAPD(err, browseGroupValue->AcceptVisitorL(*iUuidVisitor));
			if(err == KErrNone)
				{
				TUUID publicBrowseGroupUUID = TUUID(static_cast<TUint32>(KPublicBrowseGroupUUID));
				if(iUuidVisitor->GetUuids().IsPresent(publicBrowseGroupUUID))
					{
					iUuidVisitor->GetUuids().Reset();
					TRAPD(err, serviceClassValue->AcceptVisitorL(*iUuidVisitor));
					if(err == KErrNone && iUuidVisitor->GetUuids().Count() > 0)
						{
						// In eir, we use the 1st uuid on the list, as it is the most specific one
						AddUuid(iUuidVisitor->GetUuids()[0]);
						}
					}
				}
			}
		// Reset the pointers for these two values
		serviceClassValue = NULL;
		browseGroupValue = NULL;
		}
	// Only publish UUIDs in public browse group
	// 16 bit UUIDs
	TUint8 count = i16BitUUIDs.Count();
	// Skip the 1st 16 bit uuid record, it's a SDP service UUID
	if(count > 0)
		{
		// UpdateUuids expects the length in bytes, so multiply number of uuids by 2
		iEirPublisherSdpUuid16->UpdateUuids(count<<1);
		}
		
	// 128 bit UUIDs
	count = i128BitUUIDs.Count();
	if(count > 0)
		{
		// UpdateUuids expects the length in bytes, so multiply number of uuids by 16
		iEirPublisherSdpUuid128->UpdateUuids(count<<4);
		}
	}

void CSdpUuidManager::AddUuid(TUUID aUuid)
	{
	TUint8 count = aUuid.MinimumSize();
	if(count == KSizeOf16BitUUID)
		{
		// 16 bit UUID
		i16BitUUIDs.Add(aUuid);
		}
	else if(count == KSizeOf128BitUUID)
		{
		// 128 bit UUID
		i128BitUUIDs.Add(aUuid);
		}
	else
		{
		// 32 bit UUIDs are not supported
		}
	}

void CSdpUuidManager::ResetUuids()
	{
	i16BitUUIDs.Reset();
	i128BitUUIDs.Reset();
	}

HBufC8* CSdpUuidManager::GetAll16BitUUIDs(TInt aBytesAvailable, TBool& aPartial)
	{
	// Flatten the UUIDs into a little-endian buffer
	HBufC8* extracted = HBufC8::New(aBytesAvailable);
	if(extracted)
		{
		TPtr8 ptr = extracted->Des();
		TInt i = 0;
		TInt count = i16BitUUIDs.Count();
		// Iterate whilst not exhausted and there's space for a UUID
		while (i < count && ptr.Length() + KSizeOf16BitUUID <= aBytesAvailable)
			{
			// convert to 16 bit Little Endian for EIR
			TPtrC8 p = i16BitUUIDs[i].ShortestForm();
			ptr.Append(p[1]);
			ptr.Append(p[0]);
			i++;
			}
		__ASSERT_DEBUG(extracted->Length() <= aBytesAvailable, Panic(ESdpServerUuidFlattenBroken));
		// Let the caller know if there wasn't enough space for all UUIDs
		aPartial = EFalse;
		if (i < count)
			{
			aPartial = ETrue;
			}
		}
	return extracted;
	}

HBufC8* CSdpUuidManager::GetAll128BitUUIDs(TInt aBytesAvailable, TBool& aPartial)
	{
	// Flatten the UUIDs into a little-endian buffer
	HBufC8* extracted = HBufC8::New(aBytesAvailable);
	if(extracted)
		{
		TPtr8 ptr = extracted->Des();
		TInt i = 0;
		TInt count = i128BitUUIDs.Count();
		
		// Iterate whilst not exhausted and there's space for a UUID
		while (i < count && ptr.Length() + KSizeOf128BitUUID <= aBytesAvailable)
			{
			// convert to 128 bits Little Endian for EIR
			for (TUint j = 1; j<= KSdpUUIDMaxLength ;j++)
				{
				ptr.Append(i128BitUUIDs[i][KSdpUUIDMaxLength - j]);
				}
			i++;
			}
		__ASSERT_DEBUG(extracted->Length() <= aBytesAvailable, Panic(ESdpServerUuidFlattenBroken));
		// Let the caller know if there wasn't enough space for all UUIDs
		aPartial = EFalse;
		if (i < count)
			{
			aPartial = ETrue;
			}
		}
	return extracted;
	}

CSdpUuidManager::CSdpUuidManager(CSdpDatabase& aSdpDatabase)
: iSdpDatabase(aSdpDatabase)
	{
	}
	
void CSdpUuidManager::ConstructL()
	{
	iUuidVisitor = CAttrUuidVisitor::NewL();
	iEirPublisherSdpUuid16 = CEirPublisherSdpUuid16::NewL(*this);
	iEirPublisherSdpUuid128 = CEirPublisherSdpUuid128::NewL(*this);
	}

