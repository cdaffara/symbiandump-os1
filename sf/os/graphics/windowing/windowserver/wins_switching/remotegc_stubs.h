/*
 * Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 */

/* Generated from  "../BWINS/remotegcu.def" file size: 2452 */

extern "C" {
void common_dispatch();

__declspec(dllexport)
__declspec(naked)
void call_vector_1 ()
	{
	// ; CCommandBuffer::~CCommandBuffer(void)
	_asm mov eax, 1
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_2 ()
	{
	// ; CRemoteGc::~CRemoteGc(void)
	_asm mov eax, 2
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_3 ()
	{
	// ; void CRemoteGc::ExternalizeL(class RWsGraphicMsgBuf &, int)
	_asm mov eax, 3
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_4 ()
	{
	// ; void CCommandBuffer::InternalizeL(class TDesC8 const &)
	_asm mov eax, 4
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_5 ()
	{
	// ; class CCommandBuffer * CCommandBuffer::NewL(void)
	_asm mov eax, 5
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_6 ()
	{
	// ; class CRemoteGc * CRemoteGc::NewL(class CWsScreenDevice *)
	_asm mov eax, 6
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_7 ()
	{
	// ; int CCommandBuffer::Play(class TPoint const &, class TRect const &, class MWsGraphicResolver const &, class CBitmapContext &)
	_asm mov eax, 7
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_8 ()
	{
	// ; void CRemoteGc::ResetCommandBuffer(void)
	_asm mov eax, 8
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_9 ()
	{
	// ; void CRemoteGc::SetCommandBufferObserver(class MCommandBufferObserver *)
	_asm mov eax, 9
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_10 ()
	{
	// ; void CRemoteGc::BeginDraw(class TRect const &)
	_asm mov eax, 10
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_11 ()
	{
	// ; void CRemoteGc::EndDraw(void)
	_asm mov eax, 11
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_12 ()
	{
	// ; void CCommandBuffer::InternalizeAppendL(class TDesC8 const &)
	_asm mov eax, 12
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_13 ()
	{
	// ; int CCommandBuffer::Play(class TPoint const &, class TRegion const *, class TRect const &, class MWsGraphicResolver const &, class CBitmapContext &)
	_asm mov eax, 13
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_14 ()
	{
	// ; class TRegion const & CCommandBuffer::ClippingRegion(void) const
	_asm mov eax, 14
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_15 ()
	{
	// ; int CCommandBuffer::IsIdentical(class CCommandBuffer const &) const
	_asm mov eax, 15
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_16 ()
	{
	// ; int CCommandBuffer::Play(class TPoint const &, class TRegion const *, class TRect const &, class RWsSession &, class CWindowGc &)
	_asm mov eax, 16
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_17 ()
	{
	// ; int CCommandBuffer::Play(class TPoint const &, class TRegion const *, class TRect const &, class MWsGraphicResolver const &, class MWsGraphicsContext &)
	_asm mov eax, 17
	_asm jmp common_dispatch
	}

}
#define MAX_ORDINAL 18

