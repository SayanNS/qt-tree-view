//
// Created by sayan on 05.07.2022.
//

#include "Database.h"
#include <queue>

#define DEPTH 4
#define WIDTH 5

namespace Mikran {

DatabaseNode::DatabaseNode()
		: count(0)
		, deleted(false)
{
}

Database::Database() : TreeStructure<DatabaseNode>()
{
	init();
}

void Database::populate(Database::TreeNodeDescriptor t_parent, std::string &t_str, int t_current)
{
	if (t_current <= DEPTH) {
		for (int i = 0; i < WIDTH; i++) {
			t_str.resize(t_current + 1);
			t_str[t_current] = 'A' + i;
			Database::TreeNodeDescriptor node = createChild(t_parent);
			getData(t_parent).count++;
			DatabaseNode &node_data = getData(node);
			node_data.name = t_str;
			node_data.row = i;
			populate(node, t_str, t_current + 1);
		}
	}
}

void Database::init()
{
	std::string str = "A";
	Database::TreeNodeDescriptor node = createChild(nullptr);
	DatabaseNode &node_data = getData(node);
	node_data.name = str;
	node_data.row = 0;
	populate(node, str, 1);
}

void Database::reset()
{
	clear();
	init();
}

void Database::update(const DatabaseNode &t_data, Database::TreeNodeDescriptor t_node)
{
	DatabaseNode &data = getData(t_node);
	data.name = t_data.name;
	data.deleted = t_data.deleted;

	onUpdated(data.row, t_node);
}

Database::TreeNodeDescriptor Database::append(const DatabaseNode &t_data, Database::TreeNodeDescriptor t_parent)
{
	Database::TreeNodeDescriptor node = createChild(t_parent);
	DatabaseNode &parent_data = getData(t_parent);
	DatabaseNode &data = getData(node);
	data.name = t_data.name;
	data.deleted = t_data.deleted;
	data.row = parent_data.count;
	parent_data.count++;

	onAppended(parent_data.row, data.row, t_parent);

	return node;
}

void Database::setCallbacks(Database::appended_handler &&t_onAppended, Database::updated_handler &&t_onUpdated)
{
	onAppended = std::move(t_onAppended);
	onUpdated = std::move(t_onUpdated);
}

void Database::remove(Database::TreeNodeDescriptor t_node)
{
	DatabaseNode &data = getData(t_node);

	if (data.deleted)
		return;

	data.deleted = true;
	onUpdated(data.row, t_node);

	std::queue<Database::TreeNodeDescriptor> bfs_traversal;
	bfs_traversal.push(t_node);

	while (!bfs_traversal.empty()) {
		for (auto child : boost::make_iterator_range(getChildrenIterator(bfs_traversal.front()))) {
			DatabaseNode &child_data = getData(child);

			if (child_data.deleted)
				continue;

			child_data.deleted = true;
			onUpdated(child_data.row, child);

			bfs_traversal.push(child);
		}
		bfs_traversal.pop();
	}
}

}
