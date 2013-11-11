// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Decodes data into text - platform independent
// 
//

#include "../SERVER/w32cmd.h"
#include "DEBUGLOG.H"

_LIT(LogCloseBracket,")");
_LIT(LogComma,", ");
_LIT(LogString,"%S");
_LIT(LogClose,"Close()");
_LIT(LogSpaces,"  ");

void TDebugLogOverflow::Overflow(TDes &aDes)
	{
	if (!iError)		//Should never be re-entrant but just in case
		{
		_LIT(OverflowThreeDots,"...");
		const TInt LenThreeDots=3;
		iError=ETrue;
		TInt length=aDes.MaxLength();
		if (length>LenThreeDots)
			{
			length-=LenThreeDots;
			if (aDes.Length()>length)
				aDes.SetLength(length);
			}
		aDes.AppendFormat(OverflowThreeDots,this);
		}
	}

class TRawEventX : public TRawEvent
// For bypassing protected members of TRawEvent
	{
public:
	inline TUint Ticks();
	inline TInt posx();
	inline TInt posy();
	inline TInt scanCode();
	};

inline TUint TRawEventX::Ticks()
	{return(iTicks);}
inline TInt TRawEventX::posx()
	{return(iU.pos.x);}
inline TInt TRawEventX::posy()
	{return(iU.pos.y);}
inline TInt TRawEventX::scanCode()
	{return(iU.key.scanCode __REMOVE_WINS_CHARCODE);}


GLDEF_C TInt hHandleToValue(TUint32 handle)
	{
	return(handle&0xFFFF);
	}

TDesC &TWsDecoder::CommandBuf(TInt aApp)
	{
	iText.Format(_L("[start of command buffer from app %d]"), aApp);
	return iText;
	}

TDesC &TWsDecoder::Command(TInt aClass, TInt aOpcode, const TAny *aCmdData, TInt aHandle)
	{
	TPtrC details(commandDetails(aClass, aOpcode, aCmdData, aHandle));
	iText=KNullDesC;
	if (details!=KNullDesC)
		iText=details;
	return iText;
	}

TDesC &TWsDecoder::NewClient(TUint aConnectionHandle)
	{
	iText.Format(_L("Client Connected %d"), aConnectionHandle);
	iRequestFuncClass=ENewClientClass;
	return iText;
	}

TDesC &TWsDecoder::CommandReply(TInt aData)
	{
	iText.Format(_L("returns %d (0x%x)"), aData, aData);
	return iText;
	}

TDesC &TWsDecoder::CommandReplyBuf(const TDesC8 &aDes)
	{
	TPtrC details(replyBufDetails(iRequestFuncClass, iPrevOpcode, &aDes, NULL));
	iText=KNullDesC;
	if (details!=KNullDesC)
		{
		iText=LogSpaces;
		TDebugLogTextHandler::Append(iText, details);
		}
	return iText;
	}

TDesC &TWsDecoder::CommandReplyBuf(const TDesC16 &aDes)
	{
	iOverflowHandler.Reset();
	TPtrC details(replyBufDetails(iRequestFuncClass, iPrevOpcode, NULL, &aDes));
	iText=KNullDesC;
	if (details!=KNullDesC)
		{
		iText=LogSpaces;
		TDebugLogTextHandler::Append(iText, details);
		}
	return iText;
	}

TDesC &TWsDecoder::SignalEvent(TInt aApp)
	{
	iText.Format(_L("Event signalled for App %d"),aApp);
	return iText;
	}

TDesC &TWsDecoder::Panic(TInt aApp, TInt aReason)
	{
	if (aApp==CDebugLogBase::EDummyConnectionId)
		iText.Format(_L("WSERV Internal Panic (%d)"), aReason);
	else
		iText.Format(_L("App %d, Paniced (%d)"),aApp, aReason);
	return iText;
	}

void TWsDecoder::UnKnown()
	{
	}

void TWsDecoder::UnKnownOpcode(TBuf<LogTBufSize> &aText, TInt aOpcode)
	{
	aText.AppendFormat(_L("unknown function (opcode %u)"), aOpcode);
	UnKnown();
	}

void TWsDecoder::UnKnownReplyBuf(TBuf<LogTBufSize> &aText, TInt aOpcode)
	{
	aText.AppendFormat(_L("unknown reply buffer (opcode %u)"), aOpcode);
	UnKnown();
	}

TDesC &TWsDecoder::MiscMessage(const TDesC &aFormat,TInt aParam)
	{
	iText.Format(aFormat,aParam);
	return iText;
	}

void TWsDecoder::AppendPoint(TBuf<LogTBufSize> &aText, const TDesC8 *aReplyDes8)
	{
	TShortBuf buf(TDebugLogTextHandler::FormatPoint(*((TPoint *)aReplyDes8->Ptr())));
	aText.AppendFormat(LogString, &buf);
	}

void TWsDecoder::AppendRect(TBuf<LogTBufSize> &aText, const TDesC8 *aReplyDes8)
	{
	TLongBuf buf(TDebugLogTextHandler::FormatRect(*((TRect *)aReplyDes8->Ptr())));
	aText.AppendFormat(LogString, &buf);
	}

void TWsDecoder::AppendDesc(TBuf<LogTBufSize> &aText, const TDesC8 *aReplyDes8)
	{
	TBuf<LogTBufSize> buf;
	buf.Copy(*aReplyDes8);
	aText.AppendFormat(LogString, &buf);
	}

void TWsDecoder::AppendDesc(TBuf<LogTBufSize> &aText, const TDesC16 *aReplyDes16)
	{
	aText.AppendFormat(LogString, aReplyDes16);
	}

void TWsDecoder::DecodeWindowGroup(TBuf<LogTBufSize> &aText, TInt aOpcode, const TAny *aCmdData, TInt aHandle)
	{
	aText.Format(_L("RWindowGroup[%d]::"),aHandle);
	TWsWinCmdUnion pData;
	pData.any=aCmdData;
	switch (aOpcode)
		{
	case EWsWinOpReceiveFocus:
		aText.AppendFormat(_L("ReceiveFocus(%d)"), *pData.Bool);
		break;
	case EWsWinOpAutoForeground:
		aText.AppendFormat(_L("AutoForeground(%d)"), *pData.Bool);
		break;
	case EWsWinOpCancelCaptureKey:
		aText.AppendFormat(_L("CancelCaptureKey(%d)"), *pData.UInt);
		break;
	case EWsWinOpCaptureKey:
		aText.AppendFormat(_L("CaptureKey(%u, 0x%04x, 0x%04x)"),
									pData.CaptureKey->key,
									pData.CaptureKey->modifiers,
									pData.CaptureKey->modifierMask);
		break;
	case EWsWinOpCancelCaptureKeyUpsAndDowns:
		aText.AppendFormat(_L("CancelCaptureKeyUpsAndDowns(%d)"), *pData.UInt);
		break;
	case EWsWinOpCaptureKeyUpsAndDowns:
		aText.AppendFormat(_L("CaptureKeyUpsAndDowns(%u, 0x%04x, 0x%04x)"),
									pData.CaptureKey->key,
									pData.CaptureKey->modifiers,
									pData.CaptureKey->modifierMask);
		break;
	case EWsWinOpSetTextCursor:
		{
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(pData.SetTextCursor->pos));
		aText.AppendFormat(_L("SetTextCursor(%d, %S, {%u,%d,%d,%d,0x%08x})"),
										hHandleToValue(pData.SetTextCursor->window), &buf,
										pData.SetTextCursor->cursor.iType, pData.SetTextCursor->cursor.iHeight,
										pData.SetTextCursor->cursor.iAscent, pData.SetTextCursor->cursor.iWidth,
										pData.SetTextCursor->cursor.iFlags);
		}
		break;
	case EWsWinOpSetTextCursorClipped:
		{
		TShortBuf buf1(TDebugLogTextHandler::FormatPoint(pData.SetTextCursor->pos));
		TLongBuf buf2(TDebugLogTextHandler::FormatRect(pData.SetTextCursor->rect));
		aText.AppendFormat(_L("SetTextCursor(%d, %S, {%u,%d,%d,%d,0x%08x}, %S)"),
										hHandleToValue(pData.SetTextCursor->window), &buf1,
										pData.SetTextCursor->cursor.iType, pData.SetTextCursor->cursor.iHeight,
										pData.SetTextCursor->cursor.iAscent, pData.SetTextCursor->cursor.iWidth,
										pData.SetTextCursor->cursor.iFlags, &buf2);
		}
		break;
	case EWsWinOpCancelTextCursor:
		aText.AppendFormat(_L("CancelTextCursor()"));
		break;
	case EWsWinOpSetOrdinalPriorityAdjust:
		aText.AppendFormat(_L("SetOrdinalPriorityAdjust(%d)"), *pData.Int);
		break;
	case EWsWinOpAddPriorityKey:
		aText.AppendFormat(_L("AddPriorityKey(0x%x,0x%x,%d)"), pData.PriorityKey->modifiers, pData.PriorityKey->modifierMask, pData.PriorityKey->keycode);
		break;
	case EWsWinOpRemovePriorityKey:
		aText.AppendFormat(_L("RemovePriorityKey(0x%x,0x%x,%d)"), pData.PriorityKey->modifiers, pData.PriorityKey->modifierMask, pData.PriorityKey->keycode);
		break;
	case EWsWinOpSetOrdinalPositionErr:
		aText.AppendFormat(_L("SetOrdinalPositionErr(%d, %d)"), pData.OrdinalPos->pos, pData.OrdinalPos->ordinalPriority);	
		break;
	case EWsWinOpDisableKeyClick:
		aText.AppendFormat(_L("DisableKeyClick(%u)"), *pData.UInt);	
		break;
	case EWsWinOpCaptureLongKey:
		aText.AppendFormat(_L("CaptureLongKey(TTimeIntervalMicroSeconds32(%d), %u, %u, %u, %u, %d, %u)"), 
								pData.CaptureLongKey->delay.Int(), pData.CaptureLongKey->inputKey, pData.CaptureLongKey->outputKey, 
								pData.CaptureLongKey->modifierMask, pData.CaptureLongKey->modifiers, pData.CaptureLongKey->priority, pData.CaptureLongKey->flags);	
		break;
	case EWsWinOpCancelCaptureLongKey:
		aText.AppendFormat(_L("CancelCaptureLongKey(%d)"), pData.CaptureKey->key);	
		break;
	case EWsWinOpAllowChildWindowGroup:
		aText.AppendFormat(_L("AllowProcessToCreateChildWindowGroup(%d)"), *pData.Int);	
		break;
	case EWsWinOpClearChildGroup:
		aText.AppendFormat(_L("ClearChildGroup()"));	
		break;
	case EWsWinOpSetChildGroup:
		aText.AppendFormat(_L("SetChildGroup(%d)"), *pData.Int); 	
		break;
	case EWsWinOpEnableVisibilityChangeEvents:
		aText.Append(_L("EnableVisibilityChangeEvents()")); 	
		break;
	case EWsWinOpDisableVisibilityChangeEvents:
		aText.Append(_L("DisableVisibilityChangeEvents()")); 	
		break;
	case EWsWinOpWindowGroupId:
		aText.Append(_L("WindowGroupId()")); 	
		break;
	case EWsWinOpEnableFocusChangeEvents:
		aText.Append(_L("EnableFocusChangeEvents()")); 	
		break;
	case EWsWinOpDisableFocusChangeEvents:
		aText.Append(_L("DisableFocusChangeEvents()")); 	
		break;
	default:
		decodeWindow(aText,aOpcode,aCmdData);
		break;
		}
	}

