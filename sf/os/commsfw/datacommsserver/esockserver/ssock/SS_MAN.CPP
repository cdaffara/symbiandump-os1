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
//

#include <comms-infras/ss_roles.h>
#include <ss_glob.h>
//#include <es_ini.h>
#include <comms-infras/ss_log.h>
#include <ss_protprov.h>
#include <comms-infras/ss_sapshim.h>
#include <ss_sock.h>
#include "SS_rslv.H"
#include "SS_conn.H"
#include "ss_eintsockimpl.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSockS_MAN, "ESockSSockS_MAN.");
#endif

using namespace ESock;

#if defined (_DEBUG)

/** @file
Check that the protocol has filled in the protocol structure
As much as we can anyway.
Note that this function needs to be maintained in parallel with the contents of es_sock.h and es_prot.h

A panic in this function means that the protocol has returned an invalid TServerProtocolDesc

@internalComponent
*/
void CheckProtocolList(TServerProtocolDesc* aProtocolList,TInt aCount)
	{
	for (TInt i=0;i<aCount;i++)
		{
		if ((aProtocolList+i)->iServiceTypeInfo&0xFFFFFF00)
			{
			Panic(EBadProtocolDescription);
			}
		if ((aProtocolList+i)->iNumSockets==(TInt)0xa5a5a5a5)
			{
			Panic(EBadProtocolDescription);
			}
		if ((aProtocolList+i)->iName.Length()==0)
			{
			Panic(EBadProtocolDescription);
			}
		if ((aProtocolList+i)->iAddrFamily==0 || (aProtocolList+i)->iAddrFamily==0xa5a5a5a5)
			{
			Panic(EBadProtocolDescription);
			}
		if ((aProtocolList+i)->iSockType==0xa5a5a5a5)
			{
			Panic(EBadProtocolDescription);
			}
		//If the socket has no support for sockets (NumSockets is 0)
		//then it should be of SockType 0.
		if ((aProtocolList+i)->iSockType==0)
			{
			if ((aProtocolList+i)->iNumSockets!=0)
				{
				Panic(EBadProtocolDescription);
				}
			}
		if ((aProtocolList+i)->iProtocol==0 || (aProtocolList+i)->iProtocol==0xa5a5a5a5)
			{
			Panic(EBadProtocolDescription);
			}
		if ((((aProtocolList+i)->iVersion.iMajor==0)&&((aProtocolList+i)->iVersion.iMinor==0)&&((aProtocolList+i)->iVersion.iBuild==0)))
			{
			Panic(EBadProtocolDescription);
			}
		if (((aProtocolList+i)->iByteOrder!=EBigEndian && (aProtocolList+i)->iByteOrder!=ELittleEndian && (aProtocolList+i)->iByteOrder!=EOtherByteOrder))
			{
			Panic(EBadProtocolDescription);
			}
		if ((aProtocolList+i)->iServiceInfo&0xfffc0000)
			{
			Panic(EBadProtocolDescription);
			}
		if ((aProtocolList+i)->iNamingServices&0xfffffe00)
			{
			Panic(EBadProtocolDescription);
			}
		if ((aProtocolList+i)->iSecurity&0xfffffffe)
			{
			Panic(EBadProtocolDescription);
			}
		if (((aProtocolList+i)->iMessageSize<0 && (aProtocolList+i)->iMessageSize!=KSocketMessageSizeNoLimit))
			{
			Panic(EBadProtocolDescription);
			}
		}
	}
#endif

//
// Protocol manager assumes that there will be an ini file setion called
// [protocols] which will contain a single entry: protocols= the data for
// which is a comma seperated list of further section names.
// Each of these section names should contina a file name of a .PRT
// and index number of the protocol inside this family and an optional
// bindto= entry specifying which lower protocols this should be bound to.
//
// The protocol manager late loads (and binds) all protocols - keeping only CProtocolRef etc.
// classes which contain enough data to reload the protocol when the first socket is
// created. Protocols will the stay loaded unti lthe last client (who made a socket of
// the specified type) closes. I.e. protocols are created for sockets but referenced by
// sessions.
//

/**
@internalComponent
*/

_LIT(KSockManMainSectionName,"sockman");
_LIT(KProtocolListItemName,"protocols");
_LIT(KFilenameItemName,"filename");
_LIT(KIndexTagName,"index");
_LIT(KBindToItemName,"bindto");
_LIT(KBindFromItemName,"bindfrom");
_LIT(KFlowFactoryUid, "flow_factory_uid");
_LIT(KFlowProtocol, "flow_protocol_id");
_LIT(KOpenAngle, "<");
_LIT(KCloseAngle, ">");
_LIT(KComma, ",");

const TInt KDependSize = 32;


void ProtocolManager::AddDependencyL(const TDesC& aDependentsList, const TDesC& aModuleToLoad)
/**
Adds a module to the dependency list. Dependency list is of form <a>b,c<d>e,f
meaning that module "a" is dependant on "b" and "c" and requires module b and c to be loaded.

@param aDependentsList list of dependant components to be loaded
@param aModuleToLoad The module for which the dependency is added
*/
	{
	CSockManData *globals=SockManGlobals::Get();

	if (globals->iDeps == NULL)
		{
		globals->iDeps = HBufC::NewL(KDependSize);
		}

	TLex lex(aDependentsList);
	TChar ch=0;
	do
		{
		lex.Mark();

		do
			{
			ch=lex.Get();
			}
		while (ch!=',' && ch!=0);

		if (ch==',')
			{
			lex.UnGet();
			}

		TPtrC dependent=lex.MarkedToken();
		lex.Get();

		TBuf<50> searchBuf;
		searchBuf.Append(KOpenAngle);
		searchBuf.Append(dependent);
		searchBuf.Append(KCloseAngle);

		TPtr list = globals->iDeps->Des();
		TInt reqLength = list.Length() + searchBuf.Length() + aModuleToLoad.Length();
		if (reqLength >= list.MaxLength())
			{
			globals->iDeps = globals->iDeps->ReAllocL(reqLength);
			list.Set(globals->iDeps->Des());
			}

		TInt pos = list.Find(searchBuf);
		if (pos == KErrNotFound)
			{
			list.Append(searchBuf);
			list.Append(aModuleToLoad);
			}
		else
			{
			pos++;
			TInt endPos = list.Mid(pos).Find(KOpenAngle);
			if (endPos != KErrNotFound)
				{
				endPos += pos;
				list.Insert(endPos, KComma);
				list.Insert(endPos + 1, aModuleToLoad);
				}
			else
				{
				list.Append(KComma);
				list.Append(aModuleToLoad);
				}
			}
		} while (ch != 0);
	}

