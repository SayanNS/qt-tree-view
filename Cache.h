//
// Created by sayan on 07.07.2022.
//

#ifndef BOOSTGRAPHLIBRARY_CACHE_H
#define BOOSTGRAPHLIBRARY_CACHE_H

#include "TreeStructure.h"
#include "Database.h"

struct cache_node_data : public database_node_data
{
	cache_node_data();

	Database::tree_node_descriptor database_node;
	bool modified;
};

class Cache : public TreeStructure<cache_node_data>
{
public:
	Cache(Database &database);

	Cache::tree_node_descriptor findParent(Database::tree_node_descriptor database_node);

	bool isParent(Cache::tree_node_descriptor parent, Cache::tree_node_descriptor child);

	void moveNode(Cache::tree_node_descriptor parent, Cache::tree_node_descriptor child);

	Cache::tree_node_descriptor fetchFromDatabase(Cache::tree_node_descriptor parent,
			Database::tree_node_descriptor node);

	void reset();

private:
	Database &database;
};



#endif //BOOSTGRAPHLIBRARY_CACHE_H
