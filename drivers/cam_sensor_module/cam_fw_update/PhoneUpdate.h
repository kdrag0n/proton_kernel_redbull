/* SPDX-License-Identifier: GPL-2.0-only */
/**
 * @brief		LC898129 Global declaration & prototype declaration
 *
 * @author		(C) 2019 ON Semiconductor.
 * @file		PhoneUpdate.h
 * @date		svn:$Date:: 2019-10-18 16:42:00 +0900#$
 * @version		svn:$Revision: 18 $
 * @attention
 *
 **/
#ifndef PHONEUPDATE_H_
#define PHONEUPDATE_H_

//==============================================================================
//
//==============================================================================
#define	MODULE_VENDOR	0
#define	MDL_VER			4

#ifdef DEBUG
 extern void dbg_printf(const char *, ...);
 extern void dbg_Dump(const char *, int);
 #define TRACE_INIT(x)			dbgu_init(x)
 #define TRACE_USB(fmt, ...)	dbg_UsbData(fmt, ## __VA_ARGS__)
 #define TRACE(fmt, ...)		dbg_printf(fmt, ## __VA_ARGS__)
 #define TRACE_DUMP(x,y)		dbg_Dump(x,y)
#else
 #define TRACE_INIT(x)
 #define TRACE(...)
 #define TRACE_DUMP(x,y)
 #define TRACE_USB(...)
#endif

#if 0
typedef	signed char         INT_8;
typedef	short               INT_16;
typedef	long                INT_32;
typedef	long long           INT_64;
typedef	unsigned char       UINT_8;
typedef	unsigned short      UINT_16;
typedef	unsigned long       UINT_32;
typedef	unsigned long long  UINT_64;
#else
typedef	signed char         INT_8;
typedef	signed short        INT_16;
typedef	long                INT_32;
typedef	long long           INT_64;
typedef	unsigned char       UINT_8;
typedef	unsigned short      UINT_16;
typedef	unsigned int        UINT_32;
typedef	unsigned long long  UINT_64;
#endif

//****************************************************
//	STRUCTURE DEFINE
//****************************************************
typedef struct {
	UINT_16				Index;
	const UINT_8*		UpdataCode;
	UINT_32				SizeUpdataCode;
	UINT_64				SizeUpdataCodeCksm;
	const UINT_8*		FromCode;
	UINT_32				SizeFromCode;
	UINT_64				SizeFromCodeCksm;
	UINT_32				SizeFromCodeValid;
}	CODE_TBL_EXT;

typedef struct STRECALIB {
	INT_16	SsFctryOffX ;
	INT_16	SsFctryOffY ;
	INT_16	SsRecalOffX ;
	INT_16	SsRecalOffY ;
	INT_16	SsDiffX ;
	INT_16	SsDiffY ;
} stReCalib ;


typedef struct STMESRAM {
	INT_32	SlMeasureMaxValue ;
	INT_32	SlMeasureMinValue ;
	INT_32	SlMeasureAmpValue ;
	INT_32	SlMeasureAveValue ;
} stMesRam ;

typedef struct STGYROOFFSETTBL {
	struct {
		INT_32	SlOffsetX ;
		INT_32	SlOffsetY ;
		INT_32	SlOffsetZ ;
	} StAngle ;

	struct {
		INT_32	SlOffsetX ;
		INT_32	SlOffsetY ;
		INT_32	SlOffsetZ ;
	} StAccel ;
} stGyroOffsetTbl ;

typedef struct {
	UINT_32 BiasInit;
	UINT_32 XOffsetInit;
	UINT_32 XOffsetInitIn;
	UINT_32 YOffsetInit;
	UINT_32 YOffsetInitIn;
	UINT_32 OffsetMargin;
	UINT_32 XTargetRange;
	UINT_32 XTargetMax;
	UINT_32 XTargetMin;
	UINT_32 YTargetRange;
	UINT_32 YTargetMax;
	UINT_32 YTargetMin;
	UINT_32 OisSinNum;
	UINT_32 OisSinFreq;
	UINT_32 OisSinGain;
	UINT_32 AfSinNum;
	UINT_32 AfSinFreq;
	UINT_32 AfSinGainP;
	UINT_32 AfSinGainM;
	UINT_32 DecrementStep;
	UINT_32 ZBiasInit;
	UINT_32 ZOffsetInit;
	UINT_32 ZOffsetInitIn;
	UINT_32 ZTargetRange;
	UINT_32 ZTargetMax;
	UINT_32 ZTargetMin;
	UINT_32 ZHighMargin;
	UINT_32 ZLowMargin;
} ADJ_HALL;

