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
#if !defined(__PSVALUETITLES_H__)
#define __PSVALUETITLES_H__

#include <inttypes.h>
#include <avr/pgmspace.h>
#include <valuelist.h>

typedef uint8_t		VT_COMPQUALITY;
typedef uint8_t		VT_FVFILEFORMAT;    
typedef uint8_t		VT_IMAGESIZE;    
typedef uint8_t		VT_STROBE;
typedef uint8_t		VT_MODE;
typedef uint8_t		VT_APERTURE;
typedef uint8_t		VT_EZOOM;
typedef uint8_t		VT_WB;
typedef uint8_t		VT_SHSPEED;
typedef uint8_t		VT_ISO;
typedef uint8_t		VT_EXPCOMP;
typedef uint8_t		VT_CAMOUTPUT;

#define VT_COMPQUALITY_TEXT_LEN     6
#define VT_FVFILEFORMAT_TEXT_LEN    4
#define VT_IMAGESIZE_TEXT_LEN       6
#define VT_STROBE_TEXT_LEN          6
#define VT_MODE_TEXT_LEN            5
#define VT_APT_TEXT_LEN             4
#define VT_EZOOM_TEXT_LEN           4
#define VT_WB_TEXT_LEN              4
#define VT_SHSPEED_TEXT_LEN         5
#define VT_ISO_TEXT_LEN             5
#define VT_EXPCOMP_TEXT_LEN         7
#define VT_CAMOUTPUT_TEXT_LEN       4


const ValueTitle<VT_CAMOUTPUT, VT_CAMOUTPUT_TEXT_LEN> CamOutputTitles[] PROGMEM = 
{
	{0x01,	{'L', 'C', 'D', 0 }	},        // LCD
	{0x02,	{'O', 'U', 'T', 0 }	},        // Video OUT
	{0x03,	{'O', 'f', 'f', 0 }	}         // Off
};

const ValueTitle<VT_COMPQUALITY, VT_COMPQUALITY_TEXT_LEN> CompQualityTitles[] PROGMEM = 
{
	{0x00,	{'N', '/', 'D', 'e', 'f', 0 }	},		// Not Defined
	{0x01,	{'E', 'c', 'o', 'n', 'o', 0 }	},		// Econo
	{0x02,	{'N', 'o', 'r', 'm', 'l', 0 }	},		// Normal
	{0x03,	{'F', 'i', 'n', 'e', ' ', 0 }	},		// Fine
	{0x04,	{'L', 'o', 's', 's', 'l', 0 }	},		// Lossless
	{0x05,	{'S', 'u', 'p', 'e', 'r', 0 }	}		// Super Fine
};

const ValueTitle<VT_FVFILEFORMAT, VT_FVFILEFORMAT_TEXT_LEN> FullViewFileFormatTitles[] PROGMEM = 
{
	{0x00,	{'N', '/', 'D', 0 }	},
	{0x01,	{'J', 'P', 'G', 0 }	},
	{0x02,	{'C', 'R', 'W', 0 }	}
};

const ValueTitle<VT_IMAGESIZE, VT_IMAGESIZE_TEXT_LEN> ImageSizeTitles[] PROGMEM = 
{
	{0x00,	{'L', 'a', 'r', 'g', 'e', 0 }	},		// Large
	{0x01,	{'M', 'e', 'd', '-', '1', 0 }	},		// Medium 1
	{0x02,	{'S', 'm', 'a', 'l', 'l', 0 }	},		// Small
	{0x03,	{'M', 'e', 'd', '-', '2', 0 }	},		// Medium 2
	{0x07,	{'M', 'e', 'd', '-', '3', 0 }	}		// Medium 3
};

const ValueTitle<VT_STROBE, VT_STROBE_TEXT_LEN> StrobeTitles[] PROGMEM = 
{
	{0x00,	{'O', 'f', 'f', ' ', ' ', 0 }	},		// Off
	{0x01,	{'A', 'u', 't', 'o', ' ', 0 }	},		// Auto
	{0x02,	{'O', 'n', ' ', ' ', ' ', 0 }	},		// On
	{0x03,	{'R', '/', 'E', 'y', 'e', 0 }	},      // Red Eye Suppression
	{0x04,	{'L', 'o', 'w', ' ', ' ', 0 }	},      // Low Speed Synchronization
	{0x05,	{'A', 'u', 't', 'R', 'E', 0 }	},      // Auto + Red Eye Suppression
	{0x06,	{'O', 'n', 'R', 'E', ' ', 0 }	}       // On + Red Eye Suppression
};