TBool ProtocolManager::GetDependency(const TDesC& aTag,TPtrC &aList)
/**
Goes through the dependency list and gets the dependant modules for aTag

@param aTag The protocol tag based on which the dependancy list is searched
@param aList A list of components dependant on aTag protocol which are returned
@return ETrue if a dependency for the component aTag is found else EFalse
*/
	{
	CSockManData *globals=SockManGlobals::Get();

	if(globals->iDeps)
		{

		TPtrC list = *globals->iDeps;

	  	TBuf<50> tag(KOpenAngle);
		tag.Append(aTag);
		tag.Append(KCloseAngle);

		TInt offset= list.Find(tag);

		if(offset!=KErrNotFound)
			{
			aList.Set(list.Mid(offset+tag.Length()));
			offset= aList.Find(KOpenAngle);
			if(offset!=KErrNotFound)
				{
				aList.Set(aList.Mid(0,offset));
				}
			return ETrue;
			}
		}
	return EFalse;
	}

void ProtocolManager::InitL()
/**
Init data structures and hunt for protocols.
*/
	{
	LOG( ESockLog::Printf(_L("ProtocolManager: Initialising.")) );

	// Allocate global storage to be held in tls.
	CSockManData *globals=SockManGlobals::Get();

	globals->iProtocols=new(ELeave) TSglQue<CProtocolRef>(_FOFF(CProtocolRef,iLink));
	CleanupStack::PushL(globals->iProtocols);
	globals->iProtocolFamilies=new(ELeave) TSglQue<CProtocolFamilyRef>(_FOFF(CProtocolFamilyRef,iLink));
	CleanupStack::Pop(globals->iProtocols);
	}

void ProtocolManager::ShutDown()
/**
free data structures
*/
	{
	CSockManData* globals=SockManGlobals::Get();

	TSglQueIter<CProtocolRef> i(*globals->iProtocols);
	CProtocolRef* p=NULL;
	while(p=i++,p!=NULL)
		{
		globals->iProtocols->Remove(*p);
		delete p;
		}

	TSglQueIter<CProtocolFamilyRef> j(*globals->iProtocolFamilies);
	CProtocolFamilyRef* f=NULL;
	while(f=j++,f!=NULL)
		{
		globals->iProtocolFamilies->Remove(*f);
		delete f;
		}

	// delete all the containers and lists of protocols etc.
	delete globals->iProtocols;
	delete globals->iProtocolFamilies;
	delete globals->iDeps;

	}