void TWsDecoder::DecodeClient(TBuf<LogTBufSize> &aText, TInt aOpcode, const TAny *aCmdData, TInt aHandle)
	{
	TWsClCmdUnion pData;

	aText.Format(_L("RWsSession[%d]::"),aHandle);
	pData.any=aCmdData;
	switch (aOpcode)
		{
	case EWsClOpDisconnect:
		aText.AppendFormat(LogClose);
		break;
	case EWsClOpSetHotKey:
		aText.AppendFormat(_L("SetHotKey(%u, %u, 0x%04x, 0x%04x)"),
										pData.SetHotKey->type,
										pData.SetHotKey->keycode,
										pData.SetHotKey->modifiers,
										pData.SetHotKey->modifierMask);
		break;
	case EWsClOpClearHotKeys:
		aText.AppendFormat(_L("ClearHotKeys(%d)"),*pData.UInt);
		break;
	case EWsClOpRestoreDefaultHotKey:
		aText.AppendFormat(_L("ResetDefaultHotKey(%d)"),*pData.UInt);
		break;
	case EWsClOpComputeMode:
		aText.AppendFormat(_L("ComputeMode(%u)"),*pData.UInt);
		break;
	case EWsClOpEventReady:
		aText.AppendFormat(_L("EventReady(TInt *stat)"));
		break;
	case EWsClOpRedrawReady:
		aText.AppendFormat(_L("RedrawReady(TInt *stat)"));
		break;
	case EWsClOpPriorityKeyReady:
		aText.AppendFormat(_L("PriorityKeyReady(TInt *stat)"));
		break;
	case EWsClOpEventReadyCancel:
		aText.AppendFormat(_L("EventReadyCancel()"));
		break;
	case EWsClOpRedrawReadyCancel:
		aText.AppendFormat(_L("RedrawReadyCancel()"));
		break;
	case EWsClOpPriorityKeyReadyCancel:
		aText.AppendFormat(_L("PriorityKeyReadyCancel()"));
		break;
	case EWsClOpGetEvent:
		aText.AppendFormat(_L("GetEvent(TWsEvent *aEvent)"));
		break;
	case EWsClOpGetRedraw:
		aText.AppendFormat(_L("GetRedraw(TWsRedrawEvent *aEvent)"));
		break;
	case EWsClOpGetPriorityKey:
		aText.AppendFormat(_L("GetPriorityKey(TWsPriorityKeyEvent *aEvent)"));
		break;
	case EWsClOpCreateWindow:
	case EWsClOpCreateGc:
	case EWsClOpCreateWindowGroup:
	case EWsClOpCreateAnimDll:
	case EWsClOpCreateScreenDevice:
	case EWsClOpCreateSprite:
	case EWsClOpCreatePointerCursor:
	case EWsClOpCreateBitmap:
	case EWsClOpCreateDirectScreenAccess:
	case EWsClOpCreateClick:
		switch(aOpcode)
			{
			case EWsClOpCreateWindow:
				aText.AppendFormat(_L("CreateWindow(%d,%x)"),hHandleToValue(pData.CreateWindow->parent),pData.CreateWindow->clientHandle);
				break;
			case EWsClOpCreateGc:
				aText.AppendFormat(_L("CreateGc()"));
				break;
			case EWsClOpCreateWindowGroup:
				aText.AppendFormat(_L("CreateWindowGroup(%x)"),pData.CreateWindow->clientHandle);
				break;
			case EWsClOpCreateAnimDll:
				TDebugLogTextHandler::Append(aText, _L("LoadAnimDll(\""));
				if (pData.LoadAnimDll && (pData.LoadAnimDll->length >= 0) && (pData.LoadAnimDll->length < LogTBufSize - 22))
					TDebugLogTextHandler::Append(aText, TPtrC((TText *)(pData.LoadAnimDll+1), pData.LoadAnimDll->length));
				else
					TDebugLogTextHandler::Append(aText, _L("<INVALID DESCRIPTOR>"));
				TDebugLogTextHandler::Append(aText, _L("\")"));
				break;
			case EWsClOpCreateScreenDevice:
				aText.AppendFormat(_L("CreateScreenDevice()"));
				break;
			case EWsClOpCreateSprite:
				aText.AppendFormat(_L("CreateSprite(...)"));
				break;
			case EWsClOpCreatePointerCursor:
				aText.AppendFormat(_L("CreatePointerCursor(...)"));
				break;
			case EWsClOpCreateBitmap:
				aText.AppendFormat(_L("CreateBitmap(...)"));
				break;
			case EWsClOpCreateDirectScreenAccess:
				aText.AppendFormat(_L("CreateDirectScreenAccess()"));
				break;
			case EWsClOpCreateClick:
				aText.AppendFormat(_L("CreateSoundPlugin()"));
				break;
			}
		break;
	case EWsClOpHeapCount:
		aText.AppendFormat(_L("HeapCount()"));
		break;
	case EWsClOpDebugInfo:
		aText.AppendFormat(_L("DebugInfo(%d,%d)"), pData.DebugInfo->iFunction, pData.DebugInfo->iParam);
		break;
	case EWsClOpDebugInfoReplyBuf:
		aText.AppendFormat(_L("DebugInfo(%d,%d,...)"), pData.DebugInfo->iFunction, pData.DebugInfo->iParam);
		break;
	case EWsClOpTestInvariant:
		aText.AppendFormat(_L("unimplemented opcode 'EWsClOpTestInvariant' (%u)"), aOpcode);
		break;
	case EWsClOpSetShadowVector:
		{
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(*pData.Point));
		aText.AppendFormat(_L("SetShadowVector(%S)"), &buf);
		break;
		}
	case EWsClOpShadowVector:
		aText.Append(_L("ShadowVector()"));
		break;
	case EWsClOpResourceCount:
		aText.Append(_L("ResourceCount()"));
		break;
	case EWsClOpSetKeyboardRepeatRate:
		aText.Append(_L("SetKeyboardRepeatRate(...,...)"));
		break;
	case EWsClOpGetKeyboardRepeatRate:
		aText.Append(_L("GetKeyboardRepeatRate(...,...)"));
		break;
	case EWsClOpSetDoubleClick:
		aText.AppendFormat(_L("SetDoubleClickSettings(...,%d)"), pData.SetDoubleClick->distance);
		break;
	case EWsClOpGetDoubleClickSettings:
		aText.Append(_L("GetDoubleClickSettings(...,...)"));
		break;
	case EWsClOpHeapSetFail:
		aText.AppendFormat(_L("HeapSetFail(%d, %d)"), pData.HeapSetFail->type, pData.HeapSetFail->value);
		break;
	case EWsClOpNumWindowGroups:
		aText.AppendFormat(_L("NumWindowGroups(%d)"),*pData.UInt);
		break;
	case EWsClOpNumWindowGroupsAllPriorities:
		aText.Append(_L("NumWindowGroups()"));
		break;
	case EWsClOpWindowGroupList:
		aText.AppendFormat(_L("WindowGroupList(...,%d,%d)"),pData.WindowGroupList->priority, pData.WindowGroupList->count);
		break;
	case EWsClOpWindowGroupListAllPriorities:
		aText.AppendFormat(_L("WindowGroupList(...,%d)"),pData.WindowGroupList->count);
		break;
	case EWsClOpGetFocusWindowGroup:
		aText.AppendFormat(_L("GetFocusWindowGroup(%d, ...)"), *pData.UInt);
		break;
	case EWsClOpGetDefaultOwningWindow:
		aText.Append(_L("GetDefaultOwningWindow()"));
		break;
	case EWsClOpSetWindowGroupOrdinalPosition:
		aText.AppendFormat(_L("SetWindowGroupOrdinalPosition(%d, %d)"),
							pData.SetWindowGroupOrdinalPosition->identifier,
							pData.SetWindowGroupOrdinalPosition->position);
		break;
	case EWsClOpGetWindowGroupHandle:
		aText.AppendFormat(_L("GetWindowGroupHandle(%d)"),*pData.Int);
		break;
	case EWsClOpGetWindowGroupOrdinalPriority:
		aText.AppendFormat(_L("GetWindowGroupOrdinalPriority(%d)"),*pData.Int);
		break;
	case EWsClOpGetWindowGroupClientThreadId:
		aText.AppendFormat(_L("GetWindowGroupClientThreadId(%d)"),*pData.Int);
		break;
	case EWsClOpGetWindowGroupNameFromIdentifier:
		aText.AppendFormat(_L("GetWindowGroupNameFromIdentifier(%d)"),pData.GetWindowGroupNameFromIdentifier->identifier);
		break;
	case EWsClOpFindWindowGroupIdentifier:
		aText.AppendFormat(_L("FindWindowGroupIdentifier(%d,...,%d)"),pData.FindWindowGroupIdentifier->identifier,
						pData.FindWindowGroupIdentifier->offset);
		break;
	case EWsClOpFindWindowGroupIdentifierThread:
		aText.AppendFormat(_L("FindWindowGroupIdentifier(%d,0x%x)"),pData.FindWindowGroupIdentifierThread->identifier,
						pData.FindWindowGroupIdentifierThread->threadId.Id());
		break;
	case EWsClOpSendMessageToWindowGroup:
		aText.AppendFormat(_L("SendMessageToWindowGroup(0x%x,0x%x,...)"),pData.SendMessageToWindowGroup->identifierOrPriority,
						pData.SendMessageToWindowGroup->uid, pData.SendMessageToWindowGroup->dataLength);
		break;
	case EWsClOpSendMessageToAllWindowGroups:
		aText.AppendFormat(_L("SendMessageToAllWindowGroups(0x%x,...)"),pData.SendMessageToWindowGroup->uid, pData.SendMessageToWindowGroup->dataLength);
		break;
	case EWsClOpSendMessageToAllWindowGroupsPriority:
		aText.AppendFormat(_L("SendMessageToAllWindowGroups(%d,0x%x,...)"),pData.SendMessageToWindowGroup->identifierOrPriority,
						pData.SendMessageToWindowGroup->uid, pData.SendMessageToWindowGroup->dataLength);
		break;
	case EWsClOpFetchMessage:
		aText.AppendFormat(_L("FetchMessage(...)"));
		break;
	case EWsClOpSetSystemPointerCursor:
		aText.AppendFormat(_L("SetSystemPointerCursor(...,%d)"), pData.SetSystemPointerCursor->number);
		break;
	case EWsClOpClearSystemPointerCursor:
		aText.AppendFormat(_L("ClearSystemPointerCursor(%d)"), *pData.UInt);
		break;
	case EWsClOpClaimSystemPointerCursorList:
		aText.Append(_L("ClaimSystemPointerCursorList()"));
		break;
	case EWsClOpFreeSystemPointerCursorList:
		aText.Append(_L("FreeSystemPointerCursorList()"));
		break;
	case EWsClOpLogMessage:
		aText.Append(_L("LogMessage(...)"));
		break;
	case EWsClOpPasswordEntered:
		aText.Append(_L("PasswordEntered()"));
		break;
	case EWsClOpPurgePointerEvents:
		aText.Append(_L("PurgePointerEvents()"));
		break;
	case EWsClOpRawEvent:
		aText.AppendFormat(_L("SimulateRawEvent(%d,%d,{{%d,%d},%d})"),((TRawEventX *)pData.RawEvent)->Type(),
								((TRawEventX *)pData.RawEvent)->Ticks(),((TRawEventX *)pData.RawEvent)->posx(),
								((TRawEventX *)pData.RawEvent)->posy(),((TRawEventX *)pData.RawEvent)->scanCode());
		break;
	case EWsClOpSendEventToWindowGroup:
		aText.AppendFormat(_L("SendEventToWindowGroup(%d,"),pData.SendEventToWindowGroup->parameter);
		aText.Append(eventDetails(&pData.SendEventToWindowGroup->event));
		aText.Append(LogCloseBracket);
		break;
	case EWsClOpSetModifierState:
		aText.AppendFormat(_L("SetModifierState(%d,%d)"), pData.SetModifierState->modifier, pData.SetModifierState->state);
		break;
	case EWsClOpGetModifierState:
		aText.Append(_L("GetModifierState()"));
		break;
	case EWsClOpSendOffEventsToShell:
		{
		_LIT(LogClientSendOffEventsToShell,"RequestOffEvents(%u,%d)");
		aText.AppendFormat(LogClientSendOffEventsToShell
										,pData.OffEventsToShell->on,hHandleToValue(pData.OffEventsToShell->window));
		}
		break;
	case EWsClOpGetDefModeMaxNumColors:
		{
		_LIT(LogClientGetDefModeMaxNumColors,"GetDefModeMaxNumColors()");
		aText.Append(LogClientGetDefModeMaxNumColors);
		}
		break;
	case EWsClOpGetColorModeList:
		{
		_LIT(LogClientGetColorModeList,"GetColorModeList()");
		aText.Append(LogClientGetColorModeList);
		}
		break;
	case EWsClOpKeyEvent:
		{
		_LIT(LogClientKeyEvent,"SimulateKeyEvent(%d)");
		aText.AppendFormat(LogClientKeyEvent,pData.KeyEvent->iCode);
		}
		break;
	case EWsClOpSendEventToAllWindowGroup:
		{
		_LIT(LogClientSendEventToAllWindowGroups,"SendEventToAllWindowGroups(");
		aText.Append(LogClientSendEventToAllWindowGroups);
		aText.Append(eventDetails(&pData.SendEventToWindowGroup->event));
		aText.Append(LogCloseBracket);
		}
		break;
	case EWsClOpSendEventToAllWindowGroupPriority:
		{
		_LIT(LogClientSendEventToAllWindowGroupPriority,"SendEventToAllWindowGroups(%d,");
		aText.AppendFormat(LogClientSendEventToAllWindowGroupPriority,pData.SendEventToWindowGroup->parameter);
		aText.Append(eventDetails(&pData.SendEventToWindowGroup->event));
		aText.Append(LogCloseBracket);
		}
		break;
	case EWsClOpSetPointerCursorArea:
		{
		TLongBuf buf(TDebugLogTextHandler::FormatRect(pData.SetPointerCursorArea->area));
		_LIT(LogClientSetPointerCursorArea,"SetPointerCursorArea(%d,%S)");
		aText.AppendFormat(LogClientSetPointerCursorArea,pData.SetPointerCursorArea->mode,&buf);
		}
		break;
	case EWsClOpPointerCursorArea:
		{
		_LIT(LogClientPointerCursorArea,"PointerCursorArea(%d)");
		aText.AppendFormat(LogClientPointerCursorArea,*pData.Int);
		}
		break;
	case EWsClOpSetPointerCursorMode:
		{
		_LIT(LogClientSetPointerCursorMode,"SetPointerCursorMode(%d)");
		aText.AppendFormat(LogClientSetPointerCursorMode,*pData.Mode);
		}
		break;
	case EWsClOpPointerCursorMode:
		{
		_LIT(LogClientPointerCursorMode,"PointerCursorMode()");
		aText.Append(LogClientPointerCursorMode);
		}
		break;
	case EWsClOpSetDefaultSystemPointerCursor:
		{
		_LIT(LogClientSetDefaultSystemPointerCursor,"SetDefaultSystemPointerCursor(%d)");
		aText.AppendFormat(LogClientSetDefaultSystemPointerCursor,*pData.Int);
		}
		break;
	case EWsClOpClearDefaultSystemPointerCursor:
		{
		_LIT(LogClientClearDefaultSystemPointerCursor,"ClearDefaultSystemPointerCursor()");
		aText.Append(LogClientClearDefaultSystemPointerCursor);
		}
		break;
	case EWsClOpSetPointerCursorPosition:
		{
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(*pData.Point));
		_LIT(LogClientSetPointerCursorPosition,"SetPointerCursorPosition(%S)");
		aText.AppendFormat(LogClientSetPointerCursorPosition,&buf);
		}
		break;
	case EWsClOpPointerCursorPosition:
		{
		_LIT(LogClientPointerCursorPosition,"PointerCursorPosition()");
		aText.Append(LogClientPointerCursorPosition);
		}
		break;
	case EWsClOpSetDefaultFadingParams:
		{
		_LIT(LogClientSetDefaultFadingParams,"SetDefaultFadingParameters(%d,%d)");
		aText.AppendFormat(LogClientSetDefaultFadingParams
								,WservEncoding::ExtractFirst8BitValue(*pData.UInt),WservEncoding::ExtractSecond8BitValue(*pData.UInt));
		}
		break;
	case EWsClOpPrepareForSwitchOff:
		{
		_LIT(LogClientPrepareForSwitchOff,"PrepareForSwitchOff()");
		aText.Append(LogClientPrepareForSwitchOff);
		}
		break;
	case EWsClOpSetFaded:
		{
		_LIT(LogWinSetFade,"SetSystemFade(%d");
		aText.AppendFormat(LogWinSetFade,pData.SetSystemFaded->Faded());
		if (!pData.SetSystemFaded->UseDefaultMap())
			{
			_LIT(LogWinSetFade2,",%d,%d");
			TUint8 blackMap;
			TUint8 whiteMap;
			pData.SetSystemFaded->GetFadingParams(blackMap,whiteMap);
			aText.AppendFormat(LogWinSetFade2,blackMap,whiteMap);
			}
		aText.Append(LogCloseBracket);
		}
		break;
	case EWsClOpLogCommand:
		{
		_LIT(LogClientLogCommand,"LogCommand(%d)");
		aText.AppendFormat(LogClientLogCommand,*pData.UInt);
		}
		break;
	case EWsClOpSendEventToOneWindowGroupPerClient:
		{
		_LIT(LogClientSendEventToOneWindowGroupPerClient,"SendEventToOneWindowGroupsPerClient(");
		aText.Append(LogClientSendEventToOneWindowGroupPerClient);
		aText.Append(eventDetails(&pData.SendEventToWindowGroup->event));
		aText.Append(LogCloseBracket);
		}
		break;
	case EWsClOpStartSetCustomTextCursor:
		{
		_LIT(LogClientStartSetCustomTextCursor,"StartSetCustomTextCursorL(id=0x%08x, flags=0x%08x, align=%d)");
		aText.AppendFormat(LogClientStartSetCustomTextCursor, pData.CustomTextCursorData->identifier, pData.CustomTextCursorData->flags, pData.CustomTextCursorData->alignment);
		}
		break;
	case EWsClOpCompleteSetCustomTextCursor:
		{
		_LIT(LogClientCompleteSetCustomTextCursor,"CompleteSetCustomTextCursorL(err=%d)");
		aText.AppendFormat(LogClientCompleteSetCustomTextCursor, *pData.Int);
		}
		break;
#if defined(__WINS__)
	case EWsClOpRemoveKeyCode:
		{
		_LIT(LogClientRemoveKeyCode,"RemoveKeyCode(%d)");
		aText.AppendFormat(LogClientRemoveKeyCode,*pData.UInt);
		}
		break;
	case EWsClOpSimulateXyInput:
		{
		_LIT(LogClientSimulateXyInput,"SimulateXyInputType(%d)");
		aText.AppendFormat(LogClientSimulateXyInput,*pData.UInt);
		}
		break;
