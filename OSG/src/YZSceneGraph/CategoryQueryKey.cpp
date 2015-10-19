#include "CategoryQueryKey.h"

bimWorld::CategoryQueryKey::CategoryQueryKey(const MultiStringKey& types, const MultiStringKey& keys) :
_types(types),
_keys(keys)
{
}

const bimWorld::MultiStringKey bimWorld::CategoryQueryKey::getTypes() const
{
	return _types;
}

const bimWorld::MultiStringKey bimWorld::CategoryQueryKey::getKeys() const
{
	return _keys;
}