void ProtocolManager::ProcessIniDataL()
	{
	LOG( ESockLog::Printf(_L("ProtocolManager: Retrieve ESock inidata.")) );

	CSockManData *globals=SockManGlobals::Get();
	const CESockIniData* ini=globals->IniData();

	TPtrC protocols;
	if (ini->FindVar(KSockManMainSectionName,KProtocolListItemName,protocols)==EFalse)
		User::Leave(KErrBadName);

	LOG( ESockLog::Printf(_L("ProtocolManager: Protocol list is %S"), &protocols) );
	LOG( ESockLog::Printf(_L("ProtocolManager: Loading PRTs and setting up protocols.")) );

	TLex lex(protocols);
	TChar ch=0;
	do
		{
		lex.Mark();

		do
			{
			ch=lex.Get();
			}
		while (ch!=',' && ch!=0);

		if (ch==',')
			{
			lex.UnGet();
			}
		TPtrC protTag=lex.MarkedToken();
		lex.Get();
		TPtrC fileName(NULL,0);

		// If there is no filename present (legacy PRT based protocols) then we expect instead the uid of a
		// three plane comms based flow factory that can can return a protocol description
		if(ini->FindVar(protTag, KFilenameItemName, fileName) == EFalse)
			{
			// Deal with the flow case
			// Fetch the UID of the flow factory
			TInt flowFactoryUid;
			if(ini->FindVar(protTag, KFlowFactoryUid, flowFactoryUid) == EFalse)
				{
#if defined(_DEBUG)
				LOG(ESockLog::Printf(_L("ProtocolManager: error while parsing 3PC based protocol - no flow factory UID found")));
				Panic(EBadProtocolDescription);
#else
				User::Leave(KErrNotFound);
#endif
				}

			// We need the flow factory container in order to instantiate a flow factory with ECOM
			// Misconfiguration should not kill off c32 in a release build so we simply leave rather than panic
#if defined(_DEBUG)
			if(globals->iSubConnectionFlowFactories == NULL)
				{
				LOG(ESockLog::Printf(_L("ProtocolManager: error while parsing esk files - flow factories not yet initialised")));
				Panic(EBadProtocolDescription);
				}
#else
			User::LeaveIfNull(globals->iSubConnectionFlowFactories);
#endif

			// Fetch the given factory and fetch the protocol description from it
			CSubConnectionFlowFactoryBase* flowFactory =
				static_cast<CSubConnectionFlowFactoryBase*>(globals->iSubConnectionFlowFactories->FindOrCreateFactoryL(TUid::Uid(flowFactoryUid)));
			CleanupStack::PushL(flowFactory);

			// Fetch the id of the protocol itself (as one factory can create flows of more than one type)
			TInt protocol;
			if(ini->FindVar(protTag, KFlowProtocol, protocol) == EFalse)
				{
				LOG(ESockLog::Printf(_L("ProtocolManager: error while parsing esk file - no 3PC protocol found")));
				__ASSERT_DEBUG(0, Panic(EBadProtocolDescription));
				User::Leave(KErrNotFound);
				}

			// Ask for a protocol description from the flow factory and add that description to our globals
#if defined(_DEBUG)
			TServerProtocolDesc* protocolDescription = NULL;
			TRAPD(descriptionError, protocolDescription = flowFactory->CreateFlowDescriptionL(protocol););
			if(descriptionError != KErrNone)
				{
				LOG(ESockLog::Printf(_L("ProtocolManager: error while parsing esk file - no protocol description provided by factory for 3PC based flow type")));
				Panic(EBadProtocolDescription);
				}
#else
			TServerProtocolDesc* protocolDescription = flowFactory->CreateFlowDescriptionL(protocol);
#endif

			CleanupStack::PushL(protocolDescription);
			CProtocolRef::MakeRefL(protTag, protocolDescription);
			globals->iNumProtocols++;
			delete protocolDescription;

			CleanupStack::Pop(2, flowFactory);
			}
		else
			{
			// Deal with the legacy case
			TPtrC bindfrom(NULL, 0);
			if (ini->FindVar(protTag,KBindFromItemName,bindfrom))
				{
				AddDependencyL(bindfrom, protTag);
				}

			CProtocolFamilyRef* protFamily = NULL;
			TInt err;
			TRAP(err, protFamily = CProtocolFamilyRef::GetProtocolFamilyL(fileName));
			if (err != KErrNone)
				{
				continue;
				}
			CleanupStack::PushL(protFamily);

			// Query the protocol family about all the socket types it can produce
			// We'll remember them because we unload the libraries until we actually need them.
			TServerProtocolDesc *protocolList = NULL;


			// Note: some protocols will leave in their ProtocolList() method.  This
			// is handled correctly here, although the naming convention is incorrect.
			TInt numProtocols = 0;
			TRAP(err, numProtocols = protFamily->ProtocolFamilyL(EFalse).ProtocolList(protocolList));

			if (numProtocols == 0 || err != KErrNone)
				{
				LOG( ESockLog::Printf(_L("ProtocolManager: Protocol %S failed to load (error = %d, number of protocols = %d)"),
					&protTag, err, numProtocols) );
				CleanupStack::Pop(protFamily);
				continue;
				}

#if defined (_DEBUG)
			CheckProtocolList(protocolList,numProtocols);
#endif
			CleanupArrayDeletePushL(protocolList);
			TInt num=0;
			if (numProtocols>1)
				{
				if (ini->FindVar(protTag,KIndexTagName,num) == EFalse)
					{
                    LOG(ESockLog::Printf(_L("ProtocolManager: error while parsing esk file - Protocol index missing")));
					User::Leave(KErrNotFound);
					}
				num--;
				}

			if (num>numProtocols)
				{
				CleanupStack::Pop(protocolList);
				CleanupStack::Pop(protFamily);
				continue;
				}


			CProtocolRef *pp=CProtocolRef::MakeRefL(protTag,protFamily,protocolList+num);
			if (pp==NULL)
				{
				User::Leave(KErrNoMemory);
				}
			CleanupStack::Pop(protocolList);
			LOG(ESockLog::Printf(KESockSessDetailTag, _L("ProtocolManager: prot '%S' ref=%08x, famRef=%08x, num=%d"), &protTag, pp, protFamily, num));

			globals->iNumProtocols++;

			delete []protocolList;
			CleanupStack::Pop(protFamily);
			}
		}
	while (ch!=0);	// ch==0 when last token removed from TLex

	// Leave if there's no point in carrying on. - i.e. no protocols.
	if (globals->iNumProtocols==0)
		{
		LOG(ESockLog::Printf(_L("ProtocolManager: No protocols - leaving.")));
		User::Leave(KErrBadDriver);
		}

	// Delete all protocolfamilybase objects, causing DLLs to unload
	LOG(ESockLog::Printf(_L("ProtocolManager: Finished initialising. Unloading all PRTs.")));
	TSglQueIter<CProtocolFamilyRef> familyIter(*globals->iProtocolFamilies);
	CProtocolFamilyRef* family;
	while(family=familyIter++,family!=NULL)
		{
		delete &(family->ProtocolFamilyL(EFalse));
		}

	LOG(ESockLog::Printf(_L("ProtocolManager: Done.")));

	if(globals->iDeps)
		{
		LOG(ESockLog::Printf(_L("Dependencies: %S"), &*(globals->iDeps)));
		}
	}

void ProtocolManager::SafeCleanupProtocol(TAny* aProtocolRef)
/**
Try to clean up a protocol instance

This routine is pushed onto the cleanup stack in order to clean up a protocol instance.
The check against the protocol pointer being non-NULL is a safety check.  This routine was
created because ProtocolManager::CleanupProtocol(protocol) was being called in circumstances
where the protocol had already gone and the pointer to it had become invalid.  This routine
takes as argument a pointer to the CProtocolRef instance of the protocol, so that the protocol
pointer can be checked against NULL.

@param aProtocolRef pointer to the CProtocolRef instance whose protocol needs to be deleted
*/
	{
	CProtocolRef *pRef = reinterpret_cast<CProtocolRef*>(aProtocolRef);
	if (pRef->Protocol())
		{
		pRef->Protocol()->TryDelete();
		}
	}

void ProtocolManager::CleanupProtocol(TAny* aProtocol)
	{
	((CProtocolBase *)aProtocol)->TryDelete();
	}

