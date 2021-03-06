///////////////////////////////////////////////////////////////////////////////
//!
//! \file
//! \brief  Fuel Gauge main
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
#include <command.h>
#include <ctype.h>
#include <hal.h>

//
//! Length of our input line buffer
//
#define BUFFERLEN 20

//
//! The number of loop iterations we need to flash at approx 1Hz
//
#define ERRORFLASHDURATION 1000

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Device main loop
//!
///////////////////////////////////////////////////////////////////////////////
void main( void )
{
    // initialise the device
    SYSTEM_Initialize();

    // When using interrupts, you need to set the Global and Peripheral
    // Interrupt Enable bits Use the following macros to:

    // Enable the Global Interrupts
    // INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    // INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    // INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    // INTERRUPT_PeripheralInterruptDisable();

//
// Quote a version string to turn it into a C-string
//
#define VERSION_STR( x ) VERSION_STR1( x )
#define VERSION_STR1( x ) #x

    HAL_PrintText( "FuelGauge Version " VERSION_STR(
        GIT_VERSION ) "\r\n\r\nPress \"u\" for usage\r\n\r\n" );

    //
    // Check to see if we have experienced a watchdog reset.
    // Note: This needs the -mresetbits compiler option set otherwise this
    // information is clobbered by the normal start up code
    //
    if ( __timeout == 0 )
    {
        HAL_PrintText( "Watchdog timeout\r\n\r\n" );
    }

    //
    // Start the PWM output
    //
    TMR2_StartTimer();

    InitialiseGauge();

    char        rxData;
    char        lineBuffer[ BUFFERLEN ];
    const char* lineBufferBegin = &lineBuffer[ 0 ];
    const char* lineBufferEnd = lineBufferBegin + BUFFERLEN;
    char*       bufferPos = (char*)lineBufferBegin;
    int         errorCount = 0;

    *bufferPos = '\0';

    while ( 1 )
    {
        //
        // Check to see if we have a character waiting
        //
        if ( EUSART_is_rx_ready() )
        {
            rxData = EUSART_Read();

            if ( rxData == '\r' )
            {
                //
                // Echo the CR before doing any work
                //
                HAL_PrintNewline();

                *bufferPos = '\0';

                //
                // Process our line buffer as a new command
                //
                if ( ProcessCommand( lineBuffer ) )
                {
                    HAL_PrintText( "OK" );
                    HAL_PrintNewline();
                }
                else
                {
                    HAL_PrintText( "Command Error" );
                    HAL_PrintNewline();
                }
                bufferPos = (char*)lineBufferBegin;
                *bufferPos = '\0';
            }
            else if ( isprint( rxData ) )
            {
                //
                // Local echo
                //
                if ( EUSART_is_tx_ready() )
                {
                    EUSART_Write( rxData );
                }

                //
                // Store the character in our line buffer and check for length
                //
                *bufferPos = rxData;
                bufferPos++;
                if ( bufferPos == lineBufferEnd )
                {
                    HAL_PrintNewline();
                    HAL_PrintText( "Line too long" );
                    HAL_PrintNewline();
                    bufferPos = (char*)lineBufferBegin;
                    *bufferPos = '\0';
                }
            }
        }

        //
        // Run the gauge main loop
        //
        if ( !RunGauge() )
        {
            //
            // If we don't already have an error flash in progress start a
            // new one
            //
            if ( errorCount == 0 )
            {
                //
                // The error flash will have a 50% duty cycle so double the
                // interations
                //
                errorCount = ERRORFLASHDURATION * 2;
                lowFuel_SetHigh();
            }
        }

        //
        // If we have an outstanding error take care of flashing the low fuel
        // LED
        //
        if ( errorCount > 0 )
        {
            if ( errorCount == ERRORFLASHDURATION )
            {
                lowFuel_SetLow();
            }
            errorCount--;

            //
            // Wait for 1ms which should be enough to keep the main loop
            // responsive but not require huge numbers of iterations to build
            // longer delays for the error flash
            //
            __delay_ms( 1 );
        }

        //
        // Strobe the watchdog every time round the main loop so we don't reboot
        //
        CLRWDT();
    }
}
