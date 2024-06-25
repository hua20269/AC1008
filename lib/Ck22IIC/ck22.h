#ifndef _CK22_H
#define _CK22_H

#include "keros_lib.h"
#include "keros_i2c_interface.h"
#include "keros_interface.h"


/* USER CODE END Includes */
#define _APP_CAP_USE_BYPASS 				1
#define _APP_CAP_USE_AUTHENTICATION	2

#define KEROS_USE_MODE	_APP_CAP_USE_BYPASS


/* USER CODE BEGIN 0 */
#if	(KEROS_USE_MODE == _APP_CAP_USE_BYPASS)
char _bypasscmp(uint8_t *s, uint8_t *d)
{
			int i;

			for( i=0; i<16; i++)
				{
						if((s[i] ^ 0xFF) != d[i])
						{
								return FALSE;
						}
				}

				return TRUE;
}

int Test_keros_bypass( void )
{
		uint8_t nStatus, i;
		uint8_t w_buffer[ MAX_AES_BUFFER_SIZE ];
		uint8_t r_buffer[ MAX_AES_BUFFER_SIZE ];
		uint8_t bCheckNotMathed = 0;

		// write data : 30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F
		// bypass data : CF CE CD CC CB CA C9 C8 C7 C6 C5 C4 C3 C2 C1 C0

		//DEBUGOUT( "=============================================\r\n" );
		//DEBUGOUT( "Start Bypass Test...\r\n" );

		for ( i = 0; i < MAX_AES_BUFFER_SIZE; i++ )
		{
				w_buffer[ i ] = 0x30 + i; //Initiazlie bypass request data
		}

		for ( i = 0; i < MAX_AES_BUFFER_SIZE; i++ )
		{
				r_buffer[ i ] = 0;
		}

		nStatus = keros_bypass_mode( w_buffer, r_buffer );

		if ( nStatus == KEROS_STATUS_OK )
		{
				 bCheckNotMathed = _bypasscmp(w_buffer,r_buffer);

					if ( bCheckNotMathed == TRUE )
					{
							return 1;
					}
					else
					{
							return 0;
					}
		}
		else
		{
				return -1;
		}
		//DEBUGOUT( "=============================================\r\n" );
}
/* USER CODE END 0 */
#endif


int keros_main(void)
{
    uint8_t kerosSerialNumber[ 5 ];
	uint8_t status;
#if	(KEROS_USE_MODE == _APP_CAP_USE_AUTHENTICATION)
	uint8_t indata[16];
	uint8_t i;
#endif
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

/*****************************************************************************

 * Keros Bypass Test Code

 ****************************************************************************/
  		//KEROS Power ON
  	keros_power_on(); //Send Power On Command to KEROS Chip

  	//
  	keros_delay( 2 );

  	//Initiazlie KEROS LIB
  	keros_init( kerosSerialNumber );
#if	(KEROS_USE_MODE == _APP_CAP_USE_BYPASS)
   	//Start test KEROS Bypass Mode
  	status = Test_keros_bypass();
#endif
#if	(KEROS_USE_MODE == _APP_CAP_USE_AUTHENTICATION)
	for(i=0; i<16; i++)
	{
		indata[i] = keros_random();
	}
	status = keros_authentication(SET_AES_KEY_SIZE_256, 0, indata);
#endif

  	if(status == 1)
  	{
  		Serial.println( "Bypass Test OK!\r\n" );
	}
  	else //if(nStatus == 0 || nStatus == -1)
  	{
  		Serial.println( "Bypass Test Failed!\r\n" );

	}
    return status;
}
#endif