//
// Created by sayan on 07.07.2022.
//

#ifndef MIKRAN_CACHE_H
#define MIKRAN_CACHE_H


#include "TreeStructure.h"
#include "Database.h"

namespace Mikran {

enum NodeState
{
	NOT_CHANGED = 0,
	DELETED = 1,
	MODIFIED = 2,
	MODIFIED_AND_DELETED = DELETED | MODIFIED
};

struct CacheNode : DatabaseNode
{
	CacheNode() = default;

	Database::TreeNode *databaseLinkNode;
	NodeState state;
};

class Cache : public TreeStructure<CacheNode>
{
public:
	Cache(Database *t_database);

	using onNewNodeAboutToBeInsertedHandler = std::function<void (TreeNode *, int)>;
	using onNewNodeInsertedHandler = std::function<void ()>;

	using onNodeAboutToBeRemovedHandler = std::function<void (TreeNode *, int)>;
	using onNodeRemovedHandler = std::function<void ()>;

	using onNodeDataChangedHandler = std::function<void (TreeNode *)>;

	void loadFromDatabase(Database::TreeNode *databaseNode);

	bool isAncestor(TreeNode *node, TreeNode *ancestor);

	void createAndAppendNewNode(TreeNode *parent, QString &name);

	void flush();
	void reset();

	void markNodeAndDescendantsAsDeleted(TreeNode *node);

	void setOnNewNodeAboutToBeInsertedHandler(onNewNodeAboutToBeInsertedHandler &&handler);
	void setOnNewNodeInsertedHandler(onNewNodeInsertedHandler &&handler);

	void setOnNodeAboutToBeRemovedHandler(onNodeAboutToBeRemovedHandler &&handler);
	void setOnNodeRemovedHandler(onNodeRemovedHandler &&handler);

	void setOnNodeDataChangedHandler(onNodeDataChangedHandler &&handler);

private:
	TreeNode *findClosestDatabaseLinkNode(Database::TreeNode *databaseNode);

	Database *database;

	onNewNodeAboutToBeInsertedHandler onNewNodeAboutToBeInserted;
	onNewNodeInsertedHandler onNewNodeInserted;

	onNodeAboutToBeRemovedHandler onNodeAboutToBeRemoved;
	onNodeRemovedHandler onNodeRemoved;

	onNodeDataChangedHandler onNodeDataChanged;
};

}

#endif //MIKRAN_CACHE_H
