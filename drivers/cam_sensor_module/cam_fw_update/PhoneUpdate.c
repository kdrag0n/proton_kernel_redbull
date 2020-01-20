// SPDX-License-Identifier: GPL-2.0-only
/**
 * @brief		LC898129 Flash update
 *
 * @author		(C) 2019 ON Semiconductor.
 * @file		PhoneUpdate.c
 * @date		svn:$Date:: 2020-01-09 13:07:56 +0900#$
 * @version		svn:$Revision: 21 $
 * @attention
 *
 **/
#define		__OISFLSH__
//**************************
//	Include Header File
//**************************
#include	"PhoneUpdate.h"
#include	"UpdataCode129.h"
#if 0
#include     <stdlib.h>
#include     <math.h>
#endif

#include	"P20_FromCode_09_17_01_00.h"

/* Actuator calibration parameters */
#include 	"Calibration_Eve.h"

#define	FLASH_BLOCKS			14
#define	USER_RESERVE			2
#define	ERASE_BLOCKS			(FLASH_BLOCKS - USER_RESERVE)
#define CNT100MS				1352
#define CNT200MS				2703

#define F_CPU
#ifdef F_CPU
#define BURST_LENGTH_UC 		( 120 )
#else
#define BURST_LENGTH_UC 		( 250 )
#endif
#define BURST_LENGTH_FC 		( 64 )
#define	PMEM_INITIALIZED		(0x706D656D)

//****************************************************
//	CUSTOMER NECESSARY CREATING FUNCTION LIST
//****************************************************
/* for I2C communication */
extern	UINT_8 RamWrite32A(UINT_16, UINT_32);
extern 	UINT_8 RamRead32A(UINT_16, void *);
/* for I2C Multi Translation : Burst Mode*/
extern 	UINT_8 CntWrt(void *, UINT_16) ;
extern	UINT_8 CntRd(UINT_16, void *, UINT_16) ;

/* for Wait timer [Need to adjust for your system] */
extern void	WitTim(UINT_16);

//**************************
//	extern  Function LIST
//**************************

//**************************
//	Table of download file
//**************************
const CODE_TBL_EXT CdTbl[] = {
	{0x0901, CcUpdataCode129, UpDataCodeSize,  UpDataCodeCheckSum, CcFromCode129_09_17_01_00, sizeof(CcFromCode129_09_17_01_00), FromCheckSum_09_17_01_00, FromCheckSumSize_09_17_01_00 },
	{0xFFFF, (void*)0,        0,               0,                  (void*)0,                  0,                                 0,                        0}
};

//**************************
//	Local Function Prototype
//**************************
UINT_8	FlashBlockErase( UINT_8 SelMat , UINT_32 SetAddress );
float fix2float( UINT_32 );
UINT_32 float2fix( float );

//********************************************************************************
// Function Name 	: IOWrite32A
//********************************************************************************
UINT_8 IORead32A( UINT_32 IOadrs, UINT_32 *IOdata )
{
	UINT_8 retval ;

	retval = RamWrite32A( CMD_IO_ADR_ACCESS, IOadrs ) ;
	if( !retval )
		retval = RamRead32A ( CMD_IO_DAT_ACCESS, IOdata ) ;
	return( retval );
}
//********************************************************************************
// Function Name 	: IOWrite32A
//********************************************************************************
UINT_8 IOWrite32A( UINT_32 IOadrs, UINT_32 IOdata )
{
	UINT_8 retval ;

	retval = RamWrite32A( CMD_IO_ADR_ACCESS, IOadrs ) ;
	if( !retval )
		retval = RamWrite32A( CMD_IO_DAT_ACCESS, IOdata ) ;
	return( retval );
}
//********************************************************************************
// Function Name 	: BootMode
//********************************************************************************
void BootMode( void )
{
	UINT_32	ReadVal;

	IORead32A( SYSDSP_REMAP, &ReadVal ) ;
	ReadVal = (ReadVal & 0x1) | 0x00001400 ;
	IOWrite32A( SYSDSP_REMAP, ReadVal ) ;
	WitTim( 15 ) ;
}

//********************************************************************************
// Function Name 	: UnlockCodeSet
//********************************************************************************
UINT_8 UnlockCodeSet( void )
{
	UINT_32 UlReadVal, UlCnt=0 ;

	do {
		IOWrite32A( 0xE07554, 0xAAAAAAAA ) ;
		IOWrite32A( 0xE07AA8, 0x55555555 ) ;
		IORead32A( 0xE07014, &UlReadVal ) ;
		if( (UlReadVal & 0x00000080) != 0 )	return ( 0 ) ;
		WitTim( 1 ) ;
	} while( UlCnt++ < 10 ) ;
	return ( 1 );
}

//********************************************************************************
// Function Name 	: UnlockCodeClear
//********************************************************************************
UINT_8 UnlockCodeClear(void)
{
	UINT_32 UlDataVal, UlCnt=0 ;

	do {
		IOWrite32A( 0xE07014, 0x00000010 ) ;
		IORead32A( 0xE07014, &UlDataVal ) ;
		if( (UlDataVal & 0x00000080) == 0 )	return ( 0 ) ;
		WitTim( 1 ) ;
	} while( UlCnt++ < 10 ) ;
	return ( 3 ) ;
}

//********************************************************************************
// Function Name 	: WritePermission
//********************************************************************************
void WritePermission( void )
{
	IOWrite32A( 0xE074CC, 0x00000001 ) ;
	IOWrite32A( 0xE07664, 0x00000010 ) ;
}

