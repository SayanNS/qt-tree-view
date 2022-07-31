//
// Created by sayan on 05.07.2022.
//

#ifndef MIKRAN_TREESTRUCTURE_H
#define MIKRAN_TREESTRUCTURE_H


#include <QList>

template <typename T>
class TreeStructure
{
public:
	class TreeNode : public T
	{
	public:
		TreeNode *getParent() const
		{
			return parent;
		}

		TreeNode *getChildrenAtIndex(int index) const
		{
			return children.at(index);
		}

		int getIndex()
		{
			return parent->children.indexOf(this);
		}

		int getChildrenCount() const
		{
			return children.count();
		}

		QList<TreeNode *> const &getChildrenList() const
		{
			return children;
		}

		void append(TreeNode *node)
		{
			node->parent = this;
			children.append(node);
		}

		void remove(TreeNode *node)
		{
			children.removeOne(node);
		}

		~TreeNode()
		{
			qDeleteAll(children);
		}

	private:
		friend class TreeStructure<T>;

		TreeNode() = default;


		QList<TreeNode *> children;
		TreeNode *parent;
	};

	TreeNode *getRoot() const
	{
		return root;
	}

protected:
	TreeNode *createNewNode()
	{
		return new TreeNode();
	}

	TreeNode *root;
};


#endif //MIKRAN_TREESTRUCTURE_H
