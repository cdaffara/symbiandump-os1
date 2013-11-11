// session.cpp
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "server.h"
#include "pipe.h"
#include "console.h"
#include "file.h"
#include "null.h"
#include "persistentconsole.h"
#include "readwrite.h"
#include "clientserver.h"
#include "session.h"


CIoSession::CIoSession()
	{
	}

CIoServer& CIoSession::Server()
	{
#ifdef EKA2
	return *static_cast<CIoServer*>(const_cast<CServer2*>(CSession2::Server()));
#else
	return *static_cast<CIoServer*>(const_cast<CServer*>(CSharableSession::Server()));
#endif
	}

#ifdef EKA2
void CIoSession::CreateL()
#else
void CIoSession::CreateL(const CServer& aServer)
#endif
	{
#ifndef EKA2
	CSharableSession::CreateL(aServer);	// does not leave
#endif
	iHandles = CObjectIx::NewL();
	Server().AddSession();
	}

CIoSession::~CIoSession()
	{
	delete iFindByNameMatch;
	const TInt maxNumObjects = iHandles->Count();
	for (TInt i = 0; i < maxNumObjects; ++i)
		{
		CIoObject* obj = static_cast<CIoObject*>((*iHandles)[i]);
		if (obj != NULL)
			{
			obj->SessionClosed(*this);
			}
		}
	Server().DropSession();
	delete iHandles;
	LOG(CIoLog::Printf(_L("CIoSession 0x%08x destroyed\r\n"), this));
	}

