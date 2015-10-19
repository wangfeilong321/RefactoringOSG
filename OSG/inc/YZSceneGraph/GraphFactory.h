#pragma once
#include "YZComponent.h"
#include <vector>
#include <string>
#include "CategoryNode.h"
#include "YZModelCoreInternal.h"
#include "CategoryQueryKey.h"
#include "config.h"

namespace bimWorld
{
	class YIZHU_EXPORT GraphFactory
	{
	public:
		GraphFactory(YZModelCoreInternal* core);
		~GraphFactory();

		//// <���÷���͸��ӹ�ϵ����>
		//// <������categories �������vector������˳��洢������ϵ�������vector�洢�����Ĺ�ϵ>
		//// <�磺 [ [¥�㣬���࣬С��], [רҵ] ] >
		//void setCategoryAndRelation(const std::vector<std::vector<std::string> >& categories);

#pragma region ��ʹ�����ַ�ʽ
/*
		// <���ݴ����������ṹ����������>
		void generateLevelOrderList(const std::vector<CategoryNode>& children);
		void constructTree(const std::vector<CategoryNode>& children);
		// <�����곡���������еĽڵ���䵽����>
		void fillTree();*/
#pragma endregion


		void constructTree(const std::vector<YZ::Component*>& constructs, const std::vector<YZ::Component*>& groups, const std::vector<CategoryNode>& children);

		//void constructTree(const std::vector<std::vector<std::string> >& categories, const std::vector<YZ::Component*>& constructs);

		osg::Group* select(const CategoryQueryKey& queryKey);

		void clear();

	protected:
		void AddGroups(CategoryNode child, YZ::Component* com, osg::Group* tree);
		
		YZModelCoreInternal* m_host;

		//std::vector<std::vector<std::string> > m_levelOrderSet;

		typedef std::map<MultiStringKey, osg::Group*> QueryGroupMap;
		// <�洢��ѯkey�ͳ������ڵ���ֵ�>
		//QueryGroupMap m_queryGroup;

		std::map<MultiStringKey, QueryGroupMap> m_queryMap;


		//std::map<std::string, osg::Group*> m_treeCategoryGroup;
		//void fillTree(osg::Group* tree, const std::map<std::string, bimWorld::YZString>& construct);

		//void fillTree(const std::vector<YZ::Component*>& constructs);
	};

}