//********************************************************************************
// Function Name 	: AdditionalUnlockCodeSet
//********************************************************************************
void AdditionalUnlockCodeSet( void )
{
	IOWrite32A( 0xE07CCC, 0x0000ACD5 ) ;
}
//********************************************************************************
// Function Name 	: PmemUpdate129
//********************************************************************************
UINT_8 PmemUpdate129( UINT_8 dist, CODE_TBL_EXT * ptr )
{
	UINT_8	data[BURST_LENGTH_UC +2 ];
	UINT_16	Remainder;
	UINT_8	ReadData[8];
	const UINT_8 *NcDataVal;
	UINT_16 SizeofCode;
	UINT_16 SizeofCheck;
	long long CheckSumCode;
	UINT_8 *p;
	UINT_32 i, j;
	UINT_32	UlReadVal, UlCnt , UlNum ;

	if( dist != 0 ) {
		NcDataVal = ptr->FromCode + 32;
		SizeofCode = (UINT_16)ptr->FromCode[9] << 8 | (UINT_16)ptr->FromCode[8];

		CheckSumCode = (long long)ptr->FromCode[19] << 56 | (long long)ptr->FromCode[18] << 48 | (long long)ptr->FromCode[17] << 40 | (long long)ptr->FromCode[16] << 32
					 | (long long)ptr->FromCode[15] << 24 | (long long)ptr->FromCode[14] << 16 | (long long)ptr->FromCode[13] << 8 | (long long)ptr->FromCode[12];

		SizeofCheck = SizeofCode;
	} else {
		NcDataVal = ptr->UpdataCode;
		SizeofCode = ptr->SizeUpdataCode;
		CheckSumCode = ptr->SizeUpdataCodeCksm;
		SizeofCheck = SizeofCode;
	}
	p = (UINT_8 *)&CheckSumCode;

//--------------------------------------------------------------------------------
// 1.
//--------------------------------------------------------------------------------
	RamWrite32A( 0x3000, 0x00080000 );

	data[0] = 0x40;
	data[1] = 0x00;

	Remainder = ( (SizeofCode * 5) / BURST_LENGTH_UC );
	for(i=0 ; i< Remainder ; i++)
	{
		UlNum = 2;
		for(j=0 ; j < BURST_LENGTH_UC; j++){
			data[UlNum] =  *NcDataVal++;
			UlNum++;
		}

		CntWrt( data, BURST_LENGTH_UC + 2 );
	}
	Remainder = ( (SizeofCode * 5) % BURST_LENGTH_UC);
	if (Remainder != 0 )
	{
		UlNum = 2;
		for(j=0 ; j < Remainder; j++){
			data[UlNum++] = *NcDataVal++;
		}
		CntWrt( data, Remainder + 2 );  		// Cmd 2Byte
	}

//--------------------------------------------------------------------------------
// 2.
//--------------------------------------------------------------------------------
	data[0] = 0xF0;
	data[1] = 0x0E;
	data[2] = (unsigned char)((SizeofCheck >> 8) & 0x000000FF);
	data[3] = (unsigned char)(SizeofCheck & 0x000000FF);
	data[4] = 0x00;
	data[5] = 0x00;

	CntWrt( data, 6 ) ;

	UlCnt = 0;
	do{
		WitTim( 1 );
		if( UlCnt++ > 10 ) {
			return (0x21) ;
		}
		RamRead32A( 0x0088, &UlReadVal );
	}while ( UlReadVal != 0 );

	CntRd( 0xF00E, ReadData , 8 );

	for( i=0; i<8; i++) {
		if(ReadData[7-i] != *p++ ) {
			return (0x22) ;
		}
	}
	if( dist != 0 ){
		RamWrite32A( 0xF001, 0 );
	}

	return( 0 );
}

//********************************************************************************
// Function Name 	: EraseInfoMat129
//********************************************************************************
UINT_8 EraseInfoMat129( UINT_8 SelMat )
{
	UINT_8 ans = 0;

	if( SelMat != INF_MAT0 && SelMat != INF_MAT1 && SelMat != INF_MAT2  )	return 10;

	IOWrite32A( 0xE0701C , 0x00000000);
	ans = FlashBlockErase( SelMat, 0 ) ;
	IOWrite32A( 0xE0701C , 0x00000002);
	return( ans );
}

//********************************************************************************
// Function Name 	: EraseUserMat129
//********************************************************************************
UINT_8 EraseUserMat129( UINT_8 StartBlock, UINT_8 EndBlock )
{
	UINT_32 i ;
	UINT_32	UlReadVal, UlCnt ;

	IOWrite32A( 0xE0701C , 0x00000000 ) ;
	RamWrite32A( 0xF007, 0x00000000 ) ;

	for( i = StartBlock; i < EndBlock; i++ ) {
		RamWrite32A( 0xF00A, ( i << 10 ) ) ;
		RamWrite32A( 0xF00C, 0x00000020 ) ;

		WitTim( 5 ) ;
		UlCnt = 0 ;
		do {
			WitTim( 5 ) ;
			if( UlCnt++ > 100 ) {
				IOWrite32A( 0xE0701C , 0x00000002 ) ;
				return ( 0x31 ) ;
			}
			RamRead32A( 0xF00C, &UlReadVal ) ;
		}while ( UlReadVal != 0 ) ;
	}
	IOWrite32A( 0xE0701C , 0x00000002 ) ;
	return(0);
}

