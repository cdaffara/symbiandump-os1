// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

#ifndef BLUETOOTH_LOGGER_COMPONENTS_H
#define BLUETOOTH_LOGGER_COMPONENTS_H

// This file provides a centralised place for the definition
// of log component strings for use with the Bluetooth logger.

// The Bluetooth PRT
#define LOG_COMPONENT_BT_PRT				"BtPrt"

// Class of Device (CoD) Service Manager
#define LOG_COMPONENT_BT_COD				"CoD"

// Incoming connection listener
#define LOG_COMPONENT_IN_CONN_LISTENER		"InConnListener"

// The Bluetooth Logger
#define LOG_COMPONENT_LOGGER				"LogEngine"

// HCIv2 Framework
#define LOG_COMPONENT_HCICMDQ				"HciCmdQ"
#define LOG_COMPONENT_COREHCI_BASE			"CoreHciBase"
#define LOG_COMPONENT_HCTL_BASE				"HctlBase"
#define LOG_COMPONENT_QDP_BASE				"QdpBase"
#define LOG_COMPONENT_BTPOWERCONTROL		"PowerControl"
#define LOG_COMPONENT_DUT_MODE				"DutMode"
#define LOG_COMPONENT_HCISERVERCLIENT		"HciServerClient"
#define LOG_COMPONENT_HCIUTIL				"HciUtil"

// Symbian Reference HCIv2 Implementations
#define LOG_COMPONENT_COREHCI_SYMBIAN		"CoreHciSymbian"
#define LOG_COMPONENT_HCISERVER				"HciServer"
#define LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN "CommandsEvents"
#define LOG_COMPONENT_QDP_SYMBIAN			"QdpSymbian"
#define LOG_COMPONENT_INITIALISOR_SYMBIAN	"InitSymbian"
#define LOG_COMPONENT_INITIALISOR_TI		"InitTi"
#define LOG_COMPONENT_HCTL_UART_ORIGINAL	"HctlUartOriginal"
#define LOG_COMPONENT_HCTL_TI				"HctlTi"
#define LOG_COMPONENT_HCTL_BCSP				"HctlBcsp"
#define LOG_COMPONENT_HCI_FACADE			"HciFacade"
#define LOG_COMPONENT_HCI_CSCOLINK			"CSCOLink"
#define LOG_COMPONENT_HCI_WATCHDOG			"HciWatchdog"

// Bluetooth Stack Components
#define LOG_COMPONENT_SECMAN				"SecMan"
#define LOG_COMPONENT_HOSTRESOLVER			"HostResolver"
#define LOG_COMPONENT_LINKMGR				"LinkMgr"
#define LOG_COMPONENT_PAIRING_SERVER		"PairingServer"
#define LOG_COMPONENT_EIRMANAGER			"EirManager"
#define LOG_COMPONENT_L2CAP					"L2Cap"
#define LOG_COMPONENT_L2CAP_PDU				"L2CapPdu"
#define LOG_COMPONENT_L2CAP_SDU				"L2CapSdu"
#define LOG_COMPONENT_L2CAP_SDU_Q			"L2CapSduQ"
#define LOG_COMPONENT_L2CAP_DATA_CONTROLLER	"L2CapDataController"
#define LOG_COMPONENT_RFCOMM				"Rfcomm"
#define LOG_COMPONENT_SDP					"Sdp"
#define LOG_COMPONENT_AVDTP					"Avdtp"
#define LOG_COMPONENT_AVCTP					"Avctp"

// Bluetooth User Components
#define LOG_COMPONENT_BTLIB					"BtLib"
#define LOG_COMPONENT_AVLIB					"AvLib"
#define LOG_COMPONENT_AVCTP_SERVICES		"AvctpServices"

// Bluetooth SDP
#define LOG_COMPONENT_SDPDATABASE			"SdpDatabase"
#define LOG_COMPONENT_SDP_SERVER			"SdpServer"
#define LOG_COMPONENT_SDP_AGENT				"SdpAgent"

// Bluetooth Notifier Parameters
#define LOG_COMPONENT_BTNOTIFIERS			"Notifiers"

// btcomm component
#define LOG_COMPONENT_BT_COMM				"BtComm"

// Bluetooth Registry
#define LOG_COMPONENT_BT_MANAGER_CLIENT		"BtManClient"
#define LOG_COMPONENT_BT_MANAGER_SERVER		"BtManServer"
#define LOG_COMPONENT_BT_REGISTRY			"BtRegistry"

// A2DP Optimiser
#define LOG_COMPONENT_A2DP_OPTIMISER        "A2dpOptimiser"

// Bluetooth PAN profile
#define LOG_COMPONENT_PAN_BNEP				"Bnep"
#define LOG_COMPONENT_PAN_AGENT				"PanAgt"
#define LOG_COMPONENT_PAN_HELPERS			"PanHelpers"
#define LOG_COMPONENT_PAN_NAPIPHOOK			"PanNapIpHook"
#define LOG_COMPONENT_PAN_PROVIDERS			"PanProviders"

// Bluetooth Profiles
#define LOG_COMPONENT_REMCON_REFERENCE_TSP	"RemconRefTsp"
#define LOG_COMPONENT_REMCON_REF_SER_BEARER	"RemconRefSerBearer"
#define LOG_COMPONENT_REMCON_SERVER			"RemconServer"
#define LOG_COMPONENT_REMCON_UTILS			"RemconUtils"
#define LOG_COMPONENT_REMCON_TYPES			"RemconTypes"
#define LOG_COMPONENT_REMCON_CORE			"RemconCore"
#define LOG_COMPONENT_REMCON_EXTAPI1		"RemconExtApi1"
#define LOG_COMPONENT_REMCON_INNER			"RemconInner"
#define LOG_COMPONENT_REMCON_IF_BASE		"RemconIfBase"
#define LOG_COMPONENT_REMCON_IF_SEL			"RemconIfSel"
#define LOG_COMPONENT_REMCON_IL_RECV		"RemconIlRecv"
#define LOG_COMPONENT_AVRCP_BEARER			"AvrcpBearer"
#define LOG_COMPONENT_AVRCP_STATUS			"AvrcpStatus"
#define LOG_COMPONENT_AVRCP_BATTERY_STATUS	"AvrcpBattStat"
#define LOG_COMPONENT_AVRCP_GROUP_NAVIGATION "AvrcpGroupNav"
#define LOG_COMPONENT_AVRCP_MEDIA_INFO		"AvrcpMediaInfo"
#define LOG_COMPONENT_AVRCP_PLAYER_INFO		"AvrcpPlayerInfo"
#define LOG_COMPONENT_AVRCP_FRAME			"AvrcpFrame"
#define LOG_COMPONENT_REMCONABSOLUTEVOLUME  "RemConAbsoluteVolumeApi"
#define LOG_COMPONENT_REMCONMEDIABROWSEAPI  "RemConMediaBrowseApi"
#define LOG_COMPONENT_REMCONNOWPLAYINGAPI   "RemConNowPlayingApi"

// Test Components
#define LOG_COMPONENT_BT_COMM_TCSY			"BtCommTcsy"
#define LOG_COMPONENT_GAVDP_TAVSRC			"GavdpTavsrc"
#define LOG_COMPONENT_TSDPSAS				"Tsdpsas"
#define LOG_COMPONENT_BT_TESTCONSOLE		"BtTestConsole"

#endif // BLUETOOTH_LOGGER_COMPONENTS_H