void CIoSession::ServiceL(const RMsg& aMessage)
	{
	LOG(CIoLog::StartServiceLC(aMessage));

	switch (aMessage.Function())
		{
		case EIoHandleSetOwner:
			{
			TThreadId owningThread(aMessage.Int0());
			FindReadWriteObjectL(aMessage.Int3(), aMessage).SetOwnerL(owningThread);
			Complete(aMessage, KErrNone);
			break;
			}
		case EIoHandleSetUnderlyingConsole:
			{
			FindReadWriteObjectL(aMessage.Int3(), aMessage).SetConsole(FindConsoleL(aMessage.Int0(), aMessage));
			Complete(aMessage, KErrNone);
			break;
			}
		case EIoHandleAttachedToConsole:
			{
			CIoReadWriteObject& obj = FindReadWriteObjectL(aMessage.Int3(), aMessage);
			if (obj.IsType(RIoHandle::EReadObject))
				{
				Complete(aMessage, MIoReadEndPoint::AttachedToConsole((MIoReadEndPoint*)obj.EndPoint()));
				}
			else
				{
				Complete(aMessage, MIoWriteEndPoint::AttachedToConsole((MIoWriteEndPoint*)obj.EndPoint()));
				}
			break;
			}
		case EIoCreateReader:
			{
			CIoReadObject& readObj = Server().CreateReadObjLC();
			LOG(CIoLog::Printf(_L("id: %d"), readObj.Id()));
			CreateHandleL(readObj, ETrue, aMessage);
			break;
			}
		case EIoOpenReaderByThreadId:
			{
			TThreadId clientThreadId(ClientThreadIdL(aMessage));
			CIoReadObject* readObj = Server().NextReadObj(clientThreadId);
			if (readObj == NULL)
				{
				Complete(aMessage, KErrNotFound);
				}
			else
				{
#ifdef EKA2
				LOG(CIoLog::Printf(_L("thread id: %Lu"), clientThreadId.Id()));
#else
				LOG(CIoLog::Printf(_L("thread id: %u"), clientThreadId));
#endif
				OBJ_NAME(*readObj);
				LOG(CIoLog::Printf(_L("name: \"%S\""), &objName));
				User::LeaveIfError(readObj->Open(clientThreadId));
				CleanupClosePushL(*readObj);
				CreateHandleL(*readObj, EFalse, aMessage);
				CleanupStack::Pop();
				}
			break;
			}
		case EIoOpenReaderByExplicitThreadId:
			{
			TThreadId clientThreadId;
			TPckg<TThreadId> clientThreadIdPckg(clientThreadId);
			MessageReadL(aMessage, 0, clientThreadIdPckg);
			CIoReadObject* readObj = Server().NextReadObj(clientThreadId);
			if (readObj == NULL)
				{
				Complete(aMessage, KErrNotFound);
				}
			else
				{
#ifdef EKA2
				LOG(CIoLog::Printf(_L("thread id: %Lu"), clientThreadId.Id()));
#else
				LOG(CIoLog::Printf(_L("thread id: %u"), clientThreadId));
#endif
				OBJ_NAME(*readObj);
				LOG(CIoLog::Printf(_L("name: \"%S\""), &objName));
				User::LeaveIfError(readObj->Open(clientThreadId));
				CleanupClosePushL(*readObj);
				CreateHandleL(*readObj, EFalse, aMessage);
				CleanupStack::Pop();
				}
			break;
			}
		case EIoDuplicateReader:
			{
			CIoReadObject& readObj = FindReadObjectL(aMessage.Int3(), aMessage);
			CIoReadObject& duplicateReadObj = FindReadObjectL(aMessage.Int0(), aMessage);
			readObj.DuplicateL(duplicateReadObj);
			LOG(CIoLog::Printf(_L("Duplicating read object 0x%08x (id: %d) from 0x%08x (id: %d)"), &readObj, readObj.Id(), &duplicateReadObj, duplicateReadObj.Id()));
			Complete(aMessage, KErrNone);
			break;
			}
		case EIoDuplicateReaderHandleFromThread:
			{
			TThreadId threadId(aMessage.Int0());
			CIoReadObject& readObj = FindReadObjectL(aMessage.Int3(), aMessage);
			CIoReadObject* otherThreadObj = Server().LastOpenedReadObj(threadId);
			if (otherThreadObj)
				{
				LOG(CIoLog::Printf(_L("Duplicating read object 0x%08x (id: %d) from 0x%08x (id: %d)"), &readObj, readObj.Id(), otherThreadObj, otherThreadObj->Id()));
				readObj.DuplicateL(*otherThreadObj);
				}
			else
				{
				User::Leave(KErrNotFound);
				}
			Complete(aMessage, KErrNone);
			break;
			}
		case EIoCreateWriter:
			{
			CIoWriteObject& writeObj = Server().CreateWriteObjLC();
			LOG(CIoLog::Printf(_L("id: %d"), writeObj.Id()));
			CreateHandleL(writeObj, ETrue, aMessage);
			break;
			}
		case EIoOpenWriterByThreadId:
			{
			TThreadId clientThreadId(ClientThreadIdL(aMessage));
			CIoWriteObject* writeObj = Server().NextWriteObj(clientThreadId);
			if (writeObj == NULL)
				{
				Complete(aMessage, KErrNotFound);
				}
			else
				{
				LOG(CIoLog::Printf(_L("thread id: %u"), TUint(clientThreadId)));
				OBJ_NAME(*writeObj);
				LOG(CIoLog::Printf(_L("name: \"%S\""), &objName));
				User::LeaveIfError(writeObj->Open(clientThreadId));
				CleanupClosePushL(*writeObj);				
				CreateHandleL(*writeObj, EFalse, aMessage);
				CleanupStack::Pop();
				}
			break;
			}
		case EIoOpenWriterByExplicitThreadId:
			{
			TThreadId clientThreadId;
			TPckg<TThreadId> clientThreadIdPckg(clientThreadId);
			MessageReadL(aMessage, 0, clientThreadIdPckg);
			CIoWriteObject* writeObj = Server().NextWriteObj(clientThreadId);
			if (writeObj == NULL)
				{
				Complete(aMessage, KErrNotFound);
				}
			else
				{
				LOG(CIoLog::Printf(_L("thread id: %u"), TUint(clientThreadId)));
				OBJ_NAME(*writeObj);
				LOG(CIoLog::Printf(_L("name: \"%S\""), &objName));
				User::LeaveIfError(writeObj->Open(clientThreadId));
				CleanupClosePushL(*writeObj);				
				CreateHandleL(*writeObj, EFalse, aMessage);
				CleanupStack::Pop();
				}
			break;
			}
		case EIoDuplicateWriter:
			{
			CIoWriteObject& writeObj = FindWriteObjectL(aMessage.Int3(), aMessage);
			CIoWriteObject& duplicateWriteObj = FindWriteObjectL(aMessage.Int0(), aMessage);
			writeObj.DuplicateL(duplicateWriteObj);
			LOG(CIoLog::Printf(_L("Duplicating write object 0x%08x (id: %d) from 0x%08x (id: %d)"), &writeObj, writeObj.Id(), &duplicateWriteObj, duplicateWriteObj.Id()));
			Complete(aMessage, KErrNone);
			break;
			}
		case EIoDuplicateWriterHandleFromThread:
			{
			TThreadId threadId(aMessage.Int0());
			CIoWriteObject& writeObj = FindWriteObjectL(aMessage.Int3(), aMessage);
			CIoWriteObject* otherThreadObj = Server().LastOpenedWriteObj(threadId);
			if (otherThreadObj)
				{
				LOG(CIoLog::Printf(_L("Duplicating write object 0x%08x (id: %d) from 0x%08x (id: %d)"), &writeObj, writeObj.Id(), otherThreadObj, otherThreadObj->Id()));
				writeObj.DuplicateL(*otherThreadObj);
				}
			else
				{
				User::Leave(KErrNotFound);
				}
			Complete(aMessage, KErrNone);
			break;
			}
		case EIoSetReadWriteMode:
			{
			FindReadWriteObjectL(aMessage.Int3(), aMessage).SetModeL(aMessage);
			break;
			}
		case EIoSetReadMode:
			{
			TInt mode(aMessage.Int0());
			if ((mode >= RIoReadHandle::EFull) && (mode <= RIoReadHandle::EOneOrMore))
				{
				FindReadObjectL(aMessage.Int3(), aMessage).SetReadMode((RIoReadHandle::TReadMode)mode);
				Complete(aMessage, KErrNone);
				}
			else
				{
				Complete(aMessage, KErrNotSupported);
				}
			break;
			}
		case EIoSetReaderToForeground:
			{
			FindReadObjectL(aMessage.Int3(), aMessage).SetToForegroundL();
			Complete(aMessage, KErrNone);
			break;
			}
		case EIoRead:
			{
			FindReadObjectL(aMessage.Int3(), aMessage).ReadL(aMessage);
			break;
			}
		case EIoReadCancel:
			{
			FindReadObjectL(aMessage.Int3(), aMessage).ReadCancel(*this);
			Complete(aMessage, KErrNone);
			break;
			}
		case EIoSetLineSeparator:
			{
			FindReadObjectL(aMessage.Int3(), aMessage).SetLineSeparatorL(aMessage);
			break;
			}
		case EIoWrite:
			{
			FindWriteObjectL(aMessage.Int3(), aMessage).WriteL(aMessage);
			break;
			}
		case EIoWriteCancel:
			{
			FindWriteObjectL(aMessage.Int3(), aMessage).WriteCancel(*this);
			Complete(aMessage, KErrNone);
			break;
			}
		case EIoIsForegroundReader:
			{
			Complete(aMessage, FindReadObjectL(aMessage.Int3(), aMessage).IsForegroundL());
			break;
			}
		case EIoConsoleWaitForKey:
			{
			FindReadObjectL(aMessage.Int3(), aMessage).ReadKeyL(aMessage);
			break;
			}
		case EIoConsoleWaitForKeyCancel:
			{
			FindReadObjectL(aMessage.Int3(), aMessage).ReadKeyCancel(*this);
			Complete(aMessage, KErrNone);
			break;
			}
		case EIoConsoleCaptureKey:
			{
			FindReadObjectL(aMessage.Int3(), aMessage).CaptureKeyL(aMessage);
			break;
			}
		case EIoConsoleCancelCaptureKey:
			{
			FindReadObjectL(aMessage.Int3(), aMessage).CancelCaptureKey(aMessage);
			break;
			}
		case EIoConsoleCaptureAllKeys:
			{
			FindReadObjectL(aMessage.Int3(), aMessage).CaptureAllKeys(aMessage);
			break;
			}
		case EIoConsoleCancelCaptureAllKeys:
			{
			FindReadObjectL(aMessage.Int3(), aMessage).CancelCaptureAllKeys(aMessage);
			break;
			}
		case EIoConsoleCursorPos:
			{
			FindWriteObjectL(aMessage.Int3(), aMessage).CursorPosL(aMessage);
			break;
			}
		case EIoConsoleSetCursorPosAbs:
			{
			FindWriteObjectL(aMessage.Int3(), aMessage).SetCursorPosAbsL(aMessage);
			break;
			}
		case EIoConsoleSetCursorPosRel:
			{
			FindWriteObjectL(aMessage.Int3(), aMessage).SetCursorPosRelL(aMessage);
			break;
			}
		case EIoConsoleSetCursorHeight:
			{
			FindWriteObjectL(aMessage.Int3(), aMessage).SetCursorHeightL(aMessage);
			break;
			}
		case EIoConsoleSetTitle:
			{
			FindWriteObjectL(aMessage.Int3(), aMessage).SetTitleL(aMessage);
			break;
			}
		case EIoConsoleClearScreen:
			{
			FindWriteObjectL(aMessage.Int3(), aMessage).ClearScreen(aMessage);
			break;
			}
		case EIoConsoleClearToEndOfLine:
			{
			FindWriteObjectL(aMessage.Int3(), aMessage).ClearToEndOfLine(aMessage);
			break;
			}
		case EIoConsoleScreenSize:
			{
			FindWriteObjectL(aMessage.Int3(), aMessage).ScreenSizeL(aMessage);
			break;
			}
		case EIoConsoleSetAttributes:
			{
			FindWriteObjectL(aMessage.Int3(), aMessage).SetAttributesL(aMessage);
			break;
			}
		case EIoCreatePipe:
			{
			CIoPipe& pipe = Server().CreatePipeLC();
			CreateHandleL(pipe, ETrue, aMessage);
			break;
			}
		case EIoHandleClose:
			{
			const TInt handle = aMessage.Int3();
			LOG(CIoLog::Printf(_L("handle: %d"), handle));
			CIoObject* obj = (CIoObject*)iHandles->AtL(handle); // Leaves with KErrBadHandle if not found, which will result in client getting panic'd.
			obj->ClosedBy(*this);
			iHandles->Remove(handle);
			Complete(aMessage, KErrNone);
			break;
			}
		case EIoEndPointAttachReader:
			{
			CIoEndPoint& endPoint = FindEndPointL(aMessage.Int3(), aMessage);
			CIoReadObject& readObj = FindReadObjectL(aMessage.Int0(), aMessage);
			RIoEndPoint::TReadMode mode = (RIoEndPoint::TReadMode)aMessage.Int1();
			LOG(CIoLog::Printf(_L("read obj id:      %d, address: 0x%08x"), readObj.Id(), &readObj));
			LOG(CIoLog::Printf(_L("end point handle: %d, address: 0x%08x"), aMessage.Int3(), &endPoint));
			readObj.AttachL(endPoint, mode);
			Complete(aMessage, KErrNone);
			break;
			}
		case EIoEndPointAttachWriter:
			{
			CIoEndPoint& endPoint = FindEndPointL(aMessage.Int3(), aMessage);
			CIoWriteObject& writeObj = FindWriteObjectL(aMessage.Int0(), aMessage);
			LOG(CIoLog::Printf(_L("write obj id:     %d, address: 0x%08x"), writeObj.Id(), &writeObj));
			LOG(CIoLog::Printf(_L("end point handle: %d, address: 0x%08x"), aMessage.Int3(), &endPoint));
			writeObj.AttachL(endPoint);
			Complete(aMessage, KErrNone);
			break;
			}
		case EIoEndPointSetForegroundReadHandle:
			{
			CIoEndPoint& endPoint = FindEndPointL(aMessage.Int3(), aMessage);
			CIoReadObject& readObj = FindReadObjectL(aMessage.Int0(), aMessage);
			LOG(CIoLog::Printf(_L("read obj id:      %d, address: 0x%08x"), readObj.Id(), &readObj));
			LOG(CIoLog::Printf(_L("end point handle: %d, address: 0x%08x"), aMessage.Int3(), &endPoint));
			endPoint.IorepSetForegroundReaderL(readObj);
			Complete(aMessage, KErrNone);
			break;
			}
		case EIoCreateConsole:
			{
			HBufC* titleBuf = HBufC::NewLC(DesLengthL(aMessage, 0));
			TPtr titlePtr(titleBuf->Des());
			MessageReadL(aMessage, 0, titlePtr);
			TPckgBuf<TConsoleCreateParams> paramsPckg;
			MessageReadL(aMessage, 1, paramsPckg);
			HBufC* implBuf = HBufC::NewLC(DesLengthL(aMessage, 2));
			TPtr implPtr(implBuf->Des());
			MessageReadL(aMessage, 2, implPtr);
			CIoConsole* underlyingCons = NULL;
			if (paramsPckg().iUnderlyingConsoleHandle != KNullHandle)
				{
				underlyingCons = &FindConsoleL(paramsPckg().iUnderlyingConsoleHandle, aMessage);
				}
			CIoConsole& console = Server().CreateConsoleLC(*implBuf, *titleBuf, paramsPckg().iSize, underlyingCons, paramsPckg().iOptions);
			CreateHandleL(console, ETrue, aMessage); //pops console
			CleanupStack::PopAndDestroy(2, titleBuf);
			break;
			}
		case EIoOpenConsole:
			{
			CIoConsole* console = FindReadWriteObjectL(aMessage.Int0(), aMessage).Console();
			if (console)
				{
				User::LeaveIfError(console->Open());
				CleanupClosePushL(*console);
				CreateHandleL(*console, EFalse, aMessage);
				CleanupStack::Pop();
				}
			else
				{
				Complete(aMessage, KErrNotFound);
				}
			break;
			}
		case EIoConsoleImplementation:
			{
			TInt clientDesLen = MaxDesLengthL(aMessage, 0);
			const TDesC& impl(FindConsoleL(aMessage.Int3(), aMessage).Implementation());
			MessageWriteL(aMessage, 0, impl.Left(clientDesLen));
			Complete(aMessage, impl.Length() > clientDesLen ? KErrOverflow : KErrNone);
			break;
			}
		case EIoCreateFile:
			{
			HBufC* fileNameBuf = HBufC::NewLC(DesLengthL(aMessage, 0));
			TPtr fileNamePtr(fileNameBuf->Des());
			MessageReadL(aMessage, 0, fileNamePtr);
			CIoFile& file = Server().CreateFileLC(*fileNameBuf, (RIoFile::TMode)aMessage.Int1());
			CreateHandleL(file, ETrue, aMessage);
			CleanupStack::PopAndDestroy(fileNameBuf);
			break;
			}
		case EIoCreateNull:
			{
			CIoNull& null = Server().CreateNullLC();
			CreateHandleL(null, ETrue, aMessage);
			break;
			}
		case EIoSetObjectName:
			{
			CObject* obj = iHandles->AtL(aMessage.Int0());
			HBufC* name = HBufC::NewLC(DesLengthL(aMessage, 1));
			TPtr namePtr(name->Des());
			MessageReadL(aMessage, 1, namePtr);
			LOG(CIoLog::Printf(_L("Setting object (handle: %d, address: 0x%08x) to \"%S\""), aMessage.Int0(), &obj, name));
			TPtrC truncatedName(name->Left(KMaxName));
			obj->SetName(&truncatedName);
			CleanupStack::PopAndDestroy(name);
			Complete(aMessage, KErrNone);
			break;
			}
		case EIoCreatePersistentConsole:
			{
			HBufC* nameBuf = HBufC::NewLC(DesLengthL(aMessage, 1));
			TPtr namePtr(nameBuf->Des());
			MessageReadL(aMessage, 0, namePtr);
			
			HBufC* titleBuf = HBufC::NewLC(DesLengthL(aMessage, 0));
			TPtr titlePtr(titleBuf->Des());
			MessageReadL(aMessage, 1, titlePtr);

			CIoPersistentConsole& pcons = Server().CreatePersistentConsoleLC(*nameBuf, *titleBuf, aMessage);
			CreateHandleL(pcons, ETrue, aMessage);
			CleanupStack::PopAndDestroy(2, nameBuf);
			break;
			}
		case EIoOpenPersistentConsoleByName:
			{
			HBufC* nameBuf = HBufC::NewLC(DesLengthL(aMessage, 0));
			TPtr namePtr(nameBuf->Des());
			MessageReadL(aMessage, 0, namePtr);
			
			CIoPersistentConsole& pcons = Server().FindPersistentConsoleL(*nameBuf);
			User::LeaveIfError(pcons.Open());
			CleanupClosePushL(pcons);
			CreateHandleL(pcons, EFalse, aMessage);
			CleanupStack::Pop(&pcons);
			CleanupStack::PopAndDestroy(nameBuf);
			break;
			};
		case EIoPersistentConsoleAttachReadEndPoint:
			{
			CIoPersistentConsole& pcons = FindPersistentConsoleL(aMessage.Int3(), aMessage);
			CIoEndPoint& endPoint = FindEndPointL(aMessage.Int0(), aMessage);
			RIoPersistentConsole::TCloseBehaviour onClose = (RIoPersistentConsole::TCloseBehaviour)aMessage.Int1();
			pcons.AttachTransientReaderL(endPoint, onClose == RIoPersistentConsole::EDetachOnHandleClose ? this : NULL);
			Complete(aMessage, KErrNone);
			break;
			}
		case EIoPersistentConsoleAttachWriteEndPoint:
			{
			CIoPersistentConsole& pcons = FindPersistentConsoleL(aMessage.Int3(), aMessage);
			CIoEndPoint& endPoint = FindEndPointL(aMessage.Int0(), aMessage);
			RIoPersistentConsole::TCloseBehaviour onClose = (RIoPersistentConsole::TCloseBehaviour)aMessage.Int1();
			pcons.AttachTransientWriterL(endPoint, onClose == RIoPersistentConsole::EDetachOnHandleClose ? this : NULL);
			Complete(aMessage, KErrNone);
			break;
			}
		case EIoPersistentConsoleDetachReadEndPoint:
			{
			CIoPersistentConsole& pcons = FindPersistentConsoleL(aMessage.Int3(), aMessage);
			pcons.DetachTransientReader();
			Complete(aMessage, KErrNone);
			break;
			}
		case EIoPersistentConsoleDetachWriteEndPoint:
			{
			CIoPersistentConsole& pcons = FindPersistentConsoleL(aMessage.Int3(), aMessage);
			pcons.DetachTransientWriter();
			Complete(aMessage, KErrNone);
			break;
			}
		case EIoPersistentConsoleNotifyReadDetach:
			{
			CIoPersistentConsole& pcons = FindPersistentConsoleL(aMessage.Int3(), aMessage);
			pcons.NotifyReadDetachL(aMessage);
			break;
			}
		case EIoPersistentConsoleNotifyWriteDetach:
			{
			CIoPersistentConsole& pcons = FindPersistentConsoleL(aMessage.Int3(), aMessage);
			pcons.NotifyWriteDetachL(aMessage);
			break;
			}
		case EIoPersistentConsoleCancelNotifyReadDetach:
			{
			CIoPersistentConsole& pcons = FindPersistentConsoleL(aMessage.Int3(), aMessage);
			pcons.CancelNotifyReadDetach((TRequestStatus*)aMessage.Int0());
			break;
			}
		case EIoPersistentConsoleCancelNotifyWriteDetach:
			{
			CIoPersistentConsole& pcons = FindPersistentConsoleL(aMessage.Int3(), aMessage);
			pcons.CancelNotifyWriteDetach((TRequestStatus*)aMessage.Int0());
			break;
			}
		case EIoPersistentConsoleGetAttachedNames:
			{
			CIoPersistentConsole& pcons = FindPersistentConsoleL(aMessage.Int3(), aMessage);
			MessageWriteL(aMessage, 0, pcons.TransientReaderName());
			MessageWriteL(aMessage, 1, pcons.TransientWriterName());
			Complete(aMessage, KErrNone);
			break;
			}		
		case EIoPersistentConsoleGetCreatorThreadId:
			{
			CIoPersistentConsole& pcons = FindPersistentConsoleL(aMessage.Int3(), aMessage);
			MessageWriteL(aMessage, 0, TPckg<TThreadId>(pcons.Creator()));
			Complete(aMessage, KErrNone);
			break;
			}
		case EIoHandleIsType:
			{
			CIoObject& obj = FindObjectL(aMessage.Int3());
			Complete(aMessage, obj.IsType((RIoHandle::TType)aMessage.Int0()));
			break;
			}
		case EIoFindFirstHandle:
			{
			iFindByNameType = (RIoHandle::TType)aMessage.Int0();

			delete iFindByNameMatch;
			iFindByNameMatch = NULL;
			iFindByNameMatch = HBufC::NewL(DesLengthL(aMessage, 1));
			TPtr matchPtr(iFindByNameMatch->Des());
			MessageReadL(aMessage, 1, matchPtr);

			TName foundName;
			iFindByNameHandle = 0;
			CIoObject* obj = Server().FindObjectByName(iFindByNameType, iFindByNameHandle, *iFindByNameMatch, foundName);
			if (obj)
				{
				MessageWriteL(aMessage, 2, TPckg<TInt>(iFindByNameHandle));
				MessageWriteL(aMessage, 3, foundName);
				Complete(aMessage, KErrNone);	
				}
			else
				{
				Complete(aMessage, KErrNotFound);	
				}
			break;
			}
		case EIoFindNextHandle:
			{
			if (!(iFindByNameMatch && iFindByNameHandle))
				{
				PanicClient(aMessage, EPanicFindNextWithNoFindFirst);
				break;
				}
				
			TName foundName;
			CIoObject* obj = Server().FindObjectByName(iFindByNameType, iFindByNameHandle, *iFindByNameMatch, foundName);
			if (obj)
				{
				MessageWriteL(aMessage, 0, TPckg<TInt>(iFindByNameHandle));
				MessageWriteL(aMessage, 1, foundName);
				Complete(aMessage, KErrNone);	
				}
			else
				{
				Complete(aMessage, KErrNotFound);	
				}
			break;
			}
		case EIoOpenFoundHandle:
			{
			if (!(iFindByNameMatch && iFindByNameHandle))
				{
				PanicClient(aMessage, EPanicOpenFoundWithNoFind);
				break;
				}
			
			CIoObject& obj = Server().OpenObjectLC(aMessage.Int0());
			CreateHandleL(obj, EFalse, aMessage);
			CleanupStack::Pop(&obj);
			break;
			}
		case EIoHandleGetName:
			{
			CIoObject& obj = FindObjectL(aMessage.Int3());
			TName name(obj.Name());
			MessageWriteL(aMessage, 0, name.Left(MaxDesLengthL(aMessage, 0)));
			Complete(aMessage, MaxDesLengthL(aMessage, 0) < name.Length() ? KErrOverflow : KErrNone);	
			break;
			};
		case EIoHandleEquals:
			{
			CIoObject& tweedledee = FindObjectL(aMessage.Int3());
			CIoObject& tweedledum = FindObjectL(aMessage.Int0());
			Complete(aMessage, &tweedledee == &tweedledum);
			break;
			};
		case EIoReadHandleNotifyChange:
			{
			CIoReadObject& readObj = FindReadObjectL(aMessage.Int3(), aMessage);
			readObj.NotifyChange(aMessage);
			break;
			}
		case EIoReadHandleCancelNotifyChange:
			{
			CIoReadObject& readObj = FindReadObjectL(aMessage.Int3(), aMessage);
			readObj.CancelNotifyChange(*this);
			Complete(aMessage, KErrNone);
			break;
			}
		case EIoSetIsStdErr:
			{
			FindWriteObjectL(aMessage.Int3(), aMessage).SetIsStdErr((TBool)aMessage.Int0());
			Complete(aMessage, KErrNone);
			break;
			}
		default:
			{
			PanicClient(aMessage, EPanicUnknownOpcode);
			break;
			}
		}

	LOG(CleanupStack::PopAndDestroy());
	}

