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
#ifndef __MTPCONST_H__
#define __MTPCONST_H__

#include <stdarg.h>
#include <inttypes.h>

// Operation Codes
#define MTP_OC_GetObjectPropsSupported				0x9801
#define MTP_OC_GetObjectPropDesc					0x9802
#define MTP_OC_GetObjectPropValue					0x9803
#define MTP_OC_SetObjectPropValue					0x9804
#define MTP_OC_GetObjectPropList					0x9805
#define MTP_OC_SetObjectPropList					0x9806
#define MTP_OC_GetInterdependentPropDesc			0x9807
#define MTP_OC_SendObjectPropList					0x9808
#define MTP_OC_GetObjectReferences					0x9810
#define MTP_OC_SetObjectReferences					0x9811
#define MTP_OC_Skip									0x9820

// Responce Codes
#define MTP_RC_Invalid_ObjectPropCode				0xA801
#define MTP_RC_Invalid_ObjectProp_Format			0xA802
#define MTP_RC_Invalid_ObjectProp_Value				0xA803
#define MTP_RC_Invalid_ObjectReference				0xA804
#define MTP_RC_Group_Not_Supported					0xA805
#define MTP_RC_Invalid_Dataset						0xA806
#define MTP_RC_Specification_By_Group_Unsupported	0xA807
#define MTP_RC_Specification_By_Depth_Unsupported	0xA808
#define MTP_RC_Object_Too_Large						0xA809
#define MTP_RC_ObjectProp_Not_Supported				0xA80A

// Event Codes
#define MTP_EC_ObjectPropChanged					0xC801
#define MTP_EC_ObjectPropDescChanged				0xC802
#define MTP_EC_ObjectReferencesChanged				0xC803

// Device Property Codes
#define MTP_DPC_Synchronization_Partner				0xD401
#define MTP_DPC_Device_Friendly_Name				0xD402
#define MTP_DPC_Volume								0xD403
#define MTP_DPC_SupportedFormatsOrdered				0xD404
#define MTP_DPC_DeviceIcon							0xD405
#define MTP_DPC_Session_Initiator_Version_Info		0xD406
#define MTP_DPC_Perceived_Device_Type				0xD407
#define MTP_DPC_Playback_Rate						0xD410
#define MTP_DPC_Playback_Object						0xD411
#define MTP_DPC_Playback_Container					0xD412

// Object Formats
#define MTP_OFC_Undefined_Firmware					0xB802
#define MTP_OFC_Windows_Image_Format				0xB881
#define MTP_OFC_Undefined_Audio						0xB900
#define MTP_OFC_WMA									0xB901
#define MTP_OFC_OGG									0xB902
#define MTP_OFC_AAC									0xB903
#define MTP_OFC_Audible								0xB904
#define MTP_OFC_FLAC								0xB906
#define MTP_OFC_Undefined_Video						0xB980
#define MTP_OFC_WMV									0xB981
#define MTP_OFC_MP4_Container						0xB982
#define MTP_OFC_MP2									0xB983
#define MTP_OFC_3GP_Container						0xB984