#endif
	case EWsClOpSetBackgroundColor:
		{		
		TShortBuf buf(TDebugLogTextHandler::FormatRgb(*pData.rgb));
		_LIT(LogClientSetBackgroundColor,"SetBackgroundColor(%S)");
		aText.AppendFormat(LogClientSetBackgroundColor,&buf);
		}
		break;
	case EWsClOpGetBackgroundColor:
		{		
		_LIT(LogClientGetBackgroundColor,"GetBackgroundColor()");
		aText.AppendFormat(LogClientGetBackgroundColor);
		}
		break;
	case EWsClOpSystemInfo:
		{		
		_LIT(LogClientSystemInfo,"SystemInfo(%d, SSystemInfo &aSystemInfo)");
		aText.AppendFormat(LogClientSystemInfo, *pData.UInt);
		}
		break;
	case EWsClOpSetFocusScreen:
		{		
		_LIT(LogClientSetFocusScreen,"SetFocusScreen(%d)");
		aText.AppendFormat(LogClientSetFocusScreen, *pData.UInt);
		}
		break;
	case EWsClOpGetFocusScreen:
		{		
		_LIT(LogClientGetFocusScreen,"GetFocusScreen()");
		aText.AppendFormat(LogClientGetFocusScreen);
		}
		break;
	case EWsClOpWindowGroupListAndChain:
		{		
		_LIT(LogClientWindowGroupListAndChain,"WindowGroupList(%d, RArray<TWindowGroupChainInfo>* aWindowList)");
		aText.AppendFormat(LogClientWindowGroupListAndChain, *pData.UInt);
		}
		break;
	case EWsClOpWindowGroupListAndChainAllPriorities:
		{		
		_LIT(LogClientWindowGroupListAndChainAllPriorities,"WindowGroupListAndChainAllPriorities(RArray<TWindowGroupChainInfo>* aWindowList)");
		aText.AppendFormat(LogClientWindowGroupListAndChainAllPriorities);
		}
		break;
	case EWsClOpSetClientCursorMode:
		{		
		_LIT(LogClientSetClientCursorMode,"SetClientCursorMode(%d)");
		aText.AppendFormat(LogClientSetClientCursorMode, *pData.UInt);
		}
		break;
	case EWsClOpClearAllRedrawStores:
		{		
		_LIT(LogClientClearAllRedrawStores,"ClearAllRedrawStores()");
		aText.AppendFormat(LogClientClearAllRedrawStores);
		}
		break;
	case EWsClOpGraphicMessageReady:
		{		
		_LIT(LogClientGraphicMessageReady,"GraphicMessageReady(%d)");
		aText.AppendFormat(LogClientGraphicMessageReady, *pData.UInt);	
		}
		break;
	case EWsClOpGetGraphicMessage:
		{		
		_LIT(LogClientGraphicMessage,"GetGraphicMessage(%d)");
		aText.AppendFormat(LogClientGraphicMessage, *pData.UInt);	
		}
		break;
	case EWsClOpGraphicMessageCancel:
		{		
		_LIT(LogClientGraphicMessageCancel,"GraphicMessageCancel()");
		aText.AppendFormat(LogClientGraphicMessageCancel);	
		}
		break;		
	case EWsClOpNumWindowGroupsOnScreen:
		{		
		_LIT(LogClientNumWindowGroupsOnScreen,"NumWindowGroups(%d, %d)");
		aText.AppendFormat(LogClientNumWindowGroupsOnScreen, pData.NumWinGroups->screenNumber, pData.NumWinGroups->priority);
		}
		break;
	case EWsClOpGetNumberOfScreens:
		{		
		_LIT(LogClientGetNumberOfScreens,"NumberOfScreens()");
		aText.AppendFormat(LogClientGetNumberOfScreens);		
		}
		break;
	case EWsClOpNoFlickerFree:
		{		
		_LIT(LogNoFlickerFree,"EWsClOpNoFlickerFree enum");
		aText.AppendFormat(LogNoFlickerFree);		
		}
		break;
	case EWsClOpCreateGraphic:
		{		
		_LIT(LogCreateGraphic,"EWsClOpCreateGraphic enum");
		aText.AppendFormat(LogCreateGraphic);		
		}
		break;
	case EWsClOpRegisterSurface:
		{
		TInt screenNumber = pData.SurfaceRegister->screenNumber;
		const TSurfaceId& surfaceId = pData.SurfaceRegister->surfaceId;
		
		_LIT(LogRegisterSurface,"RegisterSurface(aScreenNumber(%d), TSurfaceId{%x,%x,%x,%x})");
		aText.AppendFormat(LogRegisterSurface, screenNumber, surfaceId.iInternal[0], surfaceId.iInternal[1], surfaceId.iInternal[2], surfaceId.iInternal[3]);
		}
		break;
	case EWsClOpUnregisterSurface:
		{
		TInt screenNumber = pData.SurfaceRegister->screenNumber;
		const TSurfaceId& surfaceId = pData.SurfaceRegister->surfaceId;
		
		_LIT(LogUnregisterSurface,"UnregisterSurface(aScreenNumber(%d), TSurfaceId{%x,%x,%x,%x})");
		aText.AppendFormat(LogUnregisterSurface, screenNumber, surfaceId.iInternal[0], surfaceId.iInternal[1], surfaceId.iInternal[2], surfaceId.iInternal[3]);
		}
		break;
	case EWsClOpSetCloseProximityThresholds:
		{
		_LIT(LogSetCloseProximityThresholds,"SetCloseProximityThresholds(%d, %d)");
		aText.AppendFormat(LogSetCloseProximityThresholds, pData.ZThresholdPair->enterThreshold, pData.ZThresholdPair->exitThreshold);
		}
		break;	
	case EWsClOpSetHighPressureThresholds:
		{
		_LIT(LogSetHighPressureThresholds,"SetHighPressureThresholds(%d, %d)");
		aText.AppendFormat(LogSetHighPressureThresholds, pData.ZThresholdPair->enterThreshold, pData.ZThresholdPair->exitThreshold);	
		}
		break;
	case EWsClOpGetEnterCloseProximityThreshold:
		{		
		_LIT(LogGetEnterCloseProximityThreshold,"EWsClOpGetEnterCloseProximityThreshold()");
		aText.AppendFormat(LogGetEnterCloseProximityThreshold);		
		}
		break;	
	case EWsClOpGetExitCloseProximityThreshold:
		{		
		_LIT(LogGetExitCloseProximityThreshold,"EWsClOpGetExitCloseProximityThreshold()");
		aText.AppendFormat(LogGetExitCloseProximityThreshold);		
		}
		break;
	case EWsClOpGetEnterHighPressureThreshold:
		{		
		_LIT(LogGetEnterHighPressureThreshold,"EWsClOpGetEnterHighPressureThreshold()");
		aText.AppendFormat(LogGetEnterHighPressureThreshold);		
		}
		break;	
	case EWsClOpGetExitHighPressureThreshold:
		{		
		_LIT(LogGetExitHighPressureThreshold,"EWsClOpGetExitHighPressureThreshold()");
		aText.AppendFormat(LogGetExitHighPressureThreshold);		
		}
		break;
	case EWsClOpIndicateAppOrientation:
	    {
        _LIT(LogIndicateAppOrientation,"EWsClOpIndicateAppOrientation(%d)");
        aText.AppendFormat(LogIndicateAppOrientation, *pData.Orientation);       	    
	    }
	    break;
	case EWsClOpLastEnumValue:
		{		
		_LIT(LogLastEnumValue,"EWsClOpLastEnumValue enum");
		aText.AppendFormat(LogLastEnumValue);		
		}
		break;
	default:
		UnKnownOpcode(aText, aOpcode);
		break;
		}
	}

void TWsDecoder::DecodeWindow(TBuf<LogTBufSize> &aText, TInt aOpcode, const TAny *aCmdData, TInt aHandle)
	{
	_LIT(LogWindow,"RWindow[%d]::");
	aText.Format(LogWindow,aHandle);
	decodeWindow(aText,aOpcode,aCmdData);
	}

void TWsDecoder::decodeWindow(TBuf<LogTBufSize> &aText,TInt aOpcode,const TAny *aData)
	{
	TWsWinCmdUnion pData;
	pData.any=aData;
	switch (aOpcode)
		{
	case EWsWinOpFree:
		aText.AppendFormat(LogClose);
		break;
	case EWsWinOpSetExtent:
		{
		TShortBuf buf1(TDebugLogTextHandler::FormatPoint(pData.SetEx->pos));
		TShortBuf buf2(TDebugLogTextHandler::FormatSize(pData.SetEx->size));
		aText.AppendFormat(_L("SetExtent(%S, %S)"), &buf1, &buf2);
		}
		break;
	case EWsWinOpSetExtentErr:
		{
		TShortBuf buf1(TDebugLogTextHandler::FormatPoint(pData.SetEx->pos));
		TShortBuf buf2(TDebugLogTextHandler::FormatSize(pData.SetEx->size));
		aText.AppendFormat(_L("SetExtentError(%S, %S)"), &buf1, &buf2);
		}
		break;
	case EWsWinOpOrdinalPosition:
		aText.AppendFormat(_L("OrdinalPosition()"));
		break;
	case EWsWinOpFullOrdinalPosition:
		aText.AppendFormat(_L("FullOrdinalPosition()"));
		break;
	case EWsWinOpSetOrdinalPosition:
		aText.AppendFormat(_L("SetOrdinalPosition(%d)"),*pData.Int);
		break;
	case EWsWinOpSetOrdinalPositionPri:
		aText.AppendFormat(_L("SetOrdinalPosition(%d,%d)"), pData.OrdinalPos->pos, pData.OrdinalPos->ordinalPriority);
		break;
	case EWsWinOpSetPos:
		{
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(*pData.pos));
		aText.AppendFormat(_L("SetPosition(%S)"),&buf);
		}
		break;
	case EWsWinOpSetSizeErr:
		{
		TShortBuf buf(TDebugLogTextHandler::FormatSize(*pData.size));
		aText.AppendFormat(_L("SetSizeErr(%S)"), &buf);
		}
		break;
	case EWsWinOpSetSize:
		{
		TShortBuf buf(TDebugLogTextHandler::FormatSize(*pData.size));
		aText.AppendFormat(_L("SetSize(%S)"), &buf);
		}
		break;
	case EWsWinOpPosition:
		aText.AppendFormat(_L("Position()"));
		break;
	case EWsWinOpAbsPosition:
		aText.AppendFormat(_L("AbsPosition()"));
		break;
	case EWsWinOpSize:
		aText.AppendFormat(_L("Size()"));
		break;
	case EWsWinOpActivate:
		aText.AppendFormat(_L("Activate()"));
		break;
	case EWsWinOpInvalidate:
		{
		TLongBuf buf(TDebugLogTextHandler::FormatRect(*pData.rect));
		aText.AppendFormat(_L("Invalidate(%S)"), &buf);
		}
		break;
	case EWsWinOpInvalidateFull:
		aText.AppendFormat(_L("Invalidate()"));
		break;
	case EWsWinOpBeginRedraw:
		{
		TLongBuf buf(TDebugLogTextHandler::FormatRect(*pData.rect));
		aText.AppendFormat(_L("BeginRedraw(%S)"), &buf);
		}
		break;
	case EWsWinOpBeginRedrawFull:
		aText.AppendFormat(_L("BeginRedraw()"));
		break;
	case EWsWinOpEndRedraw:
		aText.AppendFormat(_L("EndRedraw()"));
		break;
	case EWsWinOpTestInvariant:
		aText.AppendFormat(_L("__TestInvariant()"));
		break;
	case EWsWinOpParent:
		aText.AppendFormat(_L("Parent()"));
		break;
	case EWsWinOpNextSibling:
		aText.AppendFormat(_L("NextSibling()"));
		break;
	case EWsWinOpPrevSibling:
		aText.AppendFormat(_L("PrevSibling()"));
		break;
	case EWsWinOpChild:
		aText.AppendFormat(_L("Child()"));
		break;
	case EWsWinOpInquireOffset:
		aText.AppendFormat(_L("InquireOffset(%d)"),hHandleToValue(*pData.UInt));
		break;
	case EWsWinOpPointerFilter:
		aText.AppendFormat(_L("PointerFilter(0x%08x)"), *pData.UInt);
		break;
	case EWsWinOpSetPointerCapture:
		aText.AppendFormat(_L("SetPointerCapture(%u)"), *pData.UInt);
		break;
	case EWsWinOpSetPointerGrab:
		aText.AppendFormat(_L("SetPointerGrab(%u)"), *pData.UInt);
		break;
	case EWsWinOpSetNoBackgroundColor:
		aText.Append(_L("SetBackgroundColor()"));
		break;
	case EWsWinOpSetBackgroundColor:
		{
		TLongBuf buf(TDebugLogTextHandler::FormatRgb(*pData.rgb));
		aText.AppendFormat(_L("SetBackgroundColor(%S)"), &buf);
		}
		break;
	case EWsWinOpSetColor:
		{
		TLongBuf buf(TDebugLogTextHandler::FormatRgb(*pData.rgb));
		aText.AppendFormat(_L("SetColor(%S)"), &buf);
		}
		break;
	case EWsWinOpSetShadowHeight:
		aText.AppendFormat(_L("SetShadowHeight(%d)"), *pData.Int);
		break;
	case EWsWinOpShadowDisabled:
		{
		TShortBuf buf(TDebugLogTextHandler::FormatBool(*pData.Bool));
		aText.AppendFormat(_L("SetShadowDisabled(%S)"), &buf);
		}
		break;
	case EWsWinOpSetVisible:
		{
		TShortBuf buf(TDebugLogTextHandler::FormatBool(*pData.Bool));
		aText.AppendFormat(_L("SetVisible(%S)"), &buf);
		}
		break;
	case EWsWinOpUpdateScreen:
		aText.Append(_L("UpdateScreen()"));
		break;
	case EWsWinOpUpdateScreenRegion:
		aText.Append(_L("UpdateScreenRegion(TRegion(...))"));
		break;
	case EWsWinOpMaintainBackup:
		aText.Append(_L("MaintainBackup()"));
		break;
	case EWsWinOpGetInvalidRegion:
		aText.Append(_L("GetInvalidRegion(...)"));
		break;
	case EWsWinOpGetInvalidRegionCount:
		aText.Append(_L("GetInvalidRegionCount()"));
		break;
	case EWsWinOpScroll:
		{
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(pData.ScrollRect->offset));
		aText.AppendFormat(_L("Scroll(%S)"), &buf);
		}
		break;
	case EWsWinOpScrollClip:
		{
		TLongBuf buf1(TDebugLogTextHandler::FormatRect(pData.ScrollRect->clip));
		TShortBuf buf2(TDebugLogTextHandler::FormatPoint(pData.ScrollRect->offset));
		aText.AppendFormat(_L("Scroll(%S, %S)"), &buf1, &buf2);
		}
		break;
	case EWsWinOpScrollRect:
		{
		TShortBuf buf1(TDebugLogTextHandler::FormatPoint(pData.ScrollRect->offset));
		TLongBuf buf2(TDebugLogTextHandler::FormatRect(pData.ScrollRect->rect));
		aText.AppendFormat(_L("Scroll(%S, %S)"), &buf1, &buf2);
		}
		break;
	case EWsWinOpScrollClipRect:
		{
		TLongBuf buf1(TDebugLogTextHandler::FormatRect(pData.ScrollRect->clip));
		TShortBuf buf2(TDebugLogTextHandler::FormatPoint(pData.ScrollRect->offset));
		TLongBuf buf3(TDebugLogTextHandler::FormatRect(pData.ScrollRect->rect));
		aText.AppendFormat(_L("Scroll(%S, %S, %S)"), &buf1, &buf2, &buf3);
		}
		break;
	case EWsWinOpBitmapHandle:
		aText.Append(_L("BitmapHandle()"));
		break;
	case EWsWinOpUpdateBackupBitmap:
		aText.Append(_L("UpdateBackupBitmap()"));
		break;
	case EWsWinOpRequiredDisplayMode:
		aText.AppendFormat(_L("SetRequiredDisplayMode(%d)"), *pData.UInt);
		break;
	case EWsWinOpSetCornerType:
		aText.AppendFormat(_L("SetCornerType(%d)"), *pData.UInt);
		break;
	case EWsWinOpSetShape:
		aText.AppendFormat(_L("SetWindowShape(%d rects)"), pData.SetShape->count);
		break;
	case EWsWinOpSetName:
		aText.Append(_L("SetName(\"...\")"));
		break;
	case EWsWinOpName:
		aText.Append(_L("Name()"));
		break;
	case EWsWinOpSetOwningWindowGroup:
		aText.AppendFormat(_L("SetOwningWindowGroup(%d)"),*pData.Int);
		break;
	case EWsWinOpDefaultOwningWindow:
		aText.Append(_L("DefaultOwningWindow()"));
		break;
	case EWsWinOpEnableOnEvents:
		aText.AppendFormat(_L("EnableOnEvents(%d)"),*pData.Int);
		break;
	case EWsWinOpDisableOnEvents:
		aText.Append(_L("DisableOnEvents()"));
		break;
	case EWsWinOpEnableErrorMessages:
		aText.AppendFormat(_L("EnableErrorMessages(%d)"),*pData.Int);
		break;
	case EWsWinOpDisableErrorMessages:
		aText.Append(_L("DisableErrorMessages()"));
		break;
	case EWsWinOpEnableModifierChangedEvents:
		aText.Append(_L("EnableModifierChangedEvents()"));
		break;
	case EWsWinOpDisableModifierChangedEvents:
		aText.Append(_L("DisableModifierChangedEvents()"));
		break;
	case EWsWinOpEnableGroupChangeEvents:
		aText.Append(_L("EnableGroupChangeEvents()"));
		break;
	case EWsWinOpDisableGroupChangeEvents:
		aText.Append(_L("DisableGroupChangeEvents()"));
		break;
	case EWsWinOpRequestPointerRepeatEvent:
		{
		TLongBuf buf(TDebugLogTextHandler::FormatRect(pData.RequestPointerRepeatEvent->rect));
		aText.AppendFormat(_L("RequestPointerRepeatEvent(TTimeIntervalMicroSeconds32(%d), %S)"), 
								pData.RequestPointerRepeatEvent->time.Int(), &buf);
		}
		break;
	case EWsWinOpCancelPointerRepeatEventRequest:
		aText.Append(_L("CancelPointerRepeatEventRequest()"));
		break;
	case EWsWinOpSetPointerCursor:
		aText.AppendFormat(_L("SetPointerCursor(%d)"),*pData.UInt);
		break;
	case EWsWinOpSetCustomPointerCursor:
		aText.AppendFormat(_L("SetCustomPointerCursor(%d)"),*pData.UInt);
		break;
	case EWsWinOpPasswordWindow:
		aText.AppendFormat(_L("PasswordWindow(%d)"),*pData.PasswordMode);
		break;
	case EWsWinOpAddKeyRect:
		{
		TLongBuf buf(TDebugLogTextHandler::FormatRect(pData.AddKeyRect->rect));
		aText.AppendFormat(_L("AddKeyRect(%S,%d,%d)"),&buf,pData.AddKeyRect->scanCode,pData.AddKeyRect->activatedByPointerSwitchOn);
		}
		break;
	case EWsWinOpRemoveAllKeyRects:
		aText.Append(_L("RemoveAllKeyRects()"));
		break;
	case EWsWinOpAllocPointerMoveBuffer:
		aText.AppendFormat(_L("AllocPointerMoveBuffer(%d,%d)"),pData.AllocPointerMoveBuffer->maxNumPoints,pData.AllocPointerMoveBuffer->flags);
		break;
	case EWsWinOpFreePointerMoveBuffer:
		aText.Append(_L("FreePointerMoveBuffer()"));
		break;
	case EWsWinOpEnablePointerMoveBuffer:
		aText.Append(_L("EnablePointerMoveBuffer()"));
		break;
	case EWsWinOpDisablePointerMoveBuffer:
		aText.Append(_L("DisablePointerMoveBuffer()"));
		break;
	case EWsWinOpRetrievePointerMoveBuffer:
		aText.Append(_L("RetrievePointerMoveBuffer(...)"));
		break;
	case EWsWinOpDiscardPointerMoveBuffer:
		aText.Append(_L("DiscardPointerMoveBuffer()"));
		break;
	case EWsWinOpEnableBackup:
		aText.Append(_L("EnableBackup()"));
		break;
	case EWsWinOpClaimPointerGrab:
		aText.Append(_L("ClaimPointerGrab()"));
		break;
	case EWsWinOpIdentifier:
		aText.Append(_L("Identifier()"));
		break;
	case EWsWinOpSetFade:
		{
		_LIT(LogWinSetFade,"SetFade(%d,%d");
		aText.AppendFormat(LogWinSetFade,pData.SetFaded->Faded(),pData.SetFaded->IncludeChildren());
		if (!pData.SetFaded->UseDefaultMap())
			{
			_LIT(LogWinSetFade2,",%d,%d");
			TUint8 blackMap;
			TUint8 whiteMap;
			pData.SetFaded->GetFadingParams(blackMap,whiteMap);
			aText.AppendFormat(LogWinSetFade2,blackMap,whiteMap);
			}
		aText.Append(LogCloseBracket);
		}
		break;
	case EWsWinOpSetNonFading:
		{
		_LIT(LogWinSetNonFading,"SetNonFading(%d)");
		aText.AppendFormat(LogWinSetNonFading,*(pData.Bool));
		}
		break;
	case EWsWinOpFadeBehind:
		{
		_LIT(LogWinFadeBehind,"FadeBehind(%d)");
		aText.AppendFormat(LogWinFadeBehind,*(pData.Bool));
		}
		break;
	case EWsWinOpEnableScreenChangeEvents:
		{
		_LIT(LogWinEnableScreenChangeEvents,"EnableScreenChangeEvents()");
		aText.Append(LogWinEnableScreenChangeEvents);
		}
		break;
	case EWsWinOpDisableScreenChangeEvents:
		{
		_LIT(LogWinDisableScreenChangeEvents,"DisableScreenChangeEvents()");
		aText.Append(LogWinDisableScreenChangeEvents);
		}
		break;
	case EWsWinOpSendPointerEvent:
		{
		_LIT(LogWinSendPointerEvent,"SimulatePointerEvent(");
		aText.Append(LogWinSendPointerEvent);
		aText.Append(eventDetails(pData.RawEvent));
		aText.Append(LogCloseBracket);
		}
		break;
	case EWsWinOpSendAdvancedPointerEvent:
		{
		_LIT(LogWinSendPointerEvent,"SimulateAdvancedPointerEvent(");
		aText.Append(LogWinSendPointerEvent);
		aText.Append(eventDetails(pData.RawEvent));
		aText.Append(LogCloseBracket);
		}
		break;
	case EWsWinOpGetDisplayMode:
		{
		_LIT(LogWinGetDisplayMode,"GetDisplayMode()");
		aText.Append(LogWinGetDisplayMode);
		}
		break;
	case EWsWinOpGetIsFaded:
		{
		_LIT(LogWinIsFaded,"IsFaded()");
		aText.Append(LogWinIsFaded);
		}
		break;
	case EWsWinOpGetIsNonFading:
		{
		_LIT(LogWinIsNonFading,"IsNonFading()");
		aText.Append(LogWinIsNonFading);
		}
		break;
	case EWsWinOpOrdinalPriority:
		{
		_LIT(LogWinOrdinalPriority,"OrdinalPriority()");
		aText.Append(LogWinOrdinalPriority);
		}
		break;
	case EWsWinOpClearPointerCursor:
		{
		_LIT(LogWinClearPointerCursor,"ClearPointerCursor()");
		aText.Append(LogWinClearPointerCursor);
		}
		break;
	case EWsWinOpEnableGroupListChangeEvents:
		{
		_LIT(LogWinEnableGroupListChangeEvents,"EnableGroupListChangeEvents()");
		aText.Append(LogWinEnableGroupListChangeEvents);
		}
		break;
	case EWsWinOpDisableGroupListChangeEvents:
		{
		_LIT(LogWinDisableGroupListChangeEvents,"DisableGroupListChangeEvents()");
		aText.Append(LogWinDisableGroupListChangeEvents);
		}
		break;
	case EWsWinOpHandleTransparencyUpdate:
		{
		_LIT(LogWinHandleTransparencyUpdate,"HandleTransparencyUpdate()");
		aText.Append(LogWinHandleTransparencyUpdate);
		}
		break;
	case EWsWinOpSetTransparencyFactor:
		{
		_LIT(LogWinSetTransparencyFactor,"SetTransparencyFactor()");
		aText.Append(LogWinSetTransparencyFactor);
		}
		break;
	case EWsWinOpSetTransparencyBitmap:
		{
		_LIT(LogWinSetTransparencyBitmap,"SetTransparencyBitmap()");
		aText.Append(LogWinSetTransparencyBitmap);
		}
		break;
	case EWsWinOpSetTransparencyBitmapCWs:
		{
		_LIT(LogWinSetTransparencyWsBitmap,"SetTransparencyWsBitmap()");
		aText.Append(LogWinSetTransparencyWsBitmap);
		}
		break;	

	case EWsWinOpSetTransparencyAlphaChannel:
		{
		_LIT(LogWinSetTransparencyAlphaChannel,"SetTransparencyAlphaChannel()");
		aText.Append(LogWinSetTransparencyAlphaChannel);
		}
		break;	
	case EWsWinOpMoveToGroup:
		{
		_LIT(LogWinMoveToGroup,"MoveToGroup(%d)");
		aText.AppendFormat(LogWinMoveToGroup, *pData.Int);
		}
		break;	
	case EWsWinOpStoreDrawCommands:
		{
		_LIT(LogWinStoreDrawCommands,"EnableRedrawStore(%u)");
		aText.AppendFormat(LogWinStoreDrawCommands, *pData.Bool);
		}
		break;	
	case EWsWinOpSetPointerCapturePriority:
		{
		_LIT(LogWinSetPointerCapturePriority,"SetPointerCapturePriority(%d)");
		aText.AppendFormat(LogWinSetPointerCapturePriority, *pData.Int);
		}
		break;	
	case EWsWinOpGetPointerCapturePriority:
		{
		_LIT(LogWinGetPointerCapturePriority,"GetPointerCapturePriority()");
		aText.Append(LogWinGetPointerCapturePriority);
		}
		break;	
	case EWsWinOpSetTransparentRegion:
		{
		_LIT(LogWinSetTransparentRegion,"SetTransparentRegion(%d rects)");
		aText.AppendFormat(LogWinSetTransparentRegion, pData.SetShape->count);
		}
		break;	
	case EWsWinOpSetTransparencyPolicy:
		{
		_LIT(LogWinSetTransparentPolicy,"SetTransparencyPolicy(TransparencyPolicy= %d)");
		aText.AppendFormat(LogWinSetTransparentPolicy, *pData.Int);
		}
		break;	
	case EWsWinOpIsRedrawStoreEnabled:
		{
		_LIT(LogWinIsRedrawStoreEnabled,"IsRedrawStoreEnabled()");
		aText.Append(LogWinIsRedrawStoreEnabled);
		}
		break;		
	case EWsWinOpEnableOSB:
		{
		_LIT(LogOSBEnabled,"EWsWinOpEnableOSB enum");
		aText.Append(LogOSBEnabled);
		}
		break;		
	case EWsWinOpDisableOSB:
		{
		_LIT(LogOSBDisabled,"EWsWinOpDisableOSB enum");
		aText.Append(LogOSBDisabled);
		}
		break;
	case EWsWinOpClientHandle:
		{
		_LIT(LogClientHandle,"ClientHandle()");
		aText.AppendFormat(LogClientHandle);
		}
		break;
    case EWsWinOpSetBackgroundSurface:
        {
        _LIT(LogWinSetBackgroundSurface,"SetBackgroundSurface(TSurfaceId{%x,%x,%x,%x})");
        aText.AppendFormat(LogWinSetBackgroundSurface, pData.Surface->iInternal[0],pData.Surface->iInternal[1],pData.Surface->iInternal[2],pData.Surface->iInternal[3]);
        }
        break;
    case EWsWinOpKeyColor:
        {
        _LIT(LogWinKeyColor,"KeyColor()");
        aText.AppendFormat(LogWinKeyColor);
        }
        break;          
    case EWsWinOpSetBackgroundSurfaceConfig:
        {        
        TSurfaceConfiguration aSurfaceConfig = pData.SurfaceConfigurationAndTrigger->surfaceConfig;
        TSurfaceId surfaceid;
        aSurfaceConfig.GetSurfaceId(surfaceid);
        CFbsBitGc::TGraphicsOrientation orientation = aSurfaceConfig.Orientation();
        TRect extent;
        aSurfaceConfig.GetExtent(extent);
        TRect viewport;
        aSurfaceConfig.GetViewport(viewport);
        TBool flip = aSurfaceConfig.Flip();
        TBool triggerRedraw = pData.SurfaceConfigurationAndTrigger->triggerRedraw;
        
        TLongBuf extentbuf(TDebugLogTextHandler::FormatRect(extent));
        TLongBuf viewportbuf(TDebugLogTextHandler::FormatRect(viewport));
        
        _LIT(LogWinSetBackgroundSurfaceConfig,"SetBackgroundSurface({%x,%x,%x,%x},%d,%S,%S,%d,%d)");
        aText.AppendFormat(LogWinSetBackgroundSurfaceConfig, surfaceid.iInternal[0],
                                                             surfaceid.iInternal[1],
                                                             surfaceid.iInternal[2],
                                                             surfaceid.iInternal[3],
                                                             orientation,
                                                             &extentbuf, 
                                                             &viewportbuf,
                                                             flip,
                                                             triggerRedraw);
        }
        break;
    case EWsWinOpRemoveBackgroundSurface:
        {
        _LIT(LogWinRemoveBackgroundSurface,"RemoveBackgroundSurface(%d)");
        aText.AppendFormat(LogWinRemoveBackgroundSurface, *(pData.Bool));
        }
        break;
    case EWsWinOpGetBackgroundSurfaceConfig:
        {
        _LIT(LogWinGetBackgroundSurfaceConfig,"GetBackgroundSurface()");
        aText.AppendFormat(LogWinGetBackgroundSurfaceConfig);
        }
        break;
    case EWsWinOpClearRedrawStore:
        {
        _LIT(LogWinClearRedrawStore,"ClearRedrawStore()");
        aText.AppendFormat(LogWinClearRedrawStore);
        }
        break;
	default:
		UnKnownOpcode(aText, aOpcode);
		break;
		}
	}