void ProtocolManager::TransferSocketL(CSocket* aSocket, CPlayer* aPlayer)
/**
Transfer socket from one session to another
*/
	{

	//-- check the flag that enables socket transfer and the capabilities of the process that owns aSession,
	//-- where the socket is supposed to be transferred.
	if(aSocket->iSecTransferEnabled && aSocket->iTransferSecPolicy.CheckPolicy(aPlayer->SafeMessage()))
	    {//-- everything is OK, reset socket transfer flag.
	    aSocket->iSecTransferEnabled = EFalse;
	    }
	else
	    {//-- security check failed.
	    User::Leave(KErrPermissionDenied);
        }

	if (aSocket->iSSP)
	    {
 		// For non-null sockets, ask the socket provider to perform security
 		// policy checking on new session
 		User::LeaveIfError(aSocket->iSSP->SecurityCheck(aPlayer->CurrentSession()));
 		}

	// Add the protocol of the socket being transferred to the receipient session's list of protocols
	// The socket doesn't know about legacy protocols directly anymore so we need to find the protocol
	TProtocolDesc* info = aSocket->iProtocolInfo;
	__ASSERT_DEBUG(info, User::Panic(KSpecAssert_ESockSSockS_MAN, 1));
	CProtocolRef* protocolReference = FindProtocolL(info->iAddrFamily, info->iSockType, info->iProtocol);
	CProtocolBase* protocol = protocolReference->Protocol();

	// The protocol reference will not have a protocol member if it refers to a new 3PC based flow
	// Add the protocol to the sessionproxy for the session receiving the socket
	if(protocol)
		{
		aPlayer->CurrentSessionProxyL()->AddProtocolL(protocol);
		}
	}

CProtocolBase* ProtocolManager::FindAndLoadProtocolL(const TDesC& aName, TProtocolType aType)
/**
Find a protocol by name and load it.
Caller is responsible for ultimately deleting the protocol

This is for use by extension DLLs as it does not attach the protocol
to a session.

*/
	{
	TServerProtocolDesc pinfo;
	User::LeaveIfError(ProtocolInfo(aName,pinfo));
	switch (aType)
		{
	case ENormalProtocol:
		// Dont care what extra interfaces it supports
		break;
	case EInterfaceProtocol:
		// Need a protocol that support Interface extensions
		if (!(pinfo.iServiceTypeInfo & EInterface))
			{
			User::Leave(KErrBadName);
			}
		break;
		}
	return (FindAndLoadProtocolL(pinfo.iAddrFamily, pinfo.iSockType, pinfo.iProtocol));
	}

CProtocolBase* ProtocolManager::FindAndLoadProtocolL(TUint aAddrFamily,TUint aSockType,TUint aProtocol)
/**
Find a protocol by address family, socket type, and protocol ID and load it.
Caller is responsible for ultimately deleting the protocol

This is for use by extension DLLs as it does not attach the protocol
to a session.
*/
	{
	CProtocolRef* pref = FindProtocolL(aAddrFamily,aSockType,aProtocol);
	pref->LoadAndBindL();
	return pref->Protocol();
	}

void ProtocolManager::LoadProtocolL(TUint anAddrFamily, TUint aSocketType, TUint aProtocol, CPlayer* aPlayer)
/**
Manually load the specified protocol
This is used by clients who need to ensure that an open call will not take a
significant amount of time (e.g. IrCOMM) It effectively makes all the lengthy parts of Open asynchronous.

*/
    {
	CProtocolRef* prot = FindProtocolL(anAddrFamily, aSocketType, aProtocol);

	// If the protocol reference is for a "three plane comms" based flow then we silently return
	// without loading anything as their lifetime is managed by ECOM and therefore explicit loading is not supported
	if(prot->GetFlag() & CProtocolRef::EThreePlaneCommsBased)
	    {
		return;
	    }

	// Got one, make sure the library is loaded and we have an instance of the protocol object.
	prot->LoadAndBindL();
	CleanupStack::PushL(TCleanupItem(ProtocolManager::CleanupProtocol,prot->Protocol()));

	// register the  protocol with the Session
	aPlayer->CurrentSessionProxyL()->AddProtocolL(prot->Protocol());
	CleanupStack::Pop();
	}

void ProtocolManager::UnLoadProtocolL(TUint anAddrFamily, TUint aSocketType, TUint aProtocol, CPlayer* aPlayer)
/**
Do the opposite of the above function

*/
    {
    CProtocolRef* prot = FindProtocolL(anAddrFamily, aSocketType, aProtocol);

	// If the protocol reference is for a "three plane comms" based flow then we silently return
	// without attempting to unload anything as their lifetime is managed by ECOM only
	if(prot->GetFlag() & CProtocolRef::EThreePlaneCommsBased)
	    {
		return;
	    }

    aPlayer->CurrentSessionProxyL()->RemoveProtocolL(prot->Protocol());
    }

CHostResolver* ProtocolManager::NewHostResolverL(TUint anAddrFamily, TUint aProtocolId, CPlayer* aPlayer, const TSubSessionUniqueId aSubSessionUniqueId)
/**
Create a new host resolver object

*/
	{
	CProtocolRef * prot=FindProtocolL(anAddrFamily,KUndefinedSockType,aProtocolId);
	if (!(prot->Info().iNamingServices & KNSNameResolution))
	    {
		User::Leave(KErrNotSupported);
	    }

	// Got one, make sure protocol & library are loaded
	prot->LoadAndBindL();

	CleanupStack::PushL(TCleanupItem(ProtocolManager::CleanupProtocol,prot->Protocol()));

	CHostResolvProvdBase* hr=prot->Protocol()->NewHostResolverL();

	// Give the host resolver the info to clean up after itself.
	hr->iProtocol=prot->Protocol();

    CleanupStack::Pop();
	CleanupStack::PushL(hr);


	// ask the resolver provider to perform security policy checking
	User::LeaveIfError(hr->SecurityCheck(aPlayer->CurrentSession()));
	CHostResolver* r=CHostResolver::NewLC(prot, aPlayer->CurrentSession(), aPlayer, aSubSessionUniqueId);

//	CleanupStack::PushL(r);


//	CleanupStack::Pop(r);
//	CleanupStack::PushL(TCleanupItem(CloseSubSession,r));

	// register the  protocol with the Session
//	aPlayer->CurrentSessionProxyL()->AddProtocolL(prot->Protocol());

	aPlayer->SubSessions().AppendL(r);
	hr->SetNotify(r);
	r->SetResolver(hr);
	CleanupStack::Pop(r);
	CleanupStack::Pop(hr);
//	CleanupStack::Pop(1);

	return r;
	}

