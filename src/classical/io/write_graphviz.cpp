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

#if ADDON_GRAPHVIZ

#include <classical/io/write_graphviz.hpp>
#include <boost/format.hpp>

namespace cirkit
{

void compute_graphviz_layout( gv_graph& gv, const aig_graph& aig, const std::string& layout_algorithm, const std::string& render_format,
                              std::map< aig_node, gv_node >& node_map, std::map< aig_edge, gv_edge >& edge_map )
{

  /* nodes */
  for ( const auto& node : boost::make_iterator_range( vertices( aig ) ) )
  {
    const std::string name = ( boost::format("%s") % aig_to_literal(aig,node) ).str();
    node_map.insert( {node, gv_add_node(gv, name)} );
  }

  /* edges */
  for ( const auto& node : boost::make_iterator_range( vertices( aig ) ) )
  {
    const auto &src = node_map.find( node );
    if ( src == node_map.end() ) throw "Source of edge undefined";

    for ( const auto& edge : boost::make_iterator_range( out_edges( node, aig ) ) )
    {
      const auto &dst = node_map.find( target( edge, aig ) );
      if ( dst == node_map.end() ) throw "Target of edge undefined";

      edge_map.insert( {edge,gv_add_edge( gv, src->second, dst->second )} );
    }
  }

  gv_layout( gv, layout_algorithm );
}

void write_graphviz( const aig_graph& aig, const std::string& layout_algorithm, const std::string& render_format, std::ostream& os )
{
  std::map< aig_node, gv_node > node_map;
  std::map< aig_edge, gv_edge > edge_map;
  write_graphviz( aig, layout_algorithm, render_format, node_map, edge_map, os );
}

void write_graphviz( const aig_graph& aig, const std::string& layout_algorithm, const std::string& render_format,
                     std::map< aig_node, gv_node >& node_map, std::map< aig_edge, gv_edge >& edge_map, std::ostream& os )
{
  assert( num_vertices( aig ) != 0u && "Uninitialized AIG" );

  gv_graph gv;
  gv_initialize( gv, "AIG" );

  compute_graphviz_layout( gv, aig, layout_algorithm, render_format, node_map, edge_map );
  gv_render( gv, render_format, os );
}

}

#endif

// Local Variables:
// c-basic-offset: 2
// eval: (c-set-offset 'substatement-open 0)
// eval: (c-set-offset 'innamespace 0)
// End:
