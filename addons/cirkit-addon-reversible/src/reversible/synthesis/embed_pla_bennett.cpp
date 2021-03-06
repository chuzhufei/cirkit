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

#include "embed_pla.hpp"
#include "synthesis_utils_p.hpp"

#include <core/io/read_pla_to_bdd.hpp>
#include <core/utils/timer.hpp>

#include <reversible/io/read_pla.hpp>

namespace cirkit
{

/******************************************************************************
 * Private functions                                                          *
 ******************************************************************************/

/******************************************************************************
 * Public functions                                                           *
 ******************************************************************************/

bool embed_pla_bennett( rcbdd& cf, const std::string& filename,
                        properties::ptr settings = properties::ptr(),
                        properties::ptr statistics = properties::ptr() )
{
  /* Settings */
  auto truth_table = get( settings, "truth_table", false         ); /* prints the truth table (for debugging) */
  auto write_pla   = get( settings, "write_pla",   std::string() );

  /* Timing */
  properties_timer t( statistics );

  unsigned n, m;
  std::tie( n, m ) = read_pla_size( filename );

  cf.initialize_manager();
  cf.create_variables( n + m );
  cf.set_num_inputs( n );
  cf.set_num_outputs( m );
  cf.set_constant_value( false );

  BDDTable table( cf.manager().getManager() );
  {
    properties_timer t( statistics, "runtime_read" );

    auto rp_settings = std::make_shared<properties>();
    rp_settings->set( "input_generation_func", generation_func_type( [&]( DdManager*, unsigned pos ) { return cf.x( m + pos ).getNode(); } ) );
    read_pla_to_bdd( table, filename, rp_settings );
  }

  BDD chi = cf.manager().bddOne();

  for ( unsigned i = 0u; i < m; ++i )
  {
    chi &= !cf.y( i ) ^ ( cf.x( i ) ^ BDD( cf.manager(), table.outputs[i].second ) );
  }

  for ( unsigned i = m; i < m + n; ++i )
  {
    chi &= !cf.x( i ) ^ cf.y( i );
  }

  cf.set_chi( chi );

  if ( truth_table )
  {
    cf.print_truth_table();
  }

  if ( write_pla.size() )
  {
    cf.write_pla( write_pla );
  }

  return true;
}

pla_embedding_func embed_pla_bennett_func( properties::ptr settings, properties::ptr statistics )
{
  pla_embedding_func f = [&settings, &statistics]( rcbdd& cf, const std::string& filename ) {
    return embed_pla_bennett( cf, filename, settings, statistics );
  };
  f.init( settings, statistics );
  return f;
}

}

// Local Variables:
// c-basic-offset: 2
// eval: (c-set-offset 'substatement-open 0)
// eval: (c-set-offset 'innamespace 0)
// End:
