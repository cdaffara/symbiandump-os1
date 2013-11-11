// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#include "rsstd.h"
#include "bindmgr.h"
#include <elements/cfmacro.h>
#include <cflog.h>

__CFLOG_STMT(_LIT8(KLogSubSysRS, "RootServer");) // subsystem name

EXPORT_C CRootServerSession::CRootServerSession(const CRootServer* aRootServer)
    :iActiveHandlers(_FOFF(CSessionHandler, iLink))
    {
    iRootServer = (CRootServer*) aRootServer;
    }

TInt CRootServerSession::CancelLoadCPM(const RMessage2& aMessage)
    /** Attempt to cancel a load cpm request.
     If a pending request is not found, we check that the module
	 is running already.  We cannot actually cancel a request as we cannot
	 guarantee that it will not be completed just after we delete the request,
	 which will therefore lead to a stray event - bad!
     @internalComponent
     @param aMessage The message which will be completed.
     @return Whether the function was successful or not.
     */
    {
    TCFModuleNameF name;
    TInt result = aMessage.Read(0, name);
    if(result==KErrNone)
		{
		__CFLOG_1(KLogSubSysRS, KLogCode, _L8("CRootServerSession - CancelLoadCPM: %S"), &name);
		}
	else
		{
		__CFLOG_1(KLogSubSysRS, KLogCode, _L8("CRootServerSession - CancelLoadCPM: error %d reading module name"), result);
        return result;
		}

	TDblQueIter<CSessionHandler> handlerIter(iActiveHandlers);
    CSessionHandler* handler;
	result = KErrNotFound;
    while(result != KErrNone && (handler = handlerIter++) != NULL)
        {
		if(handler->HandlerType() == CSessionHandler::ESHTypeLoadCPM)
			{
			CSessionHandlerLoadCPM* loadHandler = static_cast<CSessionHandlerLoadCPM*>(handler);
			if(loadHandler->MatchPending(name))
				{
				// Complete with KErrCancel
				if(loadHandler->CancelHandler(ETrue))
					{
					delete loadHandler;
					}
				result = KErrNone;
				}
			}
        }

    return result;
    }

TInt CRootServerSession::LoadCPML(const RMessage2& aMessage)
    /** Load a module.
     Generic load function for both servers and providers.  Simply reads data from
	 message and uses aModuleRequest to decide which type of load to kick off
     @internalComponent
     @param aMessage The message which will be completed.
     @return Whether the function was successful or not.
     */
	{

	iMessage = aMessage;
    TRSStartModuleParams params;
	aMessage.ReadL(0, params);

	__CFLOG_1(KLogSubSysRSModule, KLogCode, _L8("CRootServerSession - LoadCPML: %S"), &params.iParams.iName);

    if((params.iParams.iName.Length()>KCFMaxModuleName) ||
       (params.iParams.iName.Length()<1) ||	// magic number 1 - name must have at least one character
       (params.iParams.iName.MaxLength()!=KCFMaxModuleName))
        {
        return (KErrRSInvalidParameterName);
        }

    if((params.iParams.iFilename.Length()>KMaxFileName) ||
       (params.iParams.iFilename.Length()<5) || // magic number 5 - one character plus "." plus 3 letter file extension
       (params.iParams.iFilename.MaxLength()!=KMaxFileName))
        {
        return (KErrRSInvalidParameterFile);
        }

    if(params.iParams.iThreadFunctionOrdinal<0)
        {
        return (KErrRSInvalidParameterThreadFuncOrdinal);
        }

    if(params.iParams.iStackSize<0)
        {
        return (KErrRSInvalidParameterStackSize);
        }

	if(params.iParams.iHeapType==ENewHeap)
		{
		if(params.iParams.iMinHeapSize<KMinHeapSize)
    		{
    		return (KErrRSInvalidParameterHeapSize);
    		}
		if(params.iParams.iMaxHeapSize<params.iParams.iMinHeapSize)
    		{
    		return (KErrRSInvalidParameterHeapSize);
    		}
		}

    HBufC8* inidata = NULL;
  	TInt size = 0;
	if(aMessage.Ptr1())
		{
	  	size = aMessage.GetDesLength(1);
		User::LeaveIfError(size);
		}

  	if (size>0)
  		{
  		inidata = HBufC8::NewLC(size);
    	TPtr8 tpInidata(inidata->Des());
    	aMessage.ReadL(1, tpInidata);
    	}

	__CFLOG(KLogSubSysRS, KLogCode,
				_L8("CRootServerSession - Load request in LoadModuleL"));
	iRootServer->LazyLoadL();
	CCommsProviderModule* pModule = iRootServer->CreateCpmL(params, inidata);
	if(inidata)
		{
		CleanupStack::Pop(inidata);
		}
	CleanupStack::PushL(pModule);
	pModule->CreateThreadL();

	CSessionHandlerLoadCPM* pHandler =
			CSessionHandlerLoadCPM::NewL(this, iRootServer->BindManager(), aMessage);
	RegisterHandler(*pHandler);
	pHandler->SetHandler(pModule->GetName());
	TInt result = pHandler->Start();
	CleanupStack::Pop(pModule);	// pModule

	return result;
    }

