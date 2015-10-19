//
//  FixedFramerate.cpp
//  ModelViewer
//
//  Created by yizhu on 15-2-13.
//  Copyright (c) 2015å¹?dion chen. All rights reserved.
//

#include "FixedFramerateController.h"

namespace bimWorld
{

	FixedFramerateController::FixedFramerateController(std::vector<NodeStat>& nodeStats)
	{
		setNodeStats(nodeStats);
	}
	void FixedFramerateController::setNodeStats(std::vector<NodeStat>& nodeStats)
	{
//		this->nodeStats.swap(nodeStats);
        this->nodeStats.clear();
        for (int i=0; i<nodeStats.size(); i++) {
            this->nodeStats.push_back(nodeStats[i]);
        }
        
		std::sort(nodeStats.begin(), nodeStats.end());
		splitIndex = 0;
		if (!this->nodeStats.empty())
		{
			triangeAccumulated.push_back(this->nodeStats[0].primitiveCount);
			for (size_t i = 1; i < this->nodeStats.size(); ++i)
			{
				triangeAccumulated.push_back(triangeAccumulated.back() + this->nodeStats[i].primitiveCount);
			}
			std::vector<unsigned int>::iterator pos =
				std::lower_bound(triangeAccumulated.begin(), triangeAccumulated.end(), triangeAccumulated.back() * 3 / 4);
			splitIndex = pos - triangeAccumulated.begin();
		}
	}
	bool FixedFramerateController::hide()
	{
		if (currentIndex >= splitIndex)
			return false;
		setMask(0, splitIndex);
		currentIndex = splitIndex;
		m_isHidden = true;
		/*setMask(0, nodeStats.size());
		std::vector<unsigned int> nums(nodeStats.size(), 0);
		for(size_t i = 0;i < nums.size();++ i)
		  nums[i] = i;
		int currentCount = 0;
		int index = 0;
		while(currentCount < triangeAccumulated.back() / 10){
		  int num = rand() % (nums.size() - index) + index;
		  std::swap(nums[index], nums[num]);
		  nodeStats[nums[index]].node->setNodeMask(1);
		  currentCount += nodeStats[nums[index]].primitiveCount;
		  ++ index;

		 }*/
		return true;
	}

	void FixedFramerateController::clearNodeStats()
	{
		this->nodeStats.clear();
	}

	void CollectNodeStats::apply(osg::Geode &geode)
	{
		int primtiveSetCount = 0;
		int primtiveCount = 0;
		for (unsigned int i = 0; i < geode.getNumDrawables(); ++i)
		{
			osg::Drawable *drawable = geode.getDrawable(i);
			//drawable->setUseVertexBufferObjects(true);

			osg::Geometry* geometry = drawable->asGeometry();
			if (geometry != NULL)
			{
				int geoPrimSetNum = geometry->getNumPrimitiveSets();
				for (int i = 0; i < geoPrimSetNum; ++i)
				{
					osg::PrimitiveSet* geoPrimSet = geometry->getPrimitiveSet(i);
					primtiveCount += geoPrimSet->getNumPrimitives();
				}
				primtiveSetCount += geoPrimSetNum;
			}

		}
		nodeStats.push_back(NodeStat(&geode, primtiveSetCount, primtiveCount));
	}

}