typedef struct {
	UINT_32 Hxgain;
	UINT_32 Hygain;
	UINT_32 XNoiseNum;
	UINT_32 XNoiseFreq;
	UINT_32 XNoiseGain;
	UINT_32 XGap;
	UINT_32 YNoiseNum;
	UINT_32 YNoiseFreq;
	UINT_32 YNoiseGain;
	UINT_32 YGap;
	UINT_32 XJudgeHigh;
	UINT_32 XJudgeLow;
	UINT_32 YJudgeHigh;
	UINT_32 YJudgeLow;
	UINT_32 Hzgain;
	UINT_32 ZNoiseNum;
	UINT_32 ZNoiseFreq;
	UINT_32 ZNoiseGain;
	UINT_32 ZGap;
	UINT_32 ZJudgeHigh;
	UINT_32 ZJudgeLow;
} ADJ_LOPGAN;

typedef struct {
	INT_16	SltOffsetX;
	INT_16	SltOffsetY;
	INT_16	SltDirX;
	INT_16	SltDirY;
} ADJ_LINEARITY_MIXING;

typedef struct ADJ_TEMP_COMPENSATION {
	UINT_32		rcodeX;
	UINT_32		rcodeY;
	UINT_32		rcodeZ;
	UINT_32		shag;
	UINT_32		shbg;
	UINT_32		shcg;
	UINT_32		shoutag;
	UINT_32		shoutbg;
	UINT_32		shab;
	UINT_32		shac;
	UINT_32		shaa;
	UINT_32		shbb;
	UINT_32		shbc;
	UINT_32		shba;
	UINT_32		shcb;
	UINT_32		shcc;
	UINT_32		shca;
	UINT_32		tab;
	UINT_32		tac;
	UINT_32		taa;
	UINT_32		tbb;
	UINT_32		tbc;
	UINT_32		tba;
	UINT_32		TEMPOFF;
	UINT_32		tag;
	UINT_32		tbg;
	UINT_32		shiftg;
	UINT_32		shoutag1;
	UINT_32		shoutbg1;
	UINT_8		tcx;
	UINT_8		tbx;
	UINT_8		tax;
} stAdj_Temp_Compensation;

/*** caution [little-endian] ***/
#ifdef _BIG_ENDIAN_
typedef union	DWDVAL {
	UINT_32	UlDwdVal ;
	UINT_16	UsDwdVal[ 2 ] ;
	struct {
		UINT_16	UsHigVal ;
		UINT_16	UsLowVal ;
	} StDwdVal ;
	struct {
		UINT_8	UcRamVa3 ;
		UINT_8	UcRamVa2 ;
		UINT_8	UcRamVa1 ;
		UINT_8	UcRamVa0 ;
	} StCdwVal ;
}	UnDwdVal ;
typedef union	ULLNVAL {
	UINT_64	UllnValue ;
	UINT_32	UlnValue[ 2 ] ;
	struct {
		UINT_32	UlHigVal ;	// [63:32]
		UINT_32	UlLowVal ;	// [31:0]
	} StUllnVal ;
}	UnllnVal ;

#else	// BIG_ENDDIAN
typedef union	DWDVAL {
	UINT_32	UlDwdVal ;
	UINT_16	UsDwdVal[ 2 ] ;
	struct {
		UINT_16	UsLowVal ;
		UINT_16	UsHigVal ;
	} StDwdVal ;
	struct {
		UINT_8	UcRamVa0 ;
		UINT_8	UcRamVa1 ;
		UINT_8	UcRamVa2 ;
		UINT_8	UcRamVa3 ;
	} StCdwVal ;
}	UnDwdVal ;
typedef union	ULLNVAL {
	UINT_64	UllnValue ;
	UINT_32	UlnValue[ 2 ] ;
	struct {
		UINT_32	UlLowVal ;	// [31:0]
		UINT_32	UlHigVal ;	// [63:32]
	} StUllnVal ;
}	UnllnVal ;

