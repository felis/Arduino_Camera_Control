#if !defined(__EXPCOMP_MACRO_H__)
#define __EXPCOMP_MACRO_H__

uint8_t  expComp[] = { 0xF0, 0xF3, 0xF5, 0xF8, 0xFB, 0xFD, 0, 0x03, 0x05, 0x08, 0x0B, 0x0D, 0x10 };


#define EXP_COMP_VALUE(i)((uint32_t)expComp[(i)])

#endif // __EXPCOMP_MACRO_H__