CServiceResolver* ProtocolManager::NewServiceResolverL(TUint anAddrFamily, TUint aSocketType, TUint aProtocolId, CPlayer* aPlayer, const TSubSessionUniqueId aSubSessionUniqueId)
/**
Create a new service resolver provider

*/
	{
	CProtocolRef * prot=FindProtocolL(anAddrFamily,aSocketType,aProtocolId);
	if (!(prot->Info().iNamingServices & KNSServiceResolution))
	    {
		User::Leave(KErrNotSupported);
	    }

	// Got one, make sure protocol & library are loaded
	prot->LoadAndBindL();
	CleanupStack::PushL(TCleanupItem(ProtocolManager::CleanupProtocol,prot->Protocol()));

	CServiceResolvProvdBase *sr=NULL;

	sr=prot->Protocol()->NewServiceResolverL();
	CleanupStack::PushL(sr);

	// Give the host resolver the info to clean up after itself.
	sr->iProtocol=prot->Protocol();

	// ask the resolver provider to perform security policy checking
	User::LeaveIfError(sr->SecurityCheck(aPlayer->CurrentSession()));
	CServiceResolver* s=CServiceResolver::NewLC(prot, aPlayer->CurrentSession(), aPlayer, aSubSessionUniqueId);
	sr->SetNotify(s);

//	CleanupStack::PushL(TCleanupItem(CloseSubSession,s));

	// register the  protocol with the Session
//	aPlayer->CurrentSessionProxyL()->AddProtocolL(prot->Protocol());

	aPlayer->SubSessions().AppendL(s);
	s->SetResolver(sr);

	CleanupStack::Pop(3);
	return s;
	}

CNetDatabase* ProtocolManager::NewNetDatabaseL(TUint anAddrFamily, TUint aProtocolId, CPlayer* aPlayer, const TSubSessionUniqueId aSubSessionUniqueId)
/**
Create a new database type thang

*/
	{
	CProtocolRef * prot=FindProtocolL(anAddrFamily,KUndefinedSockType,aProtocolId);
	if (!(prot->Info().iNamingServices & KNSInfoDatabase))
		{
		User::Leave(KErrNotSupported);
		}

	// Got one, make sure protocol & library are loaded
	prot->LoadAndBindL();
	CleanupStack::PushL(TCleanupItem(ProtocolManager::CleanupProtocol,prot->Protocol()));

	CNetDBProvdBase *db=NULL;
	db=prot->Protocol()->NewNetDatabaseL();

	CleanupStack::PushL(db);

	// Give the host resolver the info to clean up after itself.
	db->iProtocol=prot->Protocol();

	// ask the net database provider to perform security policy checking
	User::LeaveIfError(db->SecurityCheck(aPlayer->CurrentSession()));

	CNetDatabase* n=CNetDatabase::NewLC(prot, aPlayer->CurrentSession(), aPlayer, aSubSessionUniqueId);
	db->SetNotify(n);
//	CleanupStack::Pop();

//	CleanupStack::PushL(TCleanupItem(CloseSubSession,n));

	// register the  protocol with the Session
//	aPlayer->CurrentSessionProxyL()->AddProtocolL(prot->Protocol());

	aPlayer->SubSessions().AppendL(n);
	n->SetProvd(db);

	CleanupStack::Pop(3);
	return n;
	}

CInternalSocketImpl* ProtocolManager::NewInternalSocketL ( TAny* aParams, const TDesC& aName )
	{
	TServerProtocolDesc pinfo;
	User::LeaveIfError(ProtocolInfo(aName,pinfo));
	return ProtocolManager::NewInternalSocketL ( aParams, pinfo.iAddrFamily, pinfo.iSockType, pinfo.iProtocol );
	}

CInternalSocketImpl* ProtocolManager::NewInternalSocketL ( TAny* aParams, TUint aAddrFamily, TUint aSockType, TUint aProtocol )
	{
	CInternalSockSubSession::TParameters* params = reinterpret_cast < CInternalSockSubSession::TParameters* > ( aParams );
	CProtocolRef* prot = FindProtocolL ( aAddrFamily, aSockType, aProtocol );
	if (!(prot->Info().iServiceTypeInfo&ESocketSupport))
	    {
		User::Leave(KErrNotSupported);
		}
	prot->LoadAndBindL ();
	// Please check @NewSocketL to see why we need to push this item.
	CleanupStack::PushL ( TCleanupItem ( ProtocolManager::CleanupProtocol,prot->Protocol () ) );
	CInternalSocketImpl* intSock = CInternalSocketImpl::NewL ( *params, &( prot->Info() ), prot->Protocol() );
	CleanupStack::Pop (); // prot
	return intSock;
	}

TUint ProtocolManager::NumProtocols(void)
	{

	return SockManGlobals::Get()->iNumProtocols;
	}

TInt ProtocolManager::ProtocolInfo(const TDesC &aName,TProtocolDesc &aProtocol)
	{
	TInt ret;
	TServerProtocolDesc info;

	ret = ProtocolInfo(aName, info);
	if (ret==KErrNone)
		{
		aProtocol = info;
		}
	return ret;
	}


