/* RevKit: A Toolkit for Reversible Circuit Design (www.revkit.org)
 * Copyright (C) 2009-2013  The RevKit Developers <revkit@informatik.uni-bremen.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "exorcism_minimization.hpp"

#include <stdio.h>

#include <cstdlib>
#include <fstream>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include <core/io/pla_parser.hpp>

namespace revkit
{

/******************************************************************************
 * Private functions                                                          *
 ******************************************************************************/

class exorcism_processor : public pla_processor
{
public:
  exorcism_processor( const cube_function_t& on_cube_f ) : on_cube_f( on_cube_f ) {}

  void on_cube( const std::string& in, const std::string& out )
  {
    /* only one output functions are supported */
    assert( out.size() == 1u );

    unsigned n = in.size();
    boost::dynamic_bitset<> values( n );
    boost::dynamic_bitset<> care( n );

    for ( unsigned i = 0u; i < n; ++i )
    {
      values[i] = ( in[i] == '1' );
      care[i]   = ( in[i] != '-' );
    }

    cube_t cube = std::make_pair( values, care );
    on_cube_f( cube );
  }

private:
  const cube_function_t& on_cube_f;
};

/******************************************************************************
 * Public functions                                                           *
 ******************************************************************************/

void exorcism_minimization( DdManager * cudd, DdNode * f, properties::ptr settings, properties::ptr statistics )
{
  /* Settings */
  std::string tmpfile = get( settings, "tmpfile", std::string( "/tmp/test.pla" ) );

  /* Re-route stdout of Cudd */
  FILE * old = Cudd_ReadStdout( cudd );

  /* Print cover to file */
  FILE * fd = fopen( tmpfile.c_str(), "w" );
  Cudd_SetStdout( cudd, fd );

  fprintf( fd, ".i %d\n.o 1\n", Cudd_ReadSize( cudd ) );
  Cudd_PrintMinterm( cudd, f );
  fprintf( fd, ".e\n" );

  fclose( fd );

  /* Re-set stdout of Cudd */
  Cudd_SetStdout( cudd, old );

  /* Run exorcism based on the file */
  exorcism_minimization( tmpfile, settings, statistics );
}

void exorcism_minimization( const std::string& filename, properties::ptr settings, properties::ptr statistics )
{
  /* Settings */
  bool            verbose  = get( settings, "verbose",  false                     );
  std::string     exorcism = get( settings, "exorcism", std::string( "exorcism" ) );
  cube_function_t on_cube  = get( settings, "on_cube",  cube_function_t()         );


  /* Call */
  std::string hide_output = verbose ? "" : " > /dev/null 2>&1";
  system( boost::str( boost::format( "%s %s%s" ) % exorcism % filename % hide_output ).c_str() );

  /* Get ESOP filename */
  boost::filesystem::path path( filename );
  std::string esopname = boost::str( boost::format( "%s/%s.esop" ) % path.parent_path().string() % path.stem().string() );

  /* Parse */
  if ( on_cube )
  {
    exorcism_processor p( on_cube );
    pla_parser( esopname, p );
  }
}

dd_based_esop_optimization_func dd_based_exorcism_minimization_func(properties::ptr settings, properties::ptr statistics)
{
  dd_based_esop_optimization_func f = [&settings, &statistics]( DdManager * cudd, DdNode * f ) {
    return exorcism_minimization( cudd, f, settings, statistics );
  };
  f.init( settings, statistics );
  return f;
}

pla_based_esop_optimization_func pla_based_exorcism_minimization_func(properties::ptr settings, properties::ptr statistics)
{
  pla_based_esop_optimization_func f = [&settings, &statistics]( const std::string& filename ) {
    return exorcism_minimization( filename, settings, statistics );
  };
  f.init( settings, statistics );
  return f;
}

}

// Local Variables:
// c-basic-offset: 2
// End: