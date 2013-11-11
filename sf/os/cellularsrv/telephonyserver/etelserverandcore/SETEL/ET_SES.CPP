// Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ET_SESTraces.h"
#endif

#include "ET_SSTD.H"
#include "e32svr.h"
#include "et_record.h"

#define DOUBLE_COLON		_L("::")
#define ETEL_NAME			_L("Etel")
#define TSY_EXTENSION		_L(".TSY")
#define MBMS_CONTEXT_NAME	_L("MBMS_CONTEXT")


const TInt KLengthOfDoubleColon=2;

#ifdef __EABI__
// Patch data is used and KPriorityClientSid can be modified to a different value in ETel.iby file
extern const TInt32 KPriorityClientSid = 0x12345678 ;
// Patch data is used and KLocationServicesNetworkGatewaySid can be modified to a different value in ETel.iby file
extern const TInt32 KLocationServicesNetworkGatewaySid = 0x12345678;
// Patch data is used and KSatEngineSid can be modified to a different value in ETel.iby file
extern const TInt32 KSatEngineSid = 0x12345678;
#endif

// KEmergencyClientHeapMinSizeArray containing the bit- mask combination emergency client heap min size
// Array index 0 value refers to the decimal value 1 set from the RPhone::TEmergencyRequest bit-mask enum
// For example array index 0 value which is KEmergencyLCSClientHeapMinSize refers to the decimal value 1 set by
// passing EEmergencyLCSRequest from RPhone::TEmergencyRequest bit-mask enum
// Thus n-1 array index value refers to the decimal value n set from the RPhone::TEmergencyRequest bit-mask enum
const TInt KEmergencyClientHeapMinSizeArray[] = 
	{
	KEmergencyLCSClientHeapMinSize,
	KEmergencyCSVoiceCallClientHeapMinSize,
	KEmergencyLCSClientHeapMinSize + KEmergencyCSVoiceCallClientHeapMinSize
	//Add future bit- mask combination emergency client's heap min size here
	};

// KEmergencyClientHeapMaxSizeArray containing the bit- mask combination emergency client heap max size
// Array index 0 value refers to the decimal value 1 set from the RPhone::TEmergencyRequest bit-mask enum
// For example array index 0 value which is KEmergencyLCSClientHeapMinSize refers to the decimal value 1 set by
// passing EEmergencyLCSRequest from RPhone::TEmergencyRequest bit-mask enum
// Thus n-1 array index value refers to the decimal value n set from the RPhone::TEmergencyRequest bit-mask enum
const TInt KEmergencyClientHeapMaxSizeArray[] = 
	{
	KEmergencyLCSClientHeapMaxSize,
	KEmergencyCSVoiceCallClientHeapMaxSize,
	KEmergencyLCSClientHeapMaxSize + KEmergencyCSVoiceCallClientHeapMaxSize
	//Add future bit-mask combination emergency client's heap max size here
	};


//
// CTelSession class definitions
//

inline CTelServer* CTelSession::Server() const
	{
	return static_cast<CTelServer*>(const_cast<CServer2*>(CSession2::Server()));
	}

CTelSession::CTelSession(CPhoneManager* aPhoneManager)
	: iPhoneManager(aPhoneManager),iEmergencyHeap(NULL)
//
// C'Tor - must pass client to CSession
//
	{
	__DECLARE_NAME(_S("CTelSession"));
	}

CTelSession::~CTelSession()
//
//	clean up and go home.
//
	{
	// Just ensure that the server doing the closure all subsessions
	// if the client do not !
	if(iObjectIx)
		{
		TInt handle;
		CObject* theObj=NULL;
		TInt count=iObjectIx->Count();

		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CTELSESSION_DTOR_1, "CTelSession::~CTelSession iObjectIx->Count()=%d", count);
		
		for(TInt i=0;i<count;i++)
			{
			theObj=iObjectIx->operator[](i);
			if(theObj)
				{
				handle=iObjectIx->At(theObj);
				CTelObject* theTelObj=REINTERPRET_CAST(CTelObject*,theObj); //
				theTelObj->CloseSubSessionPreProcessing(this,handle);
				theTelObj=REINTERPRET_CAST(CTelObject*,theObj->Owner());
				iObjectIx->Remove(handle);

				if(theTelObj)
					theTelObj->TelObjectClose();

				//
				// If the array length has changed, then update count and move
				// back one space, so that the same cell will be re-inspected
				// during the next iteration of the loop.  This is because we
				// have removed the current cell and therefore the next cell
				// is actually in iObjectIx[i] (e.g. the current one).
				//
				TInt  newCount = iObjectIx->Count();

				if (newCount != count)
					{
					count = newCount;
					i--;
					}
				}
			}
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CTELSESSION_DTOR_2, "Delete iObjectIx");
		delete iObjectIx;
		}


	if (iTsyModulesIx)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CTELSESSION_DTOR_3, "Delete TsyModulesIx");
		delete iTsyModulesIx;
		}
	UnsetPriorityClient();
	UnsetEmergencyClient();
    RECORD_SESSION_CLOSE(this);
	Server()->Dec();
	}

