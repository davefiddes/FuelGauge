///////////////////////////////////////////////////////////////////////////////
//!
//! \file
//! \brief  Hardware Abstraction Library for a PIC12F1840 Fuel Gauge
//!
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 David J. Fiddes
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////

#include "mcc_generated_files/mcc.h"
#include <hal.h>
#include <mapper.h>
#include <stdarg.h>
#include <stdint.h>
#include <xc.h>

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Start and wait for an ADC conversion from the tank input
//!
//! \note   The value read from the ADC is 10-bit but LH justified
//!
///////////////////////////////////////////////////////////////////////////////
uint16_t HAL_GetTankInput()
{
    return ADC_GetConversion( tank );
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Read back the currently configured gauge PWM value and scale to
//!         16-bits
//!
///////////////////////////////////////////////////////////////////////////////
uint16_t HAL_GetGaugeOutput()
{
    // Read the 8 MSBs of pwm duty cycle from the CCPRL register
    uint16_t value = CCPR1L << 8;

    // Read the 2 LSBs of pwm duty cycle from the CCPCON register
    value = value | ( CCP1CON & 0x30 ) << 2;

    return value;
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Set the PWM output to the gauge output value scaled to 10-bits
//!
///////////////////////////////////////////////////////////////////////////////
void HAL_SetGaugeOutput( uint16_t value )
{
    EPWM_LoadDutyValue( value >> 6 );
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Send the text to the USART
//!
///////////////////////////////////////////////////////////////////////////////
void HAL_PrintText( const char* text )
{
    while ( *text )
    {
        while ( !EUSART_is_tx_ready() )
        {
        }
        EUSART_Write( *text );
        text++;
    }
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Just send a carriage-return and line-feed
//!
///////////////////////////////////////////////////////////////////////////////
void HAL_PrintNewline( void )
{
    HAL_PrintText( "\r\n" );
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Load the input and output maps from the beginning of EEPROM
//!
//! \note   The map values are stored in big-endian order
//!
///////////////////////////////////////////////////////////////////////////////
void HAL_LoadMaps( uint16_t* input, uint16_t* output )
{
    uint16_t value;
    uint8_t  addr = 0;

    for ( uint8_t i = 0; i < MAPSIZE; i++ )
    {
        value = DATAEE_ReadByte( addr ) << 8;
        addr++;
        value += DATAEE_ReadByte( addr );
        addr++;

        input[ i ] = value;
    }

    for ( uint8_t i = 0; i < MAPSIZE; i++ )
    {
        value = DATAEE_ReadByte( addr ) << 8;
        addr++;
        value += DATAEE_ReadByte( addr );
        addr++;

        output[ i ] = value;
    }
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Save the input and output maps from the beginning of EEPROM
//!
//! \note   The map values are stored in big-endian order
//!
///////////////////////////////////////////////////////////////////////////////
void HAL_SaveMaps( const uint16_t* input, const uint16_t* output )
{
    uint16_t value;
    uint8_t  addr = 0;

    for ( uint8_t i = 0; i < MAPSIZE; i++ )
    {
        value = input[ i ];

        DATAEE_WriteByte( addr, value >> 8 );
        addr++;
        DATAEE_WriteByte( addr, value & 0xFF );
        addr++;
    }

    for ( uint8_t i = 0; i < MAPSIZE; i++ )
    {
        value = output[ i ];

        DATAEE_WriteByte( addr, value >> 8 );
        addr++;
        DATAEE_WriteByte( addr, value & 0xFF );
        addr++;
    }
}