//********************************************************************************
// Function Name 	: ProgramFlash129_Standard
//********************************************************************************
UINT_8 ProgramFlash129_Standard( CODE_TBL_EXT *ptr )
{
	UINT_32	UlReadVal, UlCnt , UlNum ;
	UINT_8	data[ ( BURST_LENGTH_FC + 3 ) ] ;
	UINT_32 i, j ;

	const UINT_8 *NcFromVal = ptr->FromCode + 64 ;
	const UINT_8 *NcFromVal1st = ptr->FromCode ;
	UINT_8 UcOddEvn = 0;

	IOWrite32A( 0xE0701C, 0x00000000 );
	RamWrite32A( 0xF007, 0x00000000 );
	RamWrite32A( 0xF00A, 0x00000010 );
	data[ 0 ] = 0xF0;
	data[ 1 ] = 0x08;
	data[ 2 ] = 0x00;

	for( i = 1; i < ( ptr->SizeFromCode / 64 ); i++ ) {
		if( ++UcOddEvn > 1 )	UcOddEvn = 0 ;
		if( UcOddEvn == 0 )		data[ 1 ] = 0x08 ;
		else					data[ 1 ] = 0x09 ;

#if (BURST_LENGTH_FC == 32)
		data[ 2 ] = 0x00 ;
		UlNum = 3 ;
		for( j = 0; j < BURST_LENGTH_FC; j++ ) {
			data[ UlNum++ ] = *NcFromVal++ ;
		}
		CntWrt( data, BURST_LENGTH_FC + 3 ) ;

	  	data[ 2 ] = 0x20 ;
		UlNum = 3 ;
		for( j = 0; j < BURST_LENGTH_FC; j++ ) {
			data[ UlNum++ ] = *NcFromVal++ ;
		}
		CntWrt( data, BURST_LENGTH_FC + 3 ) ;

#elif (BURST_LENGTH_FC == 64)
		UlNum = 3 ;
		for( j = 0; j < BURST_LENGTH_FC; j++ ) {
			data[ UlNum++ ] = *NcFromVal++ ;
		}
		CntWrt( data, BURST_LENGTH_FC + 3 ) ;
#endif

		RamWrite32A( 0xF00B, 0x00000010 ) ;
		UlCnt = 0 ;
		if( UcOddEvn == 0 ) {
			do {
				WitTim( 1 );
				RamRead32A( 0xF00C, &UlReadVal ) ;
				if( UlCnt++ > 250 ) {
					IOWrite32A( 0xE0701C, 0x00000002 ) ;
					return ( 0x41 ) ;
				}
			} while ( UlReadVal != 0 ) ;
		 	RamWrite32A( 0xF00C, 0x00000004 ) ;
		} else {
			do {
				WitTim( 1 );
				RamRead32A( 0xF00C, &UlReadVal ) ;
				if( UlCnt++ > 250 ) {
					IOWrite32A( 0xE0701C, 0x00000002 ) ;
					return ( 0x41 ) ;
				}
			} while ( UlReadVal != 0 ) ;
			RamWrite32A( 0xF00C, 0x00000008 ) ;
		}
	}

	UlCnt = 0 ;
	do {
		WitTim( 1 ) ;
		RamRead32A( 0xF00C, &UlReadVal ) ;
		if( UlCnt++ > 250 ) {
			IOWrite32A( 0xE0701C, 0x00000002 ) ;
			return ( 0x41 ) ;
		}
	} while ( UlReadVal != 0 ) ;

	RamWrite32A( 0xF00A, 0x00000000 ) ;
	data[ 1 ] = 0x08 ;

#if (BURST_LENGTH_FC == 32)
	data[ 2 ] = 0x00 ;
	UlNum = 3 ;
	for( j = 0; j < BURST_LENGTH_FC; j++ ) {
		data[ UlNum++ ] = *NcFromVal1st++ ;
	}
	CntWrt( data, BURST_LENGTH_FC + 3 ) ;

  	data[ 2 ] = 0x20 ;
	UlNum = 3 ;
	for( j = 0; j < BURST_LENGTH_FC; j++ ) {
		data[ UlNum++ ] = *NcFromVal1st++ ;
	}
	CntWrt( data, BURST_LENGTH_FC + 3 ) ;
#elif (BURST_LENGTH_FC == 64)
	data[ 2 ] = 0x00 ;
	UlNum = 3 ;
	for( j = 0; j < BURST_LENGTH_FC; j++ ) {
		data[ UlNum++ ] = *NcFromVal1st++ ;
	}
	CntWrt( data, BURST_LENGTH_FC + 3 ) ;
#endif

	RamWrite32A( 0xF00B, 0x00000010 ) ;
	UlCnt = 0 ;
	do {
		WitTim( 1 );
		RamRead32A( 0xF00C, &UlReadVal ) ;
		if( UlCnt++ > 250 ) {
			IOWrite32A( 0xE0701C, 0x00000002 ) ;
			return ( 0x41 ) ;
		}
	} while ( UlReadVal != 0 ) ;
 	RamWrite32A( 0xF00C, 0x00000004 ) ;

	UlCnt = 0 ;
	do {
		WitTim( 1 ) ;
		RamRead32A( 0xF00C, &UlReadVal ) ;
		if( UlCnt++ > 250 ) {
			IOWrite32A( 0xE0701C, 0x00000002 ) ;
			return ( 0x41 ) ;
		}
	} while ( (UlReadVal & 0x0000000C) != 0 ) ;

	IOWrite32A( 0xE0701C, 0x00000002 ) ;
	return( 0 );
}



