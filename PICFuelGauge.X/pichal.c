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
//! \brief  Apply an Exponential Moving Average filter
//!
//! This applies an exponential moving average filter to the supplied value. The
//! filter is of the form:
//!
//! y[n] = alpha * x[n] + (1 - alpha) * y[n-1]
//!
//! where "alpha" is a value between 0 and 1 indicating the historical weight
//!
//! This implementation uses just addition subtraction and bit-shift to do this:
//!
//! alpha = 1 / (2^k)
//!
//! Algorithm and performance from
//! https://tttapa.github.io/Pages/Mathematics/Systems-and-Control-Theory/Digital-filters/Exponential%20Moving%20Average/Exponential-Moving-Average.html
//!
///////////////////////////////////////////////////////////////////////////////
static uint16_t Filter( uint16_t x, uint8_t k )
{
    //
    // Historic store of the last filtered value
    static uint32_t z = 0;

    z += x;
    uint32_t y = ( z + ( 1 << ( k - 1 ) ) ) >> k;
    z -= y;

    return y;
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Start and wait for an ADC conversion from the tank input
//!
//! \note   The value read from the ADC is 10-bit but LH justified
//!
///////////////////////////////////////////////////////////////////////////////
uint16_t HAL_GetTankInput()
{
    //
    // Read the ADC and run through our smoothing filter
    //
    // k = 5 results in a -3dB roll off of 5Hz @ 1kHz sample rate
    // k = 6 results in a -3dB roll off of 2.5Hz @ 1kHz sample rate
    // k = 7 results in a -3dB roll off of 1.25Hz @ 1kHz sample rate
    // k = 8 results in a -3dB roll off of 0.62Hz @ 1kHz sample rate
    //
    uint16_t value = Filter( ADC_GetConversion( tank ), 8 );

    //
    // Limit our sampling frequency to around 1kHz at a maximum
    //
    __delay_ms( 1 );

    //
    // If we consistently indicate full-scale on the ADC this means we have
    // an open input or another error - force this to be the error value
    //
    if ( value >= 0xffc0 )
    {
        return TANK_INPUT_ERROR;
    }
    else
    {
        return value;
    }
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
//! \brief  Turn on or off the Low Fuel Warning light
//!
///////////////////////////////////////////////////////////////////////////////
void HAL_SetLowFuelLight( bool newState )
{
    // lowFuel_LAT = newState;
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
void HAL_LoadMaps( uint16_t* input, uint16_t* output, uint16_t* lowFuelLevel )
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

    //
    // Load in the single low fuel setting after the maps
    //
    value = DATAEE_ReadByte( addr ) << 8;
    addr++;
    value += DATAEE_ReadByte( addr );
    addr++;
    *lowFuelLevel = value;
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Save the input and output maps from the beginning of EEPROM
//!
//! \note   The map values are stored in big-endian order
//!
///////////////////////////////////////////////////////////////////////////////
void HAL_SaveMaps(
    const uint16_t* input,
    const uint16_t* output,
    uint16_t        lowFuelLevel )
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

    //
    // Save out the single low fuel setting after the maps
    //
    value = lowFuelLevel;

    DATAEE_WriteByte( addr, value >> 8 );
    addr++;
    DATAEE_WriteByte( addr, value & 0xFF );
    addr++;
}