//#pragma check_stack(on)
void TWsDecoder::DecodeGc(TBuf<LogTBufSize> &aText, TInt aOpcode, const TAny *aCmdData, TInt aHandle)
	{
	_LIT(LogGc,"RGc[%d]::");
	aText.Format(LogGc,aHandle);
    if (aOpcode<EWsGcOpGdiBlt2)
        {
        if (aOpcode<EWsGcOpDrawPolyLineContinued)
            DecodeGc1(aText,aOpcode,aCmdData);
        else
            DecodeGc2(aText,aOpcode,aCmdData);
        }
    else
        DecodeGc3(aText,aOpcode,aCmdData);
    }
    
void TWsDecoder::DecodeGc1(TBuf<LogTBufSize> &aText, TInt aOpcode, const TAny *aCmdData)
	{
	TWsGcCmdUnion pData;

	pData.any=aCmdData;
	switch (aOpcode)
		{
	case EWsGcOpFree:
		aText.AppendFormat(LogClose);
		break;
	case EWsGcOpActivate:
		{
		_LIT(LogGcActivate,"Activate(%d)");
		aText.AppendFormat(LogGcActivate,hHandleToValue(*pData.UInt));
		}
		break;
	case EWsGcOpDeactivate:
		{
		_LIT(LogGcDeactivate,"Deactivate()");
		aText.AppendFormat(LogGcDeactivate);
		}
		break;
	case EWsGcOpDrawLine:
		{
		TShortBuf buf1(TDebugLogTextHandler::FormatPoint(pData.DrawLine->pnt1));
		TShortBuf buf2(TDebugLogTextHandler::FormatPoint(pData.DrawLine->pnt2));
		_LIT(LogGcDrawLine,"DrawLine(%S, %S)");
		aText.AppendFormat(LogGcDrawLine, &buf1, &buf2);
		}
		break;
	case EWsGcOpSetBrushStyle:
		{
		_LIT(LogGcSetBrushStyle,"SetBrushStyle(%u)");
		aText.AppendFormat(LogGcSetBrushStyle,*pData.UInt);
		}
		break;
	case EWsGcOpPlot:
		{
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(*pData.Point));
		_LIT(LogGcPlot,"Plot(%S)");
		aText.AppendFormat(LogGcPlot, &buf);
		}
		break;
	case EWsGcOpSetPenColor:
		{
		TLongBuf buf(TDebugLogTextHandler::FormatRgb(*pData.rgb));
		_LIT(LogGcSetPenColor,"SetPenColor(%S)");
		aText.AppendFormat(LogGcSetPenColor,&buf);
		}
		break;
	case EWsGcOpSetBrushColor:
		{
		TLongBuf buf(TDebugLogTextHandler::FormatRgb(*pData.rgb));
		_LIT(LogGcSetBrushColor,"SetBrushColor(%S)");
		aText.AppendFormat(LogGcSetBrushColor,&buf);
		}
		break;
	case EWsGcOpSetPenSize:
		{
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(*pData.Point));
		_LIT(LogGcSetPenSize,"SetPenSize(%S)");
		aText.AppendFormat(LogGcSetPenSize,&buf);
		}
		break;
	case EWsGcOpSetPenStyle:
		{
		_LIT(LogGcSetPenStyle,"SetPenStyle(%d)");
		aText.AppendFormat(LogGcSetPenStyle,*pData.UInt);
		}
		break;
	case EWsGcOpSetClippingRegion:
		{
		_LIT(LogGcSetClippingRegion,"SetClippingRegion(...)");
		aText.Append(LogGcSetClippingRegion);
		}
		break;
	case EWsGcOpCancelClippingRegion:
		{
		_LIT(LogGcCancelClippingRegion,"CancelClippingRegion()");
		aText.Append(LogGcCancelClippingRegion);
		}
		break;
	case EWsGcOpSetClippingRect:
		{
		TLongBuf buf(TDebugLogTextHandler::FormatRect(*pData.Rect));
		_LIT(LogGcSetClippingRect,"SetClippingRect(%S)");
		aText.AppendFormat(LogGcSetClippingRect,&buf);
		}
		break;
	case EWsGcOpCancelClippingRect:
		{
		_LIT(LogGcCancelClippingRect,"CancelClippingRect()");
		aText.Append(LogGcCancelClippingRect);
		}
		break;
	case EWsGcOpDrawTo:
		{
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(*pData.Point));
		_LIT(LogGcDrawTo,"DrawTo(%S)");
		aText.AppendFormat(LogGcDrawTo,&buf);
		}
		break;
	case EWsGcOpDrawBy:
		{
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(*pData.Point));
		_LIT(LogGcDrawBy,"DrawBy(%S)");
		aText.AppendFormat(LogGcDrawBy,&buf);
		}
		break;
	case EWsGcOpDrawPolyLine:
		{
		_LIT(LogGcDrawPolyLine,"DrawPolyLine(%d...)");
		aText.AppendFormat(LogGcDrawPolyLine,pData.PolyLine->numPoints);
		}
		break;
	case EWsGcOpDrawPolyLineContinued:
		{
		_LIT(LogGcDrawPolyLineContinued,"DrawPolyLineContinued(%d...)");
		aText.AppendFormat(LogGcDrawPolyLineContinued,pData.PolyLine->numPoints);
		}
		break;
	case EWsGcOpUseBrushPattern:
		{
		_LIT(LogGcUseBrushPattern,"UseBrushPattern({0x%x})");
		aText.AppendFormat(LogGcUseBrushPattern,hHandleToValue(*pData.handle));
		}
		break;
	case EWsGcOpDrawArc:
		{
		TLongBuf buf1(TDebugLogTextHandler::FormatRect(pData.ArcOrPie->rect));
		TShortBuf buf2(TDebugLogTextHandler::FormatPoint(pData.ArcOrPie->start));
		TShortBuf buf3(TDebugLogTextHandler::FormatPoint(pData.ArcOrPie->end));
		_LIT(LogGcDrawArc,"DrawArc(%S,%S,%S)");
		aText.AppendFormat(LogGcDrawArc,&buf1,&buf2,&buf3);
		}
		break;
	case EWsGcOpSetWordJustification:
		{
		_LIT(LogGcSetWordJustification,"SetWordJustification(%d,%d)");
		aText.AppendFormat(LogGcSetWordJustification,pData.SetJustification->excessWidth,pData.SetJustification->numGaps);
		}
		break;
	case EWsGcOpSetCharJustification:
		{
		_LIT(LogGcSetCharJustification,"SetCharJustification(%d,%d)");
		aText.AppendFormat(LogGcSetCharJustification,pData.SetJustification->excessWidth,pData.SetJustification->numGaps);
		}
		break;
	case EWsGcOpSetBrushOrigin:
		{
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(*pData.Point));
		aText.AppendFormat(_L("SetBrushOrigin(%S)"), &buf);		
		}
		break;
	case EWsGcOpDiscardBrushPattern:
		{
		_LIT(LogGcDiscardBrushPattern,"DiscardBrushPattern()");
		aText.Append(LogGcDiscardBrushPattern);
		}	
		break;	
 	default:
		UnKnownOpcode(aText, aOpcode);
		break;
		}
	}

