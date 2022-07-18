//
// Created by sayan on 07.07.2022.
//

#include "Cache.h"
#include <queue>

namespace Mikran {

CacheNode::CacheNode()
		: DatabaseNode()
		, state(State::NOT_CHANGED)
{
}

Cache::Cache(Database *t_database)
	: TreeStructure<CacheNode>()
	, m_database(t_database)
{
	createChild(nullptr);
}

Cache::TreeNodeDescriptor
Cache::fetchFromDatabase(Cache::TreeNodeDescriptor t_parent, Database::TreeNodeDescriptor t_db_node)
{
	Cache::TreeNodeDescriptor new_node = createChild(t_parent);
	int row = getData(t_parent).count++;
	CacheNode &node_data = getData(new_node);
	node_data.name = m_database->getData(t_db_node).name;
	node_data.db_node = t_db_node;
	node_data.row = row;

	return new_node;
}

Cache::TreeNodeDescriptor Cache::findParent(Database::TreeNodeDescriptor t_db_node)
{
	if (t_db_node == m_database->getRoot()) {
		for (auto child : boost::make_iterator_range(getChildrenIterator(m_root))) {
			if (getData(child).db_node == t_db_node)
				return nullptr;
		}
	} else {
		std::queue<Cache::TreeNodeDescriptor> bfs_traversal;
		Database::TreeNodeDescriptor db_node_parent = m_database->getParent(t_db_node);
		bfs_traversal.push(m_root);

		while (!bfs_traversal.empty()) {
			Cache::TreeNodeDescriptor current = bfs_traversal.front();

			for (auto child : boost::make_iterator_range(getChildrenIterator(current))) {
				if (getData(child).db_node == t_db_node)
					return nullptr;

				if (getData(child).db_node == db_node_parent) {
					for (auto child_child : boost::make_iterator_range(getChildrenIterator(child))) {
						if (getData(child_child).db_node == t_db_node) {
							return nullptr;
						}
					}
					return child;
				}
				bfs_traversal.push(child);
			}
			bfs_traversal.pop();
		}
	}
	return m_root;
}

bool Cache::isParent(Cache::TreeNodeDescriptor t_parent, Cache::TreeNodeDescriptor t_child)
{
	CacheNode child_data = getData(t_child);
	CacheNode parent_data = getData(t_parent);

	if (child_data.db_node == m_database->getRoot())
		return false;

	if (parent_data.db_node == m_database->getParent(child_data.db_node))
		return true;

	return false;
}

void Cache::reset()
{
	clear();
	createChild(nullptr);
}

void Cache::flush()
{
	std::queue<Cache::TreeNodeDescriptor> bfs_traversal;
	bfs_traversal.push(m_root);

	while (!bfs_traversal.empty()) {
		Cache::TreeNodeDescriptor current = bfs_traversal.front();

		for (auto child : boost::make_iterator_range(getChildrenIterator(current))) {
			CacheNode &data = getData(child);

			if (data.state == State::CREATED) {
				data.db_node = m_database->append(data, getData(getParent(child)).db_node);
				data.state = State::NOT_CHANGED;
			} else if (data.state == State::CHANGED) {
				m_database->update(data, data.db_node);
				data.state = State::NOT_CHANGED;
			} else if (data.state == State::DELETED) {
				m_database->remove(data.db_node);
				data.state = State::NOT_CHANGED;
				continue;
			}
			bfs_traversal.push(child);
		}
		bfs_traversal.pop();
	}
}

}