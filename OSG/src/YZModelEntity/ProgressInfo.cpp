#include "ProgressInfo.h"
#include "YZModelEntityInternal.h"

long long bimWorld::ProgressInfo::count_size = 0;

bimWorld::ProgressInfo::ProgressInfo(YZModelEntityInternal* host) :YZModelEntityModule(host)
{

}

long long bimWorld::ProgressInfo::getCurrentProgressInfo()
{
	return count_size;
}

void bimWorld::ProgressInfo::resetProgress()
{
	count_size = 0;
}