void TWsDecoder::DecodeGc2(TBuf<LogTBufSize> &aText, TInt aOpcode, const TAny *aCmdData)
	{
	TWsGcCmdUnion pData;

	pData.any=aCmdData;
	switch (aOpcode)
		{
	case EWsGcOpDrawText:
		{
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(pData.DrawText->pos));
		_LIT(LogGcDrawText1,"DrawText(\"");
		TDebugLogTextHandler::Append(aText, LogGcDrawText1);
		TDebugLogTextHandler::Append(aText, TPtrC((TText *)(pData.DrawText+1), pData.DrawText->length));
		_LIT(LogGcDrawText2,"\", %S)");
		aText.AppendFormat(LogGcDrawText2,&iOverflowHandler,&buf);
		}
		break;
	case EWsGcOpDrawTextVertical:
		{
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(pData.DrawTextVertical->pos));
		_LIT(LogGcDrawTextVertical1,"DrawTextVertical(\"");
		TDebugLogTextHandler::Append(aText, LogGcDrawTextVertical1);
		TDebugLogTextHandler::Append(aText, TPtrC((TText *)(pData.DrawTextVertical+1), pData.DrawTextVertical->length));
		_LIT(LogGcDrawTextVertical2,"\", %S, %d)");
		aText.AppendFormat(LogGcDrawTextVertical2,&iOverflowHandler,&buf,pData.DrawTextVertical->up);
		}
		break;
	case EWsGcOpDrawTextVerticalPtr:
		{
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(pData.DrawTextVertical->pos));
		_LIT(LogGcDrawTextVertical1,"DrawTextVertical(...");
		TDebugLogTextHandler::Append(aText, LogGcDrawTextVertical1);
		_LIT(LogGcDrawTextVertical2,", %S, %d)");
		aText.AppendFormat(LogGcDrawTextVertical2,&iOverflowHandler,&buf,pData.DrawTextVertical->up);
		}
		break;
	case EWsGcOpDrawTextPtr:
		{
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(pData.DrawText->pos));
		_LIT(LogGcDrawText1,"DrawText(...");
		TDebugLogTextHandler::Append(aText, LogGcDrawText1);
		_LIT(LogGcDrawText2,",%S)");
		aText.AppendFormat(LogGcDrawText2,&iOverflowHandler,&buf);
		}
		break;
	case EWsGcOpDrawBoxTextOptimised1:
		{
		TLongBuf buf(TDebugLogTextHandler::FormatRect(pData.BoxTextO1->box));
		_LIT(LogGcDrawText1,"DrawText(\"");
		aText.Append(LogGcDrawText1);
		TDebugLogTextHandler::Append(aText, TPtrC((TText *)(pData.BoxTextO1+1), pData.BoxTextO1->length));
		_LIT(LogGcDrawText2,"\", %S, %d, %d, 0)");
		aText.AppendFormat(LogGcDrawText2,&iOverflowHandler, &buf,
										pData.BoxTextO1->baselineOffset,CGraphicsContext::ELeft);
		}
		break;
	case EWsGcOpDrawBoxTextOptimised2:
		{
		TLongBuf buf(TDebugLogTextHandler::FormatRect(pData.BoxTextO2->box));
		_LIT(LogGcDrawText1,"DrawText(\"");
		aText.Append(LogGcDrawText1);
		TDebugLogTextHandler::Append(aText, TPtrC((TText *)(pData.BoxTextO2+1), pData.BoxTextO2->length));
		_LIT(LogGcDrawText2,"\", %S, %d, %d, %d)");
		aText.AppendFormat(LogGcDrawText2,&iOverflowHandler, &buf, pData.BoxTextO2->baselineOffset, 
										pData.BoxTextO2->horiz, pData.BoxTextO2->leftMrg);
		}
		break;
	case EWsGcOpDrawBoxTextVertical:
		{
		TLongBuf buf(TDebugLogTextHandler::FormatRect(pData.DrawBoxTextVertical->box));
		_LIT(LogGcDrawTextVertical1,"DrawTextVertical(\"");
		aText.Append(LogGcDrawTextVertical1);
		TDebugLogTextHandler::Append(aText, TPtrC((TText *)(pData.DrawBoxTextVertical+1), pData.DrawBoxTextVertical->length));
		_LIT(LogGcDrawTextVertical2,"\", %S, %d, %d, %d, %d)");
		aText.AppendFormat(LogGcDrawTextVertical2,&iOverflowHandler, &buf,
										pData.DrawBoxTextVertical->baselineOffset, pData.DrawBoxTextVertical->up,
										pData.DrawBoxTextVertical->vert, pData.DrawBoxTextVertical->margin);
		}
		break;
	case EWsGcOpDrawBoxTextPtr:
		{
		TLongBuf buf(TDebugLogTextHandler::FormatRect(pData.BoxText->box));
		_LIT(LogGcDrawText1,"DrawText(...");
		TDebugLogTextHandler::Append(aText, LogGcDrawText1);
		_LIT(LogGcDrawText2,", %S, %d, %d, %d)");
		aText.AppendFormat(LogGcDrawText2,&iOverflowHandler, &buf, pData.BoxText->baselineOffset,
										pData.BoxText->horiz, pData.BoxText->leftMrg);
		}
		break;
	case EWsGcOpDrawBoxTextVerticalPtr:
		{
		TLongBuf buf(TDebugLogTextHandler::FormatRect(pData.DrawBoxTextVertical->box));
		_LIT(LogGcDrawTextVertical1,"DrawTextVertical(...");
		aText.Append(LogGcDrawTextVertical1);
		_LIT(LogGcDrawTextVertical2,", %S, %d, %d, %d, %d)");
		aText.AppendFormat(LogGcDrawTextVertical2,&iOverflowHandler,&buf,
										pData.DrawBoxTextVertical->baselineOffset, pData.DrawBoxTextVertical->up,
										pData.DrawBoxTextVertical->vert, pData.DrawBoxTextVertical->margin);
		}
		break;
	case EWsGcOpCopyRect:
		{
		TShortBuf buf1(TDebugLogTextHandler::FormatPoint(pData.CopyRect->pos));
		TLongBuf buf2(TDebugLogTextHandler::FormatRect(pData.CopyRect->rect));
		_LIT(LogGcCopyRect,"CopyRect(%S, %S)");
		aText.AppendFormat(LogGcCopyRect,&buf1,&buf2);
		}
		break;
	case EWsGcOpDrawEllipse:
		{
		TLongBuf buf(TDebugLogTextHandler::FormatRect(*pData.Rect));
		_LIT(LogGcDrawEllipse,"DrawEllipse(%S)");
		aText.AppendFormat(LogGcDrawEllipse,&buf);
		}
		break;
	case EWsGcOpMoveTo:
		{
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(*pData.Point));
		_LIT(LogGcMoveTo,"MoveTo(%S)");
		aText.AppendFormat(LogGcMoveTo,&buf);
		}
		break;
	case EWsGcOpMoveBy:
		{
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(*pData.Point));
		_LIT(LogGcMoveBy,"MoveBy(%S)");
		aText.AppendFormat(LogGcMoveBy,&buf);
		}
		break;
	case EWsGcOpDrawPolygon:
		{
		_LIT(LogGcDrawPolygon,"DrawPolygon(...)");
		aText.Append(LogGcDrawPolygon);
		}
		break;
	case EWsGcOpDrawRoundRect:
		{
		TLongBuf buf1(TDebugLogTextHandler::FormatRect(pData.RoundRect->rect));
		TShortBuf buf2(TDebugLogTextHandler::FormatSize(pData.RoundRect->ellipse));
		_LIT(LogGcDrawRoundRect,"DrawRoundRect(%S,%S)");
		aText.AppendFormat(LogGcDrawRoundRect,&buf1,&buf2);
		}
		break;
	case EWsGcOpDrawPie:
		{
		TLongBuf buf1(TDebugLogTextHandler::FormatRect(pData.ArcOrPie->rect));
		TShortBuf buf2(TDebugLogTextHandler::FormatPoint(pData.ArcOrPie->start));
		TShortBuf buf3(TDebugLogTextHandler::FormatPoint(pData.ArcOrPie->end));
		_LIT(LogGcDrawPie,"DrawPie(%S,%S,%S)");
		aText.AppendFormat(LogGcDrawPie,&buf1,&buf2,&buf3);
		}
		break;
	case EWsGcOpSetOrigin:
		{
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(*pData.Point));
		_LIT(LogGcSetOrigin,"SetOrigin(%S)");
		aText.AppendFormat(LogGcSetOrigin,&buf);
		}
		break;
	case EWsGcOpStartSegmentedDrawPolygon:
		{
		_LIT(LogGcStartSegmentedDrawPolygon,"StartSegmentedDrawPolygon(%d)");
		aText.AppendFormat(LogGcStartSegmentedDrawPolygon,*pData.StartSegmentedDrawPolygon);
		}
		break;
	case EWsGcOpSegmentedDrawPolygonData:
		{
		_LIT(LogGcSegmentedDrawPolygonData,"SegmentedDrawPolygonData(%d,%d,...)");
		aText.AppendFormat(LogGcSegmentedDrawPolygonData,pData.SegmentedDrawPolygonData->index,
									pData.SegmentedDrawPolygonData->numPoints);
		}
		break;
	case EWsGcOpDrawSegmentedPolygon:
		{
		_LIT(LogGcDrawSegmentedPolygon,"DrawSegmentedPolygon()");
		aText.AppendFormat(LogGcDrawSegmentedPolygon);
		}
		break;
	case EWsGcOpDrawRect:
		{
		TLongBuf buf(TDebugLogTextHandler::FormatRect(*pData.Rect));
		_LIT(LogGcDrawRect,"DrawRect(%S)");
		aText.AppendFormat(LogGcDrawRect,&buf);
		}
		break;
	case EWsGcOpDrawBitmap:
		{
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(pData.Bitmap->pos));
		_LIT(KLitGcDrawBitmap, "DrawBitmap({%d}, %S)");
		aText.AppendFormat(KLitGcDrawBitmap, hHandleToValue(pData.Bitmap->handle), &buf);
		}
		break;
	case EWsGcOpDrawBitmap2:
		{
		TLongBuf buf(TDebugLogTextHandler::FormatRect(pData.Bitmap2->rect));
		_LIT(KLitGcDrawBitmap2, "DrawBitmap2({%d}, %S)");
		aText.AppendFormat(KLitGcDrawBitmap2, hHandleToValue(pData.Bitmap2->handle), &buf);
		}
		break;
	case EWsGcOpDrawBitmap3:
		{
		TLongBuf buf1(TDebugLogTextHandler::FormatRect(pData.Bitmap3->rect));
		TLongBuf buf2(TDebugLogTextHandler::FormatRect(pData.Bitmap3->srcRect));
		_LIT(KLitGcDrawBitmap3, "DrawBitmap3({%d}, %S, %S)");
		aText.AppendFormat(KLitGcDrawBitmap3, hHandleToValue(pData.Bitmap3->handle), &buf1, &buf2);
		}
		break;
	case EWsGcOpDrawBitmapMasked:
	case EWsGcOpWsDrawBitmapMasked:
		{
		TLongBuf buf1(TDebugLogTextHandler::FormatRect(pData.iBitmapMasked->iRect));
		TLongBuf buf2(TDebugLogTextHandler::FormatRect(pData.iBitmapMasked->iSrcRect));
		_LIT(KLitGcDrawBitmapMasked, "DrawBitmapMasked(%S, {%d}, %S, {%d}, %d)");
		aText.AppendFormat(KLitGcDrawBitmapMasked, &buf1, hHandleToValue(pData.iBitmapMasked->iHandle), 
			&buf2, hHandleToValue(pData.iBitmapMasked->iMaskHandle), pData.iBitmapMasked->iInvertMask);
		}
		break;
	case EWsGcOpDrawBoxText:
		{
		TLongBuf buf(TDebugLogTextHandler::FormatRect(pData.BoxText->box));
		_LIT(LogGcDrawText1,"DrawText(\"");
		aText.Append(LogGcDrawText1);
		TDebugLogTextHandler::Append(aText, TPtrC((TText *)(pData.BoxText+1), pData.BoxText->length));
		_LIT(LogGcDrawText2,"\", %S, %d, %d, %d)");
		aText.AppendFormat(LogGcDrawText2,&iOverflowHandler, &buf, pData.BoxText->baselineOffset, 
										pData.BoxText->horiz, pData.BoxText->leftMrg);
		}
		break;		
	default:
		UnKnownOpcode(aText, aOpcode);
		break;
		}
	}

