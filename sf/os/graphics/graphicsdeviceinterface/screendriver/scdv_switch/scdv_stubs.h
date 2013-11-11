// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// 
//

// Generated from "../bwins/scdvu.def" file size: 2505
// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).

extern "C" {
void common_dispatch();

__declspec(dllexport)
__declspec(naked)
void call_vector_1 ()
	{
	// ; class CFbsDrawDevice * CFbsDrawDevice::NewBitmapDeviceL(class TScreenInfoV01, enum TDisplayMode, int)
	_asm mov eax, 1
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_2 ()
	{
	// ; public: static class CFbsDrawDevice * __cdecl CFbsDrawDevice::NewScreenDeviceL(class TScreenInfoV01,enum TDisplayMode)
	_asm mov eax, 2
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_3 ()
	{
	// ; private: int __thiscall RHardwareBitmap::Create(enum TDisplayMode,class TSize,class TUid)
	_asm mov eax, 3
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_4 ()
	{
	// ; private: void __thiscall RHardwareBitmap::Destroy(void)
	_asm mov eax, 4
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_5 ()
	{
	// ; public: static class TGraphicsAcceleratorCaps const * __cdecl CHardwareGraphicsAccelerator::GenericCapabilities(void)
	_asm mov eax, 5
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_6 ()
	{
	// ; public: int __thiscall RHardwareBitmap::GetInfo(class TAcceleratedBitmapInfo &)const 
	_asm mov eax, 6
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_7 ()
	{
	// ; public: static class CHardwareGraphicsAccelerator * __cdecl CHardwareGraphicsAccelerator::NewL(class RHardwareBitmap)
	_asm mov eax, 7
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_8 ()
	{
	// ; private: int __thiscall RHardwareBitmap::SetAsScreenReference(int)
	_asm mov eax, 8
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_9 ()
	{
	// ; public: static class CDirectScreenBitmap * __cdecl CDirectScreenBitmap::NewL(void)
	_asm mov eax, 9
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_10 ()
	{
	// ; public: static enum TDisplayMode  __cdecl CFbsDrawDevice::DisplayMode16M(void)
	_asm mov eax, 10
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_11 ()
	{
	// ; class CFbsDrawDevice * CFbsDrawDevice::NewBitmapDeviceL(class TSize const &, enum TDisplayMode, int)
	_asm mov eax, 11
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_12 ()
	{
	// ; class CFbsDrawDevice * CFbsDrawDevice::NewScreenDeviceL(int, enum TDisplayMode)
	_asm mov eax, 12
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_13 ()
	{
	// ; class CDirectScreenBitmap * CDirectScreenBitmap::NewL(int)
	_asm mov eax, 13
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_14 ()
	{
	// ; class TGraphicsAcceleratorCaps const * CSoftwareGraphicsAccelerator::GenericCapabilities(void)
	_asm mov eax, 14
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_15 ()
	{
	// ; class CSoftwareGraphicsAccelerator * CSoftwareGraphicsAccelerator::NewL(class CFbsBitmap *)
	_asm mov eax, 15
	_asm jmp common_dispatch
	}

}
#define MAX_ORDINAL 16