#endif	// _BIG_ENDIAN_

#define	SUCCESS			0x00
#define	FAILURE			0x01

//==============================================================================
//
//==============================================================================
#define		CMD_IO_ADR_ACCESS			0xC000
#define		CMD_IO_DAT_ACCESS			0xD000
#define 	SYSDSP_DSPDIV				0xD00014
#define 	SYSDSP_SOFTRES				0xD0006C
#define 	SYSDSP_REMAP				0xD000AC
#define 	SYSDSP_CVER					0xD00100
#define		ROMINFO						0xE050D4
#define 	FLASHROM_129				0xE07000
#define 	FLASHROM_FLA_RDAT			(FLASHROM_129 + 0x00)
#define 	FLASHROM_FLA_WDAT			(FLASHROM_129 + 0x04)
#define 	FLASHROM_ACSCNT				(FLASHROM_129 + 0x08)
#define 	FLASHROM_FLA_ADR			(FLASHROM_129 + 0x0C)
#define		USER_MAT					0
#define		INF_MAT0					1
#define		INF_MAT1					2
#define		INF_MAT2					4
#define		TRIM_MAT					16

#define 	FLASHROM_CMD				(FLASHROM_129 + 0x10)
#define 	FLASHROM_FLAWP				(FLASHROM_129 + 0x14)
#define 	FLASHROM_FLAINT				(FLASHROM_129 + 0x18)
#define 	FLASHROM_FLAMODE			(FLASHROM_129 + 0x1C)
#define 	FLASHROM_TPECPW				(FLASHROM_129 + 0x20)
#define 	FLASHROM_TACC				(FLASHROM_129 + 0x24)

#define 	FLASHROM_ERR_FLA			(FLASHROM_129 + 0x98)
#define 	FLASHROM_RSTB_FLA			(FLASHROM_129 + 0x4CC)
#define 	FLASHROM_UNLK_CODE1			(FLASHROM_129 + 0x554)
#define 	FLASHROM_CLK_FLAON			(FLASHROM_129 + 0x664)
#define 	FLASHROM_UNLK_CODE2			(FLASHROM_129 + 0xAA8)
#define 	FLASHROM_UNLK_CODE3			(FLASHROM_129 + 0xCCC)

#define		READ_STATUS_INI				0x01000000

#define		INF0_DATA					0x1A00
#define		INF1_DATA					0x1B00
#define		INF2_DATA					0x1C00
#define		SNGL_DATA					0x1D00


//==============================================================================
// Prototype
//==============================================================================
extern void BootMode( void ) ;
extern UINT_8 PmemDownload129( UINT_8, UINT_8 ) ;
extern UINT_8 FlashProgram129( UINT_8, UINT_8, UINT_8 ) ;
extern UINT_8 RdFlashMulti( UINT_8, UINT_32, UINT_32 *, UINT_8 ) ;
extern UINT_8 RdFlashSingle( UINT_8, UINT_32, UINT_32 * ) ;
extern UINT_8 GyroReCalib( stReCalib * ) ;
extern void RdGyroOffsetTbl( stGyroOffsetTbl * );
extern UINT_8 WrGyroOffsetTbl( stGyroOffsetTbl * ) ;
extern UINT_8 LoadUareaToPM( CODE_TBL_EXT *, UINT_8 ) ;
extern UINT_8 RdBurstUareaFromPM( UINT_32, UINT_8 *, UINT_16, UINT_8 ) ;
extern UINT_8 RdSingleUareaFromPM( UINT_32, UINT_8 *, UINT_8, UINT_8 ) ;
extern UINT_8 WrTempCompData( stAdj_Temp_Compensation * ) ;
extern UINT_8 WrUareaToPm( UINT_32, UINT_8 *, UINT_8, UINT_8 ) ;
extern UINT_8 WrUareaToPmInt( UINT_32, UINT_32 *, UINT_8, UINT_8 ) ;
extern UINT_8 WrUareaToFlash( void ) ;
extern UINT_8 RdStatus( UINT_8 ) ;
// extern UINT_8 um2pos( float, float, float, UINT_32 *, UINT_32 * ) ;

#endif /* #ifndef OIS_H_ */