void CTelSession::CreateL()
//
// Allocate any extra knobbly bits
//
	{	
	Server()->Inc(); // increment session count by one
	iTsyModulesIx=CObjectIx::NewL();
	iObjectIx=CObjectIx::NewL();
	iNameIndex=0;
	}

void CTelSession::ServiceL(const RMessage2 &aMessage)
//
// Handle messages for this session
//
	{
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CTELSESSION_SERVICEL_1, "CTelSession::Service Called, CTelSession: 0x%08X, IPC: %{TIPCNamesList}", (TUint)this, aMessage.Function());

	iMessage=aMessage;

	switch (aMessage.Function())
		{
	case EEtelOpenFromSession:
		NewTelObject(ETelObjectOpenSourceSession);
		return;
//RMobileLocationServices::Open() behaves like any other Open()
//method except it needs to be policed with an SID check and to facilitate
//this it has a separate IPC (EEtelOpenFromSubSessionLcs)
	case EEtelOpenFromSubSessionLcs:
	case EEtelOpenFromSubSession:
		NewTelObject(ETelObjectOpenSourceSubSession);
		return;
	case EEtelOpenByNameFromSession:
		NewTelObject(ETelObjectOpenByNameSourceSession);
		return;
	case EEtelOpenByNameFromSubSession:
		NewTelObject(ETelObjectOpenByNameSourceSubSession);
		return;
#if defined (_DEBUG)
	case EEtelDbgMarkHeap:
		{
		RECORD_IPC_LOG2(this, EEtelDbgMarkHeap);
		RECORD_COMPLETE(aMessage.Session(), aMessage.Function(), KErrNone);		
		__UHEAP_MARK;
		if(!aMessage.IsNull())
			{
			aMessage.Complete(KErrNone);
			}
		return;
		}
	case EEtelDbgCheckHeap:
		{
		__UHEAP_CHECK(aMessage.Int0());
		RECORD_IPC_LOG3(this, EEtelDbgCheckHeap, aMessage.Int0());
		RECORD_COMPLETE(aMessage.Session(), aMessage.Function(), KErrNone);		
		if(!aMessage.IsNull())
			{
			aMessage.Complete(KErrNone);
			}
		return;
		}
	case EEtelDbgMarkEnd:
		{
		__UHEAP_MARKENDC(aMessage.Int0());
		RECORD_IPC_LOG3(this, EEtelDbgMarkEnd, aMessage.Int0());
		RECORD_COMPLETE(aMessage.Session(), aMessage.Function(), KErrNone);
		if(!aMessage.IsNull())
			{
			aMessage.Complete(KErrNone);
			}
		return;
		}
	case EEtelDbgFailNext:
		{
		RECORD_IPC_LOG3(this, EEtelDbgFailNext, aMessage.Int0());
		RECORD_COMPLETE(aMessage.Session(), aMessage.Function(), KErrNone);
		if (aMessage.Int0() == 0)
			{
			__UHEAP_RESET;
			}
		else
			{
			__UHEAP_FAILNEXT(aMessage.Int0());
			}
		if(!aMessage.IsNull())
			{
			aMessage.Complete(KErrNone);
			}
		return;
		}
	case EEtelDbgFailNextAvail:
		{
		TBool result = ETrue;
		
		//Create a derived class from RHeap which provides exposure to
		//the Heap's iFailType member
		class RPeekHeap: public RHeap
			{
		public:
			RAllocator::TAllocFail FailNextMode() const
				{
				return iFailType;
				}
			};

		RPeekHeap* thisHeap = static_cast<RPeekHeap*>(&User::Heap());
		if(thisHeap->FailNextMode() != RAllocator::EFailNext)
 			{
 			result = EFalse;	//Simulated OOM is not available
 			}
 			
		TPtr8 ptr(REINTERPRET_CAST(TText8*,&result),sizeof(TBool),sizeof(TBool));
		
		RECORD_IPC_LOG3(this, EEtelDbgFailNextAvail, ptr);

		TInt ret = aMessage.Write(0,ptr);
		RECORD_COMPLETE(aMessage.Session(), aMessage.Function(), ret);
		
		if(!aMessage.IsNull())
			{
			aMessage.Complete(ret);
			}
		return;
		}		
#endif

	case EEtelServerLoadPhoneModule:
 		LoadPhoneModule();
		return;

	case EEtelServerClosePhoneModule:
		ClosePhoneModule();
		return;

	case EEtelServerPhoneInfoByIndex:
			{
			PhoneInfo(aMessage.Int1()); // use Int1() same as client side
			return;
			}

	case EEtelServerEnumeratePhones:
		EnumeratePhones();
		return;

	case EEtelServerSetPriorityClient:
		SetPriorityClient();
		return;

	case EEtelServerQueryTsyFunctionality:
		IsSupportedByTsy();
		return;

	case EEtelServerGetTsyName:
		GetTsyNameByPhone(aMessage.Int0());
		return;

	case EEtelServerSetExtendedErrorGranularity:
		SetExtendedErrorGranularity();
		return;

	case EEtelServerGetTsyVersionNo:
		GetTsyVersionNumber();
		return;
	
	// ETelServer will first pre-allocate heap memory for the type of request received.
	// Once ETelServer successfully pre-allocates it will pass on EETelPhoneSetEmergencyClient IPC opcode
	// to TSY via ExtFunc for the TSY to pre-allocate. 
	case EETelPhoneSetEmergencyClient:
		{
		// ETel server pre-allocates heap memory for the type of request received
		TInt ret = SetEmergencyClient(aMessage);
		if(ret!=KErrNone)
			{
			if(!aMessage.IsNull())
				{
				RECORD_COMPLETE(aMessage.Session(), EETelPhoneSetEmergencyClient, ret);
				aMessage.Complete(ret);
				}

			return;
			}
		
		// EETelPhoneSetEmergencyClient IPC opcode is passed to TSY via ExtFunc for the TSY to pre-allocate.
		CTelObject* theObj=CObjectFromHandle(aMessage.Int3());
		if (theObj==NULL)
			{
			PanicClient(EEtelPanicBadPhoneHandle,aMessage);		
			return;
			}
		theObj->GeneralReq(aMessage,this,NULL);
		return;
		}
		
	case EEtelServerSetPriorityClientV2:
		SetPriorityClientV2();
		return;
		
	case EEtelFlushInterfaceQueue:
		{
		RECORD_COMPLETE(aMessage.Session(), aMessage.Function(), KErrNone);
		if (aMessage.IsNull() == EFalse)
			{
		    aMessage.Complete(KErrNone);
			}
		return;
		}
		
	default:
		;
		}

	// Closing
	CTelObject* theObj=CObjectFromHandle(aMessage.Int3());

 	switch (aMessage.Function())
		{
	case EEtelClose:
		{
		if (theObj==NULL && !aMessage.IsNull())
			{
			RECORD_COMPLETE_SUB(aMessage.Session(), theObj, aMessage.Int3(), aMessage.Function(), KErrBadHandle);
			aMessage.Complete(KErrBadHandle);
			}
		else
			{
			RECORD_SUBSESSION_CLOSE(this, EEtelClose, theObj, aMessage.Int3());
			
			if (theObj != NULL)
				{
				theObj->CloseSubSessionPreProcessing(this,aMessage.Int3());
				CTelObject* theObject=REINTERPRET_CAST(CTelObject*,theObj->Owner());

				iObjectIx->Remove(aMessage.Int3());

				if (theObject)
					{
					theObject->TelObjectClose();
					}
				}
			else
				{
				iObjectIx->Remove(aMessage.Int3());
				}
			
			if(!aMessage.IsNull())
				{
				RECORD_COMPLETE_SUB(aMessage.Session(), theObj, aMessage.Int3(), aMessage.Function(), KErrNone);
				aMessage.Complete(KErrNone);
				}
			}
		return;
		}
	case EEtelGlobalKernelObjectHandle:
		{
		// We log the handle received in the message and return the syncHandle in the "message.complete"
		RECORD_IPC_LOG3(this, EEtelGlobalKernelObjectHandle,aMessage.Int3());
		if (theObj==NULL)
		   {
		   RECORD_IPC_LOG3(this, EEtelGlobalKernelObjectHandle,NULL);
		   if(!aMessage.IsNull())
		   		{
		   		RECORD_COMPLETE_SUB(aMessage.Session(), theObj, aMessage.Int3(), aMessage.Function(), KErrBadHandle);
		   		aMessage.Complete(KErrBadHandle);
		   		}
		   }
		else
		   {   
			RHandleBase* syncHandle=theObj->GlobalKernelObjectHandle();
			if(syncHandle)
				{
				if(!aMessage.IsNull())	
					{
					//just record the message completion as KErrNone
					RECORD_COMPLETE_SUB(aMessage.Session(), theObj, aMessage.Int3(), aMessage.Function(), KErrNone);
					aMessage.Complete(*syncHandle);
					}
					
				}
			else
				{
				if(!aMessage.IsNull())
					{
					RECORD_COMPLETE_SUB(aMessage.Session(), theObj, aMessage.Int3(), aMessage.Function(), KErrNotSupported);
					aMessage.Complete(KErrNotSupported);
					}
				}
		    }
		return;
		}
	default:
		{
			if (theObj==NULL)
				{
				PanicClient(EEtelPanicBadPhoneHandle,aMessage);		
				return;
				}
			theObj->GeneralReq(aMessage,this,NULL);
			return;
		}
		}
	}