TInt CRootServerSession::CancelUnloadCPM(const RMessage2& aMessage)
    /** Cancel an unload cpm request.
     If a pending unload request is found, then we still
	 cannot cancel it, otherwise we may have a stray event as we
	 cannot guarantee that the request would not be completed
	 just after we attempt to cancel it.
     @internalComponent
     @param aMessage The message which will be completed.
     @return Whether the function was successful or not.
     */
	{

    TCFModuleNameF name;
    TInt result = aMessage.Read(0, name);
    if(result==KErrNone)
		{
		__CFLOG_1(KLogSubSysRS, KLogCode, _L8("CRootServerSession - CancelUnloadCPM: %S"), &name);
		}
	else
		{
		__CFLOG_1(KLogSubSysRS, KLogCode, _L8("CRootServerSession - CancelUnloadCPM: error %d reading module name"), result);
        return result;
		}

	TDblQueIter<CSessionHandler> handlerIter(iActiveHandlers);
    CSessionHandler* handler;
	result = KErrNotFound;
    while(result != KErrNone && (handler = handlerIter++) != NULL)
        {
		if(handler->HandlerType() == CSessionHandler::ESHTypeUnLoadCPM)
			{
			CSessionHandlerUnLoadCPM* unLoadHandler = static_cast<CSessionHandlerUnLoadCPM*>(handler);
			if(unLoadHandler->MatchPending(name))
				{
				// Complete with KErrCancel
				if(unLoadHandler->CancelHandler(ETrue))
					{
					delete unLoadHandler;
					}
				result = KErrNone;
				}
			}
        }
        
    #ifdef __CFLOG_ACTIVE
	    if(result != KErrNone)
	    	{
	 		__CFLOG_2(KLogSubSysRS, KLogCode, _L8("CRootServerSession::CancelUnloadCPM - error %d trying to cancel unload of module %S"), result, &name);
	 	  	}
	#endif

    return result;
	}

TInt CRootServerSession::UnloadCPML(const RMessage2& aMessage)
    /** Unload a cpm.
     If the cpm requested to unload is found, then
	 the bindmgr is called to shutdown the module
     @internalComponent
     @param aMessage The message which will be completed.
     @return TInt - whether the function was successful or not.
     */
    {
	CCommsProviderModule* pModule = NULL;

    TCFModuleNameF name;
    aMessage.ReadL(0, name);
	TRSUnLoadType unloadType = (TRSUnLoadType) aMessage.Int1();
	
	#if defined _DEBUG || defined __CFLOG_ACTIVE || defined SYMBIAN_TRACE_ENABLE
		TPtrC8 unloadTypePtr = CCommsProviderModule::GetUnloadTypeName(unloadType);

		__CFLOG_2(KLogSubSysRS, KLogCode, _L8("CRootServerSession - UnloadCPML: %S, unload type %S"), &name, &unloadTypePtr);

		// Warn the programmer that forced module unloads are unsafe.
		if((unloadType == EImmediate) || (unloadType == EUnGraceful))
			{
			_LIT8(KForcedModuleUnloadWarning, "WARNING: An %S unload of the %S module has been initiated.  This may cause panics or memory leaks/corruption, especially if any sessions are still open. Any error reported from now until server shutdown may be spurious.");
			
			#ifdef _DEBUG
				RDebug::Printf(reinterpret_cast<const char *>(TPtrC8(KForcedModuleUnloadWarning).Ptr()), &unloadTypePtr, &name);
			#endif
			
			// Log the warning to UTrace if available.
			#ifdef SYMBIAN_TRACE_ENABLE
			    enum
			        {
			        KPrimaryFilter = 194, // server den
			        KMaxLogTextLength = 250
			        };

			    class TLogIgnoreOverflow8 : public TDes8Overflow
			        {
			        public:
			            void Overflow(TDes8& /*aDes*/) { }
			        };

				// Format the log text into a buffer for UTrace.
			    TBuf8<KMaxLogTextLength> buf;
		    	TLogIgnoreOverflow8 overflowHandler;
			    buf.AppendFormat(KForcedModuleUnloadWarning, &overflowHandler, &unloadTypePtr, &name);

				UTracePfAny(KPrimaryFilter, KText, ETrue, EFalse, buf.Length(), buf.Ptr(), buf.Length());
			#endif

			__CFLOG_2(KLogSubSysRS, KLogCode, KForcedModuleUnloadWarning, &unloadTypePtr, &name);
			}
	#endif

	pModule = iRootServer->FindCpm(name);
	if( !pModule )
		{
		__CFLOG(KLogSubSysRS, KLogCode, _L8("Couldn't find cpm"));
		return KErrRSModuleNotLoaded;
		}

	if(pModule->IsSticky())
		{
		return KErrLocked;
		}

	CSessionHandlerUnLoadCPM* pHandler =
			CSessionHandlerUnLoadCPM::NewL(this, iRootServer->BindManager(), aMessage);
	RegisterHandler(*pHandler);
	pHandler->SetHandler(pModule->GetName(), unloadType);

	return pHandler->Start();
    }

