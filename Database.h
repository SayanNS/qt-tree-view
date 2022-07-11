//
// Created by sayan on 05.07.2022.
//

#ifndef BOOSTGRAPHLIBRARY_DATABASE_H
#define BOOSTGRAPHLIBRARY_DATABASE_H

#include "TreeStructure.h"

struct database_node_data
{
	database_node_data();

	std::string name;
	bool deleted;
	int row;
	int count;
};

class Database : public TreeStructure<database_node_data>
{
public:
	Database();

	/*inline*/ void init();

	/*inline*/ void reset();

private:
	void populate(typename Database::tree_node_descriptor parent, std::string &str, int current);
};


#endif //BOOSTGRAPHLIBRARY_DATABASE_H