TInt ProtocolManager::ProtocolInfo(const TDesC &aName,TServerProtocolDesc &aProtocol)
/**
Find a protocol by name - no wildcard support.

*/
	{
	TSglQueIter<CProtocolRef> i(*SockManGlobals::Get()->iProtocols);

	// Run the queue looking for a match.
	do
		{
		if(((CProtocolRef *)i)->Info().iName.Compare(aName)==0)
			{
			aProtocol=((CProtocolRef*)i)->Info();
			return KErrNone;
			}
		i++;
		}
	while((CProtocolRef *)i);

	return KErrNotFound;
	}


CProtocolRef* ProtocolManager::FindProtocolL(TInt anAddrFamily,TUint aSocketType,TInt aProtocol)
/**
 Find a protocol from the lists.
*/
	{

	TSglQueIter<CProtocolRef> i(*SockManGlobals::Get()->iProtocols);

	CProtocolRef *pP;
	// Run the protocol queue looking for a match on requested socket type.
	while ((pP=i++)!=NULL)
		{
		if (pP->Matches(anAddrFamily,aSocketType,aProtocol))
			{
			return pP;
			}
		}

	User::Leave(KErrBadName);
	return NULL;	//lint !e527	// LINT knows we can't get here
	}


CProtocolFamilyRef::CProtocolFamilyRef()
	:CBase()
	{
	}

CProtocolFamilyRef::~CProtocolFamilyRef()
	{
	if (iLibFileName)
		{
		delete iLibFileName;
		}
	}

CProtocolFamilyRef* CProtocolFamilyRef::GetProtocolFamilyL(TDesC& aName)
/**
Either return a pointer to an existing protocol ref or create a new one.

*/
	{

	TSglQue<CProtocolFamilyRef>* families=SockManGlobals::Get()->iProtocolFamilies;
	TSglQueIter<CProtocolFamilyRef> i(*families);

	// Run through the list of existing protocols looking for a match.
	CProtocolFamilyRef* protRef;
	while(protRef=(CProtocolFamilyRef *)i,protRef)
		{
		if(protRef->iLibFileName->Compare(aName)==0)
			{
			if (protRef->iProtocolFamily)
				{
				return protRef;
				}
			else
				{
				break;
				}
			}
		i++;
		}

	// Not found one yet - we must create one.
	if (!protRef)
		{
		protRef=new(ELeave) CProtocolFamilyRef;
		CleanupStack::PushL(protRef);
		protRef->iLibFileName=aName.AllocL();

      // Ensure that this protocol we're about to return is loaded
      protRef->LoadL();

      // If it didn't leave, then add it to the list
      families->AddLast(*protRef);
		}
	else
		{
		CleanupStack::PushL(protRef);
      // Ensure that this protocol we're about to return is loaded
      protRef->LoadL();
		}

	CleanupStack::Pop();

	return protRef;
	}

void CProtocolFamilyRef::LoadL()
/**
Load the protocol. Leave it un-initted until the last minute - i.e. init it in the accessor.

*/
	{
	if (iProtocolFamily)
		{
		return;
		}

	TAutoClose<RLibrary> lib;
	const TInt ret = lib.iObj.Load(*iLibFileName);
	LOG(TBuf8<64> buf8);
	LOG(buf8.Copy(iLibFileName->Des()));
	LOG(ESockLog::Printf(KESockProvChoresTag, _L8("CProtocolFamilyRef(%08x)::LoadL '%S' [ret=%d]"), this, &buf8, ret));
	if(ret!=KErrNone)
		{
		delete iLibFileName; // cleanup after aName.AllocL
		iLibFileName = NULL;
		User::Leave(ret);
		}
	lib.PushL();

	// The Uid check
#ifdef _UNICODE
	if(lib.iObj.Type()[1]!=TUid::Uid(KUidUnicodeProtocolModule))
		{
		LOG(ESockLog::Printf(_L8("FAILED uid check! (type=%08X)"), lib.iObj.Type()[1].iUid));
		User::Leave(KErrBadLibraryEntryPoint);
		}
#else
	if(lib.iObj.Type()[1]!=TUid::Uid(KUidProtocolModule))
		{
		User::Leave(KErrBadLibraryEntryPoint);
		}
#endif

	TProtocolNew libEntry=(TProtocolNew)lib.iObj.Lookup(1);
	if (libEntry==NULL)
		{
		LOG(ESockLog::Printf(_L8("FAILED retrieving factory entrypoint (ord 1)")));
		User::Leave(KErrNoMemory);
		}

	iProtocolFamily=(*libEntry)();
	if (!iProtocolFamily)
		{
		LOG(ESockLog::Printf(_L8("FAILED running factory entrypoint")));
		User::Leave(KErrNoMemory);
		}

	//	Make sure the protocol family has all the info needed to clean up after itself.
	iProtocolFamily->iManagerRef=this;
	TRAPD(res, iProtocolFamily->SetLibraryL(lib.iObj));
	if(res != KErrNone)
		{
		LOG(ESockLog::Printf(_L8("FAILED creating lib unloader (err %d)"), res));
		delete iProtocolFamily;
		iProtocolFamily=0;
		User::Leave(res);
		}

	lib.Pop();
	lib.iObj.SetHandle(0); // Dont let ~TAutoClose() close library
	}

CProtocolFamilyBase& CProtocolFamilyRef::ProtocolFamilyL(TBool aInstall)
/**
Accessor to ProtocolFamily from manager ref - this must leave because we perform late loading and
hence must allocate memory and other nasty grubby things.

*/
	{
	if (!iProtocolFamily)
		{
		LoadL();
		}
	__ASSERT_DEBUG(iProtocolFamily, User::Panic(KSpecAssert_ESockSSockS_MAN, 2));

	if (aInstall && !(iFlags&EInstalled))
		{
		TInt r=iProtocolFamily->Install();
		if (r!=KErrNone)
			{
			delete iProtocolFamily;
			iProtocolFamily=0;
			User::Leave(r);
			}
		iFlags|=EInstalled;
		}

	return *iProtocolFamily;
	}