// Object Property Codes
#define MTP_OPC_StorageID							0xDC01
#define MTP_OPC_Object_Format						0xDC02
#define MTP_OPC_Protection_Status					0xDC03
#define MTP_OPC_Object_Size							0xDC04
#define MTP_OPC_Association_Type					0xDC05
#define MTP_OPC_Association_Desc					0xDC06
#define MTP_OPC_Object_File_Name					0xDC07
#define MTP_OPC_Date_Created						0xDC08
#define MTP_OPC_Date_Modified						0xDC09
#define MTP_OPC_Keywords							0xDC0A
#define MTP_OPC_Parent_Object						0xDC0B
#define MTP_OPC_Allowed_Folder_Contents				0xDC0C
#define MTP_OPC_Hidden								0xDC0D
#define MTP_OPC_System_Object						0xDC0E
#define MTP_OPC_Persistent_Unique_Object_Identifier	0xDC41
#define MTP_OPC_SyncID								0xDC42
#define MTP_OPC_Property_Bag						0xDC43
#define MTP_OPC_Name								0xDC44
#define MTP_OPC_Created_By							0xDC45
#define MTP_OPC_Artist								0xDC46
#define MTP_OPC_Date_Authored						0xDC47
#define MTP_OPC_Description							0xDC48
#define MTP_OPC_URL_Reference						0xDC49
#define MTP_OPC_Language-Locale						0xDC4A
#define MTP_OPC_Copyright_Information				0xDC4B
#define MTP_OPC_Source								0xDC4C
#define MTP_OPC_Origin_Location						0xDC4D
#define MTP_OPC_Date_Added							0xDC4E
#define MTP_OPC_Non-Consumable						0xDC4F
#define MTP_OPC_Corrupt/Unplayable					0xDC50
#define MTP_OPC_ProducerSerialNumber				0xDC51
#define MTP_OPC_Representative_Sample_Format		0xDC81
#define MTP_OPC_Representative_Sample_Size			0xDC82
#define MTP_OPC_Representative_Sample_Height		0xDC83
#define MTP_OPC_Representative_Sample_Width			0xDC84
#define MTP_OPC_Representative_Sample_Duration		0xDC85
#define MTP_OPC_Representative_Sample_Data			0xDC86
#define MTP_OPC_Width								0xDC87
#define MTP_OPC_Height								0xDC88
#define MTP_OPC_Duration							0xDC89
#define MTP_OPC_Rating								0xDC8A
#define MTP_OPC_Track								0xDC8B
#define MTP_OPC_Genre								0xDC8C
#define MTP_OPC_Credits								0xDC8D
#define MTP_OPC_Lyrics								0xDC8E
#define MTP_OPC_Subscription_Content_ID				0xDC8F
#define MTP_OPC_Produced_By							0xDC90
#define MTP_OPC_Use_Count							0xDC91
#define MTP_OPC_Skip_Count							0xDC92
#define MTP_OPC_Last_Accessed						0xDC93
#define MTP_OPC_Parental_Rating						0xDC94
#define MTP_OPC_Meta_Genre							0xDC95
#define MTP_OPC_Composer							0xDC96
#define MTP_OPC_Effective_Rating					0xDC97
#define MTP_OPC_Subtitle							0xDC98
#define MTP_OPC_Original_Release_Date				0xDC99
#define MTP_OPC_Album_Name							0xDC9A
#define MTP_OPC_Album_Artist						0xDC9B
#define MTP_OPC_Mood								0xDC9C
#define MTP_OPC_DRM_Status							0xDC9D
#define MTP_OPC_Sub_Description						0xDC9E
#define MTP_OPC_Is_Cropped							0xDCD1
#define MTP_OPC_Is_Colour_Corrected					0xDCD2
#define MTP_OPC_Image_Bit_Depth						0xDCD3
#define MTP_OPC_Fnumber								0xDCD4
#define MTP_OPC_Exposure_Time						0xDCD5
#define MTP_OPC_Exposure_Index						0xDCD6
#define MTP_OPC_Total_BitRate						0xDE91
#define MTP_OPC_Bitrate_Type						0xDE92
#define MTP_OPC_Sample_Rate							0xDE93
#define MTP_OPC_Number_Of_Channels					0xDE94
#define MTP_OPC_Audio_BitDepth						0xDE95
#define MTP_OPC_Scan_Type							0xDE97
#define MTP_OPC_Audio_WAVE_Codec					0xDE99
#define MTP_OPC_Audio_BitRate						0xDE9A
#define MTP_OPC_Video_FourCC_Codec					0xDE9B
#define MTP_OPC_Video_BitRate						0xDE9C
#define MTP_OPC_Frames_Per_Thousand_Seconds			0xDE9D
#define MTP_OPC_KeyFrame_Distance					0xDE9E
#define MTP_OPC_Buffer_Size							0xDE9F
#define MTP_OPC_Encoding_Quality					0xDEA0
#define MTP_OPC_Encoding_Profile					0xDEA1
#define MTP_OPC_Display_Name						0xDCE0
#define MTP_OPC_Body_Text							0xDCE1
#define MTP_OPC_Subject								0xDCE2
#define MTP_OPC_Priority							0xDCE3
#define MTP_OPC_Given_Name							0xDD00
#define MTP_OPC_Middle_Names						0xDD01
#define MTP_OPC_Family_Name							0xDD02
#define MTP_OPC_Prefix								0xDD03
#define MTP_OPC_Suffix								0xDD04
#define MTP_OPC_Phonetic_Given_Name					0xDD05
#define MTP_OPC_Phonetic_Family_Name				0xDD06
#define MTP_OPC_Email_Primary						0xDD07
#define MTP_OPC_Email_Personal_1					0xDD08
#define MTP_OPC_Email_Personal_2					0xDD09
#define MTP_OPC_Email_Business_1					0xDD0A
#define MTP_OPC_Email_Business_2					0xDD0B
#define MTP_OPC_Email_Others						0xDD0C
#define MTP_OPC_Phone_Number_Primary				0xDD0D
#define MTP_OPC_Phone_Number_Personal				0xDD0E
#define MTP_OPC_Phone_Number_Personal_2				0xDD0F
#define MTP_OPC_Phone_Number_Business				0xDD10
#define MTP_OPC_Phone_Number_Business_2				0xDD11
#define MTP_OPC_Phone_Number_Mobile					0xDD12
#define MTP_OPC_Phone_Number_Mobile_2				0xDD13
#define MTP_OPC_Fax_Number_Primary					0xDD14
#define MTP_OPC_Fax_Number_Personal					0xDD15
#define MTP_OPC_Fax_Number_Business					0xDD16
#define MTP_OPC_Pager_Number						0xDD17
#define MTP_OPC_Phone_Number_Others					0xDD18
#define MTP_OPC_Primary_Web_Address					0xDD19
#define MTP_OPC_Personal_Web_Address				0xDD1A
#define MTP_OPC_Business_Web_Address				0xDD1B
#define MTP_OPC_Instant_Messenger_Address			0xDD1C
#define MTP_OPC_Instant_Messenger_Address_2			0xDD1D
#define MTP_OPC_Instant_Messenger_Address_3			0xDD1E
#define MTP_OPC_Postal_Address_Personal_Full		0xDD1F
#define MTP_OPC_Postal_Address_Personal_Line_1		0xDD20
#define MTP_OPC_Postal_Address_Personal_Line_2		0xDD21
#define MTP_OPC_Postal_Address_Personal_City		0xDD22
#define MTP_OPC_Postal_Address_Personal_Region		0xDD23
#define MTP_OPC_Postal_Address_Personal_Postal_Code	0xDD24
#define MTP_OPC_Postal_Address_Personal_Country		0xDD25
#define MTP_OPC_Postal_Address_Business_Full		0xDD26
#define MTP_OPC_Postal_Address_Business_Line_1		0xDD27
#define MTP_OPC_Postal_Address_Business_Line_2		0xDD28
#define MTP_OPC_Postal_Address_Business_City		0xDD29
#define MTP_OPC_Postal_Address_Business_Region		0xDD2A
#define MTP_OPC_Postal_Address_Business_Postal_Code	0xDD2B
#define MTP_OPC_Postal_Address_Business_Country		0xDD2C
#define MTP_OPC_Postal_Address_Other_Full			0xDD2D
#define MTP_OPC_Postal_Address_Other_Line_1			0xDD2E
#define MTP_OPC_Postal_Address_Other_Line_2			0xDD2F
#define MTP_OPC_Postal_Address_Other_City			0xDD30
#define MTP_OPC_Postal_Address_Other_Region			0xDD31
#define MTP_OPC_Postal_Address_Other_Postal_Code	0xDD32
#define MTP_OPC_Postal_Address_Other_Country		0xDD33
#define MTP_OPC_Organization_Name					0xDD34
#define MTP_OPC_Phonetic_Organization_Name			0xDD35
#define MTP_OPC_Role								0xDD36
#define MTP_OPC_Birthdate							0xDD37
#define MTP_OPC_Message_To							0xDD40
#define MTP_OPC_Message_CC							0xDD41
#define MTP_OPC_Message_BCC							0xDD42
#define MTP_OPC_Message_Read						0xDD43
#define MTP_OPC_Message_Received_Time				0xDD44
#define MTP_OPC_Message_Sender						0xDD45
#define MTP_OPC_Activity_Begin_Time					0xDD50
#define MTP_OPC_Activity_End_Time					0xDD51
#define MTP_OPC_Activity_Location					0xDD52
#define MTP_OPC_Activity_Required_Attendees			0xDD54
#define MTP_OPC_Activity_Optional_Attendees			0xDD55
#define MTP_OPC_Activity_Resources					0xDD56
#define MTP_OPC_Activity_Accepted					0xDD57
#define MTP_OPC_Owner								0xDD5D
#define MTP_OPC_Editor								0xDD5E
#define MTP_OPC_Webmaster							0xDD5F
#define MTP_OPC_URL_Source							0xDD60
#define MTP_OPC_URL_Destination						0xDD61
#define MTP_OPC_Time_Bookmark						0xDD62
#define MTP_OPC_Object_Bookmark						0xDD63
#define MTP_OPC_Byte_Bookmark						0xDD64
#define MTP_OPC_Last_Build_Date						0xDD70
#define MTP_OPC_Time_to_Live						0xDD71
#define MTP_OPC_Media_GUID							0xDD72


#endif // __MTPCONST_H__