CTelObject* CTelSession::CObjectFromHandle(TUint aHandle) const
//
// Return an Object Pointer for Given Handle
//
	{
	return REINTERPRET_CAST(CTelObject*,iObjectIx->At(aHandle));
	}

CTelServer* CTelSession::TelServer() const
	{
	return Server();
	}

void CTelSession::EnumeratePhones()
//
// get number of phone currently loaded
//
	{
	TPckgBuf<TInt> num;
	num()=iPhoneManager->EnumeratePhones();
	RECORD_IPC_LOG3(this, EEtelServerEnumeratePhones, num());
	const RMessage2& m=iMessage;

	TRAPD(ret, m.WriteL(0,num));
	RECORD_COMPLETE(m.Session(), m.Function(), ret);
	m.Complete(ret);
	}

void CTelSession::GetTsyNameByPhone(const TInt aIndexOfPhone) 
//
// get the TSY name a particular phone belongs to
//
	{
	RECORD_IPC_LOG4(this, EEtelServerGetTsyName, aIndexOfPhone, CRecordMessage::TArgAllocBufFromLen(50, CRecordMessage::KArgTypeIsDes16));
	TName tsyName;
	TInt ret;
	if ((ret=iPhoneManager->GetTsyName(aIndexOfPhone,tsyName))==KErrNone)
		{

		ret = iMessage.Write(1,tsyName);
		}
	if(!iMessage.IsNull())
		{
		RECORD_COMPLETE(iMessage.Session(), iMessage.Function(), ret);
		iMessage.Complete(ret);
		}
	}

