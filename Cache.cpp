//
// Created by sayan on 07.07.2022.
//

#include "Cache.h"
#include <queue>
#include <stack>


namespace Mikran {

Cache::Cache(Database *database)
	: TreeStructure<CacheNode>()
	, database(database)
{
	root = createNewNode();
}

void Cache::loadFromDatabase(Database::TreeNode *databaseNode)
{
	TreeNode *closestLinkNode = findClosestDatabaseLinkNode(databaseNode);

	if (closestLinkNode->databaseLinkNode == databaseNode)
		return;

	TreeNode *newNode = createNewNode();
	newNode->databaseLinkNode = databaseNode;
	newNode->name = databaseNode->name;
	newNode->deleted = closestLinkNode->deleted;

	QList<TreeNode *> const &childrenList = closestLinkNode->getChildrenList();
	int removeChildrenIndex = 0;

	for (auto childrenIterator = childrenList.begin(); childrenIterator != childrenList.end();) {
		TreeNode *child = childrenIterator.operator*();
		if (isAncestor(child, newNode)) {
			childrenIterator++;
			onNodeAboutToBeRemoved(closestLinkNode, removeChildrenIndex);
			closestLinkNode->remove(child);
			onNodeRemoved();
			newNode->append(child);
		} else {
			removeChildrenIndex++;
			childrenIterator++;
		}
	}

	onNewNodeAboutToBeInserted(closestLinkNode, closestLinkNode->getChildrenCount());
	closestLinkNode->append(newNode);
	onNewNodeInserted();
}

Cache::TreeNode *Cache::findClosestDatabaseLinkNode(Database::TreeNode *databaseNode)
{
	Cache::TreeNode *closestLinkNode = root;
	Database::TreeNode *databaseRoot = database->getRoot();
	std::list<Database::TreeNode *> databaseNodeFamily;

	for (Database::TreeNode *current = databaseNode; current != databaseRoot;) {
		databaseNodeFamily.push_front(current);
		current = current->getParent();
	}
	databaseNodeFamily.push_front(databaseRoot);

	auto databaseNodeFamilyIteratorOffset = databaseNodeFamily.begin();
loop:
	for (TreeNode *child : closestLinkNode->getChildrenList()) {
		for (auto databaseNodeFamilyIterator = databaseNodeFamilyIteratorOffset;
			 databaseNodeFamilyIterator != databaseNodeFamily.end(); databaseNodeFamilyIterator++) {
			if (child->databaseLinkNode == databaseNodeFamilyIterator.operator*()) {
				closestLinkNode = child;
				databaseNodeFamilyIteratorOffset = databaseNodeFamilyIterator;
				goto loop;
			}
		}
	}

	return closestLinkNode;
}

bool Cache::isAncestor(TreeNode *node, TreeNode *ancestor)
{
	Database::TreeNode *currentDatabaseLinkNode = node->databaseLinkNode;
	Database::TreeNode *ancestorDatabaseLinkNode = ancestor->databaseLinkNode;

	if (node->getParent() == root) {
		for (Database::TreeNode *current = currentDatabaseLinkNode; current != database->getRoot();) {
			if (current == ancestorDatabaseLinkNode)
				return true;
			current = current->getParent();
		}

		if (ancestorDatabaseLinkNode == database->getRoot())
			return true;

		return false;
	}

	Database::TreeNode *currentParentDatabaseLinkNode = node->getParent()->databaseLinkNode;

	for (Database::TreeNode *current = currentDatabaseLinkNode; current != currentParentDatabaseLinkNode;) {
		if (current == ancestorDatabaseLinkNode)
			return true;
		current = current->getParent();
	}

	return false;
}

void Cache::markNodeAndDescendantsAsDeleted(TreeNode *node)
{
	if (node->deleted)
		return;

	node->deleted = true;
	node->state = static_cast<NodeState>(node->state | DELETED);
	onNodeDataChanged(node);

	std::queue<Cache::TreeNode *> bfs_traversal;
	bfs_traversal.push(node);

	while (!bfs_traversal.empty()) {
		for (Cache::TreeNode *child : bfs_traversal.front()->getChildrenList()) {

			if (child->deleted)
				continue;

			child->deleted = true;
			onNodeDataChanged(child);

			bfs_traversal.push(child);
		}
		bfs_traversal.pop();
	}
}

void Cache::createAndAppendNewNode(TreeNode *parent, QString &name)
{
	onNewNodeAboutToBeInserted(parent, parent->getChildrenCount());
	TreeNode *newNode = createNewNode();
	newNode->name = std::move(name);
	parent->append(newNode);
	onNewNodeInserted();
}

void Cache::flush()
{
	struct depth_first_traversal_state
	{
		depth_first_traversal_state(Cache::TreeNode *node)
			: node(node)
			, iterator(node->getChildrenList().begin())
		{
		}

		Cache::TreeNode *node;
		QList<TreeNode *>::const_iterator iterator;
	};

	std::stack<depth_first_traversal_state> dfs_traversal;
	for (TreeNode *child : root->getChildrenList()) {
		dfs_traversal.push(child);
	}

	while (!dfs_traversal.empty()) {
		depth_first_traversal_state &current_state = dfs_traversal.top();

		if (current_state.node->databaseLinkNode == nullptr) {
			std::queue<TreeNode *> bfs_traversal;
			bfs_traversal.push(current_state.node);

			current_state.node->databaseLinkNode = database->appendNewNode(
					current_state.node->getParent()->databaseLinkNode, current_state.node);
			current_state.node->state = NodeState::NOT_CHANGED;

			while (!bfs_traversal.empty()) {
				for (TreeNode *child : bfs_traversal.front()->getChildrenList()) {
					child->databaseLinkNode = database->appendNewNode( child->getParent()->databaseLinkNode, child);
					child->state = NodeState::NOT_CHANGED;
					bfs_traversal.push(child);
				}
				bfs_traversal.pop();
			}

			dfs_traversal.pop();
			continue;
		}

		if (current_state.iterator != current_state.node->getChildrenList().end()) {
			dfs_traversal.push(current_state.iterator.operator*());
			current_state.iterator++;
			continue;
		}

		switch (current_state.node->state) {
		case NodeState::DELETED:
			database->deleteNode(current_state.node->databaseLinkNode);
			break;
		case NodeState::MODIFIED:
			database->changeNodeName(current_state.node->databaseLinkNode, current_state.node->name);
			break;
		case NodeState::MODIFIED_AND_DELETED:
			database->changeNodeNameAndDelete(current_state.node->databaseLinkNode, current_state.node->name);
			break;
		case NodeState::NOT_CHANGED:
			break;
		}
		current_state.node->state = NodeState::NOT_CHANGED;
		dfs_traversal.pop();
	}
}

void Cache::reset()
{
	delete root;
	root = createNewNode();
}

void Cache::setOnNewNodeAboutToBeInsertedHandler(onNewNodeAboutToBeInsertedHandler &&handler)
{
	this->onNewNodeAboutToBeInserted = handler;
}

void Cache::setOnNewNodeInsertedHandler(onNewNodeInsertedHandler &&handler)
{
	this->onNewNodeInserted = handler;
}

void Cache::setOnNodeAboutToBeRemovedHandler(onNodeAboutToBeRemovedHandler &&handler)
{
	this->onNodeAboutToBeRemoved = handler;
}

void Cache::setOnNodeRemovedHandler(onNodeRemovedHandler &&handler)
{
	this->onNodeRemoved = handler;
}

void Cache::setOnNodeDataChangedHandler(Cache::onNodeDataChangedHandler &&handler)
{
	this->onNodeDataChanged = handler;
}

}