//********************************************************************************
// Function Name 	: FlashUpdate129
//********************************************************************************
UINT_8 FlashUpdate129( UINT_8 chiperase, CODE_TBL_EXT* ptr )
{
	UINT_8 ans=0 ;
	UINT_32	UlReadVal, UlCnt ;
	stAdj_Temp_Compensation * TempCompPtr;

//--------------------------------------------------------------------------------
// 1.
//--------------------------------------------------------------------------------
 	ans = PmemUpdate129( 0, ptr ) ;
	if(ans != 0) return ( ans ) ;

//--------------------------------------------------------------------------------
// 2.
//--------------------------------------------------------------------------------
	if( UnlockCodeSet() != 0 ) 		return ( 0x33 ) ;
	WritePermission() ;
	AdditionalUnlockCodeSet() ;

	if( chiperase != 0 )
	 	ans = EraseUserMat129( 0, FLASH_BLOCKS ) ;
	else
		ans = EraseUserMat129( 0, ERASE_BLOCKS ) ;

	if(ans != 0){
		if( UnlockCodeClear() != 0 ) 	return ( 0x32 ) ;
		else					 		return ( ans ) ;
	}

//--------------------------------------------------------------------------------
// 3.
//--------------------------------------------------------------------------------
	ans = ProgramFlash129_Standard( ptr ) ;

	if(ans != 0){
		if( UnlockCodeClear() != 0 ) 	return ( 0x43 ) ;
		else					 		return ( ans ) ;
	}

	if( UnlockCodeClear() != 0 ) 	return ( 0x43 ) ;

//--------------------------------------------------------------------------------
// 4.
//--------------------------------------------------------------------------------
	IOWrite32A( 0xE0701C, 0x00000000 ) ;
	RamWrite32A( 0xF00A, 0x00000000 ) ;
	RamWrite32A( 0xF00D, ptr->SizeFromCodeValid ) ;

	RamWrite32A( 0xF00C, 0x00000100 ) ;
	WitTim( 6 ) ;
	UlCnt = 0 ;
	do {
		RamRead32A( 0xF00C, &UlReadVal ) ;
		if( UlCnt++ > 100 ) {
			IOWrite32A( 0xE0701C , 0x00000002 ) ;
			return ( 0x51 ) ;
		}
		WitTim( 1 ) ;
	} while ( UlReadVal != 0 ) ;

	RamRead32A( 0xF00D, &UlReadVal );

	if( UlReadVal != ptr->SizeFromCodeCksm ) {
		IOWrite32A( 0xE0701C , 0x00000002 );
		return( 0x52 );
	}

	TempCompPtr = (stAdj_Temp_Compensation *)&Eve_TempCompParameter[(ptr->Index & 0x000F) - 1];

	ans = WrTempCompData( TempCompPtr );
	if( ans != 0x00 ){
		return( 0x53 );
	}

	IOWrite32A( SYSDSP_REMAP, 0x00001000 ) ;
	WitTim( 15 ) ;
	IORead32A( ROMINFO,	(UINT_32 *)&UlReadVal ) ;
	if( UlReadVal != 0x0A)		return( 0x53 );

	return ( 0 );
}
//********************************************************************************
// Function Name 	: FlashBlockErase
//********************************************************************************
UINT_8	FlashBlockErase( UINT_8 SelMat , UINT_32 SetAddress )
{
	UINT_32	UlReadVal, UlCnt ;
	UINT_8	ans	= 0 ;

	if( SelMat != USER_MAT && SelMat != INF_MAT0 && SelMat != INF_MAT1 && SelMat != INF_MAT2 )
		return 10 ;
	if( SetAddress > 0x00003CFF )
		return 9 ;

	ans	= UnlockCodeSet() ;
	if( ans != 0 )	return( ans ) ;

	WritePermission() ;
	if( SelMat == TRIM_MAT ){
		IOWrite32A( 0xE07CCC, 0x00005B29 ) ;
	} else if (SelMat != USER_MAT ){
		IOWrite32A( 0xE07CCC, 0x0000C5AD ) ;
	}
	AdditionalUnlockCodeSet() ;

	IOWrite32A( FLASHROM_FLA_ADR, ((UINT_32)SelMat << 16) | ( SetAddress & 0x00003C00 )) ;
	IOWrite32A( FLASHROM_CMD, 4 ) ;

	WitTim( 5 ) ;

	UlCnt = 0 ;

	do {
		if( UlCnt++ > 100 ){	ans = 2;	break;	} ;

		IORead32A( FLASHROM_FLAINT, &UlReadVal ) ;
	} while( ( UlReadVal & 0x00000080 ) != 0 ) ;

	ans	= UnlockCodeClear() ;
	if( ans != 0 )	return( ans ) ;

	return( ans ) ;
}

//********************************************************************************
// Function Name 	: FlashSingleRead
//********************************************************************************
UINT_8	FlashSingleRead( UINT_8 SelMat, UINT_32 UlAddress, UINT_32 *PulData )
{
	if( SelMat != USER_MAT && SelMat != INF_MAT0 && SelMat != INF_MAT1 && SelMat != INF_MAT2  )
		return 10 ;
	if( UlAddress > 0x00003FFF )
		return 9 ;

	IOWrite32A( FLASHROM_ACSCNT, 0x00000000 ) ;
	IOWrite32A( FLASHROM_FLA_ADR, ((UINT_32)SelMat << 16) | ( UlAddress & 0x00003FFF ) ) ;

	IOWrite32A( FLASHROM_CMD, 0x00000001 ) ;

	IORead32A( FLASHROM_FLA_RDAT, PulData ) ;

	return( 0 ) ;
}

//********************************************************************************
// Function Name 	: FlashMultiRead
//********************************************************************************
UINT_8	FlashMultiRead( UINT_8 SelMat, UINT_32 UlAddress, UINT_32 *PulData , UINT_8 UcLength )
{
	UINT_8	i ;

	if( SelMat != USER_MAT && SelMat != INF_MAT0 && SelMat != INF_MAT1 && SelMat != INF_MAT2  )
		return 10 ;
	if( UlAddress > 0x00003FFF )
		return 9;

	IOWrite32A( FLASHROM_ACSCNT, 0x00000000 | (UINT_32)(UcLength-1) );
	IOWrite32A( FLASHROM_FLA_ADR, ((UINT_32)SelMat << 16) | ( UlAddress & 0x00003FFF ) );

	IOWrite32A( FLASHROM_CMD, 0x00000001 );
	for( i=0 ; i < UcLength ; i++ ){
		IORead32A( FLASHROM_FLA_RDAT, &PulData[i] ) ;
	}

	return( 0 ) ;
}

//********************************************************************************
// Function Name 	: FlashPageWrite
//********************************************************************************
UINT_8	FlashPageWrite( UINT_8 SelMat , UINT_32 SetAddress , UINT_32 *PulData)
{
	UINT_32	UlReadVal, UlCnt;
	UINT_8	ans	= 0 ;
	UINT_8	i	 ;

	if( SelMat != USER_MAT && SelMat != INF_MAT0 && SelMat != INF_MAT1 && SelMat != INF_MAT2  )
		return 10;

	if( SetAddress > 0x00003FFF )
		return 9;

	ans	= UnlockCodeSet();
	if( ans != 0 )	return( ans ) ;

	WritePermission();
	if( SelMat != USER_MAT ){
		IOWrite32A( 0xE07CCC, 0x0000C5AD ) ;
	}
	AdditionalUnlockCodeSet() ;

	IOWrite32A( FLASHROM_FLA_ADR, ((UINT_32)SelMat << 16) | ( SetAddress & 0x00003FF0 )) ;
	IOWrite32A( FLASHROM_CMD, 2 ) ;

	UlCnt	= 0 ;

	for( i=0 ; i< 16 ; i++ ){
		IOWrite32A( FLASHROM_FLA_WDAT, PulData[i]  ) ;
	}
	do {
		if( UlCnt++ > 100 ){	ans = 2;	break;	} ;

		IORead32A( FLASHROM_FLAINT, &UlReadVal ) ;
	} while( ( UlReadVal & 0x00000080 ) != 0 ) ;

	IOWrite32A( FLASHROM_CMD, 8  );

	do {
		if( UlCnt++ > 100 ){	ans = 2;	break;	} ;

		IORead32A( FLASHROM_FLAINT, &UlReadVal ) ;
	} while( ( UlReadVal & 0x00000080 ) != 0 ) ;

	ans	= UnlockCodeClear() ;
	return( ans ) ;

}