void CTelSession::SetExtendedErrorGranularity()
	{	
	TPtr8 ptr(REINTERPRET_CAST(TText8*,&iErrorGranularity),
		sizeof(RTelServer::TErrorGranularity),sizeof(RTelServer::TErrorGranularity));
	
	TInt res = Read(0,ptr);

	if (res == KErrNone)
		{		
		TInt descLength = iMessage.GetDesLength(0);
		if ( descLength<sizeof(iErrorGranularity) )
			{
			PanicClient(EEtelPanicBadDescriptor,iMessage);
			}	
		}		

	RECORD_IPC_LOG3(this, EEtelServerSetExtendedErrorGranularity, ptr);
	if(!iMessage.IsNull())
		{
		RECORD_COMPLETE(iMessage.Session(), iMessage.Function(), KErrNone);
		iMessage.Complete(KErrNone);
		}
	}

TBool CTelSession::IsExpectingExtendedError() const
	{
	if (iErrorGranularity==RTelServer::EErrorExtended)
		return ETrue;
	return EFalse;
	}

void CTelSession::GetTsyVersionNumber() const
	{
	TFileName name;
	if (GetModuleName(name)!=KErrNone)
		return;
	TInt len=name.Locate('.');
	name.SetLength(len);	// strip off .TSY extension
	CPhoneFactoryBase* p = NULL;
	TRAPD(ret,p = iPhoneManager->OpenPhoneFactoryFromTsyL(name));
	if (ret==KErrNone)
		{
		TVersion v = p->TsyVersionNumber();
		TPtr8 ptr(REINTERPRET_CAST(TText8*,&v),sizeof(TVersion),sizeof(TVersion));
		RECORD_IPC_LOG4(this, EEtelServerGetTsyVersionNo, name, ptr);
		
		ret=iMessage.Write(1,ptr);
		}
	if(!iMessage.IsNull())
		{
		RECORD_COMPLETE(iMessage.Session(), iMessage.Function(), ret);
		iMessage.Complete(ret);
		}
	}

TInt CTelSession::GetModuleName(TDes& aName) const
	{
	Read(0,aName);
	TInt r=aName.Locate('.');
	if (r==KErrNotFound)
		{
		if (aName.Length()>KMaxFileName-4)
			{
			PanicClient(EEtelPanicBadDescriptor,iMessage);
			return KErrGeneral;
			}
		aName.Append(TSY_EXTENSION);
		}
	aName.UpperCase();
	return KErrNone;
	}

void CTelSession::LoadPhoneModule()
//
//	Load a phone module by name specified in p[0]
//
	{
	TFileName name;
	if (GetModuleName(name)!=KErrNone)
		return;
	
	RECORD_IPC_LOG3(this, EEtelServerLoadPhoneModule, name);
		
	CPhoneFactoryBase* s=NULL;
	TRAPD(res,(s=iPhoneManager->LoadPhoneModuleL(name)));
	if (res==KErrNone)
		{
		TRAP(res,iTsyModulesIx->AddL(s));
		if(res!=KErrNone && res!=KErrAlreadyExists)
			s->Close();	
		}
	if(!iMessage.IsNull())
		{
		RECORD_COMPLETE(iMessage.Session(), iMessage.Function(), res);
		iMessage.Complete(res);
		}
	}

