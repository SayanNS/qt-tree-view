//
// Created by sayan on 05.07.2022.
//

#ifndef BOOSTGRAPHLIBRARY_TREESTRUCTURE_H
#define BOOSTGRAPHLIBRARY_TREESTRUCTURE_H

#include <boost/graph/adjacency_list.hpp>

template<typename T>
class TreeStructure
{
public:
	using graph_type = boost::adjacency_list<boost::listS, boost::listS, boost::bidirectionalS, T>;
	using TreeNodeDescriptor = typename boost::graph_traits<graph_type>::vertex_descriptor;

	TreeStructure() : m_graph(), m_root()
	{
	}

	TreeNodeDescriptor getRoot() const
	{
		return m_root;
	};

	TreeNodeDescriptor getParent(TreeNodeDescriptor t_child)
	{
		return boost::source(*(boost::in_edges(t_child, m_graph).first), m_graph);
	}

	TreeNodeDescriptor createChild(TreeNodeDescriptor t_parent)
	{
		if (t_parent == nullptr) {
			m_root = boost::add_vertex(m_graph);
			return m_root;
		} else {
			TreeNodeDescriptor node = boost::add_vertex(m_graph);
			boost::add_edge(t_parent, node, m_graph);
			return node;
		}
	}

	std::pair<typename boost::graph_traits<graph_type>::adjacency_iterator,
					 typename boost::graph_traits<graph_type>::adjacency_iterator> getChildrenIterator(
			TreeNodeDescriptor t_parent) const
	{
		if (t_parent == nullptr)
			return boost::adjacent_vertices(m_root, m_graph);

		return boost::adjacent_vertices(t_parent, m_graph);
	}

	T & getData(TreeNodeDescriptor t_node)
	{
		return m_graph[t_node];
	};

	void move(TreeNodeDescriptor parent, TreeNodeDescriptor child)
	{
		boost::remove_edge(boost::edge(m_root, child, m_graph).first, m_graph);
		boost::add_edge(parent, child, m_graph);
	}

	void clear()
	{
		m_graph.clear();
	}

//	void removeNode(TreeNodeDescriptor node)
//	{
//		boost::clear_in_edges(node, m_graph);
//		auto edges_it_pair = out_edges(node, m_graph);
//		for (auto edges_it = edges_it_pair.first; edges_it != edges_it_pair.second;) {
//			auto node_p = edges_it++;
//			removeNode(boost::target(*node_p, m_graph));
//		}
//		boost::clear_out_edges(node, m_graph);
//		boost::remove_vertex(node, m_graph);
//
//		if (node == m_root) m_root = nullptr;
//	}

protected:
	TreeNodeDescriptor m_root;

private:
	graph_type m_graph;
};


#endif //BOOSTGRAPHLIBRARY_TREESTRUCTURE_H