void CIoSession::CreateHandleL(CObject& aObject, TBool aDoPop, const RMsg& aMessage)
	{
	const TInt handle = iHandles->AddL(&aObject);
	if (aDoPop)
		{
		CleanupStack::Pop(&aObject);
		}
	TPckgC<TInt> handlePckg(handle);
	TRAPD(err, MessageWriteL(aMessage, 3, handlePckg));
	if (err)
		{
		iHandles->Remove(handle);
		User::Leave(err);
		}
	LOG(CIoLog::Printf(_L("Created handle %d"), handle));
	Complete(aMessage, err);
	}
	
CIoObject& CIoSession::FindObjectL(TInt aHandle)
	{
	CIoObject* obj = static_cast<CIoObject*>(iHandles->AtL(aHandle));
	return *obj;
	}

CIoEndPoint& CIoSession::FindEndPointL(TInt aHandle, const RMsg& aMessage)
	{
	CIoObject* obj = static_cast<CIoObject*>(iHandles->AtL(aHandle));
	__ASSERT_ALWAYS(obj->IsType(RIoHandle::EEndPoint), PanicClient(aMessage, EPanicNotAnEndPoint));
	return *static_cast<CIoEndPoint*>(obj);
	}

CIoPipe& CIoSession::FindPipeL(TInt aHandle, const RMsg& aMessage)
	{
	CIoObject* obj = static_cast<CIoObject*>(iHandles->AtL(aHandle));
	__ASSERT_ALWAYS(obj->IsType(RIoHandle::EPipe), PanicClient(aMessage, EPanicNotAPipe));
	return *static_cast<CIoPipe*>(obj);
	}