CProtocolRef::~CProtocolRef()
	{
	delete iTag;
	}

CProtocolRef::CProtocolRef()
	:CBase()
	{
	}

TBool CProtocolRef::Matches(TUint anAddrFamily,TUint aSockType,TUint aProtocol)
/**
Return ETrue if a protocol desc matches the specified family type and protocol
We allow clients to be a little hazy about their exact requirements

*/
	{
	if(iInfo.iAddrFamily==anAddrFamily &&
		(iInfo.iSockType==KUndefinedSockType || aSockType==KUndefinedSockType || iInfo.iSockType==aSockType) &&
		(iInfo.iProtocol==KUndefinedProtocol || aProtocol==KUndefinedProtocol || iInfo.iProtocol==aProtocol))
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}


CProtocolRef* CProtocolRef::GetProtocolNoCreate(TProtocolRefList *iProtocolList, const TDesC& aTag)
/**
Find a protocol from the lists by name
Return null if not found;
*/
	{
	TSglQueIter<CProtocolRef> i(*iProtocolList);

	CProtocolRef *pP = NULL;
	// Run the protocol queue looking for a match on requested name
	while ((pP=i++)!=NULL)
		{
		if (pP->iTag->Compare(aTag)==0)
			{
			return pP;
			}
		}
	return NULL;
	}

CProtocolRef* CProtocolRef::GetProtocolL(const TDesC& aTag)
/**
Find a protocol from the lists by name
Will make a new ref and families if required.

*/
	{
	TSglQue<CProtocolRef>* protocols=SockManGlobals::Get()->iProtocols;


	CProtocolRef *pP = GetProtocolNoCreate(protocols,aTag);

	if (!pP)
		{
		pP=MakeRefL(aTag);
		}

	return pP;
	}

void CProtocolRef::Cleanup(TAny* aFamily)
/**
Close a protocol family from the cleanup stack.

*/
	{
	((CProtocolFamilyRef *)aFamily)->Close();
	}

void CloseProtocol(TAny* aProtocol)
    {
     ((CProtocolBase*)aProtocol)->Close();
    }
void BindingLeave(TAny* aProtRef)
 	{
 	TUint flag=((CProtocolRef*)aProtRef)->GetFlag();
 	((CProtocolRef*)aProtRef)->SetFlag(~(CProtocolRef::EBound) & flag);
 	}

void CProtocolRef::LoadL(TBool aLoadForBinding)
/**
Fill in a protocol ref - loading any libs or families
Loads and binds the requested protocol loading any libraries required.
May go recursive if a bindee needs loading and the aloadForBinding argument is set.

*/
	{
	if (iProtocol)
 		{
 		if (!aLoadForBinding)
 			{
 			CleanupStack::PushL(TCleanupItem(BindingLeave, this));
 			ResolveBindingL();
 			CleanupStack::Pop();
 			}
   		return;
 		}

	// Three plane comms protos shouldn't be loading like this
	__ASSERT_DEBUG(! (GetFlag() & CProtocolRef::EThreePlaneCommsBased), User::Panic(KSpecAssert_ESockSSockS_MAN, 3));
	
	__ASSERT_DEBUG(iFamily, User::Panic(KSpecAssert_ESockSSockS_MAN, 4));
	iFamily->ProtocolFamilyL().Open();

	CleanupStack::PushL(TCleanupItem(CProtocolRef::Cleanup,iFamily));

	// Get the Protocol from the family
	iProtocol=iFamily->ProtocolFamilyL().NewProtocolL(iInfo.iSockType,iInfo.iProtocol);

	// At this stage the iProtocol member effectively takes over control of closing
	// the family reference
	// Note there is a work around in TCPIP 047 which fixes the problem
	// by pushing a NULL reference
	CleanupStack::Pop();

	//	Make sure the protocol has the info needed to clean up after itself.
	iProtocol->iManagerRef=this;

	CleanupStack::PushL(iProtocol);
	if (!(iFlags & EInited))
		{
		iProtocol->InitL(*iTag);
		iFlags|=EInited;
		}

	if (!aLoadForBinding)
		{
		ResolveBindingL();
		}

	CleanupStack::Pop(1);  // Was pop(2) see comment above
	}

void CProtocolRef::ResolveBindingL()
/**
Resolve binding requirements for a top level (i.e service provider) protocol

*/
	{
	if (iFlags&EBound)
		{
		return;
		}

	LoadL(ETrue);
//	CESockIniData* ini=CESockIniData::NewL();
//	CleanupStack::PushL(ini);

	const CESockIniData* ini=SockManGlobals::Get()->IniData();
	DoBindingL(*ini);
//	CleanupStack::PopAndDestroy(ini);

	iFlags|=EBound;
	}


void CProtocolRef::DoBindingL(const CESockIniData& aIniFile)
/**
 Actually perform the binding for a given protocol. Recursive
*/
	{
	if (iFlags&EBound)
		{
		return;
		}

	iFlags|=EBound;	// Set "is bound" flag here to preempt any recursive binding.

	TPtrC bindList;
	if (aIniFile.FindVar(*iTag,KBindToItemName,bindList))
		{
		// BindTo scenario. Proceed with the node current node is binding to
		ProcessBindListL(bindList, aIniFile);
		}

	TPtrC depList;
	TBool foundDep = ProtocolManager::GetDependency(*iTag,depList);
	if(foundDep)
		{
		// BindFrom scenario. Proceed with the node that is binding from to current one
		ProcessBindListL(depList, aIniFile);
		}

	StartL();
	}

