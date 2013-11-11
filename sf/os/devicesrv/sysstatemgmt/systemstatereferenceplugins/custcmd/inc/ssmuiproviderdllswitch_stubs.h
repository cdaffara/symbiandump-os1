// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// ssmcustomuidswitch_stubs.h
// 
//

/**
 @file
 @internalComponent
 @released
*/

extern "C" {

void common_dispatch();

__declspec(dllexport)
__declspec(naked)
void call_vector_1 ()
	{
	//CSsmUiSpecific::~CSsmUiSpecific(void)
	_asm mov eax, 1
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_2 ()
	{
	//class TUid CSsmUiSpecific::EmergencyCallPropertyCategory(void)
	_asm mov eax, 2
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_3 ()
	{
	//unsigned int CSsmUiSpecific::EmergencyCallPropertyKey(void)
	_asm mov eax, 3
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_4 ()
	{
	//class CSsmUiSpecific * CSsmUiSpecific::InstanceL(void)
	_asm mov eax, 4
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_5 ()
	{
	//int CSsmUiSpecific::IsSimSupported(void)
	_asm mov eax, 5
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_6 ()
	{
	//void CSsmUiSpecific::Release(void)
	_asm mov eax, 6
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_7 ()
	{
	//class TUid CSsmUiSpecific::StartupPSUid(void)
	_asm mov eax, 7
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_8 ()
	{
	//class TUid CSsmUiSpecific::SecurityPinNotifierUid(void)
	_asm mov eax, 8
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_9 ()
	{
	//class TUid CSsmUiSpecific::ScreenOutputChannelUid(void)
	_asm mov eax, 9
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_10 ()
	{
	//enum TStrtSecurityStatus CSsmUiSpecific::SecurityStatus(void) const
	_asm mov eax, 10
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_11 ()
	{
	//void CSsmUiSpecific::SetSecurityStatus(enum TStrtSecurityStatus const &)
	_asm mov eax, 11
	_asm jmp common_dispatch
	}
	
__declspec(dllexport)
__declspec(naked)
void call_vector_12 ()
	{
	//unsigned int CSsmUiSpecific::SimStatusPropertyKey(void)
	_asm mov eax, 12
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_13 ()
	{
	//int CSsmUiSpecific::IsSimStateChangeAllowed(void)
	_asm mov eax, 13
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_14 ()
	{
	//int CSsmUiSpecific::IsAmaStarterSupported(void)
	_asm mov eax, 14
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_15 ()
	{
	//class HBufC16 * CSsmUiSpecific::GetTsyModuleNameL(void)
	_asm mov eax, 15
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_16 ()
	{
	//class TUid CSsmUiSpecific::StarterPSUid(void)
	_asm mov eax, 16
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_17 ()
	{
	//class HBufC16 * CSsmUiSpecific::PhoneTsyNameL(void)
	_asm mov eax, 17
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_18 ()
	{
	//int CSsmUiSpecific::IsSimPresent(void)
	_asm mov eax, 18
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_19 ()
	{
	//int CSsmUiSpecific::IsSimlessOfflineSupported(void)
	_asm mov eax, 19
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_20 ()
	{
	//TBool IsSimPresent()
	_asm mov eax, 20
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_21 ()
	{
	//TBool IsNormalBoot();
	_asm mov eax, 21
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_22 ()
	{
	//TBool IsSimChangedReset();
	_asm mov eax, 22
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_23 ()
	{
	//TUid RFStatusPropertyKey();
	_asm mov eax, 23
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_24 ()
	{
	//TUid RFStatusPropertyCategory();
	_asm mov eax, 24
	_asm jmp common_dispatch
	}
__declspec(dllexport)
__declspec(naked)
void call_vector_25 ()
    {
    //TUint CSsmUiSpecific::ValidateRTCPropertyKey(void)
    _asm mov eax, 25
    _asm jmp common_dispatch
    }

__declspec(dllexport)
__declspec(naked)
void call_vector_26 ()
    {
    //TUid CSsmUiSpecific::ValidateRTCPropertyCategory(void)
    _asm mov eax, 26
    _asm jmp common_dispatch
    }

__declspec(dllexport)
__declspec(naked)
void call_vector_27 ()
    {
    //TInt PhoneMemoryRootDriveId()
    _asm mov eax, 27
    _asm jmp common_dispatch
    }

__declspec(dllexport)
__declspec(naked)
void call_vector_28 ()
    {
    //TInt ReservePhoneMemorySpace()
    _asm mov eax, 28
    _asm jmp common_dispatch
    }

__declspec(dllexport)
__declspec(naked)
void call_vector_29 ()
    {
    //TInt FreeReservedPhoneMemorySpace(const TInt aSpaceToFree)
    _asm mov eax, 29
    _asm jmp common_dispatch
    }
}

#define MAX_ORDINAL 29

