/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/

extern "C" {
void common_dispatch();

__declspec(dllexport)
__declspec(naked)
void call_vector_1 ()
	{
	// ; public: __thiscall CWindowGc::CWindowGc(class CWsScreenDevice *)
	_asm mov eax, 1
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_2 ()
	{
	// ; public: __thiscall CWsBitmap::CWsBitmap(class RWsSession &)
	_asm mov eax, 2
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_3 ()
	{
	// ; public: __thiscall CWsBitmap::CWsBitmap(void)
	_asm mov eax, 3
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_4 ()
	{
	// ; public: __thiscall CWsScreenDevice::CWsScreenDevice(class RWsSession &)
	_asm mov eax, 4
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_5 ()
	{
	// ; public: __thiscall CWsScreenDevice::CWsScreenDevice(void)
	_asm mov eax, 5
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_6 ()
	{
	// ; protected: __thiscall RAnim::RAnim(class RAnimDll &)
	_asm mov eax, 6
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_7 ()
	{
	// ; protected: __thiscall RAnim::RAnim(void)
	_asm mov eax, 7
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_8 ()
	{
	// ; public: __thiscall RAnimDll::RAnimDll(class RWsSession &)
	_asm mov eax, 8
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_9 ()
	{
	// ; public: __thiscall RAnimDll::RAnimDll(void)
	_asm mov eax, 9
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_10 ()
	{
	// ; public: __thiscall RBackedUpWindow::RBackedUpWindow(class RWsSession &)
	_asm mov eax, 10
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_11 ()
	{
	// ; public: __thiscall RBackedUpWindow::RBackedUpWindow(void)
	_asm mov eax, 11
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_12 ()
	{
	// ; public: __thiscall RBlankWindow::RBlankWindow(class RWsSession &)
	_asm mov eax, 12
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_13 ()
	{
	// ; public: __thiscall RBlankWindow::RBlankWindow(void)
	_asm mov eax, 13
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_14 ()
	{
	// ; public: __thiscall RWindow::RWindow(class RWsSession &)
	_asm mov eax, 14
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_15 ()
	{
	// ; public: __thiscall RWindow::RWindow(void)
	_asm mov eax, 15
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_16 ()
	{
	// ; public: __thiscall RWindowGroup::RWindowGroup(class RWsSession &)
	_asm mov eax, 16
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_17 ()
	{
	// ; public: __thiscall RWindowGroup::RWindowGroup(void)
	_asm mov eax, 17
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_18 ()
	{
	// ; public: __thiscall RWsPointerCursor::RWsPointerCursor(class RWsSession &)
	_asm mov eax, 18
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_19 ()
	{
	// ; public: __thiscall RWsPointerCursor::RWsPointerCursor(void)
	_asm mov eax, 19
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_20 ()
	{
	// ; public: __thiscall RWsSession::RWsSession(void)
	_asm mov eax, 20
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_21 ()
	{
	// ; public: __thiscall RWsSprite::RWsSprite(class RWsSession &)
	_asm mov eax, 21
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_22 ()
	{
	// ; public: __thiscall RWsSprite::RWsSprite(void)
	_asm mov eax, 22
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_23 ()
	{
	// ; protected: __thiscall RWsSpriteBase::RWsSpriteBase(class RWsSession &)
	_asm mov eax, 23
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_24 ()
	{
	// ; protected: __thiscall RWsSpriteBase::RWsSpriteBase(void)
	_asm mov eax, 24
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_25 ()
	{
	// ; public: virtual __thiscall CWindowGc::~CWindowGc(void)
	_asm mov eax, 25
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_26 ()
	{
	// ; public: virtual __thiscall CWsBitmap::~CWsBitmap(void)
	_asm mov eax, 26
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_27 ()
	{
	// ; public: virtual __thiscall CWsScreenDevice::~CWsScreenDevice(void)
	_asm mov eax, 27
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_28 ()
	{
	// ; public: virtual __thiscall RAnim::~RAnim(void)
	_asm mov eax, 28
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_29 ()
	{
	// ; public: virtual __thiscall RAnimDll::~RAnimDll(void)
	_asm mov eax, 29
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_30 ()
	{
	// ; public: virtual void __thiscall CWindowGc::Activate(class RDrawableWindow &)
	_asm mov eax, 30
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_31 ()
	{
	// ; public: void __thiscall RWindowBase::Activate(void)
	_asm mov eax, 31
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_32 ()
	{
	// ; public: int __thiscall RWsSpriteBase::Activate(void)
	_asm mov eax, 32
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_33 ()
	{
	// ; public: virtual int __thiscall CWsScreenDevice::AddFile(class TDesC16 const &,int &)
	_asm mov eax, 33
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_34 ()
	{
	// ; public: int __thiscall RWindowBase::AddKeyRect(class TRect const &,int,int)
	_asm mov eax, 34
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_35 ()
	{
	// ; public: int __thiscall RWindowGroup::AddPriorityKey(unsigned int,unsigned int,unsigned int)
	_asm mov eax, 35
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_36 ()
	{
	// ; public: int __thiscall RWindowBase::AllocPointerMoveBuffer(int,unsigned int)
	_asm mov eax, 36
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_37 ()
	{
	// ; public: int __thiscall RWsSpriteBase::AppendMember(struct TSpriteMember const &)
	_asm mov eax, 37
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_38 ()
	{
	// ; public: void __thiscall RWindowGroup::AutoForeground(int)
	_asm mov eax, 38
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_39 ()
	{
	// ; public: void __thiscall RWindow::BeginRedraw(class TRect const &)
	_asm mov eax, 39
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_40 ()
	{
	// ; public: void __thiscall RWindow::BeginRedraw(void)
	_asm mov eax, 40
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_41 ()
	{
	// ; public: virtual void __thiscall CWindowGc::BitBlt(class TPoint const &,class CFbsBitmap const *)
	_asm mov eax, 41
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_42 ()
	{
	// ; public: virtual void __thiscall CWindowGc::BitBlt(class TPoint const &,class CFbsBitmap const *,class TRect const &)
	_asm mov eax, 42
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_43 ()
	{
	// ; public: virtual void __thiscall CWindowGc::BitBlt(class TPoint const &,class CWsBitmap const *)
	_asm mov eax, 43
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_44 ()
	{
	// ; public: virtual void __thiscall CWindowGc::BitBlt(class TPoint const &,class CWsBitmap const *,class TRect const &)
	_asm mov eax, 44
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_45 ()
	{
	// ; public: virtual void __thiscall CWindowGc::BitBltMasked(class TPoint const &,class CFbsBitmap const *,class TRect const &,class CFbsBitmap const *,int)
	_asm mov eax, 45
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_46 ()
	{
	// ; public: virtual void __thiscall CWindowGc::BitBltMasked(class TPoint const &,class CWsBitmap const *,class TRect const &,class CWsBitmap const *,int)
	_asm mov eax, 46
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_47 ()
	{
	// ; int RBackedUpWindow::BitmapHandle(void) const
	_asm mov eax, 47
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_48 ()
	{
	// ; public: void __thiscall RWindowGroup::CancelCaptureKey(long)
	_asm mov eax, 48
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_49 ()
	{
	// ; public: void __thiscall RWindowGroup::CancelCaptureKeyUpAndDowns(long)
	_asm mov eax, 49
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_50 ()
	{
	// ; public: virtual void __thiscall CWindowGc::CancelClippingRect(void)
	_asm mov eax, 50
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_51 ()
	{
	// ; public: virtual void __thiscall CWindowGc::CancelClippingRegion(void)
	_asm mov eax, 51
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_52 ()
	{
	// ; public: void __thiscall RWindowBase::CancelPointerRepeatEventRequest(void)
	_asm mov eax, 52
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_53 ()
	{
	// ; public: void __thiscall RWindowGroup::CancelTextCursor(void)
	_asm mov eax, 53
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_54 ()
	{
	// ; public: long __thiscall RWindowGroup::CaptureKey(unsigned int,unsigned int,unsigned int)
	_asm mov eax, 54
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_55 ()
	{
	// ; public: long __thiscall RWindowGroup::CaptureKeyUpAndDowns(unsigned int,unsigned int,unsigned int)
	_asm mov eax, 55
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_56 ()
	{
	// ; public: unsigned long __thiscall RWindowTreeNode::Child(void)const 
	_asm mov eax, 56
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_57 ()
	{
	// ; public: void __thiscall RWindowBase::ClaimPointerGrab(int)
	_asm mov eax, 57
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_58 ()
	{
	// ; public: int __thiscall RWsSession::ClaimSystemPointerCursorList(void)
	_asm mov eax, 58
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_59 ()
	{
	// ; public: virtual void __thiscall CWindowGc::Clear(class TRect const &)
	_asm mov eax, 59
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_60 ()
	{
	// ; public: virtual void __thiscall CWindowGc::Clear(void)
	_asm mov eax, 60
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_61 ()
	{
	// ; public: int __thiscall RWsSession::ClearHotKeys(enum THotKey)
	_asm mov eax, 61
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_62 ()
	{
	// ; public: void __thiscall RWsSession::ClearSystemPointerCursor(int)
	_asm mov eax, 62
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_63 ()
	{
	// ; public: virtual void __thiscall RAnim::Close(void)
	_asm mov eax, 63
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_64 ()
	{
	// ; public: virtual void __thiscall RAnimDll::Close(void)
	_asm mov eax, 64
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_65 ()
	{
	// ; public: void __thiscall RWindowTreeNode::Close(void)
	_asm mov eax, 65
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_66 ()
	{
	// ; public: void __thiscall RWsSpriteBase::Close(void)
	_asm mov eax, 66
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_67 ()
	{
	// ; protected: void __thiscall RAnim::Command(int)
	_asm mov eax, 67
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_68 ()
	{
	// ; protected: void __thiscall RAnim::Command(int,class TPtrC8 const &)
	_asm mov eax, 68
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_69 ()
	{
	// ; protected: int __thiscall RAnim::CommandReply(int)
	_asm mov eax, 69
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_70 ()
	{
	// ; protected: int __thiscall RAnim::CommandReply(int,class TPtrC8 const &)
	_asm mov eax, 70
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_71 ()
	{
	// ; public: void __thiscall RWsSession::ComputeMode(enum RWsSession::TComputeMode)
	_asm mov eax, 71
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_72 ()
	{
	// ; public: int __thiscall RWsSession::Connect(void)
	_asm mov eax, 72
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_73 ()
	{
	// ; public: virtual int __thiscall CWindowGc::Construct(void)
	_asm mov eax, 73
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_74 ()
	{
	// ; public: int __thiscall CWsScreenDevice::Construct(void)
	_asm mov eax, 74
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_75 ()
	{
	// ; protected: int __thiscall RAnim::Construct(class RWindowBase const &,int,class TDesC8 const &)
	_asm mov eax, 75
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_76 ()
	{
	// ; public: int __thiscall RBackedUpWindow::Construct(class RWindowTreeNode const &,enum TDisplayMode,unsigned long)
	_asm mov eax, 76
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_77 ()
	{
	// ; public: int __thiscall RBlankWindow::Construct(class RWindowTreeNode const &,unsigned long)
	_asm mov eax, 77
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_78 ()
	{
	// ; public: int __thiscall RWindow::Construct(class RWindowTreeNode const &,unsigned long)
	_asm mov eax, 78
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_79 ()
	{
	// ; public: int __thiscall RWindowGroup::Construct(unsigned long)
	_asm mov eax, 79
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_80 ()
	{
	// ; public: int __thiscall RWindowGroup::Construct(unsigned long,int)
	_asm mov eax, 80
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_81 ()
	{
	// ; public: int __thiscall RWsPointerCursor::Construct(int)
	_asm mov eax, 81
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_82 ()
	{
	// ; public: int __thiscall RWsSprite::Construct(class RWindowTreeNode &,class TPoint const &,int)
	_asm mov eax, 82
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_83 ()
	{
	// ; public: virtual void __thiscall CWindowGc::CopyRect(class TPoint const &,class TRect const &)
	_asm mov eax, 83
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_84 ()
	{
	// ; public: int __thiscall CWsScreenDevice::CopyScreenToBitmap(class CFbsBitmap const *)const 
	_asm mov eax, 84
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_85 ()
	{
	// ; public: int __thiscall CWsScreenDevice::CopyScreenToBitmap(class CFbsBitmap const *,class TRect const &)const 
	_asm mov eax, 85
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_86 ()
	{
	// ; public: int __thiscall CWsBitmap::Create(class TSize const &,enum TDisplayMode)
	_asm mov eax, 86
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_87 ()
	{
	// ; public: virtual int __thiscall CWsScreenDevice::CreateContext(class CGraphicsContext * &)
	_asm mov eax, 87
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_88 ()
	{
	// ; public: virtual void __thiscall CWindowGc::Deactivate(void)
	_asm mov eax, 88
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_89 ()
	{
	// ; public: void __thiscall RWindowGroup::DefaultOwningWindow(void)
	_asm mov eax, 89
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_90 ()
	{
	// ; public: void __thiscall RAnim::Destroy(void)
	_asm mov eax, 90
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_91 ()
	{
	// ; public: void __thiscall RAnimDll::Destroy(void)
	_asm mov eax, 91
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_92 ()
	{
	// ; public: void __thiscall RWindowTreeNode::Destroy(void)
	_asm mov eax, 92
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_93 ()
	{
	// ; public: virtual class CGraphicsDevice * __thiscall CWindowGc::Device(void)const 
	_asm mov eax, 93
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_94 ()
	{
	// ; public: void __thiscall RWindowTreeNode::DisableGroupChangeEvents(void)
	_asm mov eax, 94
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_95 ()
	{
	// ; public: void __thiscall RWindowGroup::DisableKeyClick(int)
	_asm mov eax, 95
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_96 ()
	{
	// ; public: void __thiscall RWindowTreeNode::DisableModifierChangedEvents(void)
	_asm mov eax, 96
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_97 ()
	{
	// ; public: void __thiscall RWindowTreeNode::DisableOnEvents(void)
	_asm mov eax, 97
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_98 ()
	{
	// ; public: void __thiscall RWindowTreeNode::DisableErrorMessages(void)
	_asm mov eax, 98
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_99 ()
	{
	// ; public: void __thiscall RWindowBase::DisablePointerMoveBuffer(void)
	_asm mov eax, 99
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_100 ()
	{
	// ; public: virtual void __thiscall CWindowGc::DiscardBrushPattern(void)
	_asm mov eax, 100
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_101 ()
	{
	// ; public: virtual void __thiscall CWindowGc::DiscardFont(void)
	_asm mov eax, 101
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_102 ()
	{
	// ; public: void __thiscall RWindowBase::DiscardPointerMoveBuffer(void)
	_asm mov eax, 102
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_103 ()
	{
	// ; public: void __thiscall RWsSession::Close(void)
	_asm mov eax, 103
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_104 ()
	{
	// ; public: virtual enum TDisplayMode  __thiscall CWsScreenDevice::DisplayMode(void)const 
	_asm mov eax, 104
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_105 ()
	{
	// ; public: virtual void __thiscall CWindowGc::DrawArc(class TRect const &,class TPoint const &,class TPoint const &)
	_asm mov eax, 105
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_106 ()
	{
	// ; public: virtual void __thiscall CWindowGc::DrawBitmap(class TPoint const &,class CFbsBitmap const *)
	_asm mov eax, 106
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_107 ()
	{
	// ; public: virtual void __thiscall CWindowGc::DrawBitmap(class TRect const &,class CFbsBitmap const *,class TRect const &)
	_asm mov eax, 107
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_108 ()
	{
	// ; public: virtual void __thiscall CWindowGc::DrawBitmap(class TRect const &,class CFbsBitmap const *)
	_asm mov eax, 108
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_109 ()
	{
	// ; public: virtual void __thiscall CWindowGc::DrawEllipse(class TRect const &)
	_asm mov eax, 109
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_110 ()
	{
	// ; public: virtual void __thiscall CWindowGc::DrawLine(class TPoint const &,class TPoint const &)
	_asm mov eax, 110
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_111 ()
	{
	// ; public: virtual void __thiscall CWindowGc::DrawLineBy(class TPoint const &)
	_asm mov eax, 111
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_112 ()
	{
	// ; public: virtual void __thiscall CWindowGc::DrawLineTo(class TPoint const &)
	_asm mov eax, 112
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_113 ()
	{
	// ; public: virtual void __thiscall CWindowGc::DrawPie(class TRect const &,class TPoint const &,class TPoint const &)
	_asm mov eax, 113
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_114 ()
	{
	// ; public: virtual void __thiscall CWindowGc::DrawPolyLine(class CArrayFix<class TPoint> const *)
	_asm mov eax, 114
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_115 ()
	{
	// ; public: virtual void __thiscall CWindowGc::DrawPolyLine(class TPoint const *,int)
	_asm mov eax, 115
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_116 ()
	{
	// ; public: virtual int __thiscall CWindowGc::DrawPolygon(class CArrayFix<class TPoint> const *,enum CGraphicsContext::TFillRule)
	_asm mov eax, 116
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_117 ()
	{
	// ; public: virtual int __thiscall CWindowGc::DrawPolygon(class TPoint const *,int,enum CGraphicsContext::TFillRule)
	_asm mov eax, 117
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_118 ()
	{
	// ; public: virtual void __thiscall CWindowGc::DrawRect(class TRect const &)
	_asm mov eax, 118
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_119 ()
	{
	// ; public: virtual void __thiscall CWindowGc::DrawRoundRect(class TRect const &,class TSize const &)
	_asm mov eax, 119
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_120 ()
	{
	// ; public: virtual void __thiscall CWindowGc::DrawText(class TDesC16 const &,class TPoint const &)
	_asm mov eax, 120
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_121 ()
	{
	// ; public: virtual void __thiscall CWindowGc::DrawText(class TDesC16 const &,class TRect const &,int,enum CGraphicsContext::TTextAlign,int)
	_asm mov eax, 121
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_122 ()
	{
	// ; public: virtual void __thiscall CWindowGc::DrawTextVertical(class TDesC16 const &,class TPoint const &,int)
	_asm mov eax, 122
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_123 ()
	{
	// ; public: virtual void __thiscall CWindowGc::DrawTextVertical(class TDesC16 const &,class TRect const &,int,int,enum CGraphicsContext::TTextAlign,int)
	_asm mov eax, 123
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_124 ()
	{
	// ; public: int __thiscall CWsBitmap::Duplicate(int)
	_asm mov eax, 124
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_125 ()
	{
	// ; public: void __thiscall RWindowBase::EnableBackup(unsigned int)
	_asm mov eax, 125
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_126 ()
	{
	// ; public: int __thiscall RWindowTreeNode::EnableGroupChangeEvents(void)
	_asm mov eax, 126
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_127 ()
	{
	// ; public: int __thiscall RWindowTreeNode::EnableModifierChangedEvents(unsigned int,enum TEventControl)
	_asm mov eax, 127
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_128 ()
	{
	// ; public: int __thiscall RWindowTreeNode::EnableOnEvents(enum TEventControl)
	_asm mov eax, 128
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_129 ()
	{
	// ; public: int __thiscall RWindowTreeNode::EnableErrorMessages(enum TEventControl)
	_asm mov eax, 129
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_130 ()
	{
	// ; public: void __thiscall RWindowBase::EnablePointerMoveBuffer(void)
	_asm mov eax, 130
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_131 ()
	{
	// ; public: void __thiscall RWindowGroup::EnableReceiptOfFocus(int)
	_asm mov eax, 131
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_132 ()
	{
	// ; public: void __thiscall RWindow::EndRedraw(void)
	_asm mov eax, 132
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_133 ()
	{
	// ; public: void __thiscall RWsSession::EventReady(class TRequestStatus *)
	_asm mov eax, 133
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_134 ()
	{
	// ; public: void __thiscall RWsSession::EventReadyCancel(void)
	_asm mov eax, 134
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_135 ()
	{
	// ; int RWsSession::FetchMessage(class TUid &, class TPtr8 &, class TWsEvent const &) const
	_asm mov eax, 135
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_136 ()
	{
	// ; int RWsSession::FindWindowGroupIdentifier(int, class TDesC16 const &, int) const
	_asm mov eax, 136
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_137 ()
	{
	// ; int RWsSession::FindWindowGroupIdentifier(int, class TThreadId) const
	_asm mov eax, 137
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_138 ()
	{
	// ; public: void __thiscall RWsSession::Flush(void)
	_asm mov eax, 138
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_139 ()
	{
	// ; public: virtual int __thiscall CWsScreenDevice::FontHeightInPixels(int,int)const 
	_asm mov eax, 139
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_140 ()
	{
	// ; public: virtual int __thiscall CWsScreenDevice::FontHeightInTwips(int,int)const 
	_asm mov eax, 140
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_141 ()
	{
	// ; public: void __thiscall RWindowBase::FreePointerMoveBuffer(void)
	_asm mov eax, 141
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_142 ()
	{
	// ; public: void __thiscall RWsSession::FreeSystemPointerCursorList(void)
	_asm mov eax, 142
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_143 ()
	{
	// ; public: int __thiscall RWindowTreeNode::FullOrdinalPosition(void)const 
	_asm mov eax, 143
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_144 ()
	{
	// ; public: class TRgb  __thiscall RWsSession::GetBackgroundColor(void)const 
	_asm mov eax, 144
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_145 ()
	{
	// ; int RWsSession::GetDefaultOwningWindow(void) const
	_asm mov eax, 145
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_146 ()
	{
	// ; void RWsSession::GetDoubleClickSettings(class TTimeIntervalMicroSeconds32 &, int &) const
	_asm mov eax, 146
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_147 ()
	{
	// ; void RWsSession::GetEvent(class TWsEvent &) const
	_asm mov eax, 147
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_148 ()
	{
	// ; int RWsSession::GetFocusWindowGroup(void) const
	_asm mov eax, 148
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_149 ()
	{
	// ; public: int __thiscall CWsScreenDevice::GetFontById(class CFont * &,class TUid,class TAlgStyle const &)
	_asm mov eax, 149
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_150 ()
	{
	// ; void RWindow::GetInvalidRegion(class RRegion &) const
	_asm mov eax, 150
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_151 ()
	{
	// ; void RWsSession::GetKeyboardRepeatRate(class TTimeIntervalMicroSeconds32 &, class TTimeIntervalMicroSeconds32 &) const
	_asm mov eax, 151
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_152 ()
	{
	// ; public: int __thiscall RWsSession::GetModifierState(void)const 
	_asm mov eax, 152
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_153 ()
	{
	// ; public: virtual int __thiscall CWsScreenDevice::GetNearestFontInPixels(class CFont * &,class TFontSpec const &)
	_asm mov eax, 153
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_154 ()
	{
	// ; public: virtual int __thiscall CWsScreenDevice::GetNearestFontInTwips(class CFont * &,class TFontSpec const &)
	_asm mov eax, 154
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_155 ()
	{
	// ; public: virtual int __thiscall CWsScreenDevice::GetPalette(class CPalette * &)const 
	_asm mov eax, 155
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_156 ()
	{
	// ; public: virtual void __thiscall CWsScreenDevice::GetPixel(class TRgb &,class TPoint const &)const 
	_asm mov eax, 156
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_157 ()
	{
	// ; void RWsSession::GetPriorityKey(class TWsPriorityKeyEvent &) const
	_asm mov eax, 157
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_158 ()
	{
	// ; public: void __thiscall RWsSession::GetRedraw(class TWsRedrawEvent &)
	_asm mov eax, 158
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_159 ()
	{
	// ; public: virtual void __thiscall CWsScreenDevice::GetScanLine(class TDes8 &,class TPoint const &,int,enum TDisplayMode)const 
	_asm mov eax, 159
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_160 ()
	{
	// ; int RWsSession::GetWindowGroupClientThreadId(int, class TThreadId &) const
	_asm mov eax, 160
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_161 ()
	{
	// ; int RWsSession::GetWindowGroupHandle(int) const
	_asm mov eax, 161
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_162 ()
	{
	// ; int RWsSession::GetWindowGroupNameFromIdentifier(int, class TDes16 &) const
	_asm mov eax, 162
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_163 ()
	{
	// ; int RWsSession::GetWindowGroupOrdinalPriority(int) const
	_asm mov eax, 163
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_164 ()
	{
	// ; public: int __thiscall RWsSession::HeapCount(void)const 
	_asm mov eax, 164
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_165 ()
	{
	// ; public: void __thiscall RWsSession::HeapSetFail(int,int)
	_asm mov eax, 165
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_166 ()
	{
	// ; public: virtual int __thiscall CWsScreenDevice::HorizontalPixelsToTwips(int)const 
	_asm mov eax, 166
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_167 ()
	{
	// ; public: virtual int __thiscall CWsScreenDevice::HorizontalTwipsToPixels(int)const 
	_asm mov eax, 167
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_168 ()
	{
	// ; public: int __thiscall RWindowGroup::Identifier(void)const 
	_asm mov eax, 168
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_169 ()
	{
	// ; public: class TPoint  __thiscall RWindowBase::InquireOffset(class RWindowTreeNode const &)const 
	_asm mov eax, 169
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_170 ()
	{
	// ; public: void __thiscall CWsBitmap::InternalizeL(class RReadStream &)
	_asm mov eax, 170
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_171 ()
	{
	// ; public: void __thiscall RWindow::Invalidate(class TRect const &)
	_asm mov eax, 171
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_172 ()
	{
	// ; public: void __thiscall RWindow::Invalidate(void)
	_asm mov eax, 172
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_173 ()
	{
	// ; public: int __thiscall CWsBitmap::Load(class TDesC16 const &,long,int)
	_asm mov eax, 173
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_174 ()
	{
	// ; public: int __thiscall RAnimDll::Load(class TDesC16 const &)
	_asm mov eax, 174
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_175 ()
	{
	// ; public: void __thiscall RWsSession::LogMessage(class TBuf<128> const &)
	_asm mov eax, 175
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_176 ()
	{
	// ; public: void __thiscall RBackedUpWindow::MaintainBackup(void)
	_asm mov eax, 176
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_177 ()
	{
	// ; public: virtual void __thiscall CWindowGc::MapColors(class TRect const &,class TRgb const *,int,int)
	_asm mov eax, 177
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_178 ()
	{
	// ; public: virtual void __thiscall CWindowGc::MoveBy(class TPoint const &)
	_asm mov eax, 178
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_179 ()
	{
	// ; public: virtual void __thiscall CWindowGc::MoveTo(class TPoint const &)
	_asm mov eax, 179
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_180 ()
	{
	// ; public: int __thiscall RWindowGroup::Name(class TDes16 &)const 
	_asm mov eax, 180
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_181 ()
	{
	// ; public: unsigned long __thiscall RWindowTreeNode::NextSibling(void)const 
	_asm mov eax, 181
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_182 ()
	{
	// ; public: virtual int __thiscall CWsScreenDevice::NumTypefaces(void)const 
	_asm mov eax, 182
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_183 ()
	{
	// ; public: int __thiscall RWsSession::NumWindowGroups(int)const 
	_asm mov eax, 183
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_184 ()
	{
	// ; public: int __thiscall RWsSession::NumWindowGroups(void)const 
	_asm mov eax, 184
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_185 ()
	{
	// ; public: int __thiscall RWindowTreeNode::OrdinalPosition(void)const 
	_asm mov eax, 185
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_186 ()
	{
	// ; public: virtual void __thiscall CWsScreenDevice::PaletteAttributes(int &,int &)const 
	_asm mov eax, 186
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_187 ()
	{
	// ; public: unsigned long __thiscall RWindowTreeNode::Parent(void)const 
	_asm mov eax, 187
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_188 ()
	{
	// ; public: void __thiscall RWsSession::PasswordEntered(void)
	_asm mov eax, 188
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_189 ()
	{
	// ; public: int __thiscall RWindowBase::PasswordWindow(enum TPasswordMode)
	_asm mov eax, 189
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_190 ()
	{
	// ; public: virtual void __thiscall CWindowGc::Plot(class TPoint const &)
	_asm mov eax, 190
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_191 ()
	{
	// ; public: void __thiscall RWindowBase::PointerFilter(unsigned long,unsigned long)
	_asm mov eax, 191
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_192 ()
	{
	// ; public: class TRect  __thiscall CWsScreenDevice::PointerRect(void)const 
	_asm mov eax, 192
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_193 ()
	{
	// ; public: class TPoint  __thiscall RWindowBase::Position(void)const 
	_asm mov eax, 193
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_194 ()
	{
	// ; public: unsigned long __thiscall RWindowTreeNode::PrevSibling(void)const 
	_asm mov eax, 194
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_195 ()
	{
	// ; public: void __thiscall RWsSession::PriorityKeyReady(class TRequestStatus *)
	_asm mov eax, 195
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_196 ()
	{
	// ; public: void __thiscall RWsSession::PriorityKeyReadyCancel(void)
	_asm mov eax, 196
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_197 ()
	{
	// ; public: void __thiscall RWsSession::PurgePointerEvents(void)
	_asm mov eax, 197
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_198 ()
	{
	// ; int CWsScreenDevice::RectCompare(class TRect const &, class TRect const &) const
	_asm mov eax, 198
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_199 ()
	{
	// ; public: void __thiscall RWsSession::RedrawReady(class TRequestStatus *)
	_asm mov eax, 199
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_200 ()
	{
	// ; public: void __thiscall RWsSession::RedrawReadyCancel(void)
	_asm mov eax, 200
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_201 ()
	{
	// ; public: virtual void __thiscall CWsScreenDevice::ReleaseFont(class CFont *)
	_asm mov eax, 201
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_202 ()
	{
	// ; public: void __thiscall RWindowBase::RemoveAllKeyRects(void)
	_asm mov eax, 202
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_203 ()
	{
	// ; public: virtual void __thiscall CWsScreenDevice::RemoveFile(int)
	_asm mov eax, 203
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_204 ()
	{
	// ; public: void __thiscall RWindowGroup::RemovePriorityKey(unsigned int,unsigned int,unsigned int)
	_asm mov eax, 204
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_205 ()
	{
	// ; public: void __thiscall RWindowBase::RequestPointerRepeatEvent(class TTimeIntervalMicroSeconds32,class TRect const &)
	_asm mov eax, 205
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_206 ()
	{
	// ; public: virtual void __thiscall CWindowGc::Reset(void)
	_asm mov eax, 206
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_207 ()
	{
	// ; public: void __thiscall CWsBitmap::Reset(void)
	_asm mov eax, 207
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_208 ()
	{
	// ; int RWsSession::ResourceCount(void) const
	_asm mov eax, 208
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_209 ()
	{
	// ; public: int __thiscall RWsSession::RestoreDefaultHotKey(enum THotKey)
	_asm mov eax, 209
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_210 ()
	{
	// ; int RWindowBase::RetrievePointerMoveBuffer(class TDes8 &) const
	_asm mov eax, 210
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_211 ()
	{
	// ; public: void __thiscall RDrawableWindow::Scroll(class TPoint const &)
	_asm mov eax, 211
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_212 ()
	{
	// ; public: void __thiscall RDrawableWindow::Scroll(class TPoint const &,class TRect const &)
	_asm mov eax, 212
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_213 ()
	{
	// ; public: void __thiscall RDrawableWindow::Scroll(class TRect const &,class TPoint const &,class TRect const &)
	_asm mov eax, 213
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_214 ()
	{
	// ; public: void __thiscall RDrawableWindow::Scroll(class TRect const &,class TPoint const &)
	_asm mov eax, 214
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_215 ()
	{
	// ; public: int __thiscall RWsSession::SendEventToWindowGroup(int,class TWsEvent const &)
	_asm mov eax, 215
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_216 ()
	{
	// ; public: int __thiscall RWsSession::SendMessageToWindowGroup(int,class TUid,class TDesC8 const &)
	_asm mov eax, 216
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_217 ()
	{
	// ; public: int __thiscall RWsSession::SetAutoFlush(int)
	_asm mov eax, 217
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_218 ()
	{
	// ; public: void __thiscall RWindow::SetBackgroundColor(class TRgb)
	_asm mov eax, 218
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_219 ()
	{
	// ; public: void __thiscall RWindow::SetBackgroundColor(void)
	_asm mov eax, 219
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_220 ()
	{
	// ; public: void __thiscall RWsSession::SetBackgroundColor(class TRgb)
	_asm mov eax, 220
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_221 ()
	{
	// ; public: virtual void __thiscall CWindowGc::SetBrushColor(class TRgb const &)
	_asm mov eax, 221
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_222 ()
	{
	// ; public: virtual void __thiscall CWindowGc::SetBrushOrigin(class TPoint const &)
	_asm mov eax, 222
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_223 ()
	{
	// ; public: virtual void __thiscall CWindowGc::SetBrushStyle(enum CGraphicsContext::TBrushStyle)
	_asm mov eax, 223
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_224 ()
	{
	// ; public: virtual void __thiscall CWindowGc::SetCharJustification(int,int)
	_asm mov eax, 224
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_225 ()
	{
	// ; public: virtual void __thiscall CWindowGc::SetClippingRect(class TRect const &)
	_asm mov eax, 225
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_226 ()
	{
	// ; public: virtual int __thiscall CWindowGc::SetClippingRegion(class TRegion const &)
	_asm mov eax, 226
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_227 ()
	{
	// ; public: void __thiscall RBlankWindow::SetColor(class TRgb)
	_asm mov eax, 227
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_228 ()
	{
	// ; public: int __thiscall RWindowBase::SetCornerType(enum TCornerType,int)
	_asm mov eax, 228
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_229 ()
	{
	// ; public: void __thiscall RWindowTreeNode::SetCustomPointerCursor(class RWsPointerCursor const &)
	_asm mov eax, 229
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_230 ()
	{
	// ; public: virtual void __thiscall CWindowGc::SetDitherOrigin(class TPoint const &)
	_asm mov eax, 230
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_231 ()
	{
	// ; public: int __thiscall RWsSession::SetDoubleClick(class TTimeIntervalMicroSeconds32 const &,int)
	_asm mov eax, 231
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_232 ()
	{
	// ; public: virtual void __thiscall CWindowGc::SetDrawMode(enum CGraphicsContext::TDrawMode)
	_asm mov eax, 232
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_233 ()
	{
	// ; public: void __thiscall RBlankWindow::SetExtent(class TPoint const &,class TSize const &)
	_asm mov eax, 233
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_234 ()
	{
	// ; public: void __thiscall RWindow::SetExtent(class TPoint const &,class TSize const &)
	_asm mov eax, 234
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_235 ()
	{
	// ; public: int __thiscall RWindowBase::SetExtentErr(class TPoint const &,class TSize const &)
	_asm mov eax, 235
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_236 ()
	{
	// ; public: int __thiscall RWsSession::SetHotKey(enum THotKey,unsigned int,unsigned int,unsigned int)
	_asm mov eax, 236
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_237 ()
	{
	// ; public: int __thiscall RWsSession::SetKeyboardRepeatRate(class TTimeIntervalMicroSeconds32 const &,class TTimeIntervalMicroSeconds32 const &)
	_asm mov eax, 237
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_238 ()
	{
	// ; public: int __thiscall RWsSession::SetModifierState(enum TEventModifier,enum TModifierState)
	_asm mov eax, 238
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_239 ()
	{
	// ; public: int __thiscall RWindowGroup::SetName(class TDesC16 const &)
	_asm mov eax, 239
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_240 ()
	{
	// ; public: void __thiscall RWindowTreeNode::SetOrdinalPosition(int)
	_asm mov eax, 240
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_241 ()
	{
	// ; public: void __thiscall RWindowTreeNode::SetOrdinalPosition(int,int)
	_asm mov eax, 241
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_242 ()
	{
	// ; public: void __thiscall RWindowGroup::SetOrdinalPriorityAdjust(int)
	_asm mov eax, 242
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_243 ()
	{
	// ; public: virtual void __thiscall CWindowGc::SetOrigin(class TPoint const &)
	_asm mov eax, 243
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_244 ()
	{
	// ; public: void __thiscall RWindowGroup::SetOwningWindowGroup(int)
	_asm mov eax, 244
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_245 ()
	{
	// ; public: virtual void __thiscall CWsScreenDevice::SetPalette(class CPalette *)
	_asm mov eax, 245
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_246 ()
	{
	// ; public: virtual void __thiscall CWindowGc::SetPenColor(class TRgb const &)
	_asm mov eax, 246
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_247 ()
	{
	// ; public: virtual void __thiscall CWindowGc::SetPenSize(class TSize const &)
	_asm mov eax, 247
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_248 ()
	{
	// ; public: virtual void __thiscall CWindowGc::SetPenStyle(enum CGraphicsContext::TPenStyle)
	_asm mov eax, 248
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_249 ()
	{
	// ; public: void __thiscall RWindowBase::SetPointerCapture(int)
	_asm mov eax, 249
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_250 ()
	{
	// ; public: int __thiscall RWindowTreeNode::SetPointerCursor(int)
	_asm mov eax, 250
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_251 ()
	{
	// ; public: void __thiscall RWindowBase::SetPointerGrab(int)
	_asm mov eax, 251
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_252 ()
	{
	// ; public: void __thiscall RWindowBase::SetPosition(class TPoint const &)
	_asm mov eax, 252
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_253 ()
	{
	// ; public: void __thiscall RWsSprite::SetPosition(class TPoint const &)
	_asm mov eax, 253
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_254 ()
	{
	// ; public: int __thiscall RWindowBase::SetRequiredDisplayMode(enum TDisplayMode)
	_asm mov eax, 254
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_255 ()
	{
	// ; public: void __thiscall RWindowBase::SetShadowDisabled(int)
	_asm mov eax, 255
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_256 ()
	{
	// ; public: void __thiscall RWindowBase::SetShadowHeight(int)
	_asm mov eax, 256
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_257 ()
	{
	// ; public: void __thiscall RWsSession::SetShadowVector(class TPoint const &)
	_asm mov eax, 257
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_258 ()
	{
	// ; public: int __thiscall RWindowBase::SetShape(class TRegion const &)
	_asm mov eax, 258
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_259 ()
	{
	// ; public: void __thiscall RBlankWindow::SetSize(class TSize const &)
	_asm mov eax, 259
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_260 ()
	{
	// ; public: void __thiscall RWindow::SetSize(class TSize const &)
	_asm mov eax, 260
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_261 ()
	{
	// ; public: int __thiscall RWindowBase::SetSizeErr(class TSize const &)
	_asm mov eax, 261
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_262 ()
	{
	// ; public: virtual void __thiscall CWindowGc::SetStrikethroughStyle(enum TFontStrikethrough)
	_asm mov eax, 262
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_263 ()
	{
	// ; public: int __thiscall RWsSession::SetSystemPointerCursor(class RWsPointerCursor const &,int)
	_asm mov eax, 263
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_264 ()
	{
	// ; public: void __thiscall RWindowGroup::SetTextCursor(class RWindowBase &,class TPoint const &,struct TTextCursor const &)
	_asm mov eax, 264
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_265 ()
	{
	// ; public: void __thiscall RWindowGroup::SetTextCursor(class RWindowBase &,class TPoint const &,struct TTextCursor const &,class TRect const &)
	_asm mov eax, 265
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_266 ()
	{
	// ; public: virtual void __thiscall CWindowGc::SetUnderlineStyle(enum TFontUnderline)
	_asm mov eax, 266
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_267 ()
	{
	// ; public: void __thiscall RWindowBase::SetVisible(int)
	_asm mov eax, 267
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_268 ()
	{
	// ; public: int __thiscall RWsSession::SetWindowGroupOrdinalPosition(int,int)
	_asm mov eax, 268
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_269 ()
	{
	// ; public: virtual void __thiscall CWindowGc::SetWordJustification(int,int)
	_asm mov eax, 269
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_270 ()
	{
	// ; public: class TPoint  __thiscall RWsSession::ShadowVector(void)const 
	_asm mov eax, 270
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_271 ()
	{
	// ; public: void __thiscall RWsSession::SimulateRawEvent(class TRawEvent)
	_asm mov eax, 271
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_272 ()
	{
	// ; public: class TSize  __thiscall RWindowBase::Size(void)const 
	_asm mov eax, 272
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_273 ()
	{
	// ; public: virtual class TSize  __thiscall CWsScreenDevice::SizeInPixels(void)const 
	_asm mov eax, 273
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_274 ()
	{
	// ; public: virtual class TSize  __thiscall CWsScreenDevice::SizeInTwips(void)const 
	_asm mov eax, 274
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_275 ()
	{
	// ; public: void __thiscall RWsSession::SystemInfo(int &,struct RWsSession::SSystemInfo &)
	_asm mov eax, 275
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_276 ()
	{
	// ; public: void __thiscall RWsSession::TestWrite(int,int,void const *,int)
	_asm mov eax, 276
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_277 ()
	{
	// ; public: void __thiscall RWsSession::TestWriteReply(int,int,void const *,int)
	_asm mov eax, 277
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_278 ()
	{
	// ; public: void __thiscall RWsSession::TestWriteReplyP(int,int,void const *,int,class TDes8 *)
	_asm mov eax, 278
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_279 ()
	{
	// ; public: virtual void __thiscall CWsScreenDevice::TypefaceSupport(class TTypefaceSupport &,int)const 
	_asm mov eax, 279
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_280 ()
	{
	// ; public: void __thiscall RBackedUpWindow::UpdateBackupBitmap(void)
	_asm mov eax, 280
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_281 ()
	{
	// ; public: int __thiscall RWsSpriteBase::UpdateMember(int,struct TSpriteMember const &)
	_asm mov eax, 281
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_282 ()
	{
	// ; public: void __thiscall RWsSpriteBase::UpdateMember(int)
	_asm mov eax, 282
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_283 ()
	{
	// ; public: void __thiscall RBackedUpWindow::UpdateScreen(class TRegion const &)
	_asm mov eax, 283
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_284 ()
	{
	// ; public: void __thiscall RBackedUpWindow::UpdateScreen(void)
	_asm mov eax, 284
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_285 ()
	{
	// ; public: virtual void __thiscall CWindowGc::UseBrushPattern(class CFbsBitmap const *)
	_asm mov eax, 285
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_286 ()
	{
	// ; public: virtual void __thiscall CWindowGc::UseFont(class CFont const *)
	_asm mov eax, 286
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_287 ()
	{
	// ; class TVersion RWsSession::Version(void) const
	_asm mov eax, 287
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_288 ()
	{
	// ; public: virtual int __thiscall CWsScreenDevice::VerticalPixelsToTwips(int)const 
	_asm mov eax, 288
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_289 ()
	{
	// ; public: virtual int __thiscall CWsScreenDevice::VerticalTwipsToPixels(int)const 
	_asm mov eax, 289
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_290 ()
	{
	// ; int RWsSession::WindowGroupList(int, class CArrayFixFlat<int> *) const
	_asm mov eax, 290
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_291 ()
	{
	// ; int RWsSession::WindowGroupList(class CArrayFixFlat<int> *) const
	_asm mov eax, 291
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_292 ()
	{
	// ; public: int __thiscall RWsSession::RequestOffEvents(int,class RWindowTreeNode *)
	_asm mov eax, 292
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_293 ()
	{
	// ; public: void __thiscall RWindowTreeNode::__DbgTestInvariant(void)const 
	_asm mov eax, 293
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_294 ()
	{
	// ; public: void __thiscall RWindowGroup::DisableScreenChangeEvents(void)
	_asm mov eax, 294
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_295 ()
	{
	// ; public: int __thiscall RWindowGroup::EnableScreenChangeEvents(void)
	_asm mov eax, 295
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_296 ()
	{
	// ; public: void __thiscall RWindowBase::FadeBehind(int)
	_asm mov eax, 296
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_297 ()
	{
	// ; public: void __thiscall CWsScreenDevice::GetDefaultScreenSizeAndRotation(struct TPixelsAndRotation &)const 
	_asm mov eax, 297
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_298 ()
	{
	// ; public: void __thiscall CWsScreenDevice::GetDefaultScreenSizeAndRotation(struct TPixelsTwipsAndRotation &)const 
	_asm mov eax, 298
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_299 ()
	{
	// ; enum TDisplayMode RWindowBase::DisplayMode(void) const
	_asm mov eax, 299
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_300 ()
	{
	// ; public: enum TDisplayMode  __thiscall RWsSession::GetDefModeMaxNumColors(int &,int &)const 
	_asm mov eax, 300
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_301 ()
	{
	// ; public: void __thiscall CWsScreenDevice::GetScreenModeSizeAndRotation(int,struct TPixelsAndRotation &)const 
	_asm mov eax, 301
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_302 ()
	{
	// ; public: void __thiscall CWsScreenDevice::GetScreenModeSizeAndRotation(int,struct TPixelsTwipsAndRotation &)const 
	_asm mov eax, 302
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_303 ()
	{
	// ; public: int __thiscall CWsScreenDevice::NumScreenModes(void)const 
	_asm mov eax, 303
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_304 ()
	{
	// ; public: enum TScreenModeEnforcement  __thiscall CWsScreenDevice::ScreenModeEnforcement(void)const 
	_asm mov eax, 304
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_305 ()
	{
	// ; public: virtual void __thiscall CWindowGc::SetFaded(int)
	_asm mov eax, 305
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_306 ()
	{
	// ; public: void __thiscall RWindowTreeNode::SetFaded(int,enum RWindowTreeNode::TFadeControl)
	_asm mov eax, 306
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_307 ()
	{
	// ; public: void __thiscall RWindowTreeNode::SetNonFading(int)
	_asm mov eax, 307
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_308 ()
	{
	// ; public: void __thiscall CWsScreenDevice::SetScreenMode(int)
	_asm mov eax, 308
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_309 ()
	{
	// ; public: void __thiscall CWsScreenDevice::SetScreenModeEnforcement(enum TScreenModeEnforcement)const 
	_asm mov eax, 309
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_310 ()
	{
	// ; public: void __thiscall CWsScreenDevice::SetScreenSizeAndRotation(struct TPixelsAndRotation const &)
	_asm mov eax, 310
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_311 ()
	{
	// ; public: void __thiscall CWsScreenDevice::SetScreenSizeAndRotation(struct TPixelsTwipsAndRotation const &)
	_asm mov eax, 311
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_312 ()
	{
	// ; public: void __thiscall RWindowGroup::SimulatePointerEvent(class TRawEvent)
	_asm mov eax, 312
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_313 ()
	{
	// ; public: int __thiscall RWsSession::GetColorModeList(class CArrayFixFlat<int> *)const 
	_asm mov eax, 313
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_314 ()
	{
	// ; int RWindowBase::IsFaded(void) const
	_asm mov eax, 314
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_315 ()
	{
	// ; int RWindowBase::IsNonFading(void) const
	_asm mov eax, 315
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_316 ()
	{
	// ; protected: int __thiscall RAnim::Construct(class RWsSprite const &,int,class TDesC8 const &)
	_asm mov eax, 316
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_317 ()
	{
	// ; public: int __thiscall CWsScreenDevice::GetRotationsList(int,class CArrayFixFlat<int> *)const 
	_asm mov eax, 317
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_318 ()
	{
	// ; public: int __thiscall RWindowTreeNode::OrdinalPriority(void)const 
	_asm mov eax, 318
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_319 ()
	{
	// ; public: int __thiscall RWsSession::SendEventToAllWindowGroups(class TWsEvent const &)
	_asm mov eax, 319
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_320 ()
	{
	// ; public: int __thiscall RWsSession::SendEventToAllWindowGroups(int,class TWsEvent const &)
	_asm mov eax, 320
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_321 ()
	{
	// ; public: void __thiscall CWsScreenDevice::SetCurrentRotations(int,enum CFbsBitGc::TGraphicsOrientation)const 
	_asm mov eax, 321
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_322 ()
	{
	// ; public: void __thiscall RWsSession::SimulateKeyEvent(struct TKeyEvent)
	_asm mov eax, 322
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_323 ()
	{
	// ; public: void __thiscall RWsSession::SetRemoveKeyCode(int)
	_asm mov eax, 323
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_324 ()
	{
	// ; public: void __thiscall RWsSession::ClearDefaultSystemPointerCursor(void)
	_asm mov eax, 324
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_325 ()
	{
	// ; public: void __thiscall RWindowTreeNode::ClearPointerCursor(void)
	_asm mov eax, 325
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_326 ()
	{
	// ; public: class TRect  __thiscall RWsSession::PointerCursorArea(int)const 
	_asm mov eax, 326
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_327 ()
	{
	// ; public: class TRect  __thiscall RWsSession::PointerCursorArea(void)const 
	_asm mov eax, 327
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_328 ()
	{
	// ; public: enum TPointerCursorMode  __thiscall RWsSession::PointerCursorMode(void)const 
	_asm mov eax, 328
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_329 ()
	{
	// ; public: class TPoint  __thiscall RWsSession::PointerCursorPosition(void)const 
	_asm mov eax, 329
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_330 ()
	{
	// ; public: void __thiscall RWsSession::SetDefaultSystemPointerCursor(int)
	_asm mov eax, 330
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_331 ()
	{
	// ; public: void __thiscall RWsSession::SetPointerCursorArea(class TRect const &)
	_asm mov eax, 331
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_332 ()
	{
	// ; public: void __thiscall RWsSession::SetPointerCursorArea(int,class TRect const &)
	_asm mov eax, 332
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_333 ()
	{
	// ; public: void __thiscall RWsSession::SetPointerCursorMode(enum TPointerCursorMode)
	_asm mov eax, 333
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_334 ()
	{
	// ; public: int __thiscall RWsSession::SetPointerCursorPosition(class TPoint const &)
	_asm mov eax, 334
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_335 ()
	{
	// ; public: void __thiscall RWsSession::SimulateXyInputType(enum TXYInputType)
	_asm mov eax, 335
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_336 ()
	{
	// ; public: int __thiscall RWindowBase::MoveToGroup(int)
	_asm mov eax, 336
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_337 ()
	{
	// ; public: int __thiscall RWsSession::SendMessageToAllWindowGroups(int,class TUid,class TDesC8 const &)
	_asm mov eax, 337
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_338 ()
	{
	// ; public: int __thiscall RWsSession::SendMessageToAllWindowGroups(class TUid,class TDesC8 const &)
	_asm mov eax, 338
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_339 ()
	{
	// ; public: void __thiscall RWindowTreeNode::DisableFocusChangeEvents(void)
	_asm mov eax, 339
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_340 ()
	{
	// ; public: int __thiscall RWindowTreeNode::EnableFocusChangeEvents(void)
	_asm mov eax, 340
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_341 ()
	{
	// ; public: void __thiscall RWsSession::SetDefaultFadingParameters(unsigned char,unsigned char)
	_asm mov eax, 341
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_342 ()
	{
	// ; public: void __thiscall RWindowTreeNode::SetFaded(int,enum RWindowTreeNode::TFadeControl,unsigned char,unsigned char)
	_asm mov eax, 342
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_343 ()
	{
	// ; public: virtual void __thiscall CWindowGc::SetFadingParameters(unsigned char,unsigned char)
	_asm mov eax, 343
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_344 ()
	{
	// ; public: void __thiscall RWsSession::PrepareForSwitchOff(void)
	_asm mov eax, 344
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_345 ()
	{
	// ; public: int __thiscall CWsScreenDevice::SetCustomPalette(class CPalette const *)
	_asm mov eax, 345
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_346 ()
	{
	// ; public: __thiscall RDirectScreenAccess::RDirectScreenAccess(class RWsSession &)
	_asm mov eax, 346
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_347 ()
	{
	// ; public: __thiscall RDirectScreenAccess::RDirectScreenAccess(void)
	_asm mov eax, 347
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_348 ()
	{
	// ; public: void __thiscall RDirectScreenAccess::Cancel(void)
	_asm mov eax, 348
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_349 ()
	{
	// ; public: void __thiscall RDirectScreenAccess::Close(void)
	_asm mov eax, 349
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_350 ()
	{
	// ; public: void __thiscall RDirectScreenAccess::Completed(void)
	_asm mov eax, 350
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_351 ()
	{
	// ; public: int __thiscall RDirectScreenAccess::Construct(void)
	_asm mov eax, 351
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_352 ()
	{
	// ; public: static class CDirectScreenAccess * __cdecl CDirectScreenAccess::NewL(class RWsSession &,class CWsScreenDevice &,class RWindowBase &,class MDirectScreenAccess &)
	_asm mov eax, 352
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_353 ()
	{
	// ; public: int __thiscall RDirectScreenAccess::Request(class RRegion * &,class TRequestStatus &,class RWindowBase const &)
	_asm mov eax, 353
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_354 ()
	{
	// ; public: void __thiscall CDirectScreenAccess::StartL(void)
	_asm mov eax, 354
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_355 ()
	{
	// ; public: void __thiscall RWsSession::SetBufferSizeL(int)
	_asm mov eax, 355
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_356 ()
	{
	// ; public: int __thiscall RWsSession::SetSystemFaded(int)
	_asm mov eax, 356
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_357 ()
	{
	// ; public: int __thiscall RWsSession::SetSystemFaded(int,unsigned char,unsigned char)
	_asm mov eax, 357
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_358 ()
	{
	// ; public: void __thiscall RWindowTreeNode::DisableGroupListChangeEvents(void)
	_asm mov eax, 358
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_359 ()
	{
	// ; public: int __thiscall RWindowTreeNode::EnableGroupListChangeEvents(void)
	_asm mov eax, 359
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_360 ()
	{
	// ; public: int __thiscall RSoundPlugIn::Construct(class TUid)
	_asm mov eax, 360
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_361 ()
	{
	// ; public: void __thiscall RSoundPlugIn::Destroy(void)
	_asm mov eax, 361
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_362 ()
	{
	// ; int RSoundPlugIn::IsLoaded(int &) const
	_asm mov eax, 362
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_363 ()
	{
	// ; public: int __thiscall RSoundPlugIn::Load(class TDesC16 const &)
	_asm mov eax, 363
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_364 ()
	{
	// ; public: int __thiscall RSoundPlugIn::Unload(void)
	_asm mov eax, 364
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_365 ()
	{
	// ; public: int __thiscall CWsScreenDevice::CurrentScreenMode(void)const 
	_asm mov eax, 365
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_366 ()
	{
	// ; public: void __thiscall RWindowGroup::CancelCaptureLongKey(long)
	_asm mov eax, 366
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_367 ()
	{
	// ; public: long __thiscall RWindowGroup::CaptureLongKey(unsigned int,unsigned int,unsigned int,unsigned int,int,unsigned int)
	_asm mov eax, 367
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_368 ()
	{
	// ; public: long __thiscall RWindowGroup::CaptureLongKey(class TTimeIntervalMicroSeconds32,unsigned int,unsigned int,unsigned int,unsigned int,int,unsigned int)
	_asm mov eax, 368
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_369 ()
	{
	// ; public: int __thiscall RWsSession::SendEventToOneWindowGroupsPerClient(class TWsEvent const &)
	_asm mov eax, 369
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_370 ()
	{
	// ; int RSoundPlugIn::KeyClickEnabled(void) const
	_asm mov eax, 370
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_371 ()
	{
	// ; int RSoundPlugIn::PenClickEnabled(void) const
	_asm mov eax, 371
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_372 ()
	{
	// ; public: void __thiscall RSoundPlugIn::SetKeyClick(int)
	_asm mov eax, 372
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_373 ()
	{
	// ; public: void __thiscall RSoundPlugIn::SetPenClick(int)
	_asm mov eax, 373
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_374 ()
	{
	// ; public: long __thiscall RWindowGroup::CaptureKey(unsigned int,unsigned int,unsigned int,int)
	_asm mov eax, 374
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_375 ()
	{
	// ; public: long __thiscall RWindowGroup::CaptureKeyUpAndDowns(unsigned int,unsigned int,unsigned int,int)
	_asm mov eax, 375
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_376 ()
	{
	// ; public: void __thiscall RWsSession::LogCommand(enum RWsSession::TLoggingCommand)
	_asm mov eax, 376
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_377 ()
	{
	// ; public: __thiscall RSoundPlugIn::RSoundPlugIn(class RWsSession &)
	_asm mov eax, 377
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_378 ()
	{
	// ; public: __thiscall RSoundPlugIn::RSoundPlugIn(void)
	_asm mov eax, 378
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_379 ()
	{
	// ; public: void __thiscall RSoundPlugIn::Close(void)
	_asm mov eax, 379
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_380 ()
	{
	// ; public: int __thiscall RSoundPlugIn::CommandReply(int,class TPtrC8 const &)
	_asm mov eax, 380
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_381 ()
	{
	// ; public: int __thiscall RWsSession::SetCustomTextCursor(int,class TArray<struct TSpriteMember> const &,unsigned int,enum RWsSession::TCustomTextCursorAlignment)
	_asm mov eax, 381
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_382 ()
	{
	// ; public: void __thiscall RWindow::HandleTransparencyUpdate(void)
	_asm mov eax, 382
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_383 ()
	{
	// ; public: int __thiscall RWindow::SetTransparencyBitmap(class CFbsBitmap const &)
	_asm mov eax, 383
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_384 ()
	{
	// ; public: int __thiscall RWindow::SetTransparencyFactor(class TRgb const &)
	_asm mov eax, 384
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_385 ()
	{
	// ; public: void __thiscall RWindow::SetNonTransparent(void)
	_asm mov eax, 385
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_386 ()
	{
	// ; public: int __thiscall RWsSession::TestWriteReplyByProvidingRemoteReadAccess(int,int,class TDesC8 const &,class TDesC16 const &)
	_asm mov eax, 386
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_387 ()
	{
	// ; public: int __thiscall RWsSession::TestWriteReplyByProvidingRemoteReadAccess(int,int,class TDesC8 const &,class TDesC8 const &)
	_asm mov eax, 387
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_388 ()
	{
	// ; int RAnim::CommandReply(int, class TDesC8 const &, class TIpcArgs const &)
	_asm mov eax, 388
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_389 ()
	{
	// ; int RAnim::Construct(class RWindowBase const &, int, class TDesC8 const &, class TIpcArgs const &)
	_asm mov eax, 389
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_390 ()
	{
	// ; int RAnim::Construct(class RWsSprite const &, int, class TDesC8 const &, class TIpcArgs const &)
	_asm mov eax, 390
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_391 ()
	{
	// ; void RAnim::AsyncCommandReply(class TRequestStatus &, int, class TIpcArgs const &)
	_asm mov eax, 391
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_392 ()
	{
	// ; public: class TPoint  __thiscall RWindowBase::AbsPosition(void)const 
	_asm mov eax, 392
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_393 ()
	{
	// ; public: int __thiscall CWsScreenDevice::RectCompare(class TRect const &,class TRect const &,unsigned int)const 
	_asm mov eax, 393
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_394 ()
	{
	// ; public: class TPoint  __thiscall CWsScreenDevice::GetDefaultScreenModeOrigin(void)const 
	_asm mov eax, 394
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_395 ()
	{
	// ; public: class TPoint  __thiscall CWsScreenDevice::GetScreenModeOrigin(int)const 
	_asm mov eax, 395
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_396 ()
	{
	// ; void RWindow::EnableRedrawStore(int)
	_asm mov eax, 396
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_397 ()
	{
	// ; void CWindowGc::AlphaBlendBitmaps(class TPoint const &, class CFbsBitmap const *, class TRect const &, class CFbsBitmap const *, class TPoint const &)
	_asm mov eax, 397
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_398 ()
	{
	// ; void CWindowGc::AlphaBlendBitmaps(class TPoint const &, class CWsBitmap const *, class TRect const &, class CWsBitmap const *, class TPoint const &)
	_asm mov eax, 398
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_399 ()
	{
	// ; void CWindowGc::SetOpaque(int)
	_asm mov eax, 399
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_400 ()
	{
	// ; public: class TSizeMode  __thiscall CWsScreenDevice::GetCurrentScreenModeAttributes(void)const 
	_asm mov eax, 400
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_401 ()
	{
	// ; public: class TPoint  __thiscall CWsScreenDevice::GetCurrentScreenModeScaledOrigin(void)const 
	_asm mov eax, 401
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_402 ()
	{
	// ; public: class TSize  __thiscall CWsScreenDevice::GetCurrentScreenModeScale(void)const 
	_asm mov eax, 402
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_403 ()
	{
	// ; public: class TPoint  __thiscall CWsScreenDevice::GetScreenModeScaledOrigin(int)const 
	_asm mov eax, 403
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_404 ()
	{
	// ; public: class TSize  __thiscall CWsScreenDevice::GetScreenModeScale(int)const 
	_asm mov eax, 404
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_405 ()
	{
	// ; public: void __thiscall CWsScreenDevice::SetAppScreenMode(int)
	_asm mov eax, 405
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_406 ()
	{
	// ; public: void __thiscall CWsScreenDevice::SetCurrentScreenModeAttributes(class TSizeMode const &)
	_asm mov eax, 406
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_407 ()
	{
	// ; void CWindowGc::DrawBitmapMasked(class TRect const &, class CFbsBitmap const *, class TRect const &, class CFbsBitmap const *, int)
	_asm mov eax, 407
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_408 ()
	{
	// ; int CWsScreenDevice::Construct(int)
	_asm mov eax, 408
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_409 ()
	{
	// ; int CWsScreenDevice::GetScreenNumber(void) const
	_asm mov eax, 409
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_410 ()
	{
	// ; int RWsSession::GetFocusScreen(void) const
	_asm mov eax, 410
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_411 ()
	{
	// ; int RWsSession::SetFocusScreen(int)
	_asm mov eax, 411
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_412 ()
	{
	// ; public: int __thiscall RWindowGroup::ConstructChildApp(int,unsigned long)
	_asm mov eax, 412
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_413 ()
	{
	// ; public: int __thiscall RWindowGroup::ConstructChildApp(int,unsigned long,int)
	_asm mov eax, 413
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_414 ()
	{
	// ; int RWsSession::WindowGroupList(int, class RArray<struct RWsSession::TWindowGroupChainInfo> *) const
	_asm mov eax, 414
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_415 ()
	{
	// ; int RWsSession::WindowGroupList(class RArray<struct RWsSession::TWindowGroupChainInfo> *) const
	_asm mov eax, 415
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_416 ()
	{
	// ; public: void __thiscall RWindowGroup::AllowProcessToCreateChildWindowGroups(class TUid)
	_asm mov eax, 416
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_417 ()
	{
	// ; public: int __thiscall RWindow::SetTransparencyWsBitmap(class CWsBitmap const &)
	_asm mov eax, 417
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_418 ()
	{
	// ; int CWsScreenDevice::GetScreenSizeModeList(class RArray<int> *) const
	_asm mov eax, 418
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_419 ()
	{
	// ; int CWsScreenDevice::GetNearestFontToDesignHeightInPixels(class CFont * &, class TFontSpec const &)
	_asm mov eax, 419
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_420 ()
	{
	// ; int CWsScreenDevice::GetNearestFontToDesignHeightInTwips(class CFont * &, class TFontSpec const &)
	_asm mov eax, 420
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_421 ()
	{
	// ; int CWsScreenDevice::GetNearestFontToMaxHeightInPixels(class CFont * &, class TFontSpec const &, int)
	_asm mov eax, 421
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_422 ()
	{
	// ; int CWsScreenDevice::GetNearestFontToMaxHeightInTwips(class CFont * &, class TFontSpec const &, int)
	_asm mov eax, 422
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_423 ()
	{
	// ; void RWindowTreeNode::DisableVisibilityChangeEvents(void)
	_asm mov eax, 423
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_424 ()
	{
	// ; int RWindowTreeNode::EnableVisibilityChangeEvents(void)
	_asm mov eax, 424
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_425 ()
	{
	// ; int RWindow::SetTransparencyAlphaChannel(void)
	_asm mov eax, 425
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_426 ()
	{
	// ; void RBlankWindow::SetColor(void)
	_asm mov eax, 426
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_427 ()
	{
	// ; int RWsSession::SetClientCursorMode(enum TPointerCursorMode)
	_asm mov eax, 427
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_428 ()
	{
	// ; class TRect RDrawableWindow::GetDrawRect(void) const
	_asm mov eax, 428
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_429 ()
	{
	// (noname)
	_asm mov eax, 429
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_430 ()
	{
	// (noname)
	_asm mov eax, 430
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_431 ()
	{
	// ; void CWindowGc::Reserved_CWindowGc_3(void)
	_asm mov eax, 431
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_432 ()
	{
	// ; void CWindowGc::Reserved_CWindowGc_4(void)
	_asm mov eax, 432
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_433 ()
	{
	// ; void CWindowGc::Reserved_CWindowGc_5(void)
	_asm mov eax, 433
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_434 ()
	{
	// ; void CWindowGc::Reserved_CBitmapContext_1(void)
	_asm mov eax, 434
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_435 ()
	{
	// ; void CWindowGc::Reserved_CBitmapContext_2(void)
	_asm mov eax, 435
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_436 ()
	{
	// ; void CWindowGc::Reserved_CBitmapContext_3(void)
	_asm mov eax, 436
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_437 ()
	{
	// ; int CWindowGc::APIExtension(class TUid, int*&, int *)
	_asm mov eax, 437
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_438 ()
	{
	// ; void CWindowGc::Reserved_CGraphicsContext_2(void)
	_asm mov eax, 438
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_439 ()
	{
	// ; void CWindowGc::DrawBitmapMasked(class TRect const &, class CWsBitmap const *, class TRect const &, class CWsBitmap const *, int)
	_asm mov eax, 439
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_440 ()
	{
	// ; int RWsSession::Connect(class RFs &)
	_asm mov eax, 440
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_441 ()
	{
	// ; enum TDisplayMode CWsScreenDevice::GetScreenModeDisplayMode(int const &) const
	_asm mov eax, 441
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_442 ()
	{
	// ; public: void __thiscall RWsSession::ClearAllRedrawStores(void)
	_asm mov eax, 442
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_443 ()
	{
	// ; int RWindowTreeNode::WindowGroupId(void) const
	_asm mov eax, 443
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_444 ()
	{
	// ; int RWindowBase::GetPointerCapturePriority(void) const
	_asm mov eax, 444
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_445 ()
	{
	// ; void RWindowBase::SetPointerCapturePriority(int)
	_asm mov eax, 445
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_446 ()
	{
	// ; int RWindow::SetTransparentRegion(class TRegion const &)
	_asm mov eax, 446
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_447 ()
	{
	// ; int RWindow::SetTransparencyPolicy(enum TWsTransparencyPolicy)
	_asm mov eax, 447
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_448 ()
	{
	// ; int RWindow::IsRedrawStoreEnabled(void) const
	_asm mov eax, 448
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_449 ()
	{
	// ; int CWsScreenDevice::SetBackLight(int)
	_asm mov eax, 449
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_450 ()
	{
	// ; int RWindowGroup::SetOrdinalPositionErr(int, int)
	_asm mov eax, 450
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_451 ()
	{
	// ; int RWindowGroup::ClearChildGroup(void)
	_asm mov eax, 451
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_452 ()
	{
	// ; int RWindowGroup::SetChildGroup(int)
	_asm mov eax, 452
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_453 ()
	{
	// ; class TUid TWsGraphicId::Uid(void) const
	_asm mov eax, 453
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_454 ()
	{
	// ; CWsGraphic::CWsGraphic(void)
	_asm mov eax, 454
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_455 ()
	{
	// ; RWsGraphicMsgBuf::RWsGraphicMsgBuf(void)
	_asm mov eax, 455
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_456 ()
	{
	// ; TWsGraphicMsgFixedBase::TWsGraphicMsgFixedBase(class TUid, int)
	_asm mov eax, 456
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_457 ()
	{
	// ; CWsGraphic::~CWsGraphic(void)
	_asm mov eax, 457
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_458 ()
	{
	// ; int RWsGraphicMsgBuf::Append(class TWsGraphicMsgFixedBase const &)
	_asm mov eax, 458
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_459 ()
	{
	// ; int RWsGraphicMsgBuf::Append(class TUid, class TDesC16 const &)
	_asm mov eax, 459
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_460 ()
	{
	// ; int RWsGraphicMsgBuf::Append(class TUid, class TDesC8 const &)
	_asm mov eax, 460
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_461 ()
	{
	// ; int RWsGraphicMsgBuf::Append(class TUid, int, class TPtr8 &)
	_asm mov eax, 461
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_462 ()
	{
	// ; void CWsGraphic::BaseConstructL(class TWsGraphicId const &, class TUid, class TDesC8 const &)
	_asm mov eax, 462
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_463 ()
	{
	// ; void CWsGraphic::BaseConstructL(class TUid, class TUid, class TDesC8 const &)
	_asm mov eax, 463
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_464 ()
	{
	// ; void CWsGraphic::BaseConstructL(class TUid, class TDesC8 const &)
	_asm mov eax, 464
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_465 ()
	{
	// ; int CWsGraphic::CWsGraphic_Reserved1(void)
	_asm mov eax, 465
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_466 ()
	{
	// ; int CWsGraphic::CWsGraphic_Reserved2(void)
	_asm mov eax, 466
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_467 ()
	{
	// ; int CWsGraphic::CWsGraphic_Reserved3(void)
	_asm mov eax, 467
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_468 ()
	{
	// ; int TWsGraphicId::Compare(class TWsGraphicId const &) const
	_asm mov eax, 468
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_469 ()
	{
	// ; int RWsGraphicMsgBuf::Count(void) const
	_asm mov eax, 469
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_470 ()
	{
	// ; class TPtrC8 RWsGraphicMsgBuf::Data(int) const
	_asm mov eax, 470
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_471 ()
	{
	// ; void CWsGraphic::Destroy(void)
	_asm mov eax, 471
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_472 ()
	{
	// ; void RWsGraphicMsgBuf::GetFixedMsg(class TWsGraphicMsgFixedBase &, int) const
	_asm mov eax, 472
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_473 ()
	{
	// ; class TWsGraphicId const & CWsGraphic::Id(void) const
	_asm mov eax, 473
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_474 ()
	{
	// ; int CWsGraphic::IsActive(void) const
	_asm mov eax, 474
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_475 ()
	{
	// ; int TWsGraphicId::IsId(void) const
	_asm mov eax, 475
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_476 ()
	{
	// ; void CWsGraphic::OnClientClose(void)
	_asm mov eax, 476
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_477 ()
	{
	// ; class TDesC8 const & RWsGraphicMsgBuf::Pckg(void) const
	_asm mov eax, 477
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_478 ()
	{
	// ; class TPtrC8 TWsGraphicMsgFixedBase::Pckg(void) const
	_asm mov eax, 478
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_479 ()
	{
	// ; void RWsGraphicMsgBuf::Remove(int)
	_asm mov eax, 479
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_480 ()
	{
	// ; void CWsGraphic::SendMessage(class TDesC8 const &) const
	_asm mov eax, 480
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_481 ()
	{
	// ; void TWsGraphicId::Set(int)
	_asm mov eax, 481
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_482 ()
	{
	// ; void TWsGraphicId::Set(class TUid)
	_asm mov eax, 482
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_483 ()
	{
	// ; int CWsGraphic::Share(class TSecureId)
	_asm mov eax, 483
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_484 ()
	{
	// ; int CWsGraphic::ShareGlobally(void)
	_asm mov eax, 484
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_485 ()
	{
	// ; int TWsGraphicMsgFixedBase::Size(void) const
	_asm mov eax, 485
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_486 ()
	{
	// ; class TUid RWsGraphicMsgBuf::TypeId(int) const
	_asm mov eax, 486
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_487 ()
	{
	// ; class TUid TWsGraphicMsgFixedBase::TypeId(void) const
	_asm mov eax, 487
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_488 ()
	{
	// ; int CWsGraphic::UnShare(class TSecureId)
	_asm mov eax, 488
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_489 ()
	{
	// ; int CWsGraphic::UnShareGlobally(void)
	_asm mov eax, 489
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_490 ()
	{
	// ; class TPtr8 RWsGraphicMsgBuf::Data(int)
	_asm mov eax, 490
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_491 ()
	{
	// ; TWsGraphicId::TWsGraphicId(class TWsGraphicId const &)
	_asm mov eax, 491
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_492 ()
	{
	// ; TWsGraphicId::TWsGraphicId(int)
	_asm mov eax, 492
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_493 ()
	{
	// ; int CWsGraphic::Flush(void) const
	_asm mov eax, 493
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_494 ()
	{
	// ; int RWindowGroup::Construct(unsigned long, int, class CWsScreenDevice *)
	_asm mov eax, 494
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_495 ()
	{
	// ; int RWindowGroup::Construct(unsigned long, class CWsScreenDevice *)
	_asm mov eax, 495
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_496 ()
	{
	// ; int RWsSession::GetColorModeList(int, class CArrayFixFlat<int> *) const
	_asm mov eax, 496
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_497 ()
	{
	// ; enum TDisplayMode RWsSession::GetDefModeMaxNumColors(int, int &, int &) const
	_asm mov eax, 497
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_498 ()
	{
	// ; int RWsSession::GetDefaultOwningWindow(int) const
	_asm mov eax, 498
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_499 ()
	{
	// ; int RWsSession::GetFocusWindowGroup(int) const
	_asm mov eax, 499
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_500 ()
	{
	// ; int RWsSession::NumWindowGroups(int, int) const
	_asm mov eax, 500
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_501 ()
	{
	// ; int RWsSession::NumberOfScreens(void) const
	_asm mov eax, 501
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_502 ()
	{
	// ; int RWsSession::WindowGroupList(class CArrayFixFlat<int> *, int, int) const
	_asm mov eax, 502
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_503 ()
	{
	// ; TWsGraphicId::TWsGraphicId(class TUid)
	_asm mov eax, 503
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_504 ()
	{
	// ; public: void __thiscall RWsSession::SetMaxBufferSizeL(int)
	_asm mov eax, 504
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_505 ()
	{
	// ; void RWindow::EnableOSB(int)
	_asm mov eax, 505
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_506 ()
	{
	// ; int TWsGraphicId::Id(void) const
	_asm mov eax, 506
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_507 ()
	{
	// ; int TWsGraphicId::IsUid(void) const
	_asm mov eax, 507
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_508 ()
	{
	// ; void CWsGraphic::SetGraphicExtension(class MWsObjectProvider *)
	_asm mov eax, 508
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_509 ()
	{
	// ; int CWsGraphic::SendSynchronMessage(class TDesC8 const &) const
	_asm mov eax, 509
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_510 ()
	{
	// ; int RWsSession::DebugInfo(int, class TDes8 &, int) const
	_asm mov eax, 510
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_511 ()
	{
	// ; int RWsSession::DebugInfo(int, int) const
	_asm mov eax, 511
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_512 ()
	{
	// ; unsigned long RWindowTreeNode::ClientHandle(void) const
	_asm mov eax, 512
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_513 ()
	{
	// ; int RWindowBase::SetBackgroundSurface(class TSurfaceId const &)
	_asm mov eax, 513
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_514 ()
	{
	// (noname)
	_asm mov eax, 514
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_515 ()
	{
	// ; class TRgb RWindowBase::KeyColor(void) const
	_asm mov eax, 515
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_516 ()
	{
	// (noname)
	_asm mov eax, 516
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_517 ()
	{
	// (noname)
	_asm mov eax, 517
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_518 ()
	{
	// (noname)
	_asm mov eax, 518
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_519 ()
	{
	// (noname)
	_asm mov eax, 519
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_520 ()
	{
	// (noname)
	_asm mov eax, 520
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_521 ()
	{
	// ; int RWindowBase::GetBackgroundSurface(class TSurfaceConfiguration &) const
	_asm mov eax, 521
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_522 ()
	{
	// (noname)
	_asm mov eax, 522
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_523 ()
	{
	// ; int RWsSession::PreferredSurfaceConfigurationSize(void) const
	_asm mov eax, 523
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_524 ()
	{
	// ; int RWsSession::RegisterSurface(int, class TSurfaceId const &)
	_asm mov eax, 524
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_525 ()
	{
	// (noname)
	_asm mov eax, 525
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_526 ()
	{
	// ; void RWindowBase::RemoveBackgroundSurface(int)
	_asm mov eax, 526
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_527 ()
	{
	// (noname)
	_asm mov eax, 527
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_528 ()
	{
	// ; int RWindowBase::SetBackgroundSurface(class TSurfaceConfiguration const &, int)
	_asm mov eax, 528
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_529 ()
	{
	// ; void RWsSession::UnregisterSurface(int, class TSurfaceId const &)
	_asm mov eax, 529
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_530 ()
	{
	// ; void RWindow::ClearRedrawStore(void)
	_asm mov eax, 530
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_531 ()
	{
	// ; int RWsSession::Finish(void)
	_asm mov eax, 531
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_532 ()
	{
	// ; void RWsSession::SyncMsgBuf(void)
	_asm mov eax, 532
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_533 ()
	{
	// ; class RWsSession & CWsGraphic::Session(void)
	_asm mov eax, 533
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_534 ()
	{
	// ; int RWindowTreeNode::ScreenNumber(void) const
	_asm mov eax, 534
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_535 ()
	{
	// ; void TWsEvent::SetPointerNumber(unsigned char)
	_asm mov eax, 535
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_536 ()
	{
	// ; int TAdvancedPointerEvent::DoGetPointerNumber(void) const
	_asm mov eax, 536
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_537 ()
	{
	// ; int RWindowBase::ClaimPointerGrab(unsigned char, int)
	_asm mov eax, 537
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_538 ()
	{
	// ; int RWindowBase::RequestPointerRepeatEvent(class TTimeIntervalMicroSeconds32, class TRect const &, unsigned char)
	_asm mov eax, 538
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_539 ()
	{
	// ; int RWsSession::GetExitHighPressureThreshold(void) const
	_asm mov eax, 539
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_540 ()
	{
	// ; int RWsSession::SetCloseProximityThresholds(int, int)
	_asm mov eax, 540
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_541 ()
	{
	// ; int RWsSession::SetHighPressureThresholds(int, int)
	_asm mov eax, 541
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_542 ()
	{
	// ; int RWindowBase::CancelPointerRepeatEventRequest(unsigned char)
	_asm mov eax, 542
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_543 ()
	{
	// ; void TWsEvent::InitAdvancedPointerEvent(enum TPointerEvent::TType, unsigned int, class TPoint3D const &, unsigned char)
	_asm mov eax, 543
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_544 ()
	{
	// ; int RWsSession::GetExitCloseProximityThreshold(void) const
	_asm mov eax, 544
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_545 ()
	{
	// ; void RWindowBase::EnableAdvancedPointers(void)
	_asm mov eax, 545
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_546 ()
	{
	// ; void TWsEvent::SetPointerZ(int)
	_asm mov eax, 546
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_547 ()
	{
	// ; int TAdvancedPointerEvent::DoGetProximityAndPressure(void) const
	_asm mov eax, 547
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_548 ()
	{
	// ; int TAdvancedPointerEvent::DoGetProximity(void) const
	_asm mov eax, 548
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_549 ()
	{
	// ; int TAdvancedPointerEvent::DoGetPressure(void) const
	_asm mov eax, 549
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_550 ()
	{
	// ; void RWindowGroup::SimulateAdvancedPointerEvent(class TRawEvent)
	_asm mov eax, 550
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_551 ()
	{
	// ; int RWsSession::GetEnterHighPressureThreshold(void) const
	_asm mov eax, 551
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_552 ()
	{
	// ; int RWsSession::GetEnterCloseProximityThreshold(void) const
	_asm mov eax, 552
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_553 ()
	{
	// ; class TAdvancedPointerEvent & TAdvancedPointerEvent::operator=(class TAdvancedPointerEvent const &)
	_asm mov eax, 553
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_554 ()
	{
	// ; TAdvancedPointerEvent::TAdvancedPointerEvent(class TAdvancedPointerEvent const &)
	_asm mov eax, 554
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_555 ()
	{
	// ; RWsDrawableSource::RWsDrawableSource(void)
	_asm mov eax, 555
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_556 ()
	{
	// ; void RWsDrawableSource::Close(void)
	_asm mov eax, 556
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_557 ()
	{
	// ; int RWsDrawableSource::Create(class RSgDrawable const &)
	_asm mov eax, 557
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_558 ()
	{
	// ; class TSgDrawableId const & RWsDrawableSource::DrawableId(void) const
	_asm mov eax, 558
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_559 ()
	{
	// ; void CWindowGc::DrawResource(class TPoint const &, class RWsDrawableSource const &, enum CWindowGc::TGraphicsRotation)
	_asm mov eax, 559
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_560 ()
	{
	// ; void CWindowGc::DrawResource(class TRect const &, class RWsDrawableSource const &, enum CWindowGc::TGraphicsRotation)
	_asm mov eax, 560
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_561 ()
	{
	// ; void CWindowGc::DrawResource(class TRect const &, class RWsDrawableSource const &, class TRect const &, enum CWindowGc::TGraphicsRotation)
	_asm mov eax, 561
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_562 ()
	{
	// ; void CWindowGc::DrawResource(class TRect const &, class RWsDrawableSource const &, class TDesC8 const &)
	_asm mov eax, 562
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_563 ()
	{
	// ; int RDirectScreenAccess::Construct(int)
	_asm mov eax, 563
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_564 ()
	{
	// ; class CDirectScreenAccess * CDirectScreenAccess::NewL(class RWsSession &, class CWsScreenDevice &, class RWindowBase &, class MDirectScreenAccess &, int)
	_asm mov eax, 564
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_565 ()
	{
	// ; int RWsDrawableSource::Create(class RSgDrawable const &, int)
	_asm mov eax, 565
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_566 ()
	{
	// ; RWsDrawableSource::RWsDrawableSource(class RWsSession &)
	_asm mov eax, 566
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_567 ()
	{
	// ; int RWsDrawableSource::ScreenNumber(void) const
	_asm mov eax, 567
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_568 ()
	{
	// ; void * CWsScreenDevice::GetInterface(unsigned int)
	_asm mov eax, 568
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_569 ()
	{
	// ; int CWsScreenDevice::IsCurrentModeDynamic(void) const
	_asm mov eax, 569
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_570 ()
	{
	// ; int CWsScreenDevice::IsModeDynamic(int) const
	_asm mov eax, 570
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_571 ()
	{
	// ; void const * CWindowGc::Interface(class TUid) const
	_asm mov eax, 571
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_572 ()
	{
	// ; void * CWindowGc::Interface(class TUid)
	_asm mov eax, 572
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_573 ()
	{
	// ; class RWsSession * RWindowTreeNode::Session(void) const
	_asm mov eax, 573
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_574 ()
	{
	// ; void RWsSession::HeapSetBurstFail(int, int, int)
	_asm mov eax, 574
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_575 ()
	{
	// ; void RWsSession::EnableWindowSizeCacheL(void)
	_asm mov eax, 575
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_576 ()
	{
	// ; void RWindowBase::SetSurfaceTransparency(int)
	_asm mov eax, 576
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_577 ()
	{
	// ; class TSize RWindowBase::SizeForEgl(void) const
	_asm mov eax, 577
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_578 ()
	{
	// ; int RWindowBase::FixNativeOrientation(void)
	_asm mov eax, 578
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_579 ()
	{
	// ; void RWindowBase::SetPurpose(int)
	_asm mov eax, 579
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_580 ()
	{
	// ; void RWindowBase::SendEffectCommand(int, class TDesC8 const &)
	_asm mov eax, 580
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_581 ()
	{
	// ; void RWsSession::SendEffectCommand(int, class TDesC8 const &)
	_asm mov eax, 581
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_582 ()
	{
	// ; void RWsSession::RegisterEffect(int, int, class TBuf<256> const &, class TBuf<256> const &, class TBuf<256> const &, unsigned int, class TBitFlagsT<unsigned long>)
	_asm mov eax, 582
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_583 ()
	{
	// ; void RWindowBase::OverrideEffects(int, class TBuf<256> const &, class TBuf<256> const &, class TBuf<256> const &, class TBitFlagsT<unsigned long>)
	_asm mov eax, 583
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_584 ()
	{
	// ; void RWsSession::OverrideEffects(int, int, class TBuf<256> const &, class TBuf<256> const &, class TBuf<256> const &, class TBitFlagsT<unsigned long>)
	_asm mov eax, 584
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_585 ()
	{
	// ; void RWsSession::UnregisterEffect(int, int, unsigned int)
	_asm mov eax, 585
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_586 ()
	{
	// ; void RWsSession::UnregisterAllEffects(void)
	_asm mov eax, 586
	_asm jmp common_dispatch
	}

__declspec(dllexport)
__declspec(naked)
void call_vector_587 ()
	{
	// ; void RWsSession::IndicateAppOrientation(enum TRenderOrientation)
	_asm mov eax, 587
	_asm jmp common_dispatch
	}

}
#define MAX_ORDINAL 588

