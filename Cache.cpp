//
// Created by sayan on 07.07.2022.
//

#include "Cache.h"
#include "Database.h"
#include <queue>
//#include <boost/graph/breadth_first_search.hpp>


//struct bfs_visitor_duplicate_exception
//{
//};
//
//struct bfs_visitor_parent_found_exception
//{
//	bfs_visitor_parent_found_exception(Cache::tree_node_descriptor node) : node(node) {}
//
//	Cache::tree_node_descriptor node;
//};
//
//class bfs_visitor : public boost::default_bfs_visitor
//{
//public:
//	bfs_visitor(Database &database, Database::tree_node_descriptor node)
//		: database(database)
//		, node(node)
//	{
//	}
//
//	template <typename Vertex, typename Graph>
//	void discover_vertex(Vertex u, const Graph &g) const
//	{
//		const Cache &cache = reinterpret_cast<const Cache &>(g);
//		Cache::tree_node_descriptor vertex = reinterpret_cast<Cache::tree_node_descriptor>(u);
//		cache_node_data &data = cache.getNodeData(vertex);
//
//		if (data.database_node == node)
//			throw bfs_visitor_duplicate_exception();
//
//		if (node != database.getRootNode()
//			&& data.database_node == database.getParentNode(node))
//			throw bfs_visitor_parent_found_exception(vertex);
//	}
//
//private:
//	Database &database;
//	Database::tree_node_descriptor node;
//};

cache_node_data::cache_node_data() : database_node_data(), database_node(nullptr), modified(false)
{
}

Cache::Cache(Database &database) : TreeStructure<cache_node_data>(), database(database)
{
	createChildNode(nullptr);
}

Cache::tree_node_descriptor Cache::fetchFromDatabase(Cache::tree_node_descriptor parent, Database::tree_node_descriptor node)
{
	Cache::tree_node_descriptor new_node = createChildNode(parent);
	int row = getNodeData(parent).count++;
	cache_node_data &node_data = getNodeData(new_node);
	node_data.name = database.getNodeData(node).name;
	node_data.database_node = node;
	node_data.row = row;

	return new_node;
}

Cache::tree_node_descriptor Cache::findParent(Database::tree_node_descriptor database_node)
{
//	std::map<Cache::tree_node_descriptor, size_t> i_map;
//	for (auto v : boost::make_iterator_range(vertices(*this))) {
//		i_map.emplace(v, i_map.size());
//	}
//
//	auto ipmap = boost::make_assoc_property_map(i_map);
//
//	std::vector<boost::default_color_type> c_map(num_vertices(*this));
//	auto cpmap = boost::make_iterator_property_map(c_map.begin(), ipmap);
//
//	bfs_visitor vis(database, database_node);
//
//	try {
//		boost::breadth_first_search(*this, root, boost::visitor(vis).vertex_index_map(ipmap).color_map(cpmap));
//	} catch(bfs_visitor_duplicate_exception &exc) {
//		return true;
//	} catch(bfs_visitor_parent_found_exception &exc) {
//		cache_node = exc.node;
//		return false;
//	}

	if (database_node == database.getRootNode()) {
		for (auto child : boost::make_iterator_range(getNodeChildrenIterator(root))) {
			if (getNodeData(child).database_node == database_node)
				return nullptr;
		}
	} else {
		std::list<Cache::tree_node_descriptor> family_list;

		for (Database::tree_node_descriptor current = database_node; current != database.getRootNode();) {
			family_list.push_front(current);
			current = database.getParentNode(current);
		}
		family_list.push_front(database.getRootNode());

		for (auto child : boost::make_iterator_range(getNodeChildrenIterator(root))) {
			auto it = family_list.begin();

			for (; it != family_list.end(); it++) {
				cache_node_data &data = getNodeData(child);
				if (data.database_node == nullptr)
					break;
				if (data.database_node == *it) {
					Cache::tree_node_descriptor current = child;
					for (it++; it != family_list.end(); it++) {
						bool failed = true;
						for (auto current_child : boost::make_iterator_range(getNodeChildrenIterator(current))) {
							if (getNodeData(current_child).database_node == *it) {
								current = current_child;
								failed = false;
								break;
							}
						}
						if (failed) {
							break;
						}
					}
					if (it == family_list.end()) {
						return nullptr;
					}
					if (next(it) == family_list.end()) {
						return current;
					}
				}
			}
		}
	}
	return root;
}

bool Cache::isParent(Cache::tree_node_descriptor parent, Cache::tree_node_descriptor child)
{
	cache_node_data child_data = getNodeData(child);
	cache_node_data parent_data = getNodeData(parent);

	if (child_data.database_node == database.getRootNode())
		return false;

	if (parent_data.database_node == database.getParentNode(child_data.database_node))
		return true;

	return false;
}

void Cache::moveNode(Cache::tree_node_descriptor parent, Cache::tree_node_descriptor child)
{
	boost::remove_edge(boost::edge(root, child, *this).first, *this);
	getNodeData(root).count--;
	boost::add_edge(parent, child, *this);
	getNodeData(child).row = getNodeData(parent).count++;
}

void Cache::reset()
{
	this->clear();
	createChildNode(nullptr);
}