const ValueTitle<VT_MODE, VT_MODE_TEXT_LEN> ModeTitles[] PROGMEM =
{
	{0x00,	{'A','u','t','o',0} },		// Auto
	{0x01,	{'P',' ',' ',' ',0} },		// Program
	{0x02,	{'T','v',' ',' ',0} },		// Tv
	{0x03,	{'A','v',' ',' ',0} },		// Av
	{0x04,	{'M',' ',' ',' ',0} },		// Manual
	{0x05,	{'A','D','E','P',0} },		// A-DEP
	{0x05,	{'M','D','E','P',0} },		// M-DEP
	{0x06,	{'B','u','l','b',0} },		// Bulb
	{0x80,	{'C','a','m','M',0} },		// Camera M
	{0x81,	{'M','y','C','o',0} },		// My Color
	{0x82,	{'P','o','r','t',0} },		// Portrait
	{0x83,	{'L','a','n','d',0} },		// Landscape
	{0x84,  {'N','g','h','t',0} },		// Night Scene
	{0x85,	{'F','r','s','t',0} },		// Forest
	{0x86,	{'S','n','o','w',0} },		// Snow
	{0x87,	{'B','e','a','c',0} },		// Beach
	{0x88,	{'F','i','r','e',0} },		// Fireworks
	{0x89,	{'P','r','t','y',0} },		// Party
	{0x8A,	{'N','t','S','n',0} },		// Night Snap
	{0x8B,	{'S','t','c','h',0} },		// Stich
	{0x8C,	{'M','o','v',' ',0} },		// Movie
	{0x8D,	{'C','u','s','t',0} },		// Custom
	{0x8E,	{'I','n','t','r',0} },		// Interval
	{0x8F,	{'D','M','c','r',0} },		// Digital Macro
	{0x90,	{'L','g','S','h',0} },		// Long Shutter
	{0x91,	{'U','W','t','r',0} },		// Underwater
	{0x92,	{'K','i','d','s',0} },		// Kids And Pets
	{0x93,	{'F','t','S','h',0} },		// Fast Shutter
	{0x94,	{'S','l','S','h',0} },		// Slow Shutter 
	{0x95,	{'C','u','s','1',0} },		// Custom 1
	{0x96,	{'C','u','s','2',0} },		// Custom 2
	{0x97,	{'N','e','u','t',0} },		// Neutral
	{0x98,	{'G','r','a','y',0} },		// Gray
	{0x99,	{'S','e','p','i',0} },		// Sepia
	{0x9A,	{'V','i','v','d',0} },		// Vivid
	{0x9B,	{'S','p','r','t',0} },		// Sport
	{0x9C,	{'M','a','c','r',0} },		// Macro
	{0x9D,	{'S','M','a','c',0} },		// Super Macro
	{0x9E,	{'P','a','m','F',0} },		// Pan Focus
	{0x9F,	{'B','/','W',' ',0} },		// Black & White
	{0xA0,	{'I','n','h','b',0} }		// Flash Inhibit
};

const ValueTitle<VT_EZOOM, VT_EZOOM_TEXT_LEN> EZoomTitles[] PROGMEM = 
{
	{0x00,	{'O', 'f', 'f', 0 }	},
	{0x01,	{'x', '2', ' ', 0 }	},
	{0x02,	{'x', '4', ' ', 0 }	},
	{0x03,	{'S', 't', 'h', 0 }	}
};

const ValueTitle<VT_WB, VT_WB_TEXT_LEN> WbTitles[] PROGMEM = 
{ 
	{0,	{'A','W','B',0} },      // Auto
	{1,	{'D','a','y',0} },      // Daylight
	{2,	{'C','l','d',0} },      // Cloudy
	{3, {'T','n','g',0} },      // Tungsteen
	{4, {'F','l','r',0} },      // Fluorescent
	{6,	{'P','r','e',0} },      // Preset
	{7, {'F','/','H',0} },      // Fluorescent H
	{9,	{'T','m','p',0} },      // Color temperature
	{10,	{'P','C','1',0} },  // Custom White Balance PC-1
	{11,	{'P','C','2',0} },  // Custom White Balance PC-2
	{12,	{'P','C','3',0} },  // Custom White Balance PC-3
	{13,	{'M','i','s',0} },  // Missing number
	{14,	{'F','/','H',0} },  // Fluorescent H
	{0xFF,	{'I','n','v',0} }   // Invalid
};

