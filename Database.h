//
// Created by sayan on 05.07.2022.
//

#ifndef BOOSTGRAPHLIBRARY_DATABASE_H
#define BOOSTGRAPHLIBRARY_DATABASE_H

#include "TreeStructure.h"

namespace Mikran {

struct DatabaseNode
{
	DatabaseNode();

	std::string name;
	int row;
	int count;
	bool deleted;
};

class Database : public TreeStructure<DatabaseNode>
{
public:
	using appended_handler = std::function<void (int, int, Database::TreeNodeDescriptor)>;
	using updated_handler = std::function<void (int, Database::TreeNodeDescriptor)>;

	Database();

	void init();

	void reset();

	void setCallbacks(appended_handler &&t_onAppended, updated_handler &&t_onUpdated);

	void update(const DatabaseNode &t_data, Database::TreeNodeDescriptor t_node);

	void remove(Database::TreeNodeDescriptor t_node);

	Database::TreeNodeDescriptor append(const DatabaseNode &t_data, Database::TreeNodeDescriptor t_parent);

private:
	void populate(typename Database::TreeNodeDescriptor t_parent, std::string &t_str, int t_current);

	appended_handler onAppended;
	updated_handler onUpdated;
};

}

#endif //BOOSTGRAPHLIBRARY_DATABASE_H
