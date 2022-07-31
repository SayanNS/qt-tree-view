//
// Created by sayan on 05.07.2022.
//

#include "Database.h"
#include <queue>


#define DEPTH 4
#define WIDTH 5

namespace Mikran {

Database::Database() : TreeStructure<DatabaseNode>()
{
	init();
}

void Database::init()
{
	QString string = "A";
	root = createNewNode();
	root->name = string;
	populate(root, string, 1);
}

void Database::reset()
{
	delete root;
	init();
}

void Database::populate(TreeNode *parent, QString &string, int current)
{
	if (current > DEPTH)
		return;

	for (int i = 0; i < WIDTH; i++) {
		string.resize(current + 1);
		string[current] = 'A' + i;
		Database::TreeNode *node = createNewNode();
		node->name = string;
		parent->append(node);
		populate(node, string, current + 1);
	}
}

Database::TreeNode *Database::appendNewNode(TreeNode *parent, DatabaseNode *node)
{
	onNewNodeAboutToBeInserted(parent, parent->getChildrenCount());
	TreeNode *newNode = createNewNode();
	newNode->name = node->name;
	newNode->deleted = node->deleted;
	parent->append(newNode);
	onNewNodeInserted();

	return newNode;
}

void Database::deleteNode(TreeNode *node)
{
	markNodeAndDescendantsAsDeleted(node);
}

void Database::changeNodeName(TreeNode *node, QString &name)
{
	node->name = name;
	onNodeDataChanged(node);
}

void Database::changeNodeNameAndDelete(TreeNode *node, QString &name)
{
	node->name = name;
	markNodeAndDescendantsAsDeleted(node);
}

void Database::markNodeAndDescendantsAsDeleted(TreeNode *node)
{
	if (node->deleted)
		return;

	node->deleted = true;
	onNodeDataChanged(node);

	std::queue<TreeNode *> bfs_traversal;
	bfs_traversal.push(node);

	while (!bfs_traversal.empty()) {
		for (TreeNode *child : bfs_traversal.front()->getChildrenList()) {

			if (child->deleted)
				continue;

			child->deleted = true;
			onNodeDataChanged(child);

			bfs_traversal.push(child);
		}
		bfs_traversal.pop();
	}
}

void Database::setOnNodeDataChangedHandler(onNodeDataChangedHandler &&handler)
{
	this->onNodeDataChanged = handler;
}

void Database::setOnNewNodeAboutToBeInsertedHandler(onNewNodeAboutToBeInsertedHandler &&handler)
{
	this->onNewNodeAboutToBeInserted = handler;
}

void Database::setOnNewNodeInsertedHandler(onNewNodeInsertedHandler &&handler)
{
	this->onNewNodeInserted = handler;
}



}