void TWsDecoder::DecodeGc3(TBuf<LogTBufSize> &aText, TInt aOpcode, const TAny *aCmdData)
	{
	TWsGcCmdUnion pData;

	pData.any=aCmdData;
	switch (aOpcode)
		{
	case EWsGcOpSetDrawMode:
		{
		_LIT(LogGcSetDrawMode,"SetDrawMode(%u)");
		aText.AppendFormat(LogGcSetDrawMode,*pData.UInt);
		}
		break;
	case EWsGcOpClearRect:
		{
		TLongBuf buf(TDebugLogTextHandler::FormatRect(*pData.Rect));
		_LIT(LogGcClear,"Clear(%S)");
		aText.AppendFormat(LogGcClear, &buf);
		}
		break;
	case EWsGcOpClear:
		{
		_LIT(LogGcClear,"Clear()");
		aText.AppendFormat(LogGcClear);
		}
		break;
	case EWsGcOpGdiBlt2:
	case EWsGcOpGdiWsBlt2:
		{
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(pData.GdiBlt2->pos));
		_LIT(LogGcGdiBlt,"GdiBlt(%S, {%d})");
		aText.AppendFormat(LogGcGdiBlt,&buf,hHandleToValue(pData.GdiBlt2->handle));
		}
		break;
	case EWsGcOpGdiBlt3:
	case EWsGcOpGdiWsBlt3:
		{
		TShortBuf buf1(TDebugLogTextHandler::FormatPoint(pData.GdiBlt3->pos));
		TLongBuf buf2(TDebugLogTextHandler::FormatRect(pData.GdiBlt3->rect));
		_LIT(LogGcGdiBlt,"GdiBlt(%S, {%d}, %S)");
		aText.AppendFormat(LogGcGdiBlt,&buf1,hHandleToValue(pData.GdiBlt3->handle),&buf2);
		}
		break;
	case EWsGcOpGdiBltMasked:
	case EWsGcOpGdiWsBltMasked:
		{
		TShortBuf buf1(TDebugLogTextHandler::FormatPoint(pData.GdiBltMasked->destination));
		TLongBuf buf2(TDebugLogTextHandler::FormatRect(pData.GdiBltMasked->source));
		_LIT(LogGcBitBltMasked,"BitBltMasked(%S, {%d}, %S, {%d}, %d)");
		aText.AppendFormat(LogGcBitBltMasked, &buf1, hHandleToValue(pData.GdiBltMasked->handle),
										&buf2, hHandleToValue(pData.GdiBltMasked->maskHandle),
										pData.GdiBltMasked->invertMask);
		}
		break;
	case EWsGcOpSize:
		{
		_LIT(LogGcSize,"Size()");
		aText.AppendFormat(LogGcSize);
		}
		break;
	case EWsGcOpUseFont:
		{
		_LIT(LogGcUseFont,"UseFont({0x%x})");
		aText.AppendFormat(LogGcUseFont,hHandleToValue(*pData.handle));
		}
		break;
	case EWsGcOpDiscardFont:
		{
		_LIT(LogGcDiscardFont,"DiscardFont()");
		aText.AppendFormat(LogGcDiscardFont);
		}
		break;
	case EWsGcOpTestInvariant:
		{
		_LIT(LogGcTestInvariant,"unimplemented opcode 'EWsGcOpTestInvariant' (%u)");
		aText.AppendFormat(LogGcTestInvariant, aOpcode);
		}
		break;
	case EWsGcOpReset:
		{
		_LIT(LogGcReset,"Reset()");
		aText.Append(LogGcReset);
		}
		break;
	case EWsGcOpSetDitherOrigin:
		{
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(*pData.Point));
		_LIT(LogGcSetDitherOrigin,"SetDitherOrigin(%S)");
		aText.AppendFormat(LogGcSetDitherOrigin,&buf);
		}
		break;
	case EWsGcOpMapColors:
		{
		TLongBuf buf(TDebugLogTextHandler::FormatRect(pData.MapColors->rect));
		_LIT(LogGcMapColors,"MapColors(%S,?,%d)");
		aText.AppendFormat(LogGcMapColors,&buf,pData.MapColors->numPairs);
		}
		break;
	case EWsGcOpSetUnderlineStyle:
		{
		_LIT(LogGcSetUnderlineStyle,"SetUnderlineStyle(%x)");
		aText.AppendFormat(LogGcSetUnderlineStyle,*pData.SetUnderlineStyle);
		}
		break;
	case EWsGcOpSetStrikethroughStyle:
		{
		_LIT(LogGcSetStrikethroughStyle,"SetStrikethroughStyle(%x)");
		aText.AppendFormat(LogGcSetStrikethroughStyle,*pData.SetStrikethroughStyle);
		}
		break;
	case EWsGcOpDrawWsGraphic:
	case EWsGcOpDrawWsGraphicPtr:
		{
		TLongBuf buf(TDebugLogTextHandler::FormatRect(pData.WsGraphic->iRect));
		_LIT(KLitGcDrawWsGraphic,"DrawWsGraphic(%x, %d, %S, %d)");
		aText.AppendFormat(KLitGcDrawWsGraphic, pData.WsGraphic->iFlags, pData.WsGraphic->iId, &buf, pData.WsGraphic->iDataLen);
		}
		break;
	case EWsGcOpSetFaded:
		{
		_LIT(KLitGcSetFaded, "SetFaded(%d)");
		aText.AppendFormat(KLitGcSetFaded, *pData.Bool);
		}
		break;
	case EWsGcOpSetFadeParams:
		{
		_LIT(KLitGcSetFadeParams, "SetFadeParams(BlackMap %d, WhiteMap %d)");
		const unsigned char *bytePtr = (const unsigned char*)(pData.UInt);
		aText.AppendFormat(KLitGcSetFadeParams, bytePtr[0], bytePtr[1]);
		}
		break;
	case EWsGcOpGdiAlphaBlendBitmaps:
	case EWsGcOpGdiWsAlphaBlendBitmaps:
		{
		TShortBuf buf1(TDebugLogTextHandler::FormatPoint(pData.AlphaBlendBitmaps->point));
		TLongBuf buf2(TDebugLogTextHandler::FormatRect(pData.AlphaBlendBitmaps->source));
		TShortBuf buf3(TDebugLogTextHandler::FormatPoint(pData.AlphaBlendBitmaps->alphaPoint));
		_LIT(KLitGcAlphaBlendBitmaps,"AlphaBlendBitmaps(%S, {%d}, %S, {%d}, %S)");
		aText.AppendFormat(KLitGcAlphaBlendBitmaps, &buf1, hHandleToValue(pData.AlphaBlendBitmaps->bitmapHandle),
										&buf2, hHandleToValue(pData.AlphaBlendBitmaps->alphaHandle),
										&buf3);
		}
		break;
	case EWsGcOpSetOpaque:
		{
		_LIT(KLitGcOpSetOpaque, "SetOpaque(%d)");
		aText.AppendFormat(KLitGcOpSetOpaque, *pData.Bool);
		}
		break;
	default:
		UnKnownOpcode(aText, aOpcode);
		break;
		}
	}

void TWsDecoder::DecodeScreenDevice(TBuf<LogTBufSize> &aText, TInt aOpcode, const TAny *aCmdData, TInt aHandle)
	{
	TWsSdCmdUnion pData;
	_LIT(LogScreenDevice,"CWsScreenDevice[%d]::");

	aText.Format(LogScreenDevice,aHandle);
	pData.any=aCmdData;
	switch (aOpcode)
		{
	case EWsSdOpFree:
		{
		_LIT(LogScreenDeviceDelete,"~CWsScreenDevice()");
		aText.Append(LogScreenDeviceDelete);
		}
		break;
	case EWsSdOpPixel:
		{
		_LIT(LogScreenDevicePixel,"Pixel(%S)");
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(*pData.Point));
		aText.AppendFormat(LogScreenDevicePixel,&buf);
		}
		break;
	case EWsSdOpGetScanLine:
		{		//This fills up the log two much for test code
		aText=KNullDesC;
		/*_LIT(LogScreenDeviceGetScanLine,"GetScanLine(TRgb *aScanLine, %d, %S)");
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(pData.GetScanLine->pos));
		aText.AppendFormat(LogScreenDeviceGetScanLine,pData.GetScanLine->len,&buf);*/
		}
		break;
	case EWsSdOpHorizontalTwipsToPixels:
		{
		_LIT(LogScreenDeviceHorizontalTwipsToPixels,"HorizontalTwipsToPixels()");
		aText.Append(LogScreenDeviceHorizontalTwipsToPixels);
		}
		break;
	case EWsSdOpVerticalTwipsToPixels:
		{
		_LIT(LogScreenDeviceVerticalTwipsToPixels,"VerticalTwipsToPixels()");
		aText.Append(LogScreenDeviceVerticalTwipsToPixels);
		}
		break;
	case EWsSdOpHorizontalPixelsToTwips:
		{
		_LIT(LogScreenDeviceHorizontalPixelsToTwips,"HorizontalPixelsToTwips()");
		aText.Append(LogScreenDeviceHorizontalPixelsToTwips);
		}
		break;
	case EWsSdOpVerticalPixelsToTwips:
		{
		_LIT(LogScreenDeviceVerticalPixelsToTwips,"VerticalPixelsToTwips()");
		aText.Append(LogScreenDeviceVerticalPixelsToTwips);
		}
		break;
	case EWsSdOpPixelSize:
		{
		_LIT(LogScreenDeviceSizeInPixels,"SizeInPixels()");
		aText.Append(LogScreenDeviceSizeInPixels);
		}
		break;
	case EWsSdOpTwipsSize:
		{
		_LIT(LogScreenDeviceSizeInTwips,"SizeInTwips()");
		aText.Append(LogScreenDeviceSizeInTwips);
		}
		break;
	case EWsSdOpDisplayMode:
		{
		_LIT(LogScreenDeviceDisplayMode,"DisplayMode()");
		aText.Append(LogScreenDeviceDisplayMode);
		}
		break;
	case EWsSdOpRectCompare:
		{
		_LIT(LogScreenDeviceRectCompare,"RectCompare(%S, %S)");
		TLongBuf buf1(TDebugLogTextHandler::FormatRect(pData.RectCompare->rect1));
		TLongBuf buf2(TDebugLogTextHandler::FormatRect(pData.RectCompare->rect2));
		aText.AppendFormat(LogScreenDeviceRectCompare,&buf1,&buf2);
		}
		break;
	case EWsSdOpPointerRect:
		{
		_LIT(LogScreenDevicePointerRect,"PointerRect()");
		aText.Append(LogScreenDevicePointerRect);
		}
		break;
	case EWsSdOpCopyScreenToBitmap:
		{
		_LIT(LogScreenDeviceCopyScreenToBitmap,"CopyScreenToBitmap(%d)");
		aText.AppendFormat(LogScreenDeviceCopyScreenToBitmap,pData.CopyScreenToBitmap->handle);
		}
		break;
	case EWsSdOpCopyScreenToBitmap2:
		{
		_LIT(LogScreenDeviceCopyScreenToBitmap2,"CopyScreenToBitmap(%d,%S)");
		TLongBuf buf(TDebugLogTextHandler::FormatRect(pData.CopyScreenToBitmap2->rect));
		aText.AppendFormat(LogScreenDeviceCopyScreenToBitmap2,pData.CopyScreenToBitmap2->handle,&buf);
		}
		break;
	case EWsSdOpSetScreenSizeAndRotation:
		{
		_LIT(LogScreenDeviceSetScreenSizeAndRotation,"SetScreenSizeAndRotation({%S, %S, %d})");
		TShortBuf buf1(TDebugLogTextHandler::FormatSize(pData.PixelsTwipsAndRotation->iPixelSize));
		TShortBuf buf2(TDebugLogTextHandler::FormatSize(pData.PixelsTwipsAndRotation->iTwipsSize));
		aText.AppendFormat(LogScreenDeviceSetScreenSizeAndRotation,&buf1,&buf2,pData.PixelsTwipsAndRotation->iRotation);
		}
		break;
	case EWsSdOpSetScreenSizeAndRotation2:
		{
		_LIT(LogScreenDeviceSetScreenSizeAndRotation,"SetScreenSizeAndRotation({%S, %d})");
		TShortBuf buf(TDebugLogTextHandler::FormatSize(pData.PixelsAndRotation->iPixelSize));
		aText.AppendFormat(LogScreenDeviceSetScreenSizeAndRotation,&buf,pData.PixelsAndRotation->iRotation);
		}
		break;
	case EWsSdOpGetDefaultScreenSizeAndRotation:
	case EWsSdOpGetDefaultScreenSizeAndRotation2:
		{
		_LIT(LogScreenDeviceGetDefaultScreenSizeAndRotation,"GetDefaultScreenSizeAndRotation(...)");
		aText.Append(LogScreenDeviceGetDefaultScreenSizeAndRotation);
		}
		break;
	case EWsSdOpGetNumScreenModes:
		{
		_LIT(LogScreenDeviceNumScreenModes,"NumScreenModes");
		aText.Append(LogScreenDeviceNumScreenModes);
		}
		break;
	case EWsSdOpSetScreenMode:
		{
		_LIT(LogScreenDeviceSetScreenMode,"ScreenMode(%d)");
		aText.AppendFormat(LogScreenDeviceSetScreenMode,*pData.Int);
		}
		break;
	case EWsSdOpGetScreenModeSizeAndRotation:
	case EWsSdOpGetScreenModeSizeAndRotation2:
		{
		_LIT(LogScreenDeviceGetScreenModeSizeAndRotation,"GetScreenModeSizeAndRotation(%d)");
		aText.AppendFormat(LogScreenDeviceGetScreenModeSizeAndRotation,*pData.Int);
		}
		break;
	case EWsSdOpSetScreenModeEnforcement:
		{
		_LIT(LogScreenDeviceSetScreenModeEnforcement,"SetScreenModeEnforcement(%d)");
		aText.AppendFormat(LogScreenDeviceSetScreenModeEnforcement,*pData.Int);
		}
		break;
	case EWsSdOpScreenModeEnforcement:
		{
		_LIT(LogScreenDeviceScreenModeEnforcement,"ScreenModeEnforcement");
		aText.Append(LogScreenDeviceScreenModeEnforcement);
		}
		break;
	case EWsSdOpSetModeRotation:
		{
		_LIT(LogScreenDeviceSetModeRotation,"SetModeRotation(%d,%d)");
		aText.AppendFormat(LogScreenDeviceSetModeRotation,pData.SetScreenRotation->mode,pData.SetScreenRotation->rotation);
		}
		break;
	case EWsSdOpGetRotationList:
		{
		_LIT(LogScreenDeviceGetRotationList,"GetRotationList(%d)");
		aText.AppendFormat(LogScreenDeviceGetRotationList,*pData.Int);
		}
		break;
	case EWsSdOpPaletteAttributes:
		{
		_LIT(LogScreenDevicePaletteAttributes,"PaletteAttributes(...)");
		aText.Append(LogScreenDevicePaletteAttributes);
		}
		break;
	case EWsSdOpSetPalette:
		{
		_LIT(LogScreenDeviceSetPalette,"SetPalette(...)");
		aText.Append(LogScreenDeviceSetPalette);
		}
		break;
	case EWsSdOpGetPalette:
		{
		_LIT(LogScreenDeviceGetPalette,"GetPalette()");
		aText.Append(LogScreenDeviceGetPalette);
		}
		break;
	case EWsSdOpGetScreenMode:
		{
		_LIT(LogScreenDeviceGetScreenMode,"CurrentScreenMode()");
		aText.Append(LogScreenDeviceGetScreenMode);
		}
		break;
	case EWsSdOpGetDefaultScreenModeOrigin:
		{
		_LIT(LogScreenDeviceGetDefaultScreenModeOrigin,"GetDefaultScreenModeOrigin()");
		aText.Append(LogScreenDeviceGetDefaultScreenModeOrigin);
		}	
		break;
	case EWsSdOpGetScreenModeOrigin:
		{
		_LIT(LogScreenDeviceGetScreenModeOrigin,"GetScreenModeOrigin(%d)");
		aText.AppendFormat(LogScreenDeviceGetScreenModeOrigin, *pData.Int);
		}	
		break;
	case EWsSdOpGetScreenModeScale:
		{
		_LIT(LogScreenDeviceGetScreenModeScale,"GetScreenModeScale(%d)");
		aText.AppendFormat(LogScreenDeviceGetScreenModeScale, *pData.Int);
		}	
		break;
	case EWsSdOpGetCurrentScreenModeScale:
		{
		_LIT(LogScreenDeviceGetCurrentScreenModeScale,"GetCurrentScreenModeScale()");
		aText.Append(LogScreenDeviceGetCurrentScreenModeScale);
		}	
		break;		
	case EWsSdOpSetAppScreenMode:
		{
		_LIT(LogScreenDeviceSetAppScreenMode,"SetAppScreenMode(%d)");
		aText.AppendFormat(LogScreenDeviceSetAppScreenMode, *pData.Int);
		}	
		break;		
	case EWsSdOpGetScreenModeScaledOrigin:
		{
		_LIT(LogScreenDeviceGetScreenModeScaledOrigin,"GetScreenModeScaledOrigin(%d)");
		aText.AppendFormat(LogScreenDeviceGetScreenModeScaledOrigin, *pData.Int);
		}	
		break;		
	case EWsSdOpGetCurrentScreenModeScaledOrigin:
		{
		_LIT(LogScreenDeviceGetCurrentScreenModeScaledOrigin,"GetCurrentScreenModeScaledOrigin()");
		aText.Append(LogScreenDeviceGetCurrentScreenModeScaledOrigin);
		}	
		break;		
	case EWsSdOpSetCurrentScreenModeAttributes:
		{
		_LIT(LogScreenDeviceSetCurrentScreenModeAttributes,"SetCurrentScreenModeAttributes(%S)");
		TShortBuf buf(TDebugLogTextHandler::FormatSize(pData.ScreenSizeMode->iScreenSize));
		aText.AppendFormat(LogScreenDeviceSetCurrentScreenModeAttributes,&buf);
		}	
		break;		
	case EWsSdOpGetCurrentScreenModeAttributes:
		{
		_LIT(LogScreenDeviceGetCurrentScreenModeOrigin,"GetCurrentScreenModeAttributes()");
		aText.Append(LogScreenDeviceGetCurrentScreenModeOrigin);
		}	
		break;		
	case EWsSdOpGetScreenNumber:
		{
		_LIT(LogScreenDeviceGetScreenNumber,"GetScreenNumber()");
		aText.Append(LogScreenDeviceGetScreenNumber);
		}	
		break;		
	case EWsSdOpGetScreenSizeModeList:
		{
		_LIT(LogScreenDeviceGetScreenSizeModeList,"GetScreenSizeModeList(RArray<TInt>* aModeList)");
		aText.Append(LogScreenDeviceGetScreenSizeModeList);
		}	
		break;		
	case EWsSdOpGetScreenModeDisplayMode:
		{
		_LIT(LogScreenDeviceGetScreenModeDisplayMode,"GetScreenModeDisplayMode(%d)");
		aText.AppendFormat(LogScreenDeviceGetScreenModeDisplayMode, *pData.Int);
		}	
		break;		
	case EWsClOpSetBackLight:
		{
		_LIT(LogScreenDeviceSetBackLight,"SetBackLight(%u)");
		aText.AppendFormat(LogScreenDeviceSetBackLight, *pData.UInt);
		}	
		break;		
	default:
		UnKnownOpcode(aText, aOpcode);
		break;
		}
	}

