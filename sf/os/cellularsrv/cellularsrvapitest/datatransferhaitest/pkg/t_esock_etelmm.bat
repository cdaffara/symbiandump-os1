@rem
@rem Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem

testexecute z:\mixed_technology\telephony_comms\telephony_comms-validation-automated_download.script -tcx z:\mixed_technology\telephony_comms\t_esock_etelmm.tcs
testexecute z:\mixed_technology\telephony_comms\telephony_comms-validation-automated_upload.script -tcx z:\mixed_technology\telephony_comms\t_esock_etelmm.tcs
testexecute z:\mixed_technology\telephony_comms\telephony_comms-validation-automated_suspending.script -tcx z:\mixed_technology\telephony_comms\t_esock_etelmm.tcs
testexecute z:\mixed_technology\telephony_comms\telephony_comms-validation-manual_suspending.script -tcx z:\mixed_technology\telephony_comms\t_esock_etelmm.tcs