TInt CRootServerSession::CancelBind(const RMessage2& aMessage)
    /** Cancel a bind request.
     Attempt to cancel a pending bind request between two modules. Simply
	 looks for a match in the list of pending session handler requests.
     @internalComponent
     @param aMessage The message which will be completed.
     @return TInt - whether the function was successful or not.
     */
	{
	__CFLOG(KLogSubSysRS, KLogCode, _L8("CRootSession - CancelBind"));

	TRSSubModuleAddress binfo;
	TInt result = aMessage.Read(0, binfo);
    if(result!=KErrNone)
    	{
        return result;
        }

	TCFSubModuleAddress moduleFrom = binfo.iAddress;

	result = aMessage.Read(1, binfo);
    if(result!=KErrNone)
	    {
        return result;
        }

	TCFSubModuleAddress moduleTo = binfo.iAddress;

	if(moduleFrom.Module().Length()>KCFMaxModuleName)
		{
		return KErrBadName;
		}

	if(moduleTo.Module().Length()>KCFMaxModuleName)
		{
		return KErrBadName;
		}

	TDblQueIter<CSessionHandler> handlerIter(iActiveHandlers);
    CSessionHandler* handler;
	result = KErrNotFound;
    while(result != KErrNone && (handler = handlerIter++) != NULL)
        {
		if(handler->HandlerType() == CSessionHandler::ESHTypeBinding)
			{
			CSessionHandlerBinding* bindingHandler = static_cast<CSessionHandlerBinding*>(handler);
			if(bindingHandler->MatchPending(moduleFrom, moduleTo))
				{
				// Complete with KErrCancel
				bindingHandler->Cancel();
				result = KErrNone;
				}
			}
        }
    return result;
	}