void CTelSession::ClosePhoneModule()
//
// Close a ETEL TSY module
//
	{
	TName name;
	Read(0,name);
	CPhoneFactoryBase* s=NULL;
	
	RECORD_IPC_LOG3(this, EEtelServerClosePhoneModule, name);
	
	// strip ".tsy" from name if it is there
	TInt len=name.Locate('.');
	if (len!=KErrNotFound)
		name.SetLength(len);

	TRAPD(res,(s=iPhoneManager->OpenPhoneFactoryFromTsyL(name)));
	if (res==KErrNone)
		{
		TInt handle=iTsyModulesIx->At(s);
		if (handle==KErrNotFound)
			res=KErrNotFound;
		else
			{
			iTsyModulesIx->Remove(handle);
			}
		}
	iPhoneManager->RemoveDuplicatePhoneInfo(name);
	if(!iMessage.IsNull())
		{
		RECORD_COMPLETE(iMessage.Session(), iMessage.Function(), res);
		iMessage.Complete(res);
		}
	}
#pragma warning(disable:4702) // Disable "C4702: unreachable code". 
							// Reason is KPriorityClientHeapMinSize is a const TInt but depends
							// on the sizes of CReqEntry and CBuffer. Currently, therefore, 
							// min is less than max. If code change causes sizes of CReqEntry or
							// CBuffer then min may become greater than max.

void CTelSession::SetPriorityClient()
//
// Check to see if there is already a priority client if there is complete
// Else set as priority client and allocate memory
//
	{
	RECORD_IPC_LOG2(this, EEtelServerSetPriorityClient);
	TInt ret=Server()->SetPriorityClient(this);
	if (ret==KErrNone)
		{
		if(iEmergencyHeap)
			{
			if(!iMessage.IsNull())
				{
				RECORD_COMPLETE(iMessage.Session(), iMessage.Function(), KErrAlreadyExists);
				iMessage.Complete(KErrAlreadyExists);
				return;
				}
			}
		ret = PreallocateEmergencyHeap(RPhone::EEmergencyCSVoiceCallRequest);
		}
	
	if(!iMessage.IsNull())
		{
		RECORD_COMPLETE(iMessage.Session(), iMessage.Function(), ret);
		iMessage.Complete(ret);
		}
	}
#pragma warning(default:4702) // Turn it back on.

void CTelSession::UnsetPriorityClient()
//
// If this is the priority client then remove it and deallocate memory
//
	{
	TInt ret;
	ret = Server()->RemovePriorityClient(this);
	if (ret==KErrNone && iEmergencyHeap!=NULL) //iEmergencyHeap will be NULL when SetPriorityClientV2 is called
											   //iEmergencyHeap will be set when SetPriorityClient is called
		{
		iEmergencyHeap->Close();
		iEmergencyHeap=NULL;
		}

	}

TBool CTelSession::IsUnicodeReq(TInt aReq) const
//
// Check whether aReq is involves passing unicode data
//
	{
	if (aReq&KUnicodeReq)
		return ETrue;
	return EFalse;
	}

void CTelSession::GenerateName(TDes& aName)
//
// Append a Default Name to 'aName'
//
	{
	aName.Append(ETEL_NAME);
	TUint32 num=Server()->Count()<<16;
	num|=iNameIndex++;
	aName.AppendNum((TUint)num,EHex);
	}

void CTelSession::CheckAndAppendNewName(TDes& aName)
//
// Check if name is empty then fill with Etel default name
//
	{
	if (aName.Length()==0) // client does not give a name - TSY will generate one
		return;
	if (aName.Find(DOUBLE_COLON)== KErrNotFound) // it's a genuine name
		return;
	// ready to parse the name ...
	TFullName fullName=aName;
	aName.SetLength(0);
	TPtrC remaining(fullName);
	TPtrC name(NULL,0); // store individual name NULL if no name
	TInt len=0;
	while( (len=remaining.Find(DOUBLE_COLON))!=KErrNotFound)
		{
		name.Set(remaining.Left(len));
		remaining.Set(remaining.Right(remaining.Length()-len-KLengthOfDoubleColon));
		if (name.Length()!=0) 
			{
			aName.Append(name);
			aName.Append(DOUBLE_COLON);
			}
		
		if (remaining.Find(DOUBLE_COLON)==KErrNotFound)
			{
			if (remaining.Length())
				aName.Append(remaining);
			}
		}
	}

