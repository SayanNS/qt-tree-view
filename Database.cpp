//
// Created by sayan on 05.07.2022.
//

#include "Database.h"

#define DEPTH 4
#define WIDTH 5


Database::Database() : TreeStructure<database_node_data>()
{
	init();
}

void Database::populate(Database::tree_node_descriptor parent, std::string &str, int current)
{
	if (current <= DEPTH) {
		for (int i = 0; i < WIDTH; i++) {
			str.resize(current + 1);
			str[current] = 'A' + i;
			Database::tree_node_descriptor node = createChildNode(parent);
			getNodeData(parent).count++;
			database_node_data &node_data = getNodeData(node);
			node_data.name = str;
			node_data.row = i;
			populate(node, str, current + 1);
		}
	}
}

void Database::init()
{
	std::string str = "A";
	Database::tree_node_descriptor node = createChildNode(nullptr);
	database_node_data &node_data = getNodeData(node);
	node_data.name = str;
	node_data.row = 0;
	populate(node, str, 1);
}

database_node_data::database_node_data() : deleted(false), count(0)
{
}

void Database::reset()
{
	this->clear();
	this->init();
}
