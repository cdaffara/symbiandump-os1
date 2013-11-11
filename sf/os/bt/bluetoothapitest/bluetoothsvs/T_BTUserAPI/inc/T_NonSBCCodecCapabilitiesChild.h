/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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


#if (!defined __T_NON_SBC_CODEC_CAPABILITIES_CHILD_H__)
#define __T_NON_SBC_CODEC_CAPABILITIES_CHILD_H__

class T_NonSBCCodecCapabilitiesChild : public TNonSBCCodecCapabilities
	{
public:
	T_NonSBCCodecCapabilitiesChild(SymbianBluetoothAV::TBluetoothMediaType aMediaType,
			TBluetoothMediaCodecType aCodecType);
	void SetMediaCodecType(TBluetoothMediaCodecType aMediaCodecType);
	void SetMediaType(SymbianBluetoothAV::TBluetoothMediaType aMediaType);
	};

#endif /* __T_NON_SBC_CODEC_CAPABILITIES_CHILD_H__ */