void CTelSession::NewTelObject(TTelObjectOpenSource aSource)
//
// Handle New Line Request
//
	{
	CPhoneFactoryBase* phoneFactory=NULL ;
	CTelObject* newObject=NULL;
	TFullName fullName;
	Read(0,fullName);
	
	if(iMessage.IsNull()) // Read panicked client
	 return;
	
#ifdef ETEL_RECORDER	
	TSessionOpenArgs openArgs;
	CaptureArgs(iMessage, openArgs, fullName);
#endif
	
	CheckAndAppendNewName(fullName);
	TFullName formName;
	TInt res(KErrNone);
	
	if(aSource==ETelObjectOpenSourceSubSession		   ||	 
	   aSource==ETelObjectOpenByNameSourceSubSession)
		{
		newObject=CObjectFromHandle(iMessage.Int2());
		if (newObject)
			{
			formName=newObject->FullName();
			}
		else
			{
			PanicClient(EEtelPanicBadSubSessionHandle,iMessage);
			return;
			}
		}
	else
		{
		TPtrC remain(fullName);
		TPtrC phoneName(StripOutNextName(remain,fullName)); // get the phone name
		phoneFactory=iPhoneManager->PhoneFactoryObjectFromPhoneName(phoneName);
		if (phoneFactory==NULL)
			{
			if(!iMessage.IsNull())
				{
				RECORD_COMPLETE(iMessage.Session(), iMessage.Function(), KErrNotFound);
				iMessage.Complete(KErrNotFound);
				}
			return;
			}
		formName=phoneFactory->FullName();
		}
	formName.Append(DOUBLE_COLON);
	formName.Append(fullName);
	fullName.Copy(formName);

	TPtrC remaining(fullName);
	TPtrC name(StripOutNextName(remaining,fullName));
	TRAP(res,(phoneFactory=iPhoneManager->OpenPhoneFactoryFromTsyL(name)));
	if (res!=KErrNone)
		{
		if(phoneFactory)
			phoneFactory->Close();
		if(!iMessage.IsNull())
			{
			RECORD_COMPLETE(iMessage.Session(), iMessage.Function(), res);
			iMessage.Complete(res);
			}
		return;
		}

	__ASSERT_ALWAYS(phoneFactory!=NULL,Fault(EEtelFaultBadPhoneFactoryPointer));
	phoneFactory->Open();
	// Open the Phone from the Phone Factory
	name.Set(StripOutNextName(remaining,fullName));

	TRAP(res,(newObject=iPhoneManager->OpenPhoneFromFactoryL(phoneFactory,name)));
	if(res!=KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CTELSESSION_NEWTELOBJECT_1, "Open Phone from Factory returned %d", res);
		if(newObject)
			newObject->TelObjectClose();
		else
			phoneFactory->Close();
		if(!iMessage.IsNull())
			{
			RECORD_COMPLETE(iMessage.Session(), iMessage.Function(), res);
			iMessage.Complete(res);
			}
		return;
		}
				
// Open Sub-session below the phone
	CTelObject* oldObject=newObject;
	__ASSERT_ALWAYS(oldObject!=NULL,Fault(EEtelFaultBadPhonePointer));

	while(remaining.Length()!=0)
		{
		name.Set(StripOutNextName(remaining,fullName));
		TRAP(res,(newObject=iPhoneManager->OpenSubSessionObjectByNameL(newObject,name/*,aSource*/)));
		if(res!=KErrNone)
			{
			if(newObject)
				newObject->TelObjectClose();
			else
				oldObject->TelObjectClose();
			if(!iMessage.IsNull())
				{
				RECORD_COMPLETE(iMessage.Session(), iMessage.Function(), res);
				iMessage.Complete(res);
				}
			return;
			}
		oldObject=newObject;
		}
	if (fullName.Right(2)==DOUBLE_COLON)	
		{
		__ASSERT_ALWAYS((aSource==ETelObjectOpenSourceSession ||  
						 aSource==ETelObjectOpenSourceSubSession),PanicClient(EEtelPanicBadName,iMessage));

		if (fullName.Mid(fullName.Length()-3,2)!=DOUBLE_COLON)
		// if there are three colons at the end, server will have already asked TSY to open
		// create the object and generate a new name for it.
			{
			TName aNewName;
			TInt ret = iMessage.Read(1,aNewName);
  			if (ret != KErrNone)
  				{
  				RECORD_COMPLETE(iMessage.Session(), iMessage.Function(), ret);
  				iMessage.Complete(ret);
  				return;
  				}
  			if(aNewName.Compare(MBMS_CONTEXT_NAME))
  				{
  				aNewName = _L("");
  				}
			TRAP(res,(newObject=iPhoneManager->OpenSubSessionObjectL(newObject,aNewName)));
			if (res)
				{	
				if(newObject)
					newObject->TelObjectClose();
				else
					oldObject->TelObjectClose();
				if(!iMessage.IsNull())
					{
					RECORD_COMPLETE(iMessage.Session(), iMessage.Function(), res);
					iMessage.Complete(res);
					}
				return;
				}
			res = iMessage.Write(1,aNewName);
			}
		else
			{				
			res = iMessage.Write(1,newObject->Name());
			}

		__ASSERT_ALWAYS(newObject!=NULL,Fault(EEtelFaultBadTelObjectPointer));
		if(res!=KErrNone)
			{
			newObject->TelObjectClose(); 
			if(!iMessage.IsNull())
				{
				RECORD_COMPLETE(iMessage.Session(), iMessage.Function(), res);
				iMessage.Complete(res);
				}
			return;
			}		
		}


	__ASSERT_ALWAYS(newObject!=NULL,Fault(EEtelFaultBadTelObjectPointer));
	TInt handle=0;
	TRAP(res,handle=iObjectIx->AddL(newObject));
	if(res!=KErrNone)
		{
		newObject->TelObjectClose(); // the assertion above ensures that newObject is not NULL
		if(!iMessage.IsNull())
			{
			RECORD_COMPLETE(iMessage.Session(), iMessage.Function(), res);
			iMessage.Complete(res);
			}
		return;
		}
	
	// 
	// Creating the dummy session object, which will
	// be used only when the client closes the handle
	// to this object.
	//	
	TRAP(res,newObject->CreateDummySessionObjectL(this));
	if (res != KErrNone)
		{
		newObject->TelObjectClose();
		phoneFactory->Close();
		if(!iMessage.IsNull())
			{
			RECORD_COMPLETE(iMessage.Session(), iMessage.Function(), res);
			iMessage.Complete(res);
			}
		return;
		}
	newObject->OpenPostProcessing(this,handle);
	TPtrC8 pH(REINTERPRET_CAST(TUint8*,&handle),sizeof(TInt));
	
	
	RECORD_SUBSESSION_NEW(this, iMessage.Function(), newObject, handle, openArgs.iArg0, CRecordMessage::TArgAllocBufFromLen(openArgs.iArg1Size, CRecordMessage::KArgTypeIsDes16), openArgs.iArg2);
	res = iMessage.Write(3,pH);
	if(!iMessage.IsNull())
		{
		RECORD_COMPLETE_SUB(this, newObject, handle, iMessage.Function(), KErrNone);
		iMessage.Complete(res);
		}
		
	}