void TWsDecoder::DecodeAnimDll(TBuf<LogTBufSize>& aText,TInt aOpcode,const TAny* /*aCmdData*/,TInt aHandle)
	{
	aText.Format(_L("CAnimDll[%d]::"),aHandle);
	switch (aOpcode)
		{
	case EWsAnimDllOpCreateInstance:
		TDebugLogTextHandler::Append(aText, _L("CreateInstance(...)"));
		break;
	case EWsAnimDllOpDestroyInstance:
		TDebugLogTextHandler::Append(aText, LogClose);
		break;
	case EWsAnimDllOpCommandReply:
		TDebugLogTextHandler::Append(aText, _L("CommandReply(...)"));
		break;
	case EWsAnimDllOpCommand:
		TDebugLogTextHandler::Append(aText, _L("Command(...)"));
		break;
	case EWsAnimDllOpFree:
		aText.Append(_L("~CAnimDll()"));
		break;
	case EWsAnimDllOpCreateInstanceSprite:
		aText.Append(_L("EWsAnimDllOpCreateInstanceSprite enum"));
		break;
	default:
		UnKnownOpcode(aText, aOpcode);
		break;
		}
	}

void TWsDecoder::DecodeSpriteBase(TBuf<LogTBufSize> &aText, TInt aOpcode, const TAny *aCmdData, TInt aHandle)
	{
	TWsSpriteCmdUnion pData;

	aText.Format(_L("RWsSpriteBase[%d]::"),aHandle);
	pData.any=aCmdData;
	switch (aOpcode)
		{
	case EWsSpriteOpFree:
		aText.Append(LogClose);
		break;
	case EWsSpriteOpSetPosition:
		aText.Append(_L("SetPosition(...)"));
		break;
	case EWsSpriteOpUpdateMember:
		aText.AppendFormat(_L("UpdateMember(%d)"),pData.UpdateMember->index);
		break;
	case EWsSpriteOpUpdateMember2:
		aText.AppendFormat(_L("UpdateMember(%d,{%d,%d,%d,%d,{%d,%d},%d})"),pData.UpdateMember->index,
								pData.UpdateMember->data.iBitmap,pData.UpdateMember->data.iMaskBitmap,
								pData.UpdateMember->data.iInvertMask,pData.UpdateMember->data.iDrawMode,
								pData.UpdateMember->data.iOffset.iX,pData.UpdateMember->data.iOffset.iY,
								pData.UpdateMember->data.iInterval.Int());
		break;
	case EWsSpriteOpAppendMember:
		aText.Append(_L("AppendMember(...)"));
		break;
	case EWsSpriteOpActivate:
		aText.Append(_L("Activate()"));
		break;
	default:
		UnKnownOpcode(aText, aOpcode);
		break;
		}
	}

void TWsDecoder::DecodeBitmap(TBuf<LogTBufSize>& aText,TInt aOpcode,const TAny* /*aCmdData*/,TInt aHandle)
	{
	aText.Format(_L("CWsBitmap[%d]::"),aHandle);
	switch (aOpcode)
		{
	case EWsBitmapOpFree:
		aText.Append(LogClose);
		break;
	default:
		UnKnownOpcode(aText, aOpcode);
		break;
		}
	}

void TWsDecoder::DecodeDirect(TBuf<LogTBufSize> &aText, TInt aOpcode, const TAny *aCmdData, TInt aHandle)
	{
	TWsDirectCmdUnion pData;
	
	_LIT(KClassName,"RDirectScreenAccess[%d]::");
	aText.Format(KClassName,aHandle);
	pData.any=aCmdData;
	switch (aOpcode)
		{
	case EWsDirectOpFree:
		aText.Append(LogClose);
		break;
	case EWsDirectOpRequest:
		{
		_LIT(LogDirectRequest,"Request(TInt *stat,%d)");
		aText.AppendFormat(LogDirectRequest,hHandleToValue(*pData.Int));
		}
		break;
	case EWsDirectOpInitFailed:
		{
		_LIT(LogDirectInitFailed,"InitFailed()");
		aText.Append(LogDirectInitFailed);
		}
		break;
	case EWsDirectOpGetRegion:
		{
		_LIT(LogDirectGetRegion,"GetRegion(%d)");
		aText.AppendFormat(LogDirectGetRegion,*pData.Int);
		}
		break;
	case EWsDirectOpCancel:
		{
		_LIT(LogDirectCancel,"Cancel()");
		aText.Append(LogDirectCancel);
		}
		break;
	case EWsDirectOpGetSendQueue:
		{
		_LIT(LogGetSendQueue,"EWsDirectOpGetSendQueue enum");
		aText.Append(LogGetSendQueue);
		}
		break;	
	case EWsDirectOpGetRecQueue:
		{
		_LIT(LogGetRecQueue,"EWsDirectOpGetRecQueue enum");
		aText.Append(LogGetRecQueue);
		}
		break;	
	default:
		UnKnownOpcode(aText, aOpcode);
		break;
		}
	}

void TWsDecoder::DecodeClick(TBuf<LogTBufSize> &aText, TInt aOpcode, const TAny *aCmdData, TInt aHandle)
	{
	TWsClickCmdUnion pData;

	_LIT(KClassName,"RSoundPlugIn[%d]::");
	aText.Format(KClassName,aHandle);
	pData.any=aCmdData;
	switch (aOpcode)
		{
	case EWsClickOpFree:
		aText.Append(LogClose);
		break;
	case EWsClickOpIsLoaded:
		{
		_LIT(LogClickIsLoaded,"IsLoaded()");
		aText.Append(LogClickIsLoaded);
		}
		break;
	case EWsClickOpUnLoad:
		{
		_LIT(LogClickUnLoad,"UnLoad()");
		aText.Append(LogClickUnLoad);
		}
		break;
	case EWsClickOpLoad:
		{
		_LIT(LogClickLoad1,"Load(\"");
		_LIT(LogClickLoad2,"\")");
		TDebugLogTextHandler::Append(aText, LogClickLoad1);
		TDebugLogTextHandler::Append(aText, TPtrC((TText *)(pData.Int+1), *pData.Int));
		TDebugLogTextHandler::Append(aText, LogClickLoad2);
		}
		break;
	case EWsClickOpCommandReply:
		{
		_LIT(LogClickCommandReply,"CommandReply(...)");
		aText.Append(LogClickCommandReply);
		}
		break;
	case EWsClickOpSetKeyClick:
		{			
		_LIT(LogSetKeyClick,"SetKeyClick(%u)");
		aText.AppendFormat(LogSetKeyClick, *pData.Bool);
		}
		break;
	case EWsClickOpSetPenClick:
		{			
		_LIT(LogSetPenClick,"SetPenClick(%u)");
		aText.AppendFormat(LogSetPenClick, *pData.Bool);
		}	
		break;
	case EWsClickOpKeyClickEnabled:
		{
		_LIT(LogKeyClickEnabled,"KeyClickEnabled()");
		aText.Append(LogKeyClickEnabled);
		}
		break;
	case EWsClickOpPenClickEnabled:
		{
		_LIT(LogPenClickEnabled,"PenClickEnabled()");
		aText.Append(LogPenClickEnabled);
		}
		break;
	default:
		UnKnownOpcode(aText, aOpcode);
		break;
		}
	}

TDesC &TWsDecoder::replyBufDetails(TInt aClass, TInt aOpcode, const TDesC8 *aReplyDes8, const TDesC *aReplyDesText)
	{
	iCommandDetailsText.Format(_L("Reply Buffer: "));
	switch (aClass)
		{
	case WS_HANDLE_GROUP_WINDOW:
		ReplyBufWindowGroup(iCommandDetailsText, aOpcode, aReplyDes8, aReplyDesText);
		break;
	case WS_HANDLE_CLIENT:
		ReplyBufClient(iCommandDetailsText, aOpcode, aReplyDes8, aReplyDesText);
		break;
	case WS_HANDLE_WINDOW:
		ReplyBufWindow(iCommandDetailsText, aOpcode, aReplyDes8, aReplyDesText);
		break;
	case WS_HANDLE_SCREEN_DEVICE:
		ReplyBufScreenDevice(iCommandDetailsText, aOpcode, aReplyDes8, aReplyDesText);
		break;
	default:
		iCommandDetailsText.Format(_L("Reply buffer not handled (%u)"), aClass);
		UnKnown();
		break;
		}
	return iCommandDetailsText;
	}

TDesC &TWsDecoder::commandDetails(TInt aClass, TInt aOpcode, const TAny *aCmdData, TInt aHandle)
	{
	switch (aClass)
		{
	case WS_HANDLE_ROOT_WINDOW:
		iCommandDetailsText.Format(_L("invalid class"));
		break;
	case WS_HANDLE_GROUP_WINDOW:
		DecodeWindowGroup(iCommandDetailsText, aOpcode, aCmdData, aHandle);
		break;
	case WS_HANDLE_CLIENT:
		DecodeClient(iCommandDetailsText, aOpcode, aCmdData, aHandle);
		break;
	case WS_HANDLE_WINDOW:
		DecodeWindow(iCommandDetailsText, aOpcode, aCmdData, aHandle);
		break;
	case WS_HANDLE_GC:
		DecodeGc(iCommandDetailsText, aOpcode, aCmdData, aHandle);
		break;
	case WS_HANDLE_SCREEN_DEVICE:
		DecodeScreenDevice(iCommandDetailsText, aOpcode, aCmdData, aHandle);
		break;
	case WS_HANDLE_ANIM_DLL:
		DecodeAnimDll(iCommandDetailsText, aOpcode, aCmdData, aHandle);
		break;
	case WS_HANDLE_SPRITE:
	case WS_HANDLE_POINTER_CURSOR:
	case WS_HANDLE_TEXT_CURSOR:
		DecodeSpriteBase(iCommandDetailsText, aOpcode, aCmdData, aHandle);
		break;
	case WS_HANDLE_BITMAP:
		DecodeBitmap(iCommandDetailsText, aOpcode, aCmdData, aHandle);
		break;
	case WS_HANDLE_DIRECT:
		DecodeDirect(iCommandDetailsText, aOpcode, aCmdData, aHandle);
		break;
	case WS_HANDLE_CLICK:
		DecodeClick(iCommandDetailsText, aOpcode, aCmdData, aHandle);
		break;
	default:
		iCommandDetailsText.Format(_L("unknown class (%u)"), aClass);
		UnKnown();
		break;
		}
	iRequestFuncClass=aClass;
	iPrevOpcode=aOpcode;
	return iCommandDetailsText;
	}

TDesC &TWsDecoder::eventDetails(const TWsEvent *aWsEvent)
	{
	switch (aWsEvent->Type())
		{
	case EEventKey:
		iEventDetailsText.Format(_L("EEventKey (code %u, scanCode %u, mdfs 0x%04x, repeats %u)"),
											aWsEvent->Key()->iCode,
											aWsEvent->Key()->iScanCode,
											aWsEvent->Key()->iModifiers,
											aWsEvent->Key()->iRepeats);
		break;
	case EEventKeyUp:
		iEventDetailsText.Format(_L("EEventKeyUp (scanCode %u, mdfs 0x%04x)"),
											aWsEvent->Key()->iScanCode,
											aWsEvent->Key()->iModifiers);
		break;
	case EEventKeyDown:
		iEventDetailsText.Format(_L("EEventKeyDown (scanCode %u, mdfs 0x%04x)"),
											aWsEvent->Key()->iScanCode,
											aWsEvent->Key()->iModifiers);
		break;
	case EEventPointer:
		{
		TShortBuf buf1(TDebugLogTextHandler::PointerEventType(aWsEvent->Pointer()->iType));
		TShortBuf buf2(TDebugLogTextHandler::FormatPoint(aWsEvent->Pointer()->iPosition));
		TShortBuf buf3(TDebugLogTextHandler::FormatPoint(aWsEvent->Pointer()->iParentPosition));
		iEventDetailsText.Format(_L("EEventPointer (type %S, modifiers 0x%x, pos %S, global-pos %S)"),
											&buf1,aWsEvent->Pointer()->iModifiers,&buf2,&buf3);
		}
		break;
	case EEventDragDrop:
		{
		TShortBuf buf1(TDebugLogTextHandler::PointerEventType(aWsEvent->Pointer()->iType));
		TShortBuf buf2(TDebugLogTextHandler::FormatPoint(aWsEvent->Pointer()->iPosition));
		TShortBuf buf3(TDebugLogTextHandler::FormatPoint(aWsEvent->Pointer()->iParentPosition));
		iEventDetailsText.Format(_L("EEventDragDrop (type %S, modifiers 0x%x, pos %S, global-pos %S)"),
											&buf1,aWsEvent->Pointer()->iModifiers,&buf2,&buf3);
		}
		break;
	case EEventPointerEnter:
		iEventDetailsText.Format(_L("EEventPointerEnter"));
		break;
	case EEventPointerExit:
		iEventDetailsText.Format(_L("EEventPointerExit"));
		break;
	case EEventFocusLost:
		iEventDetailsText.Format(_L("EEventFocusLost"));
		break;
	case EEventFocusGained:
		iEventDetailsText.Format(_L("EEventFocusGained"));
		break;
	case EEventModifiersChanged:
		iEventDetailsText.Format(_L("EEventModifiersChanged"));
		break;
	case EEventSwitchOn:
		iEventDetailsText.Format(_L("EEventSwitchOn"));
		break;
	case EEventPassword:
		iEventDetailsText.Format(_L("EEventPassword"));
		break;
	case EEventWindowGroupsChanged:
		iEventDetailsText.Format(_L("EEventWindowGroupsChanged"));
		break;
	case EEventErrorMessage:
		iEventDetailsText.Format(_L("EEventErrorMessage (%d,%d)"),aWsEvent->ErrorMessage()->iErrorCategory,aWsEvent->ErrorMessage()->iError);
		break;
	case EEventMessageReady:
		iEventDetailsText.Format(_L("EEventMessageReady"));
		break;
	case EEventScreenDeviceChanged:
		iEventDetailsText.Format(_L("EEventScreenDeviceChanged"));
		break;
	case EEventSwitchOff:
		{
		_LIT(LogEventSwitchOff,"EEventSwitchOff");
		iEventDetailsText.Format(LogEventSwitchOff);
		}
		break;
	case EEventKeySwitchOff:
		{
		_LIT(LogEventKeySwitchOff,"EEventKeySwitchOff");
		iEventDetailsText.Format(LogEventKeySwitchOff);
		}
		break;
	case EEventFocusGroupChanged:
		{
		_LIT(LogEventFocusGroupChanged,"EEventFocusGroupChanged");
		iEventDetailsText.Format(LogEventFocusGroupChanged);
		}
		break;
	case EEventCaseOpened:
		{
		_LIT(LogEventCaseOpened,"EEventCaseOpened");
		iEventDetailsText.Format(LogEventCaseOpened);
		}
		break;
	case EEventCaseClosed:
		{
		_LIT(LogEventCaseClosed,"EEventCaseClosed");
		iEventDetailsText.Format(LogEventCaseClosed);
		}
		break;
	default:
		if (aWsEvent->Type()>=EEventUser)
			iEventDetailsText.Format(_L("EEventUser[%d]"), aWsEvent->Type()-EEventUser);
		else
			{
			UnKnown();
			iEventDetailsText.Format(_L("Unknown event type=%d "), aWsEvent->Type());
			}
		break;
		}
	return iEventDetailsText;
	}

