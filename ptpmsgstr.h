/* Copyright (C) 2010-2011 Circuits At Home, LTD. All rights reserved.

This software may be distributed and modified under the terms of the GNU
General Public License version 2 (GPL2) as published by the Free Software
Foundation and appearing in the file GPL2.TXT included in the packaging of
this file. Please note that GPL2 Section 2[b] requires that all works based
on this software must also be made publicly available under the terms of
the GPL2 ("Copyleft").

Contact information
-------------------

Circuits At Home, LTD
Web      :  http://www.circuitsathome.com
e-mail   :  support@circuitsathome.com
*/
#ifndef __PTPMSGSTR_H__
#define __PTPMSGSTR_H__

const char MsgErrDeviceConf[]     PROGMEM   = "Error configuring device. Returned:";
const char MsgErrProtoConf[]      PROGMEM   = "Error configuring boot protocol. Returned:";
const char MsgDeviceInitialized[] PROGMEM   = "Device initialized";

#ifdef PTP_HANDLE_RESPONSES
//const char msgUndefined[] PROGMEM					= "Undefined";
const char msgOK[] PROGMEM							= "OK";
const char msgGeneralError[] PROGMEM				= "GeneralError";
const char msgSessionNotOpen[] PROGMEM				= "SessionNotOpen";
const char msgInvalidTransactionID[] PROGMEM		= "InvalidTransactionID";
const char msgOperationNotSupported[] PROGMEM		= "OperationNotSupported";
const char msgParameterNotSupported[] PROGMEM		= "ParameterNotSupported";
const char msgIncompleteTransfer[] PROGMEM			= "IncompleteTransfer";
const char msgInvalidStorageId[] PROGMEM			= "InvalidStorageId";
const char msgInvalidObjectHandle[] PROGMEM			= "InvalidObjectHandle";
const char msgDevicePropNotSupported[] PROGMEM		= "DevicePropNotSupported";
const char msgInvalidObjectFormatCode[] PROGMEM		= "InvalidObjectFormatCode";
const char msgStoreFull[] PROGMEM					= "StoreFull";
const char msgObjectWriteProtected[] PROGMEM		= "ObjectWriteProtected";
const char msgStoreReadOnly[] PROGMEM				= "StoreReadOnly";
const char msgAccessDenied[] PROGMEM				= "AccessDenied";
const char msgNoThumbnailPresent[] PROGMEM			= "NoThumbnailPresent";
const char msgSelfTestFailed[] PROGMEM				= "SelfTestFailed";
const char msgPartialDeletion[] PROGMEM				= "PartialDeletion";
const char msgStoreNotAvailable[] PROGMEM			= "StoreNotAvailable";
const char msgSpecificationByFormatUnsupported [] PROGMEM	= "SpecificationByFormatUnsupported";
const char msgNoValidObjectInfo[] PROGMEM			= "NoValidObjectInfo";
const char msgInvalidCodeFormat[] PROGMEM			= "InvalidCodeFormat";
const char msgUnknownVendorCode[] PROGMEM			= "UnknownVendorCode";
const char msgCaptureAlreadyTerminated[] PROGMEM	= "CaptureAlreadyTerminated";
const char msgDeviceBusy[] PROGMEM					= "DeviceBusy";
const char msgInvalidParentObject[] PROGMEM			= "InvalidParentObject";
const char msgInvalidDevicePropFormat[] PROGMEM		= "InvalidDevicePropFormat";
const char msgInvalidDevicePropValue[] PROGMEM		= "InvalidDevicePropValue";
const char msgInvalidParameter[] PROGMEM			= "InvalidParameter";
const char msgSessionAlreadyOpened[] PROGMEM		= "SessionAlreadyOpened";
const char msgTransactionCanceled[] PROGMEM			= "TransactionCanceled";
const char msgSpecificationOfDestinationUnsupported[] PROGMEM	= "SpecificationOfDestinationUnsupported";
#endif

#if 0
const char msgUndefined[] PROGMEM					= "Undefined";
const char msgBatteryLevel[] PROGMEM				= "BatteryLevel";				
const char msgFunctionalMode[] PROGMEM				= "FunctionalMode";
const char msgImageSize[] PROGMEM					= "ImageSize";				
const char msgCompressionSetting[] PROGMEM			= "CompressionSetting";
const char msgWhiteBalance[] PROGMEM				= "WhiteBalance";			
const char msgRGBGain[] PROGMEM						= "RGBGain";
const char msgFNumber[] PROGMEM						= "FNumber";
const char msgFocalLength[] PROGMEM					= "FocalLength";
const char msgFocusDistance[] PROGMEM				= "FocusDistance";
const char msgFocusMode[] PROGMEM					= "FocusMode";				
const char msgExposureMeteringMode[] PROGMEM		= "ExposureMeteringMode";
const char msgFlashMode[] PROGMEM					= "FlashMode";			
const char msgExposureTime[] PROGMEM				= "ExposureTime";
const char msgExposureProgramMode[] PROGMEM			= "ExposureProgramMode";
const char msgExposureIndex[] PROGMEM				= "ExposureIndex";			
const char msgExposureBiasCompensation[] PROGMEM	= "ExposureBiasCompensation";
const char msgDateTime[] PROGMEM					= "DateTime";		
const char msgCaptureDelay[] PROGMEM				= "CaptureDelay";
const char msgStillCaptureMode[] PROGMEM			= "StillCaptureMode";
const char msgContrast[] PROGMEM					= "Contrast";
const char msgSharpness[] PROGMEM					= "Sharpness";
const char msgDigitalZoom[] PROGMEM					= "DigitalZoom";
const char msgEffectMode[] PROGMEM					= "EffectMode";
const char msgBurstNumber[] PROGMEM					= "BurstNumber";
const char msgBurstInterval[] PROGMEM				= "BurstInterval";
const char msgTimelapseNumber[] PROGMEM				= "TimelapseNumber";
const char msgTimelapseInterval[] PROGMEM			= "TimelapseInterval";
const char msgFocusMeteringMode[] PROGMEM			= "FocusMeteringMode";
const char msgUploadURL[] PROGMEM					= "UploadURL";
const char msgArtist[] PROGMEM						= "Artist";
const char msgCopyrightInfo[] PROGMEM				= "CopyrightInfo";
#endif

#endif //__PTPMSGSTR_H__