TPtrC CTelSession::StripOutNextName(TPtrC& aRemainingName,const TFullName& aFullName)
//
// Return Descriptor of the the Next Name
//
	{
	TInt flen=aRemainingName.Find(DOUBLE_COLON);
	if (flen==KErrNotFound)	 // it is the last name in string
		{
		aRemainingName.Set(NULL,0);
		return aFullName;
		}
	else
		{
		aRemainingName.Set(aRemainingName.Right(aRemainingName.Length()-flen-KLengthOfDoubleColon));
		return aFullName.Left(aFullName.Length()-aRemainingName.Length()-KLengthOfDoubleColon);
		}
	}

void CTelSession::PhoneInfo(TInt aIndex)
//
//	Get phone info by index
//
	{
	RTelServer::TPhoneInfo phone;
	TInt ret=0;
	if((ret=iPhoneManager->GetPhoneInfo(aIndex,phone))==KErrNone)
		{
		CPhoneFactoryBase* fact = iPhoneManager->PhoneFactoryObjectFromPhoneIndex(aIndex);
		
		iPhoneManager->ConvertPhoneNameFromOriginal(fact->Name(),phone.iName);	
			// replace phone name with munged name if it has been munged

		TPckgC<RTelServer::TPhoneInfo> p(phone);
		RECORD_IPC_LOG4(this, EEtelServerPhoneInfoByIndex, p, aIndex);

		ret = iMessage.Write(0,p);
		}
	if(!iMessage.IsNull())
		{
		RECORD_COMPLETE(iMessage.Session(), iMessage.Function(), ret);
		iMessage.Complete(ret);
		}
	}

void CTelSession::IsSupportedByTsy()
	{
	TFileName name;
	if (GetModuleName(name)!=KErrNone)
		return;
	TInt len=name.Locate('.');
	name.SetLength(len);	// strip off .TSY extension
	CPhoneFactoryBase* phoneFactory=NULL;
	TRAPD(res,(phoneFactory=iPhoneManager->OpenPhoneFactoryFromTsyL(name)));
	if (res==KErrNone)
		{
		__ASSERT_ALWAYS(phoneFactory,Fault(EEtelFaultBadPhoneFactoryPointer));
		TInt mixin;
		TPtr8 ptr1(REINTERPRET_CAST(TText8*,CONST_CAST(TInt*,&mixin)),sizeof(TInt),sizeof(TInt));
		res = iMessage.Read(1,ptr1);
		if ( res == KErrNone )
			{ 
			TInt descLength = iMessage.GetDesLength(1);
			if ( descLength < sizeof(mixin) )
				{	// Wrong sized data in a buffer, since it is binary data all sorts of alignment problems could occur.
				res = KErrOverflow;
				}
			else
				{
				TBool result = phoneFactory->IsSupported(mixin);
				TPtr8 ptr2(REINTERPRET_CAST(TText8*,&result),sizeof(TBool),sizeof(TBool));
				RECORD_IPC_LOG5(this, EEtelServerQueryTsyFunctionality, name, ptr1, ptr2);
	
				res = iMessage.Write(2,ptr2);				
				}
			}
		}
	if(!iMessage.IsNull())
		{
		RECORD_COMPLETE(iMessage.Session(), iMessage.Function(), res);
		iMessage.Complete(res);
		}
	}

TInt CTelSession::Write(TUint aIndexOfMsgParam,const TDesC8& aDes,TInt aOffset) const
//
// Write and kill the client if it leaves.
//
	{
	TRAPD(ret,iMessage.WriteL(aIndexOfMsgParam,aDes,aOffset);)
	if (ret!=KErrNone)
		PanicClient(EEtelPanicBadDescriptor,iMessage);
	return ret;
	}