const ValueTitle<VT_ISO, VT_ISO_TEXT_LEN> IsoTitles[] PROGMEM =
{
	{0x00,	{'A','u','t','o',0} },
	{0x28,	{'6',' ',' ',' ',0} },
	{0x30,	{'1','2',' ',' ',0} },
	{0x38,	{'2','5',' ',' ',0} },
	{0x40,	{'5','0',' ',' ',0} },
	{0x45,	{'8','0',' ',' ',0} },
	{0x48,	{'1','0','0',' ',0} },
	{0x4b,	{'1','2','5',' ',0} },
	{0x4d,	{'1','6','0',' ',0} },
	{0x50,	{'2','0','0',' ',0} },
	{0x53,	{'2','5','0',' ',0} },
	{0x55,	{'3','2','0',' ',0} },
	{0x58,	{'4','0','0',' ',0} },
	{0x5b,	{'5','0','0',' ',0} },
	{0x5d,	{'6','4','0',' ',0} },
	{0x60,	{'8','0','0',' ',0} },
	{0x63,	{'1','0','0','0',0} },
	{0x65,	{'1','2','5','0',0} },
	{0x68,	{'1','6','0','0',0} },
	{0x70,	{'3','2','0','0',0} },
	{0x78,	{'6','4','0','0',0} }
};

const ValueTitle<VT_APERTURE, VT_APT_TEXT_LEN> ApertureTitles[] PROGMEM = 
{
	{0x00,	{' ', ' ', '0', 0 }	},
	{0x08,	{' ', ' ', '1', 0 } },
	{0x0B,	{'1', '.', '1', 0 }	},
	{0x0C,	{'1', '.', '2', 0 }	},
	{0x0D,	{'1', '.', '2', 0 }	},
	{0x10,	{'1', '.', '4', 0 }	},
	{0x13,	{'1', '.', '6', 0 }	},
	{0x14,	{'1', '.', '8', 0 }	},
	{0x15,	{'1', '.', '8', 0 }	},
	{0x18,	{'2', '.', '0', 0 }	},
	{0x1B,	{'2', '.', '2', 0 }	},
	{0x1C,	{'2', '.', '5', 0 }	},
	{0x1D,	{'2', '.', '5', 0 }	},
	{0x20,	{'2', '.', '8', 0 }	},
	{0x23,	{'3', '.', '2', 0 }	},
	{0x24,	{'3', '.', '5', 0 }	},
	{0x25,	{'3', '.', '5', 0 }	},
	{0x28,	{'4', '.', '0', 0 }	},
	{0x2B,	{'4', '.', '5', 0 }	},
	{0x2C,	{'4', '.', '5', 0 }	},
	{0x2D,	{'5', '.', '0', 0 }	},
	{0x30,	{'5', '.', '6', 0 }	},
	{0x33,	{'6', '.', '3', 0 }	},
	{0x34,	{'6', '.', '7', 0 }	},
	{0x35,	{'7', '.', '1', 0 }	},
	{0x38,	{'8', '.', '0', 0 }	},
	{0x3B,	{'9', '.', '0', 0 }	},
	{0x3C,	{'9', '.', '5', 0 }	},
	{0x3D,	{' ', '1', '0', 0 }	},
	{0x40,	{' ', '1', '1', 0 }	},
	{0x43,	{' ', '1', '3', 0 }	},
	{0x44,	{' ', '1', '3', 0 }	},
	{0x45,	{' ', '1', '4', 0 }	},
	{0x48,	{' ', '1', '6', 0 }	},
	{0x4B,	{' ', '1', '8', 0 }	},
	{0x4C,	{' ', '1', '9', 0 }	},
	{0x4D,	{' ', '2', '0', 0 }	},
	{0x50,	{' ', '2', '2', 0 }	},
	{0x53,	{' ', '2', '5', 0 }	},
	{0x54,	{' ', '2', '7', 0 }	},
	{0x55,	{' ', '2', '9', 0 }	},
	{0x58,	{' ', '3', '2', 0 }	},
	{0x5B,	{' ', '3', '6', 0 }	},
	{0x5C,	{' ', '3', '8', 0 }	},
	{0x5D,	{' ', '4', '0', 0 }	},
	{0x60,	{' ', '4', '5', 0 }	},
	{0x63,	{' ', '5', '1', 0 }	},
	{0x64,	{' ', '5', '4', 0 }	},
	{0x65,	{' ', '5', '7', 0 }	},
	{0x68,	{' ', '6', '4', 0 }	},
	{0x6B,	{' ', '7', '2', 0 }	},
	{0x6C,	{' ', '7', '6', 0 }	},
	{0x6D,	{' ', '8', '0', 0 }	},
	{0x70,	{' ', '9', '1', 0 }	}
};