CIoConsole& CIoSession::FindConsoleL(TInt aHandle, const RMsg& aMessage)
	{
	CIoObject* obj = static_cast<CIoObject*>(iHandles->AtL(aHandle));
	__ASSERT_ALWAYS(obj->IsType(RIoHandle::EConsole), PanicClient(aMessage, EPanicNotAConsole));
	return *static_cast<CIoConsole*>(obj);
	}
	
CIoPersistentConsole& CIoSession::FindPersistentConsoleL(TInt aHandle, const RMsg& aMessage)
	{
	CIoObject* obj = static_cast<CIoObject*>(iHandles->AtL(aHandle));
	__ASSERT_ALWAYS(obj->IsType(RIoHandle::EPersistentConsole), PanicClient(aMessage, EPanicNotAPersistentConsole));
	return *static_cast<CIoPersistentConsole*>(obj);
	}

CIoReadWriteObject& CIoSession::FindReadWriteObjectL(TInt aHandle, const RMsg& aMessage)
	{
	CIoObject* obj = static_cast<CIoObject*>(iHandles->AtL(aHandle));
	__ASSERT_ALWAYS(obj->IsType(RIoHandle::EReadWriteObject), PanicClient(aMessage, EPanicNotAReadWriteObject));
	return *static_cast<CIoReadWriteObject*>(obj);
	}

CIoReadObject& CIoSession::FindReadObjectL(TInt aHandle, const RMsg& aMessage)
	{
	CIoObject* obj = static_cast<CIoObject*>(iHandles->AtL(aHandle));
	__ASSERT_ALWAYS(obj->IsType(RIoHandle::EReadObject), PanicClient(aMessage, EPanicNotAReadObject));
	return *static_cast<CIoReadObject*>(obj);
	}

CIoWriteObject& CIoSession::FindWriteObjectL(TInt aHandle, const RMsg& aMessage)
	{
	CIoObject* obj = static_cast<CIoObject*>(iHandles->AtL(aHandle));
	__ASSERT_ALWAYS(obj->IsType(RIoHandle::EWriteObject), PanicClient(aMessage, EPanicNotAWriteObject));
	return *static_cast<CIoWriteObject*>(obj);
	}

void CIoSession::Complete(const RMsg& aMessage, TInt aError)
	{
	LOG(CIoLog::LogCompletion(aMessage, aError));
	aMessage.Complete(aError);
	}
