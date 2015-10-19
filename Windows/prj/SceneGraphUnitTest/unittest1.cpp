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
			// TODO:  在此输入测试代码
			//bimWorld::Test t;
			//t.call();
		}

		TEST_METHOD(TestCategoryNode)
		{
			// TODO:  在此输入测试代码
			bimWorld::CategoryNode cateX("小类");
			bimWorld::CategoryNode cateD("大类");
			bimWorld::CategoryNode floor("楼层");
			bimWorld::CategoryNode major("专业");
			cateD.addParent(major);
			cateD.addParent(floor);
			cateX.addParent(cateD);

			std::vector<bimWorld::MultiStringKey> keys;
			auto done = cateX.getCategoryKey(keys);
			Assert::IsTrue(done, L"获取分类键列表失败");
			auto result1 = bimWorld::MultiStringKey({ "专业", "大类", "小类" });
			auto result2 = bimWorld::MultiStringKey({ "楼层", "大类", "小类" });
			bool condition = (keys[0] == result1 && keys[1] == result2) ||
				(keys[0] == result2 && keys[1] == result1);
			Assert::IsTrue(keys.size() == 2 && condition, L"结果不正确，或者键的判断方式有问题");

			std::map<bimWorld::MultiStringKey, int> map;
			map[result1] = 1;
			map[result2] = 2;
		}

		TEST_METHOD(TestCategoryQueryKey)
		{
			// TODO:  在此输入测试代码
			bimWorld::CategoryQueryKey key({ "floor", "category" }, { "2ndF", "pipeline" });
			auto k = key.getKeys();
			auto t = key.getTypes();
		}

	};
}