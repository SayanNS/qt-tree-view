//
// Created by sayan on 05.07.2022.
//

#ifndef MIKRAN_DATABASE_H
#define MIKRAN_DATABASE_H

#include "TreeStructure.h"

namespace Mikran {

struct DatabaseNode
{
	DatabaseNode() = default;

	QString name;
	bool deleted;
};

class Database : public TreeStructure<DatabaseNode>
{
public:
	using onNewNodeAboutToBeInsertedHandler = std::function<void (TreeNode *, int)>;
	using onNewNodeInsertedHandler = std::function<void ()>;

	using onNodeDataChangedHandler = std::function<void (TreeNode *)>;

	Database();

	void reset();

	void changeNodeNameAndDelete(TreeNode *node, QString &name);
	void changeNodeName(TreeNode *node, QString &name);
	void deleteNode(TreeNode *node);

	TreeNode *appendNewNode(TreeNode *parent, DatabaseNode *node);

	void setOnNewNodeAboutToBeInsertedHandler(onNewNodeAboutToBeInsertedHandler &&handler);
	void setOnNewNodeInsertedHandler(onNewNodeInsertedHandler &&handler);

	void setOnNodeDataChangedHandler(onNodeDataChangedHandler &&handler);

private:
	void markNodeAndDescendantsAsDeleted(TreeNode *node);

	void init();

	void populate(TreeNode *parent, QString &str, int current);

	onNewNodeAboutToBeInsertedHandler onNewNodeAboutToBeInserted;
	onNewNodeInsertedHandler onNewNodeInserted;

	onNodeDataChangedHandler onNodeDataChanged;

};

}

#endif //MIKRAN_DATABASE_H