//********************************************************************************
// Function Name 	: RdErInfMAT
//********************************************************************************
UINT_8	RdErInfMAT( UINT_8 SelMat, UINT_32 *InfMat, UINT_16 Length )
{
	UINT_8	ans	= 0 ;

	ans =FlashMultiRead( SelMat, 0, InfMat, Length ) ;

	if( ans == 0 ) {
		ans = FlashBlockErase( SelMat , 0 ) ;
		if( ans != 0 ) {
			ans = 2 ;
		}
	} else {
		ans = 1 ;
	}
	return( ans ) ;
}

//********************************************************************************
// Function Name 	: WrInfMAT
//********************************************************************************
UINT_8	WrInfMAT( UINT_8 SelMat, UINT_32 *InfMat, UINT_16 Length )
{
	UINT_8	ans	= 0 ;
	UINT_32	address ;

	for( address = 0; ( address < Length ) && ( ans == 0 ); address += 16 ) {
		ans = FlashPageWrite( SelMat , address , &InfMat[address] );
	}
	return( ans ) ;
}

//********************************************************************************
// Function Name 	: MkInfMATsum
//********************************************************************************
UINT_16	MkInfMATsum( UINT_32 *InfMAT )
{
	UINT_16 UsCkVal = 0 ;
	UINT_16 i ;

	for( i = 0; i < 63; i++ ) {
		UsCkVal +=  (UINT_8)(InfMAT[ i ] >> 0);
		UsCkVal +=  (UINT_8)(InfMAT[ i ] >> 8);
		UsCkVal +=  (UINT_8)(InfMAT[ i ] >> 16);
		UsCkVal +=  (UINT_8)(InfMAT[ i ] >> 24);
	}
	UsCkVal +=  (UINT_8)(InfMAT[ i ] >> 16);
	UsCkVal +=  (UINT_8)(InfMAT[ i ] >> 24);

	return ( UsCkVal ) ;
}

//********************************************************************************
// Function Name 	: GyroReCalib
//********************************************************************************
UINT_8	GyroReCalib( stReCalib * pReCalib )
{
	UINT_8	UcSndDat ;
	UINT_32	UlRcvDat ;
	UINT_32	UlGofX, UlGofY ;
	UnDwdVal UnFactoryOffset, UnGyroOffsetX, UnGyroOffsetY;

	RamRead32A( 0x1A28, (UINT_32 *)&UnGyroOffsetX);
	RamRead32A( 0x1A2C, (UINT_32 *)&UnGyroOffsetY);
	RamRead32A( 0x1A6C, (UINT_32 *)&UnFactoryOffset);

	RamWrite32A( 0xF014, 0x00000000 ) ;

	do {
		UcSndDat = RdStatus( 1 );
	} while ( UcSndDat != 0 );

	RamRead32A( 0xF014, &UlRcvDat ) ;
	UcSndDat = (unsigned char)( UlRcvDat >> 24 );

	if( UnFactoryOffset.StDwdVal.UsLowVal == 0xFFFF )
		pReCalib->SsFctryOffX = (INT_16)UnGyroOffsetX.StDwdVal.UsLowVal ;
	else
		pReCalib->SsFctryOffX = (INT_16)UnFactoryOffset.StDwdVal.UsLowVal ;

	if( UnFactoryOffset.StDwdVal.UsHigVal == 0xFFFF )
		pReCalib->SsFctryOffY = (INT_16)UnGyroOffsetY.StDwdVal.UsHigVal ;
	else
		pReCalib->SsFctryOffY = (INT_16)UnFactoryOffset.StDwdVal.UsHigVal ;

	RamRead32A( 0x0240, &UlGofX ) ;
	RamRead32A( 0x0244, &UlGofY ) ;

	pReCalib->SsRecalOffX = ( UlGofX >> 16 ) ;
	pReCalib->SsRecalOffY = ( UlGofY >> 16 ) ;
	pReCalib->SsDiffX = pReCalib->SsFctryOffX - pReCalib->SsRecalOffX ;
	pReCalib->SsDiffY = pReCalib->SsFctryOffY - pReCalib->SsRecalOffY ;

	return( UcSndDat );
}

//********************************************************************************
// Function Name 	: RdGyroOffsetTbl
//********************************************************************************
void RdGyroOffsetTbl( stGyroOffsetTbl *pTbl )
{
	RamRead32A( 0x0240, &pTbl->StAngle.SlOffsetX ) ;
	RamRead32A( 0x0244, &pTbl->StAngle.SlOffsetY ) ;
	RamRead32A( 0x03A8, &pTbl->StAngle.SlOffsetZ ) ;
	RamRead32A( 0x0454, &pTbl->StAccel.SlOffsetX ) ;
	RamRead32A( 0x0480, &pTbl->StAccel.SlOffsetY ) ;
	RamRead32A( 0x04AC, &pTbl->StAccel.SlOffsetZ ) ;
}