void CProtocolRef::ProcessBindListL(TPtrC aBindList, const CESockIniData& aIniFile)
	{
	LOG( ESockLog::Printf(_L("ProcessBindListL(aBindList = %S)"), &aBindList) );
	TLex lex(aBindList);
	TChar ch=0;
	do
		{
		lex.Mark();
		do
			{
			ch=lex.Get();
			}
		while (ch!=',' && ch!=0);

		if (ch==',')
			{
			lex.UnGet();
			}

		CProtocolRef* prot=GetProtocolL(lex.MarkedToken());
		lex.Get();
		prot->LoadL(ETrue);
		prot->iProtocol->Open();

		CleanupStack::PushL(TCleanupItem(CloseProtocol, prot->iProtocol));
		CleanupStack::PushL(TCleanupItem(BindingLeave, prot));

		prot->DoBindingL(aIniFile);
		prot->StartL();

		LOG_STMT(TPtrC bindFrom = iProtocol->Tag();)
		LOG_STMT(TPtrC bindTo = prot->iProtocol->Tag();)
		LOG( ESockLog::Printf(_L("ProcessBindListL(Binding: %S -> %S)"), &bindFrom, &bindTo) );

		iProtocol->BindToL(prot->iProtocol);

		CleanupStack::Pop();
		CleanupStack::PopAndDestroy();
		}
	while (ch!=0); // ch == 0 when last token extracted from TLex
	}


CProtocolRef* CProtocolRef::MakeRefL(const TDesC& aTag,CProtocolFamilyRef* aFamily,TServerProtocolDesc* anInfo)
/**
Protocol reference 'constructor'.
CProtocolRef really is a refernece and hence it runs the list of currently loaded protocols
before really and truly loading

*/
	{
	CProtocolRef* pP=new(ELeave) CProtocolRef;
	CleanupStack::PushL(pP);
	pP->iTag=aTag.AllocL();

	if (aFamily==NULL && anInfo==NULL)
		{
		TPtrC familyName(NULL,0);

		const CESockIniData* ini=SockManGlobals::Get()->IniData();
		if (!ini->FindVar(aTag,KFilenameItemName,familyName))
			{
			User::Leave(KErrBadName);
			}
		pP->iFamily=CProtocolFamilyRef::GetProtocolFamilyL(familyName);

		TServerProtocolDesc* protocolList;
		TInt numProtocols=pP->iFamily->ProtocolFamilyL().ProtocolList(protocolList);
		TInt index=0;

		if (numProtocols>1 && !ini->FindVar(aTag,KIndexTagName,index))
			{
			User::Leave(KErrBadName);
			}

		pP->iInfo=protocolList[index-1];
		delete[] protocolList;
		}
	else
		{
		pP->iInfo=*anInfo;
		pP->iFamily=aFamily;
		}

	TSglQue<CProtocolRef>* protList = SockManGlobals::Get()->iProtocols;
#ifdef _DEBUG
	CProtocolRef* ref = GetProtocolNoCreate(protList,aTag);
	if (ref) // uhoh we have a duplicate protocol better panic in debug mode
		{
		LOG( ESockLog::Printf(_L("ERROR in config - duplicate protocol loaded: %S loaded more than once"),&aTag));
		Panic(EBadProtocolDescription);
		}
#endif
	protList->AddLast(*pP);
	CleanupStack::Pop(pP);
	return pP;
	}

CProtocolRef* CProtocolRef::MakeRefL(const TDesC& aName, TServerProtocolDesc* aInfo)
/**
Protocol reference 'constructor'.
This overload of is for "three plane comms" based flows only. It creates a protocol reference
that is then not subject to particular requirements of legacy protocols such as explicit
loading with `RSocketServ::StartProtocol()`
*/
	{
	__ASSERT_DEBUG(aInfo, Panic(EBadProtocolDescription));
	User::LeaveIfNull(aInfo);

	// Create our protocol reference and designate it as being 3PC based
	CProtocolRef* pP = new(ELeave) CProtocolRef;
	pP->SetFlag(EThreePlaneCommsBased);

	CleanupStack::PushL(pP);
	pP->iTag = aName.AllocL();
	pP->iInfo = *aInfo;
	pP->iFamily = NULL;

	// Now add it to our global list of protocols
	TSglQue<CProtocolRef>* protList = SockManGlobals::Get()->iProtocols;

#ifdef _DEBUG
	CProtocolRef* ref = GetProtocolNoCreate(protList, aName);
	if (ref) // uhoh we have a duplicate protocol better panic in debug mode
		{
		LOG( ESockLog::Printf(_L("ERROR in config - duplicate protocol loaded: %S loaded more than once"), &aName));
		Panic(EBadProtocolDescription);
		}
#endif

	protList->AddLast(*pP);
	CleanupStack::Pop(pP);
	return pP;
	}


void CProtocolRef::StartL()
/**
Start the protocol - only once mind lad.

*/
	{
	if (!(iFlags&EStarted))
		{
		iProtocol->StartL();
		iFlags|=EStarted;
		}
	}

#ifdef __FLOG_ACTIVE

void TProtocolManagerLogger::LogLoadedInfo()
	{
	CSockManData* globals=SockManGlobals::Get();
	TBuf<256> descList;
	TLogIgnoreOverflow16 overflow;

	TSglQueIter<CProtocolFamilyRef> j(*globals->iProtocolFamilies);
	CProtocolFamilyRef* f=NULL;
	while(f=j++,f!=NULL)
		{
		if(f->iProtocolFamily)
			{
			descList.AppendFormat(_L(" %08x,"), &overflow, f->iProtocolFamily);
			}
		}
	LOG( ESockLog::Printf(_L("Families (addr):%S"), &descList) );

	descList.SetLength(0);
	TSglQueIter<CProtocolRef> i(*globals->iProtocols);
	CProtocolRef* p=NULL;
	while(p=i++,p!=NULL)
		{
		if(p->Protocol())
			{
			const TDesC& tag(p->Tag());
			descList.AppendFormat(_L(" %S[%08x],"), &overflow, &tag, p->Family());
			}
		}
	LOG( ESockLog::Printf(_L("Protocols + fam addr:%S"), &descList) );
	}

#endif