TInt CRootServerSession::BindL(const RMessage2& aMessage)
    /** Bind two submodules.
     Binds two modules together by prompting the binding manager in the right way.
	 If the top module is a server, then the bind type must be a hierarchical bind!
     @internalComponent
     @param aMessage The message which will be completed.
     @return TInt - whether the function was successful or not.
     */
    {

	TRSBindingInfo params;
	aMessage.ReadL(0, params);
 
	TCFSubModuleAddress moduleFrom(params.iParams.iAddress1);
	TCFSubModuleAddress moduleTo(params.iParams.iAddress2);
	TRSBindType bindType = params.iParams.iType;

    __CFLOG_3(KLogSubSysRS, KLogCode,_L8("CRootServerSession - BindL: binding %S with %S, bind type %d"), &moduleFrom, &moduleTo, bindType);

	if(moduleFrom.Module().Length() > KCFMaxModuleName)
		{
	    __CFLOG(KLogSubSysRS, KLogCode,_L8("CRootServerSession - BindL KErrBadName"));
		return KErrBadName;
		}

	if(moduleFrom.SubModule().Length() > KCFMaxSubModuleName)
		{
	    __CFLOG(KLogSubSysRS, KLogCode,_L8("CRootServerSession - BindL KErrBadName"));
		return KErrBadName;
		}

	if(moduleTo.Module().Length() > KCFMaxModuleName)
		{
	    __CFLOG(KLogSubSysRS, KLogCode,_L8("CRootServerSession - BindL KErrBadName"));
		return KErrBadName;
		}

	if(moduleTo.SubModule().Length() > KCFMaxSubModuleName)
		{
	    __CFLOG(KLogSubSysRS, KLogCode,_L8("CRootServerSession - BindL KErrBadName"));
		return KErrBadName;
		}

	if(params.iParams.iForwardQLength < TRSBindingInfo::EMinQueueLength ||
	   params.iParams.iReverseQLength < TRSBindingInfo::EMinQueueLength ||
	   params.iParams.iForwardQLength > TRSBindingInfo::EMaxQueueLength ||
	   params.iParams.iReverseQLength > TRSBindingInfo::EMaxQueueLength)
		{
	    __CFLOG(KLogSubSysRS, KLogCode,_L8("CRootServerSession - BindL KErrRSInvalidQueueLength"));
		return KErrRSInvalidQueueLength;
		}

   __CFLOG_4(KLogSubSysRS, KLogCode,_L8("CRootServerSession. Binding %S with %S. Queues[%d,%d]"), &moduleFrom, &moduleTo, params.iParams.iForwardQLength, params.iParams.iReverseQLength);

	// do checking here
	CCommsProviderModule* pModuleFrom = iRootServer->FindCpm(moduleFrom.Module());
	CCommsProviderModule* pModuleTo = iRootServer->FindCpm(moduleTo.Module());
	if( !pModuleFrom )
		{
		return KErrRSModuleNotLoaded;
		}
	if( !pModuleTo )
		{
		return KErrRSModuleNotLoaded;
		}
	if(bindType == EHierarchical && pModuleTo->IsServer())
		{
		return KErrRSInvalidBinding;
		}

	CSessionHandlerBinding* pHandler =
			CSessionHandlerBinding::NewL(this, iRootServer->BindManager(), aMessage);
	RegisterHandler( *pHandler );

	pHandler->SetHandler(moduleFrom, moduleTo, bindType,
	  					 params.iParams.iForwardQLength,
						 params.iParams.iReverseQLength);

    return pHandler->Start();
    }

TInt CRootServerSession::CancelUnbind(const RMessage2& aMessage)
    /** Cancel an unbind request.
     Attempt to cancel a previous request to unbind two submodules.
	 Again tries to match a pending session handler request.
     @internalComponent
     @param aMessage The message which will be completed.
     @return Whether the function was successful or not.
     */
	{

	__CFLOG(KLogSubSysRS, KLogCode, _L8("CRootServerSession - CancelUnbind"));

	TRSSubModuleAddress binfo;
	TInt result = aMessage.Read(0, binfo);
    if(result!=KErrNone)
    	{
        return result;
        }

	TCFSubModuleAddress moduleFrom = binfo.iAddress;
	result = aMessage.Read(1, binfo);
    if(result!=KErrNone)
    	{
        return result;
        }

	TCFSubModuleAddress moduleTo = binfo.iAddress;

	if(moduleFrom.Module().Length()>KCFMaxModuleName)
		{
		return KErrBadName;
		}

	if(moduleTo.Module().Length()>KCFMaxModuleName)
		{
		return KErrBadName;
		}

	TDblQueIter<CSessionHandler> handlerIter(iActiveHandlers);
    CSessionHandler* handler;
	result = KErrNotFound;
    while(result != KErrNone && (handler = handlerIter++) != NULL)
        {
		if(handler->HandlerType() == CSessionHandler::ESHTypeUnBinding)
			{
			CSessionHandlerUnBinding* unbindingHandler = static_cast<CSessionHandlerUnBinding*>(handler);
			if(unbindingHandler->MatchPending(moduleFrom, moduleTo))
				{
				// Complete with KErrCancel
				unbindingHandler->Cancel();
				result = KErrNone;
				}
			}
        }

    return result;
	}