//********************************************************************************
// Function Name 	: WrGyroOffsetTbl
//********************************************************************************
UINT_8	WrGyroOffsetTbl( stGyroOffsetTbl *pTbl )
{
	UINT_32	UlMAT0[64];
	UINT_8 ans = 0;
	UINT_16	UsCkVal,UsCkVal_Bk ;

	IOWrite32A( 0xE0701C , 0x00000000 ) ;

	ans = RdErInfMAT( INF_MAT0, UlMAT0, 64 );
	if( ans ) {
		IOWrite32A( 0xE0701C , 0x00000002 ) ;
		return( ans );
	}

	UlMAT0[ 10 ] = (UINT_32)((UINT_32)(pTbl->StAccel.SlOffsetX & 0xFFFF0000) | (UINT_32)(pTbl->StAngle.SlOffsetX) >> 16);
	UlMAT0[ 11 ] = (UINT_32)((UINT_32)(pTbl->StAccel.SlOffsetY & 0xFFFF0000) | (UINT_32)(pTbl->StAngle.SlOffsetY) >> 16);
	UlMAT0[ 12 ] = (UINT_32)((UINT_32)(pTbl->StAccel.SlOffsetZ & 0xFFFF0000) | (UINT_32)(pTbl->StAngle.SlOffsetZ) >> 16);

	UsCkVal = MkInfMATsum( UlMAT0 ) ;
	UlMAT0[ 63 ] &= (UINT_32)0xFFFF0000 ;
	UlMAT0[ 63 ] |= (UINT_32)UsCkVal ;

	ans = WrInfMAT( INF_MAT0, UlMAT0, 64 ) ;
	if( ans != 0 ) {
		IOWrite32A( 0xE0701C , 0x00000002 ) ;
		return( 3 ) ;
	}

	UsCkVal_Bk = UsCkVal;
	ans =FlashMultiRead( INF_MAT0, 0, UlMAT0, 64 );
	if( ans ) {
		IOWrite32A( 0xE0701C , 0x00000002 ) ;
		return( 4 );
	}
	UsCkVal = MkInfMATsum( UlMAT0 ) ;

	if( UsCkVal != UsCkVal_Bk ) {
		IOWrite32A( 0xE0701C , 0x00000002 ) ;
		return(5);
	}

	IOWrite32A( 0xE0701C , 0x00000002 ) ;
	return(0);
}


//********************************************************************************
// Function Name 	: RdFlashMulti
//********************************************************************************
UINT_8	RdFlashMulti( UINT_8 SelMat, UINT_32 UlAddress, UINT_32 *PulData , UINT_8 UcLength )
{
	UINT_8	ans ;

	IOWrite32A( 0xE0701C, 0x00000000 ) ;
	ans = FlashMultiRead( SelMat, UlAddress, PulData, UcLength ) ;
	IOWrite32A( 0xE0701C, 0x00000002 ) ;

	return( ans ) ;
}

//********************************************************************************
// Function Name 	: RdFlashSingle
//********************************************************************************
UINT_8	RdFlashSingle( UINT_8 SelMat, UINT_32 UlAddress, UINT_32 *PulData )
{
	UINT_8	ans ;

	IOWrite32A( 0xE0701C, 0x00000000 ) ;
	ans = FlashSingleRead( SelMat, UlAddress, PulData ) ;
	IOWrite32A( 0xE0701C, 0x00000002 ) ;

	return( ans ) ;
}

//********************************************************************************
// Function Name 	: WrTempCompData
//********************************************************************************
UINT_8	WrTempCompData( stAdj_Temp_Compensation * TempCompPtr )
{
	UINT_32	UlMAT1[ 64 ];
	UINT_8 ans = 0;
	UINT_16	UsCkVal,UsCkVal_Bk ;

	IOWrite32A( 0xE0701C , 0x00000000 ) ;

	ans = RdErInfMAT( INF_MAT1, UlMAT1, 64 );
	if( ans ) {
		IOWrite32A( 0xE0701C , 0x00000002 ) ;
		return( ans );
	}

	UlMAT1[  0 ] = TempCompPtr->rcodeX;
	UlMAT1[  1 ] = TempCompPtr->rcodeY;
	UlMAT1[  2 ] = TempCompPtr->rcodeZ;
	UlMAT1[  3 ] = TempCompPtr->shag;
	UlMAT1[  4 ] = TempCompPtr->shbg;
	UlMAT1[  5 ] = TempCompPtr->shcg;
	UlMAT1[  6 ] = TempCompPtr->shoutag;
	UlMAT1[  7 ] = TempCompPtr->shoutbg;
	UlMAT1[  8 ] = TempCompPtr->shoutag1;
	UlMAT1[  9 ] = TempCompPtr->shoutbg1;
	UlMAT1[ 10 ] = TempCompPtr->tag;
	UlMAT1[ 11 ] = TempCompPtr->tbg;
	UlMAT1[ 12 ] = TempCompPtr->shiftg;
	UlMAT1[ 13 ] = TempCompPtr->shab;
	UlMAT1[ 14 ] = TempCompPtr->shac;
	UlMAT1[ 15 ] = TempCompPtr->shaa;
	UlMAT1[ 16 ] = TempCompPtr->shbb;
	UlMAT1[ 17 ] = TempCompPtr->shbc;
	UlMAT1[ 18 ] = TempCompPtr->shba;
	UlMAT1[ 19 ] = TempCompPtr->shcb;
	UlMAT1[ 20 ] = TempCompPtr->shcc;
	UlMAT1[ 21 ] = TempCompPtr->shca;
	UlMAT1[ 22 ] = TempCompPtr->tab;
	UlMAT1[ 23 ] = TempCompPtr->tac;
	UlMAT1[ 24 ] = TempCompPtr->taa;
	UlMAT1[ 25 ] = TempCompPtr->tbb;
	UlMAT1[ 26 ] = TempCompPtr->tbc;
	UlMAT1[ 27 ] = TempCompPtr->tba;
	UlMAT1[ 28 ] = TempCompPtr->TEMPOFF;

	UlMAT1[ 29 ] = ((UINT_32)TempCompPtr->tcx << 16) | ((UINT_32)TempCompPtr->tbx << 8) | ((UINT_32)TempCompPtr->tax) ;

	UsCkVal = MkInfMATsum( UlMAT1 ) ;
	UlMAT1[ 63 ] &= (UINT_32)0xFFFF0000 ;
	UlMAT1[ 63 ] |= (UINT_32)UsCkVal ;

	ans = WrInfMAT( INF_MAT1, UlMAT1, 64 ) ;
	if( ans != 0 ) {
		IOWrite32A( 0xE0701C , 0x00000002 ) ;
		return( 3 ) ;
	}

	UsCkVal_Bk = UsCkVal;
	ans =FlashMultiRead( INF_MAT1, 0, UlMAT1, 64 );
	if( ans ) {
		IOWrite32A( 0xE0701C , 0x00000002 ) ;
		return( 4 );
	}
	UsCkVal = MkInfMATsum( UlMAT1 ) ;

	if( UsCkVal != UsCkVal_Bk ) {
		IOWrite32A( 0xE0701C , 0x00000002 ) ;
		return( 5 );
	}

	IOWrite32A( 0xE0701C , 0x00000002 ) ;
	return( 0 );
}
//********************************************************************************
// Function Name 	: LoadUareaToPM
//********************************************************************************
UINT_8 LoadUareaToPM( CODE_TBL_EXT* ptr , UINT_8 mode )
{
	UINT_8 ans=0;
	UINT_32	UlReadVal=0;
	UINT_32	UlCnt=0;
	UINT_32	UlCver=0;

	IORead32A( SYSDSP_CVER , &UlCver );
	if( UlCver == 0x0141  ){
		return( 0xF0 );
	}

	RamRead32A( 0x500F, (UINT_32 *)&UlReadVal );

	if( UlReadVal != PMEM_INITIALIZED ){

		IORead32A( SYSDSP_CVER , &UlCver );

		BootMode();

	 	ans = PmemUpdate129( 0, ptr );
		if(ans != 0){
			return( ans );
		}
		RamWrite32A( 0xF007, 0x00000000 );
	}

	IOWrite32A( 0xE0701C , 0x00000000);
	RamWrite32A( 0x5004 , 0x00000000 );

	do{
		WitTim( 1 );
		if( UlCnt++ > 100 ) {
			IOWrite32A( 0xE0701C , 0x00000002);
			return (0x10) ;
		}
		RamRead32A( 0x5004, &UlReadVal );
	}while ( UlReadVal != 0 );
	IOWrite32A( 0xE0701C , 0x00000002);

	return( 0 );
}

