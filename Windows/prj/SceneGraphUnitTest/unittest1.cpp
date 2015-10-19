#include "stdafx.h"
#include "CppUnitTest.h"
#include "CategoryNode.h"
#include <map>
#include "test.h"
#include "CategoryQueryKey.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SceneGraphUnitTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			// TODO:  �ڴ�������Դ���
			//bimWorld::Test t;
			//t.call();
		}

		TEST_METHOD(TestCategoryNode)
		{
			// TODO:  �ڴ�������Դ���
			bimWorld::CategoryNode cateX("С��");
			bimWorld::CategoryNode cateD("����");
			bimWorld::CategoryNode floor("¥��");
			bimWorld::CategoryNode major("רҵ");
			cateD.addParent(major);
			cateD.addParent(floor);
			cateX.addParent(cateD);

			std::vector<bimWorld::MultiStringKey> keys;
			auto done = cateX.getCategoryKey(keys);
			Assert::IsTrue(done, L"��ȡ������б�ʧ��");
			auto result1 = bimWorld::MultiStringKey({ "רҵ", "����", "С��" });
			auto result2 = bimWorld::MultiStringKey({ "¥��", "����", "С��" });
			bool condition = (keys[0] == result1 && keys[1] == result2) ||
				(keys[0] == result2 && keys[1] == result1);
			Assert::IsTrue(keys.size() == 2 && condition, L"�������ȷ�����߼����жϷ�ʽ������");

			std::map<bimWorld::MultiStringKey, int> map;
			map[result1] = 1;
			map[result2] = 2;
		}

		TEST_METHOD(TestCategoryQueryKey)
		{
			// TODO:  �ڴ�������Դ���
			bimWorld::CategoryQueryKey key({ "floor", "category" }, { "2ndF", "pipeline" });
			auto k = key.getKeys();
			auto t = key.getTypes();
		}

	};
}