TInt CRootServerSession::UnbindL(const RMessage2& aMessage)
    /** Unbind two submodules.
     Unbinds two modules by asking the bind manager to deal with request.
	 Uses rootserver findProvider() and findServer() functions to check if requested modules are loaded
     @internalComponent
     @param aMessage The message which will be completed.
     @return Whether the function was successful or not.
     */
    {
    __CFLOG(KLogSubSysRS, KLogCode,_L8("CRootServerSession - Unbind"));

	TRSUnBindingInfo params;
	aMessage.ReadL( 0, params );

	TCFSubModuleAddress moduleFrom = params.iParams.iAddress1;
	TCFSubModuleAddress moduleTo = params.iParams.iAddress2;

	if(moduleFrom.Module().Length()>KCFMaxModuleName)
		{
		return KErrBadName;
		}

	if(moduleTo.Module().Length()>KCFMaxModuleName)
		{
		return KErrBadName;
		}

	// do checking here
	CCommsProviderModule* pModuleFrom = iRootServer->FindCpm(moduleFrom.Module());
	CCommsProviderModule* pModuleTo = iRootServer->FindCpm(moduleTo.Module());

	if((!pModuleFrom) || (!pModuleTo))
		{
		return KErrRSModuleNotLoaded;
		}

	CSessionHandlerUnBinding* pHandler =
			CSessionHandlerUnBinding::NewL(this, iRootServer->BindManager(), aMessage);
	RegisterHandler(*pHandler);
	pHandler->SetHandler(moduleFrom, moduleTo);

    return pHandler->Start();
    }

TInt CRootServerSession::EnumerateBindings(const RMessage2& aMessage)
    /** Get binding info for a module.
     We fill a descriptor with bindInfo objects
     @internalComponent
     @param aMessage The message to be filled with bindInfo.
     @return Whether the function was successful or not.
     */

    {
	TRSSubModuleAddress moduleAddr;
	TInt result = aMessage.Read(0, moduleAddr);
	if(KErrNone == result)
		{
		TPckgBuf<TRSIter> ipcPosition;
		result = aMessage.Read(1, ipcPosition);
		if(KErrNone == result)
			{
			TInt& position = ipcPosition()();
			// Locate the right one
			CBindManager* const bindMgr = iRootServer->BindManager();
			CBindManager::TBindingInfo info;
			result = bindMgr->EnumerateBindings(moduleAddr.iAddress, ETrue, info);
			for(TInt cursor = 0; (cursor < position) && (KErrNone == result); cursor++)
				{
				result = bindMgr->EnumerateBindings(moduleAddr.iAddress, EFalse, info);
				}
			if(KErrNone == result)
       			{
				// Got one, copy the information to the client
				TRSBindingInfo bindInfo;
				bindInfo.iParams.iType = info.iType;
				bindInfo.iParams.iAddress1 = info.iSubModuleAddr1;
				bindInfo.iParams.iAddress2 = info.iSubModuleAddr2;
				bindInfo.iParams.iState1 = info.iSubModuleState1;
				bindInfo.iParams.iState2 = info.iSubModuleState2;
				result = aMessage.Write(2, bindInfo);
				if(KErrNone == result)
       				{
					++position;
					result = aMessage.Write(1, ipcPosition);
					}
				}
			}
		}
	return result;
	}


TInt CRootServerSession::GetModuleInfo(const RMessage2& aMessage)
    /** Get info for a module.
     Fills a descriptor with information regarding the
	 module specified in the message
     @internalComponent
     @param aMessage The message to be filled with moduleInfo.
     @return Whether the function was successful or not.
     */
    {
    TCFModuleNameF name;
    TRSModuleInfo info;
    TInt result = aMessage.Read(0, name);
    if(result!=KErrNone)
        {
        return result;
        }

	if(name.Length()>KCFMaxModuleName)
		{
		return KErrBadName;
		}

    result = iRootServer->GetModuleInfo(name, info);

    if(result!=KErrNone)
        {
        return result;
        }
  	result = aMessage.Write(1, info);
    return result;
    }