TDesC &TWsDecoder::eventDetails(const TRawEvent *aEvent)
	{
	iEventDetailsText.Zero();
	switch (aEvent->Type())
		{
	case TRawEvent::ENone:
		{
		_LIT(LogEventDetailsNone,"ENone");
		iEventDetailsText.Append(LogEventDetailsNone);
		}
		break;
	case TRawEvent::EPointerMove:
		{
		_LIT(LogEventDetailsPointerMove,"EPointerMove(");
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(aEvent->Pos()));
		iEventDetailsText.Append(LogEventDetailsPointerMove);
		iEventDetailsText.Append(buf);
		iEventDetailsText.Append(LogCloseBracket);
		}
		break;
    case TRawEvent::EPointerSwitchOn:
		{
		_LIT(LogEventDetailsPointerSwitchOn,"EPointerSwitchOn(");
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(aEvent->Pos()));
		iEventDetailsText.Append(LogEventDetailsPointerSwitchOn);
		iEventDetailsText.Append(buf);
		iEventDetailsText.Append(LogCloseBracket);
		}
		break;
	case TRawEvent::EKeyDown:
		{
		_LIT(LogEventDetailsKeyDown,"EKeyDown(%d)");
		iEventDetailsText.Format(LogEventDetailsKeyDown,aEvent->ScanCode());
		}
		break;
	case TRawEvent::EKeyUp:
		{
		_LIT(LogEventDetailsKeyUp,"EKeyUp(%d)");
		iEventDetailsText.Format(LogEventDetailsKeyUp,aEvent->ScanCode());
		}
		break;
	case TRawEvent::ERedraw:
		{
		_LIT(LogEventDetailsRedraw,"ERedraw");
		iEventDetailsText.Append(LogEventDetailsRedraw);
		}
		break;
	case TRawEvent::ESwitchOn:
 		{
		_LIT(LogEventDetailsSwitchOn,"ESwitchOn");
		iEventDetailsText.Append(LogEventDetailsSwitchOn);
		}
		break;
	case TRawEvent::EActive:
		{
		_LIT(LogEventDetailsActive,"EActive");
		iEventDetailsText.Append(LogEventDetailsActive);
		}
		break;
	case TRawEvent::EInactive:
		{
		_LIT(LogEventDetailsInactive,"EInactive");
		iEventDetailsText.Append(LogEventDetailsInactive);
		}
		break;
	case TRawEvent::EUpdateModifiers:
		{
		_LIT(LogEventDetailsModifiers,"EUpdateModifiers(%d)");
		iEventDetailsText.Format(LogEventDetailsModifiers,aEvent->Modifiers());
		}
		break;
	case TRawEvent::EButton1Down:
		{
		_LIT(LogEventDetailsButton1Down,"EButton1Down(");
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(aEvent->Pos()));
		iEventDetailsText.Append(LogEventDetailsButton1Down);
		iEventDetailsText.Append(buf);
		iEventDetailsText.Append(LogCloseBracket);
		}
		break;
	case TRawEvent::EButton1Up:
		{
		_LIT(LogEventDetailsButton1Up,"EButton1Up(");
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(aEvent->Pos()));
		iEventDetailsText.Append(LogEventDetailsButton1Up);
		iEventDetailsText.Append(buf);
		iEventDetailsText.Append(LogCloseBracket);
		}
		break;
	case TRawEvent::EButton2Down:
		{
		_LIT(LogEventDetailsButton2Down,"EButton2Down(");
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(aEvent->Pos()));
		iEventDetailsText.Append(LogEventDetailsButton2Down);
		iEventDetailsText.Append(buf);
		iEventDetailsText.Append(LogCloseBracket);
		}
		break;
	case TRawEvent::EButton2Up:
		{
		_LIT(LogEventDetailsButton2Up,"EButton2Up(");
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(aEvent->Pos()));
		iEventDetailsText.Append(LogEventDetailsButton2Up);
		iEventDetailsText.Append(buf);
		iEventDetailsText.Append(LogCloseBracket);
		}
		break;
	case TRawEvent::EButton3Down:
		{
		_LIT(LogEventDetailsButton3Down,"EButton3Down(");
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(aEvent->Pos()));
		iEventDetailsText.Append(LogEventDetailsButton3Down);
		iEventDetailsText.Append(buf);
		iEventDetailsText.Append(LogCloseBracket);
		}
		break;
	case TRawEvent::EButton3Up:
		{
		_LIT(LogEventDetailsButton3Up,"EButton3Up(");
		TShortBuf buf(TDebugLogTextHandler::FormatPoint(aEvent->Pos()));
		iEventDetailsText.Append(LogEventDetailsButton3Up);
		iEventDetailsText.Append(buf);
		iEventDetailsText.Append(LogCloseBracket);
		}
		break;
	case TRawEvent::ESwitchOff:
		{
		_LIT(LogEventDetailsSwitchOff,"ESwitchOff");
		iEventDetailsText.Append(LogEventDetailsSwitchOff);
		}
		break;
	case TRawEvent::EKeyRepeat:
 		{
  		_LIT(LogEventDetailsKeyRepeat,"ERepeatKey(scancode=%d,repeat=%d");
 		iEventDetailsText.Format(LogEventDetailsKeyRepeat,aEvent->ScanCode(),aEvent->Repeats());
 		}
		break;
	case TRawEvent::EPointer3DOutOfRange:
 		{
  		_LIT(LogEventDetailsOutOfRange,"EPointer3DOutOfRange");
 		iEventDetailsText.Format(LogEventDetailsOutOfRange);
 		}
		break;
	default:
		{
		_LIT(LogEventDetailsUnknown,"Unknown event type: %d");
		iEventDetailsText.Format(LogEventDetailsUnknown,aEvent->Type());
		UnKnown();
		}
		break;
		}
	return iEventDetailsText;
	}

void TWsDecoder::ReplyBufWindowGroup(TBuf<LogTBufSize> &aText, TInt aOpcode, const TDesC8 *aReplyDes8, const TDesC *aReplyDesText)
	{
	ReplyBufWindow(aText, aOpcode, aReplyDes8, aReplyDesText);
	}

void TWsDecoder::ReplyBufWindow(TBuf<LogTBufSize> &aText, TInt aOpcode, const TDesC8 *aReplyDes8, const TDesC *aReplyDesText)
	{
	switch (aOpcode)
		{
	case EWsWinOpPosition:
		AppendPoint(aText,aReplyDes8);
		break;
	case EWsWinOpAbsPosition:
		AppendPoint(aText,aReplyDes8);
		break;
	case EWsWinOpSize:
		AppendPoint(aText,aReplyDes8);
		break;
	case EWsWinOpInquireOffset:
		AppendPoint(aText,aReplyDes8);
		break;
	case EWsWinOpName:
		AppendDesc(aText, aReplyDesText);
		break;
	case EWsWinOpRetrievePointerMoveBuffer:
		aText.Append(_L("..."));
		break;
	case EWsWinOpGetInvalidRegion:
		{
		TRect *rect=(TRect *)aReplyDes8->Ptr();
		TInt count=aReplyDes8->Size()/sizeof(TRect);
		TBuf<2> comma;
		_LIT(LogTwoStrings,"%S%S");
		for(TInt index=0;index<count && !iOverflowHandler.IsError();index++)
			{
			TLongBuf buf(TDebugLogTextHandler::FormatRect(*rect++));
			aText.AppendFormat(LogTwoStrings,&iOverflowHandler,&comma,&buf);
			if (index==0)
				comma=LogComma;
			}
		break;
		}
	default:
		UnKnownReplyBuf(aText, aOpcode);
		break;
		}
	}

void TWsDecoder::ReplyBufClient(TBuf<LogTBufSize> &aText, TInt aOpcode, const TDesC8 *aReplyDes8, const TDesC *aReplyDesText)
	{
	switch (aOpcode)
		{
	case EWsClOpGetEvent:
		{
		const TWsEvent *wsEvent=(TWsEvent *)aReplyDes8->Ptr();
		TPtrC details(eventDetails(wsEvent));
		aText=KNullDesC;
		if (details!=KNullDesC)
			{
			TDebugLogTextHandler::Append(aText, details);
			TBuf<128> buf;
			_LIT(LogReplyClientDestTime,"(dest %d, time ");
			_LIT(LogReplyClientTime,"%D%M%Y%/0%1%/1%2%/2%3%/3 %:0%H%:1%T%:2%S%.%*C2%:3)");
			buf.Format(LogReplyClientDestTime, wsEvent->Handle());
			TDebugLogTextHandler::Append(aText, buf);
			TRAPD(err,wsEvent->Time().FormatL(buf,LogReplyClientTime));
			if (err==KErrNone)
				TDebugLogTextHandler::Append(aText, buf);
			else
				{
				_LIT(LogReplyClientTimeErr,"######)");
				TDebugLogTextHandler::Append(aText,LogReplyClientTimeErr);
				}
			}
		}
		break;
	case EWsClOpGetRedraw:
		{
		const TWsRedrawEvent *redrawEvent=(TWsRedrawEvent *)aReplyDes8->Ptr();
		TLongBuf buf(TDebugLogTextHandler::FormatRect(redrawEvent->Rect()));
		aText.Format(_L("Redraw (handle %d): "), redrawEvent->Handle());
		aText.AppendFormat(_L("rect %S"), &buf);
		}
		break;
	case EWsClOpGetPriorityKey:
		{
		const TWsPriorityKeyEvent *abortEvent=(TWsPriorityKeyEvent *)aReplyDes8->Ptr();
		aText.Format(_L("Abort key handle %d"), abortEvent->Handle());
		}
		break;
	case EWsClOpShadowVector:
		AppendPoint(aText,aReplyDes8);
		break;
	case EWsClOpGetWindowGroupClientThreadId:
		{
		const TThreadId *threadId=(TThreadId *)aReplyDes8->Ptr();
		aText.Format(_L("TThreadId=%d"), *(TInt *)threadId);
		}
		break;
	case EWsClOpGetWindowGroupNameFromIdentifier:
		AppendDesc(aText, aReplyDesText);
		break;
	case EWsClOpWindowGroupList:
	case EWsClOpWindowGroupListAndChain: 
	case EWsClOpWindowGroupListAllPriorities:
		{
		TInt *handlPtr=(TInt *)aReplyDes8->Ptr();
		TInt count=aReplyDes8->Size()/sizeof(TInt);
		TBuf<2> comma;
		_LIT(LogStringInt,"%S%d");
		for(TInt index=0;index<count && !iOverflowHandler.IsError();index++)
			{
			aText.AppendFormat(LogStringInt,&iOverflowHandler,&comma,*handlPtr++);
			if (index==0)
				comma=LogComma;
			}
		}
		break;
	case EWsClOpGetKeyboardRepeatRate:
		{
		SKeyRepeatSettings *settings=(SKeyRepeatSettings *)aReplyDes8->Ptr();
		aText.AppendFormat(_L("initial=%d,normal=%d"),settings->iInitialTime.Int(),settings->iTime.Int());
		}
		break;
	case EWsClOpGetDoubleClickSettings:
		{
		SDoubleClickSettings *settings=(SDoubleClickSettings *)aReplyDes8->Ptr();
		aText.AppendFormat(_L("interval=%d,distance=%d"),settings->iInterval.Int(),settings->iDistance);
		}
		break;
	case EWsClOpFetchMessage:
		{
		_LIT(LogClientRetFetchMessage,"FetchMessage");
		aText.Append(LogClientRetFetchMessage);
		}
		break;
	case EWsClOpCreateGraphic:
		aText.AppendFormat(_L("Creating graphic: %s"), aReplyDes8);
		break;
	case EWsClOpGetGraphicMessage: 
		aText.AppendFormat(_L("Graphic message: %s"), aReplyDes8);
		break;
	case EWsClOpPointerCursorPosition:
		{
		TWsClCmdSetPointerCursorArea *settings=(TWsClCmdSetPointerCursorArea*)aReplyDes8->Ptr();
		TLongBuf buf1(TDebugLogTextHandler::FormatRect(settings->area));
		aText.AppendFormat(_L("Cursor position area=%S"), &buf1);
		}
		break;
	case EWsClOpPointerCursorArea:
		{
		TWsClCmdSetPointerCursorArea *settings=(TWsClCmdSetPointerCursorArea*)aReplyDes8->Ptr();
		TLongBuf buf1(TDebugLogTextHandler::FormatRect(settings->area));
		aText.AppendFormat(_L("Cursor area=%S"), &buf1);
		}
		break;
	case EWsClOpGetDefModeMaxNumColors:
		{
		SDefModeMaxNumColors *settings=(SDefModeMaxNumColors*)aReplyDes8->Ptr();
		aText.AppendFormat(_L("Num of colors=%d, num of grays=%d, display mode=%d"),settings->iColors, settings->iGrays, settings->iDisplayMode);
		}
		break;
	default:
		UnKnownReplyBuf(aText, aOpcode);
		break;
		}
	}

void TWsDecoder::ReplyBufScreenDevice(TBuf<LogTBufSize> &aText, TInt aOpcode, const TDesC8 *aReplyDes8, const TDesC *)
	{
	switch (aOpcode)
		{
	case EWsSdOpGetScanLine:
		{			//This fills up the log two much for test code
		aText=KNullDesC;
		//Alternate code that would log this information.
		/*_LIT(LogScreenDeviceReplyBufGetScanLine,"aScanLine set to ...");
		aText=LogScreenDeviceReplyBufGetScanLine;*/
		/*TDebugLogTextHandler::Append(aText, TDebugLogTextHandler::FormatArray(TDebugLogTextHandler::ERgb,
														((TDesC8 *)aReplyData)->Ptr(),((TDesC8 *)aReplyData)->Length()*sizeof(TRgb)));*/
		}
		break;
	case EWsSdOpHorizontalTwipsToPixels:
	case EWsSdOpVerticalTwipsToPixels:
	case EWsSdOpHorizontalPixelsToTwips:
	case EWsSdOpVerticalPixelsToTwips:
	case EWsSdOpPixelSize:
	case EWsSdOpTwipsSize:
		AppendPoint(aText,aReplyDes8);
		break;
	case EWsSdOpPointerRect:
		AppendRect(aText,aReplyDes8);
		break;
	case EWsSdOpGetDefaultScreenSizeAndRotation:
	case EWsSdOpGetScreenModeSizeAndRotation:
		{
		_LIT(LogScreenDeviceReplyBufPTR,"%S %S %d");
		const TPixelsTwipsAndRotation *sar=(TPixelsTwipsAndRotation *)aReplyDes8->Ptr();
		TShortBuf buf1(TDebugLogTextHandler::FormatSize(sar->iPixelSize));
		TShortBuf buf2(TDebugLogTextHandler::FormatSize(sar->iTwipsSize));
		aText.AppendFormat(LogScreenDeviceReplyBufPTR,&buf1,&buf2,sar->iRotation);
		}
		break;
	case EWsSdOpGetDefaultScreenSizeAndRotation2:
	case EWsSdOpGetScreenModeSizeAndRotation2:
		{
		_LIT(LogScreenDeviceReplyBufPR,"%S %d");
		const TPixelsAndRotation *sar=(TPixelsAndRotation *)aReplyDes8->Ptr();
		TShortBuf buf(TDebugLogTextHandler::FormatSize(sar->iPixelSize));
		aText.AppendFormat(LogScreenDeviceReplyBufPR,&buf,sar->iRotation);
		}
		break;
	case EWsSdOpGetPalette:
		{
		_LIT(LogScreenDeviceReplyBufGetPalette,"Palette returned ...");
		aText.Append(LogScreenDeviceReplyBufGetPalette);
		}
		break;
	case EWsSdOpGetDefaultScreenModeOrigin:	
		{
		const TPoint *point = reinterpret_cast<const TPoint*>(aReplyDes8->Ptr());
		TShortBuf buf1(TDebugLogTextHandler::FormatPoint(*point));
		_LIT(LogScreenDeviceReplyBufGetDefaultScreenModeOrigin,"Default screen mode origin=%S");
		aText.AppendFormat(LogScreenDeviceReplyBufGetDefaultScreenModeOrigin, &buf1);
		}
		break;
	case EWsSdOpGetScreenModeOrigin:	
		{
		const TPoint *point = reinterpret_cast<const TPoint*>(aReplyDes8->Ptr());
		TShortBuf buf1(TDebugLogTextHandler::FormatPoint(*point));
		_LIT(LogScreenDeviceReplyBufGetScreenModeOrigin,"Screen mode origin=%S");
		aText.AppendFormat(LogScreenDeviceReplyBufGetScreenModeOrigin, &buf1);
		}
		break;
	case EWsSdOpGetScreenModeScale: 	
	case EWsSdOpGetCurrentScreenModeScale:	
		{
		const TSize *size = reinterpret_cast<const TSize*>(aReplyDes8->Ptr());
		TShortBuf buf1(TDebugLogTextHandler::FormatSize(*size));
		_LIT(LogScreenDeviceReplyBufGetScreenScale,"Screen scale=%S");
		aText.AppendFormat(LogScreenDeviceReplyBufGetScreenScale, &buf1);
		}
		break;
	case EWsSdOpGetScreenModeScaledOrigin: 
		{
		const TPoint *point = reinterpret_cast<const TPoint*>(aReplyDes8->Ptr());
		TShortBuf buf1(TDebugLogTextHandler::FormatPoint(*point));
		_LIT(LogScreenDeviceReplyBufGetScreenModeScaledOrigin,"Screen mode scaled origin=%S");
		aText.AppendFormat(LogScreenDeviceReplyBufGetScreenModeScaledOrigin, &buf1);
		}
		break;
	case EWsSdOpGetCurrentScreenModeAttributes:	
		{
		TSizeMode *settings=(TSizeMode*)aReplyDes8->Ptr();
		TShortBuf origin(TDebugLogTextHandler ::FormatPoint(settings->iOrigin));
		TShortBuf size(TDebugLogTextHandler ::FormatSize(settings->iScreenSize));
		TShortBuf area(TDebugLogTextHandler ::FormatRect(settings->iPointerCursorArea));
		aText.AppendFormat(_L("Screen attribute: origin=%S, size=%s, rotation=%d, area=$s, display mode = %d"), 
							   &origin, &size, settings->iAlternativeRotations, &area, settings->iDefaultDisplayMode);
		}
		break;
	case EWsSdOpGetScreenSizeModeList:	
		AppendDesc(aText, aReplyDes8);
		break;	
	default:
		UnKnownReplyBuf(aText, aOpcode);
		break;
		}
	}