//********************************************************************************
// Function Name 	: RdBurstUareaFromPM
//********************************************************************************
UINT_8	RdBurstUareaFromPM( UINT_32 UlAddress, UINT_8 *PucData , UINT_16 UsLength , UINT_8 mode )
{
	UINT_32	UlCver=0;

	IORead32A( SYSDSP_CVER , &UlCver );
	if( UlCver == 0x0141  ){
		return( 0xF0 );
	}

	if( !UsLength )	return( 0xFF );
	if( !mode ){
		RamWrite32A( 0x5000 , UlAddress );
	}
	RamWrite32A( 0x5002 , (UINT_32)UsLength - 1 );
	WitTim( 1 ) ;
	CntRd( 0x5002 , PucData , (UINT_16)UsLength );

	return( 0 );
}

//********************************************************************************
// Function Name 	: RdSingleUareaFromPM
//********************************************************************************
UINT_8	RdSingleUareaFromPM( UINT_32 UlAddress, UINT_8 *PucData , UINT_8 UcLength , UINT_8 mode )
{
	UINT_32	ReadData;
	UINT_32	UlCver=0;
	UINT_8 i;

	IORead32A( SYSDSP_CVER , &UlCver );
	if( UlCver == 0x0141  ){
		return( 0xF0 );
	}

	if( !UcLength )	return( 0xFF );
	if( !mode ){
		RamWrite32A( 0x5000 , UlAddress );
	}
	for(i = 0 ; i < UcLength;)
	{
		RamRead32A( 0x5001 , &ReadData );
		PucData[i++] = (UINT_8)(ReadData >> 24);
		PucData[i++] = (UINT_8)(ReadData >> 16);
		PucData[i++] = (UINT_8)(ReadData >> 8);
		PucData[i++] = (UINT_8)(ReadData >> 0);
	}

	return( 0 );
}

//********************************************************************************
// Function Name 	: WrUareaToPm
//********************************************************************************
UINT_8	WrUareaToPm( UINT_32 UlAddress, UINT_8 *PucData , UINT_8 UcLength , UINT_8 mode )
{
	UINT_8	PucBuf[256];
	UINT_32	UlCver=0;
	UINT_8 i;

	IORead32A( SYSDSP_CVER , &UlCver );
	if( UlCver == 0x0141  ){
		return( 0xF0 );
	}

	if(!UcLength || UcLength > 254)	return( 0xFF );

	PucBuf[0] = 0x50;
	PucBuf[1] = 0x01;

	for(i=0 ; i<UcLength ; i++)
	{
		PucBuf[i+2] = PucData[i];
	}

	if( !mode ){
		RamWrite32A( 0x5000 , UlAddress );
	}
	CntWrt( PucBuf , (UINT_16)UcLength + 2 );

	return( 0 );
}

//********************************************************************************
// Function Name 	: WrUareaToPmInt
//********************************************************************************
UINT_8	WrUareaToPmInt( UINT_32 UlAddress, UINT_32 *PulData , UINT_8 UcLength , UINT_8 mode )
{
	UINT_8	PucBuf[256];
	UINT_32	UlCver=0;
	UINT_8 i;

	IORead32A(SYSDSP_CVER, &UlCver);
	if( UlCver == 0x0141  ){
		return( 0xF0 );
	}

	if(!UcLength || UcLength > 63)	return(0xff);

	PucBuf[0] = 0x50;
	PucBuf[1] = 0x01;

	for(i = 0; i < UcLength; i++) {
		PucBuf[i*4 + 2] = (UINT_8)(PulData[i] >> 0);
		PucBuf[i*4 + 3] = (UINT_8)(PulData[i] >> 8);
		PucBuf[i*4 + 4] = (UINT_8)(PulData[i] >> 16);
		PucBuf[i*4 + 5] = (UINT_8)(PulData[i] >> 24);
	}

	if( !mode ){
		RamWrite32A( 0x5000 , UlAddress );
	}
	CntWrt( PucBuf , (UINT_16)UcLength * 4 + 2 );

	return( 0 );
}