TInt CRootServerSession::EnumerateModules(const RMessage2& aMessage)
    /** Enumerate the rootserver.
     Finds loaded modules in the root server and fills a descriptor
	 list in the message
     @internalComponent
     @param aMessage The client message.
     @return Whether the function was successful or not.
	 */
    {
	TPckgBuf<TRSIter> ipcPosition;
	TInt result = aMessage.Read(0, ipcPosition);
	if(KErrNone == result)
		{
		TInt& position = ipcPosition()();

		TCFModuleNameF moduleName;
		result = iRootServer->EnumerateModules(position, moduleName);
		if(KErrNone == result)
       		{
			result = aMessage.Write(1, moduleName);
			if(KErrNone == result)
       			{
				++position;
				result = aMessage.Write(0, ipcPosition);
				}
			}
		}
    return result;
    }

TInt CRootServerSession::EnumerateSubModules(const RMessage2& aMessage)
    /** Enumerate the rootserver.
     Finds loaded modules in the root server and fills a descriptor
	 list in the message
     @internalComponent
     @param aMessage The message to be filled with list of modules found.
     @return Whether the function was successful or not.
     */
    {
	TCFModuleNameF moduleName;
	TInt result = aMessage.Read(0, moduleName);
	if(KErrNone == result)
		{
		if(!iRootServer->FindCpm(moduleName))
			{
 			return KErrNotFound;
 			}
		TPckgBuf<TRSIter> ipcPosition;
		result = aMessage.Read(1, ipcPosition);
		if(KErrNone == result)
			{
			TInt& position = ipcPosition()();
		    TCFSubModuleNameF subModuleName;
			result = iRootServer->EnumerateSubModules(moduleName, position, subModuleName);
			if(KErrNone == result)
       			{
				result = aMessage.Write(2, subModuleName);
				if(KErrNone == result)
       				{
					++position;
					result = aMessage.Write(1, ipcPosition);
					}
				}
			}
		}
	return result;
	}

EXPORT_C void CRootServerSession::ServiceL(const RMessage2& aMessage)
	/** Messages are handled here.
	 @internalComponent
	 */
    {

    TInt result = KErrGeneral;
    TBool completeNow = ETrue;
    switch ((aMessage.Function()))
		{
		case RSIsCallerConfigurator:
			result = IsCallerConfigurator(aMessage);
			break;

		case RSLoadModule:
			TRAP(result, result = LoadCPML(aMessage));
			
			#ifdef __CFLOG_ACTIVE
				if( result != KErrNone )
					{
					__CFLOG_1(KLogSubSysRS, KLogCode, _L8("CRootServerSession::ServiceL: RSLoadModule - error %d starting to load CPM"), result);
					}
			#endif
			
			completeNow = (result != KErrNone);
			break;
		case RSUnloadModule:
			TRAP(result, result = UnloadCPML(aMessage));
			
			#ifdef __CFLOG_ACTIVE
				if( result != KErrNone )
					{
					__CFLOG_1(KLogSubSysRS, KLogCode, _L8("CRootServerSession::ServiceL: RSUnloadModule - error %d starting to unload CPM"), result);
					}
			#endif
			
			completeNow = (result != KErrNone);
			break;
		case RSGetModuleInfo:
			result = GetModuleInfo(aMessage);
			break;
		case RSEnumerateModules:
			result = EnumerateModules(aMessage);
			break;
		case RSEnumerateSubModules:
			result = EnumerateSubModules(aMessage);
			break;

		case RSCancelLoadModule:
			result = CancelLoadCPM(aMessage);
			break;
		case RSCancelUnloadModule:
			result = CancelUnloadCPM(aMessage);
			break;
		case RSCloseSession:
			CloseSession();
			result = KErrNone;
			break;
		case RSShutdown:
			result = Shutdown();
			break;
		case RSSendMessage:
			result = Forward( aMessage );
			completeNow = (result != KErrNone);
			break;
		case RSBind:
			TRAP(result, result = BindL(aMessage));
			
			#ifdef __CFLOG_ACTIVE
				if( result != KErrNone )
					{
					__CFLOG_1(KLogSubSysRS, KLogCode, _L8("CRootServerSession::ServiceL: RSBind - error %d starting to bind modules"), result);
					}
			#endif
			
			completeNow = (result != KErrNone);
			break;
		case RSUnbind:
			TRAP(result, result = UnbindL(aMessage));
			
			#ifdef __CFLOG_ACTIVE
				if( result != KErrNone )
					{
					__CFLOG_1(KLogSubSysRS, KLogCode, _L8("CRootServerSession::ServiceL: RSUnbind - error %d starting to unbind modules"), result);
					}
			#endif
			
			completeNow = (result != KErrNone);
			break;
		case RSEnumerateBindings:
			result = EnumerateBindings(aMessage);
			break;
		case RSCancelBind:
			result = CancelBind(aMessage);
			break;
		case RSCancelUnbind:
			result = CancelUnbind(aMessage);
			break;

			// Server debug messages
#if defined (_DEBUG_ROOTSERVER_FUNCTIONS)	// {
		case RSDbgMarkHeap:
			result = KErrNone;
			__UHEAP_MARK;
			break;
		case RSDbgCheckHeap:
			result = KErrNone;
			__UHEAP_CHECK(aMessage.Int0());
			break;
		case RSDbgMarkEnd:
			result = KErrNone;
			__UHEAP_MARKENDC(aMessage.Int0());
			break;
		case RSDbgFailNext:
			result = KErrNone;
			if(aMessage.Int0() >= 0)
				{
				__UHEAP_FAILNEXT(aMessage.Int0());
				}
			else
				{
				__UHEAP_RESET;
				}
			break;
#endif		// DEBUG }
		default:
			result = KErrNotSupported;
			break;
		}

    if(completeNow)
        {
        aMessage.Complete(result);
        }
    }