const ValueTitle<VT_SHSPEED, VT_SHSPEED_TEXT_LEN> ShutterSpeedTitles[] PROGMEM =
{
	{0x0c,	{'B','u','l','b',0} },
	{0x10,	{' ','3','0','"',0} },
	{0x13,	{' ','2','5','"',0} },
	{0x14,	{' ','2','0','"',0} },
	{0x15,	{' ','2','0','"',0} },
	{0x18,	{' ','1','5','"',0} },
	{0x1B,	{' ','1','3','"',0} },
	{0x1C,	{' ','1','0','"',0} },
	{0x1D,	{' ','1','0','"',0} },
	{0x20,	{' ',' ','8','"',0} },
	{0x23,	{' ',' ','6','"',0} },
	{0x24,	{' ',' ','6','"',0} },
	{0x25,	{' ',' ','5','"',0} },
	{0x28,	{' ',' ','4','"',0} },
	{0x2B,	{' ','3','"','2',0} },
	{0x2C,	{' ',' ','3','"',0} },
	{0x2D,	{' ','2','"','5',0} },
	{0x30,	{' ',' ','2','"',0} },
	{0x33,	{' ','1','"','6',0} },
	{0x34,	{' ','1','"','5',0} },
	{0x35,	{' ','1','"','3',0} },
	{0x38,	{' ',' ','1','"',0} },
	{0x3B,	{' ','0','"','8',0} },
	{0x3C,	{' ','0','"','7',0} },
	{0x3D,	{' ','0','"','6',0} },
	{0x40,	{' ','0','"','5',0} },
	{0x43,	{' ','0','"','4',0} },
	{0x44,	{' ','0','"','3',0} },
	{0x45,	{' ','0','"','3',0} },
	{0x48,	{' ',' ',' ','4',0} },
	{0x4B,	{' ',' ',' ','5',0} },
	{0x4C,	{' ',' ',' ','6',0} },
	{0x4D,	{' ',' ',' ','6',0} },
	{0x50,	{' ',' ',' ','8',0} },
	{0x53,	{' ',' ','1','0',0} },
	{0x54,	{' ',' ','1','0',0} },
	{0x55,	{' ',' ','1','3',0} },
	{0x58,	{' ',' ','1','5',0} },
	{0x5B,	{' ',' ','2','0',0} },
	{0x5C,	{' ',' ','2','0',0} },
	{0x5D,	{' ',' ','2','5',0} },
	{0x60,	{' ',' ','3','0',0} },
	{0x63,	{' ',' ','4','0',0} },
	{0x64,	{' ',' ','4','5',0} },
	{0x65,	{' ',' ','5','0',0} },
	{0x68,	{' ',' ','6','0',0} },
	{0x6B,	{' ',' ','8','0',0} },
	{0x6C,	{' ',' ','9','0',0} },
	{0x6D,	{' ','1','0','0',0} },
	{0x70,	{' ','1','2','5',0} },
	{0x73,	{' ','1','6','0',0} },
	{0x74,	{' ','1','8','0',0} },
	{0x75,	{' ','2','0','0',0} },
	{0x78,	{' ','2','5','0',0} },
	{0x7B,	{' ','3','2','0',0} },
	{0x7C,	{' ','3','5','0',0} },
	{0x7D,	{' ','4','0','0',0} },
	{0x80,	{' ','5','0','0',0} },
	{0x83,	{' ','6','4','0',0} },
	{0x84,	{' ','7','5','0',0} },
	{0x85,	{' ','8','0','0',0} },
	{0x88,	{'1','0','0','0',0} },
	{0x8B,	{'1','2','5','0',0} },
	{0x8C,	{'1','5','0','0',0} },
	{0x8D,	{'1','6','0','0',0} },
	{0x90,	{'2','0','0','0',0} },
	{0x93,	{'2','5','0','0',0} },
	{0x94,	{'3','0','0','0',0} },
	{0x95,	{'3','2','0','0',0} },
	{0x98,	{'4','0','0','0',0} },
	{0x9B,	{'5','0','0','0',0} },
	{0x9C,	{'6','0','0','0',0} },
	{0x9D,	{'6','4','0','0',0} },
	{0xA0,	{'8','0','0','0',0} }
};

