//
// Created by sayan on 05.07.2022.
//

#ifndef BOOSTGRAPHLIBRARY_TREESTRUCTURE_H
#define BOOSTGRAPHLIBRARY_TREESTRUCTURE_H

#include <boost/graph/adjacency_list.hpp>

template<typename T>
class TreeStructure : public boost::adjacency_list<boost::listS, boost::listS, boost::bidirectionalS, T>
{
public:
	typedef typename boost::graph_traits<TreeStructure<T>>::vertex_descriptor tree_node_descriptor;

	TreeStructure();

	inline tree_node_descriptor getRootNode() const
	{
		return root;
	};

	tree_node_descriptor getParentNode(tree_node_descriptor child);

	tree_node_descriptor createChildNode(tree_node_descriptor parent);

	inline std::pair<typename boost::graph_traits<TreeStructure<T>>::adjacency_iterator,
					 typename boost::graph_traits<TreeStructure<T>>::adjacency_iterator> getNodeChildrenIterator(
			tree_node_descriptor parent) const;

	inline T & getNodeData(tree_node_descriptor node)
	{
		return this->operator[](node);
	};

	void removeNode(tree_node_descriptor node);

protected:
	tree_node_descriptor root;
};

template<typename T> TreeStructure<T>::TreeStructure()
		: boost::adjacency_list<boost::listS, boost::listS, boost::bidirectionalS, T>(), root(nullptr)
{
}

template<typename T> typename TreeStructure<T>::tree_node_descriptor TreeStructure<T>::getParentNode(tree_node_descriptor child)
{
	return boost::source(*(boost::in_edges(child, *this).first), *this);
}

template<typename T> typename TreeStructure<T>::tree_node_descriptor TreeStructure<T>::createChildNode(tree_node_descriptor parent)
{
	if (parent == nullptr) {
		root = boost::add_vertex(*this);
		return root;
	} else {
		tree_node_descriptor node = boost::add_vertex(*this);
		boost::add_edge(parent, node, *this);
		return node;
	}
}

template<typename T> void TreeStructure<T>::removeNode(tree_node_descriptor node)
{
	boost::clear_in_edges(node, *this);
	auto edges_it_pair = out_edges(node, *this);
	for (auto edges_it = edges_it_pair.first; edges_it != edges_it_pair.second;) {
		auto node_p = edges_it++;
		removeNode(boost::target(*node_p, *this));
	}
	boost::clear_out_edges(node, *this);
	boost::remove_vertex(node, *this);

//	if (node == root) root = nullptr;
}

template<typename T> inline std::pair<typename boost::graph_traits<TreeStructure<T>>::adjacency_iterator,
		  typename boost::graph_traits<TreeStructure<T>>::adjacency_iterator>
TreeStructure<T>::getNodeChildrenIterator(tree_node_descriptor parent) const
{
	if (parent == nullptr)
		return boost::adjacent_vertices(root, *this);

	return boost::adjacent_vertices(parent, *this);
}


#endif //BOOSTGRAPHLIBRARY_TREESTRUCTURE_H
