///////////////////////////////////////////////////////////////////////////////
//!
//! \file
//! \brief  Command processor for the Fuel Gauge
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

#ifndef COMMAND_H
#define COMMAND_H

#include <stdbool.h>
#include <stdint.h>

#if defined( __XC )
#include <xc.h> /* XC8 General Include File */
#endif

#ifdef __cplusplus // Provide C++ Compatibility
extern "C" {
#endif

void InitialiseGauge( void );
bool ProcessCommand( const char* command );
void RunGauge( void );
bool IsRunning( void );

#ifdef __cplusplus // Provide C++ Compatibility
}
#endif

#endif // COMMAND_H
