#include "CategoryNode.h"


bimWorld::CategoryNode::CategoryNode(const std::string& value)
{
	m_value = value;
}

std::string bimWorld::CategoryNode::Value() const
{
	return m_value;
}

void bimWorld::CategoryNode::addParent(CategoryNode& parent)
{
	_parents.push_back(parent);
}

void bimWorld::CategoryNode::addValue(const std::string& value)
{
	_categoryValues.push_back(value);
}

bool bimWorld::CategoryNode::hasParent() const
{
	return _parents.size() > 0;
}

const std::vector<bimWorld::CategoryNode> bimWorld::CategoryNode::Parents() const
{
	return _parents;
}

void traverse(const bimWorld::CategoryNode& node, std::vector<bimWorld::MultiStringKey>& keys, int current)
{
	auto& key = keys[current];
	key.insert(key.begin(), node.Value());
	if (node.hasParent())
	{
		bimWorld::MultiStringKey lastKey(key.size());
		std::copy(key.begin(), key.end(), lastKey.begin());
		for (int i = 0; i < node.Parents().size(); i++)
		{
			if (i == 0)
			{
				traverse(node.Parents()[i], keys, current);
			}
			else
			{
				bimWorld::MultiStringKey newKey(lastKey.size());
				std::copy(lastKey.begin(), lastKey.end(), newKey.begin());
				keys.push_back(newKey);
				traverse(node.Parents()[i], keys, current + i);
			}
		}
	}
}

bool bimWorld::CategoryNode::getCategoryKey(std::vector<bimWorld::MultiStringKey>& outKeys) const
{
	// traverse parents get key.
	bimWorld::MultiStringKey newKey({});
	outKeys.push_back(newKey);
	traverse(*this, outKeys, 0);

	return true;
}