EXPORT_C void CRootServerSession::SuddenDeath(const TInt aError)
	/** Handles Sudden Death completion events
	 @internalComponent
	 */
    {
    if(iSuddenDeathListener)
        {
        iSuddenDeathMessage.Complete(aError);
        iSuddenDeathListener = EFalse;
        }
    };

void CRootServerSession::RegisterHandler(CSessionHandler& aHandler)
    /** Register session handlers.
     @internalComponent
     @param aHandler Reference to handler to be registered to allow deletion
     */
	{
	__CFLOG( KLogSubSysRS, KLogCode,_L8("CRootServerSession - Registering handler"));
	iActiveHandlers.AddLast(aHandler);
    }

EXPORT_C CRootServerSession::~CRootServerSession()
	/** RootserverSession destructor.
     @internalComponent
     */
    {
	TDblQueIter<CSessionHandler> handlerIter(iActiveHandlers);
    CSessionHandler* pHandler;

    // cancel handlers without completing their RMessages
    while((pHandler = handlerIter++)!=NULL)
		{
		if(pHandler->CancelHandler(EFalse, ETrue))
			{
			delete pHandler;
			}
        }
    }

void CRootServerSession::CloseSession()
    /** Close session to rootserver.
     @internalComponent
     */
	{
	// cancel all the client's handlers before we die
    __CFLOG(KLogSubSysRS, KLogCode, _L8("CRootServerSession::CloseSession"));

	TDblQueIter<CSessionHandler> handlerIter(iActiveHandlers);
    CSessionHandler* pHandler;

    // release any pending messages
    while((pHandler = handlerIter++)!=NULL)
		{
		__CFLOG_1(KLogSubSysRS, KLogCode,
			_L8("CRootServerSession::CloseSession cancelling handle %X"), pHandler);
		if(pHandler->CancelHandler(ETrue, ETrue))
			{
			delete pHandler;
			}
		}
	}

TInt CRootServerSession::Shutdown()
	/** Shutdown root server
	 If there are any modules running, it creates a new handler
	 for each one, which unload their module.  When there are no
	 more modules left running, the root server is shutdown
	 @internalComponent
	 */
	{
	__CFLOG(KLogSubSysRS, KLogCode, _L8("CRootServerSession::Shutdown"));
	return iRootServer->Shutdown();
	}

TInt CRootServerSession::Forward(const RMessage2& aMessage)
	/** Forwards a message to a module
	 @internalComponent
	 */
	{
	__CFLOG(KLogSubSysRS, KLogCode, _L8("CRootServerSession::Forward"));
	TCFModuleNameF name;
	TInt result = aMessage.Read(0, name);
	if(result!=KErrNone)
		{
		return result;
		}

   return iRootServer->Forward(name, aMessage);
	}

CRootServer* CRootServerSession::RootServer()
	{
	return iRootServer;
	}

TInt CRootServerSession::IsCallerConfigurator(const RMessage2& aMessage)
	{
	return (TInt) iRootServer->IsCallerConfigurator(this, aMessage);
	}

EXPORT_C void CRootServerSession::Disconnect(const RMessage2& aMessage)
	{
	iRootServer->DisconnectSession(this);
	CSession2::Disconnect(aMessage);
	}