TInt CTelSession::Read(TUint aIndexOfMsgParam,TDes8 &aDes,TInt aOffset) const
//
// Write and kill the client if it leaves.
//
	{
	TRAPD(ret,iMessage.ReadL(aIndexOfMsgParam,aDes,aOffset);)
	if (ret!=KErrNone)
		PanicClient(EEtelPanicBadDescriptor,iMessage);
	return ret;
	}

TInt CTelSession::Write(TUint aIndexOfMsgParam,const TDesC16& aDes,TInt aOffset) const
//
// Write and kill the client if it leaves.
//
	{
	TRAPD(ret,iMessage.WriteL(aIndexOfMsgParam,aDes,aOffset);)
	if (ret!=KErrNone)
		PanicClient(EEtelPanicBadDescriptor,iMessage);
	return ret;
	}

TInt CTelSession::Read(TUint aIndexOfMsgParam,TDes16 &aDes,TInt aOffset) const
//
// Write and kill the client if it leaves.
//
	{
	TRAPD(ret,iMessage.ReadL(aIndexOfMsgParam,aDes,aOffset);)
	if (ret!=KErrNone)
		PanicClient(EEtelPanicBadDescriptor,iMessage);
	return ret;
	}

TInt CTelSession::SetEmergencyClient(const RMessage2& aMessage)
	{
	TInt ret = KErrNone;
	TUint32 emergencyRequest;
	TPckg<TUint32> emergencyRequest_asDescriptor(emergencyRequest);
	
	__ASSERT_ALWAYS(Server()->EmergencyClientSessionsCount()<=KNumberofEmergencyClients,PanicClient(EEtelPanicExceededAllowedNoOfEmergencyClientSessions));
	
	if(iEmergencyHeap)
		{
		return KErrAlreadyExists;
		}
	
	TRAP(ret,aMessage.ReadL(0,emergencyRequest_asDescriptor);)
	if (ret!=KErrNone)
		{
		PanicClient(EEtelPanicBadDescriptor,aMessage);
		return KErrGeneral;
		}
	RECORD_IPC_LOG3(this, EETelPhoneSetEmergencyClient, emergencyRequest_asDescriptor);
	return ret = PreallocateEmergencyHeap(emergencyRequest);
	}

TInt CTelSession::PreallocateEmergencyHeap(TUint32 aEmergencyRequest)
	{
	TInt ret = KErrNone;
	TInt min = 0;
	TInt max = 0;
	
	switch(aEmergencyRequest)
		{
	case RPhone::EEmergencyLCSRequest: // Indicates bit 1 is set and the decimal value is 1
	case RPhone::EEmergencyCSVoiceCallRequest: // Indicates bit 2 is set and the decimal value is 2
	case RPhone::EEmergencyLCSRequest | RPhone::EEmergencyCSVoiceCallRequest: // Indicates bit 1 and 2 are set and the decimal value is 3
	// Add future bit- mask combination here starting from decimal value 4 onwards 
	// and also append the KEmergencyClientHeapMinSizeArray and KEmergencyClientHeapMaxSizeArray arrays appropriately.
		min = Max(KMinHeapSize,KEmergencyClientHeapMinSizeArray[aEmergencyRequest-1]);
		max = KEmergencyClientHeapMaxSizeArray[aEmergencyRequest-1];
		break;	
	default:
		PanicClient(EEtelPanicBadDescriptor,iMessage);
		return KErrGeneral;
		}	
	
	if (min>max)			
		{
		max=2*min;
		}
	iEmergencyHeap = UserHeap::ChunkHeap(NULL,min,max);
	if (iEmergencyHeap == NULL)
		{
		ret = KErrNoMemory;
		}
	else
		{
		Server()->SetEmergencyClientSession(this);
		ret=KErrNone;
		}
		
	return ret;	
	}

void CTelSession::UnsetEmergencyClient()
	{
	TInt ret = Server()->RemoveEmergencyClient(this);
	if (ret==KErrNone && iEmergencyHeap!=NULL)
		{
		iEmergencyHeap->Close();
		iEmergencyHeap = NULL;
		}
	}

RHeap* CTelSession::EmergencyClientHeap(TInt aReq) const
	{
	if (IsEmergencyClientReq(aReq))
		{
		return iEmergencyHeap;
		}
	return NULL;
	}

TBool CTelSession::IsEmergencyClientReq(TInt aReq) const
	{
	if (aReq&KEmergencyClientReq)
		{
		return ETrue;
		}
	return EFalse;
	}

void CTelSession::SetPriorityClientV2()
//
// Check to see if there is already a priority client. If there is 
// then ret will be KErrAlreadyExists, otherwise ret will be KErrNone and the client
// will be set as priority client
//
	{
	RECORD_IPC_LOG2(this, EEtelServerSetPriorityClientV2);
	TInt ret=Server()->SetPriorityClient(this);
	
	if(!iMessage.IsNull())
		{
		RECORD_COMPLETE(iMessage.Session(), iMessage.Function(), ret);
		iMessage.Complete(ret);
		}
	}






