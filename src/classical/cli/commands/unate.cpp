/* CirKit: A circuit toolkit
 * Copyright (C) 2009-2015  University of Bremen
 * Copyright (C) 2015-2016  EPFL
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

#include "unate.hpp"

#include <fstream>
#include <iostream>

#include <boost/format.hpp>

#include <core/utils/program_options.hpp>
#include <classical/utils/unateness.hpp>
#include <classical/verification/unate.hpp>

using namespace boost::program_options;

namespace cirkit
{

/******************************************************************************
 * Types                                                                      *
 ******************************************************************************/

/******************************************************************************
 * Private functions                                                          *
 ******************************************************************************/

void print_unateness( std::ostream& os, const boost::dynamic_bitset<>& u, const aig_graph_info& info )
{
  for ( auto po = 0u; po < info.outputs.size(); ++po )
  {
    for ( auto pi = 0u; pi < info.inputs.size(); ++pi )
    {
      switch ( get_unateness_kind( u, po, pi, info ) )
      {
      case unate_kind::binate:
        os << ".";
        break;
      case unate_kind::unate_pos:
        os << "p";
        break;
      case unate_kind::unate_neg:
        os << "n";
        break;
      default:
        os << " ";
        break;
      }
    }
    os << std::endl;
  }
}

/******************************************************************************
 * Public functions                                                           *
 ******************************************************************************/

unate_command::unate_command( const environment::ptr& env )
  : aig_base_command( env, "Checks function for unateness" )
{
  opts.add_options()
    ( "progress,p",                                                        "Show progress" )
    ( "approach",   value_with_default( &approach ),                       "0: direct\n"
                                                                           "1: via mapped based CNFization\n"
                                                                           "2: Split outputs first\n"
                                                                           "3: Split outputs first (parallel)\n"
                                                                           "4: Split inputs first (parallel)\n" )
    ( "skiplist,s",                                                        "Compute skip list to skip functional support checks (only with approach 1)" )
    ( "matrix,m",   value( &matrixname )->implicit_value( std::string() ), "Prints unateness matrix:\n"
                                                                           "  rows: POs, columns: PIs\n"
                                                                           "  . = binate\n"
                                                                           "  p = positive unate\n"
                                                                           "  n = negative unate\n"
                                                                           "If no arg is given, prints to stdout, otherwise takes arg as filename" )
    ( "print",                                                             "Prints unateness matrix of current AIG without computing it" )
    ;
  be_verbose();
}

bool unate_command::execute()
{
  const auto settings = make_settings();
  settings->set( "progress", is_set( "progress" ) );
  settings->set( "skiplist", is_set( "skiplist" ) );

  if ( is_set( "print" ) )
  {
    if ( info().unateness.empty() )
    {
      std::cout << "[w] AIG has no unateness information" << std::endl;
    }
    else
    {
      print_unateness( std::cout, info().unateness, info() );
    }
    return true;
  }

  boost::dynamic_bitset<> u;

  switch ( approach )
  {
  case 0u:
    u = unateness_naive( aig(), settings, statistics );
    break;
  case 1u:
    u = unateness( aig(), settings, statistics );
    break;
  case 2u:
    u = unateness_split( aig(), settings, statistics );
    break;
  case 3u:
    u = unateness_split_parallel( aig(), settings, statistics );
    break;
  case 4u:
    u = unateness_split_inputs_parallel( aig(), settings, statistics );
    break;
  }

  info().unateness = u;

  if ( is_set( "matrix" ) )
  {
    std::streambuf * buf;
    std::ofstream of;

    if ( matrixname.empty() )
    {
      buf = std::cout.rdbuf();
    }
    else
    {
      of.open( matrixname.c_str(), std::ofstream::out );
      buf = of.rdbuf();
    }
    std::ostream os( buf );

    print_unateness( os, u, info() );
  }

  std::cout << boost::format( "[i] run-time (total): %.2f secs" ) % statistics->get<double>( "runtime" ) << std::endl
            << boost::format( "[i] run-time (wall): %.2f secs" ) % statistics->get<double>( "runtime_wall" ) << std::endl;

  if ( approach == 1u )
  {
    std::cout << boost::format( "[i] run-time (SAT):   %.2f secs" ) % statistics->get<double>( "sat_runtime" ) << std::endl;
  }

  return true;
}

command::log_opt_t unate_command::log() const
{
  if ( is_set( "print" ) )
  {
    return boost::none;
  }
  else
  {
    return log_opt_t({
        {"approach", approach},
        {"runtime", statistics->get<double>( "runtime" )},
        {"runtime_wall", statistics->get<double>( "runtime_wall" )}
      });
  }
}

}

// Local Variables:
// c-basic-offset: 2
// eval: (c-set-offset 'substatement-open 0)
// eval: (c-set-offset 'innamespace 0)
// End:
