//
// Created by sayan on 07.07.2022.
//

#ifndef BOOSTGRAPHLIBRARY_CACHE_H
#define BOOSTGRAPHLIBRARY_CACHE_H

#include "TreeStructure.h"
#include "Database.h"

namespace Mikran {

enum class State
{
	NOT_CHANGED,
	CREATED,
	DELETED,
	CHANGED
};

struct CacheNode : public DatabaseNode
{
	CacheNode();

	Database::TreeNodeDescriptor db_node;
	State state;
};

class Cache : public TreeStructure<CacheNode>
{
public:
	Cache(Database *t_database);

	Cache::TreeNodeDescriptor fetchFromDatabase(Cache::TreeNodeDescriptor t_parent,
			Database::TreeNodeDescriptor t_db_node);

	Cache::TreeNodeDescriptor findParent(Database::TreeNodeDescriptor t_database_node);

	bool isParent(Cache::TreeNodeDescriptor t_parent, Cache::TreeNodeDescriptor t_child);

	void flush();

	void reset();

private:
	Database *m_database;
};

}

#endif //BOOSTGRAPHLIBRARY_CACHE_H