const ValueTitle<VT_EXPCOMP, VT_EXPCOMP_TEXT_LEN> ExpCompTitles[] PROGMEM =
{
	{0x00,	{'+','3',' ',' ',' ',' ',0} },				
	{0x03,	{'+','2',' ','2','/','3',0} },				
	{0x04,	{'+','2',' ','1','/','2',0} },				
	{0x05,	{'+','2',' ','1','/','3',0} },				
	{0x08,	{'+','2',' ',' ',' ',' ',0} },				
	{0x0b,	{'+','1',' ','2','/','3',0} },				
	{0x0c,	{'+','1',' ','1','/','2',0} },				
	{0x0d,	{'+','1',' ','1','/','3',0} },				
	{0x10,	{'+','1',' ',' ',' ',' ',0} },				
	{0x13,	{'+','2','/','3',' ',' ',0} },				
	{0x14,	{'+','1','/','2',' ',' ',0} },				
	{0x15,	{'+','1','/','3',' ',' ',0} },				
	{0x18,	{'0',' ',' ',' ',' ',' ',0} },				
	{0x1b,	{'-','1','/','3',' ',' ',0} },				
	{0x1c,	{'-','1','/','2',' ',' ',0} },				
	{0x1d,	{'-','2','/','3',' ',' ',0} },				
	{0x20,	{'-','1',' ',' ',' ',' ',0} },				
	{0x23,	{'-','1',' ','1','/','3',0} },				
	{0x24,	{'-','1',' ','1','/','2',0} },				
	{0x25,	{'-','1',' ','2','/','3',0} },				
	{0x28,	{'-','2',' ',' ',' ',' ',0} },				
	{0x2b,	{'-','2',' ','1','/','3',0} },				
	{0x2c,	{'-','2',' ','1','/','2',0} },				
	{0x2d,	{'-','2',' ','2','/','3',0} },				
	{0x30,	{'-','3',' ',' ',' ',' ',0} }
};

#define VT_COMPQUALITY_COUNT     sizeof(CompQualityTitles)	/ (sizeof(VT_COMPQUALITY)	+ VT_COMPQUALITY_TEXT_LEN)
#define VT_FVFILEFORMAT_COUNT    sizeof(FvFileFormatTitles) / (sizeof(VT_FVFILEFORMAT)	+ VT_FVFILEFORMAT_TEXT_LEN)
#define VT_IMAGESIZE_COUNT       sizeof(ImageSizeTitles)	/ (sizeof(VT_IMAGESIZE)		+ VT_IMAGESIZE_TEXT_LEN)
#define VT_STROBE_COUNT          sizeof(StrobeTitles)		/ (sizeof(VT_STROBE)		+ VT_STROBE_TEXT_LEN)
#define VT_MODE_COUNT            sizeof(ModeTitles)			/ (sizeof(VT_MODE)			+ VT_MODE_TEXT_LEN)
#define VT_APT_COUNT             sizeof(ApertureTitles)		/ (sizeof(VT_APERTURE)		+ VT_APT_TEXT_LEN)
#define VT_EZOOM_COUNT           sizeof(EZoomTitles)		/ (sizeof(VT_EZOOM)			+ VT_EZOOM_TEXT_LEN)
#define VT_WB_COUNT              sizeof(WbTitles)			/ (sizeof(VT_WB)			+ VT_WB_TEXT_LEN)
#define VT_SHSPEED_COUNT         sizeof(ShutterSpeedTitles) / (sizeof(VT_SHSPEED)		+ VT_SHSPEED_TEXT_LEN)
#define VT_ISO_COUNT             sizeof(IsoTitles)			/ (sizeof(VT_ISO)			+ VT_ISO_TEXT_LEN)
#define VT_EXPCOMP_COUNT         sizeof(ExpCompTitles)		/ (sizeof(VT_EXPCOMP)		+ VT_EXPCOMP_TEXT_LEN)
#define VT_CAMOUTPUT_COUNT       sizeof(CamOutputTitles)	/ (sizeof(VT_CAMOUTPUT)		+ VT_CAMOUTPUT_TEXT_LEN)

#endif //__PSVALUETITLES_H__