//********************************************************************************
// Function Name 	: WrUareaToFlash
//********************************************************************************
UINT_8	WrUareaToFlash( void )
{
	UINT_32	UlReadVal;
	UINT_32	UlCntE=0;
	UINT_32	UlCntW=0;
	UINT_8	ans=0;
	UINT_32	UlCver=0;

	IORead32A( SYSDSP_CVER , &UlCver );
	if( UlCver == 0x0141  ){
		return( 0xF0 );
	}

	ans	= UnlockCodeSet();
	if( ans != 0 )	return( ans ) ;
	WritePermission();
	AdditionalUnlockCodeSet();

	IOWrite32A( 0xE0701C , 0x00000000);
	RamWrite32A( 0x5005 , 0x00000000 );

	WitTim( 10 );
	do{
		WitTim( 1 );
		if( UlCntE++ > 20 ) {
			IOWrite32A( 0xE0701C , 0x00000002);
			if( UnlockCodeClear() != 0 ) 	return (0x11) ;
			return (0x10) ;
		}
		RamRead32A( 0x5005, &UlReadVal );
	}while ( UlReadVal != 0 );

	RamWrite32A( 0x5006 , 0x00000000 );

	WitTim( 300 );
	do{
		WitTim( 1 );
		if( UlCntW++ > 300 ) {
			IOWrite32A( 0xE0701C , 0x00000002);
			if( UnlockCodeClear() != 0 ) 	return (0x21) ;
			return (0x20) ;
		}
		RamRead32A( 0x5006, &UlReadVal );
	}while ( UlReadVal != 0 );
	IOWrite32A( 0xE0701C , 0x00000002);
	if( UnlockCodeClear() != 0 ) 	return (0x31) ;

	return( 0 );
}
//********************************************************************************
// Function Name 	: RdStatus
//********************************************************************************
UINT_8	RdStatus( UINT_8 UcStBitChk )
{
	UINT_32	UlReadVal ;

	RamRead32A( 0xF100 , &UlReadVal );
	if( UcStBitChk ){
		UlReadVal &= READ_STATUS_INI ;
	}
	if( !UlReadVal ){
		return( SUCCESS );
	}else{
		return( FAILURE );
	}
}

//********************************************************************************
// Function Name 	: fix2float
//********************************************************************************
float fix2float( UINT_32 fix )
{
	if((fix & 0x80000000) > 0)
	{
		return ((float)fix - (float)0x100000000) / (float)0x7FFFFFFF;
	} else {
		return (float)fix / (float)0x7FFFFFFF;
	}
}

//********************************************************************************
// Function Name 	: float2fix
//********************************************************************************
UINT_32 float2fix( float f )
{
	if(f < 0)
	{
		return (UINT_32)(f * (float)0x7FFFFFFF + 0x100000000);
	} else {
		return (UINT_32)(f * (float)0x7FFFFFFF);
	}
}

#if 0
//********************************************************************************
// Function Name 	: um2pos
//********************************************************************************
UINT_8 um2pos( float fRadius, float fDgree, float fPixelSize, UINT_32 *UlXpos, UINT_32 *UlYpos )
{
	UINT_32	calibdata;
	UINT_32	linbuf[8];
	UINT_32	uixpxl,uiypxl;
	INT_32	sixstp,siystp;
	UINT_16	uiIndex;
	float xRadius, yRadius;

	RamRead32A( 0x8010 , &calibdata ) ;
	if( calibdata & 0x00000400 ){
		*UlXpos = *UlYpos = 0;
		return FAILURE;
	}

	for( uiIndex = 0; uiIndex < 8; uiIndex++ ) {
		RamRead32A( INF0_DATA + ((15 + uiIndex) * 4), (UINT_32 *)linbuf[uiIndex] );
	}
	uiypxl = (( linbuf[4] & 0xFFFF0000 ) - ( linbuf[3] & 0xFFFF0000 )) >> 16;
	uixpxl = (( linbuf[4] & 0x0000FFFF ) - ( linbuf[3] & 0x0000FFFF )) >>  0;
	siystp  = ( INT_32 )( linbuf[7] & 0xFFFF0000 ) << 0;
	sixstp  = ( INT_32 )( linbuf[7] & 0x0000FFFF ) << 16;

	xRadius = ( fabsf(fix2float(sixstp)) * 10.0f) / ((float)uixpxl * fPixelSize);
	yRadius = ( fabsf(fix2float(siystp)) * 10.0f) / ((float)uiypxl * fPixelSize);

	xRadius = xRadius * fRadius ;
	yRadius = yRadius * fRadius ;

	*UlXpos = float2fix( xRadius * cos( fDgree * M_PI/180.0f ));
	*UlYpos = float2fix( yRadius * sin( fDgree * M_PI/180.0f ));

	return SUCCESS;
}
#endif

//********************************************************************************
// Function Name 	: FlashProgram129
//********************************************************************************
UINT_8 FlashProgram129( UINT_8 chiperase, UINT_8 ModuleVendor, UINT_8 ActVer )
{
	CODE_TBL_EXT* ptr ;

	ptr = ( CODE_TBL_EXT * )CdTbl ;
	do {
		if( ptr->Index == ( ((UINT_16)ModuleVendor << 8) + ActVer) ) {
			BootMode() ;
			return FlashUpdate129( chiperase, ptr );
		}
		ptr++ ;
	} while (ptr->Index != 0xFFFF ) ;

	return ( 0xF0 ) ;
}

//********************************************************************************
// Function Name 	: PmemDownload129
//********************************************************************************
UINT_8 PmemDownload129( UINT_8 ModuleVendor, UINT_8 ActVer )
{
	CODE_TBL_EXT* ptr ;

	ptr = ( CODE_TBL_EXT * )CdTbl ;
	do {
		if( ptr->Index == ( ( (UINT_16)ModuleVendor << 8 ) + ActVer ) ) {
			BootMode() ;
			return PmemUpdate129( 1, ptr ) ;
		}
		ptr++ ;
	} while( ptr->Index != 0xFFFF ) ;

	return ( 0xF0 ) ;
}

