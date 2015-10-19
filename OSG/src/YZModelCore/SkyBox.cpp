#include "SkyBox.h"
#include <osg/Texture2D>
#include "osg/TexEnv"
#include "osg/TexGen"

bimWorld::SkyBox::SkyBox()
{
	setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	setCullingActive(false);
	osg::StateSet* ss = getOrCreateStateSet();
	ss->setAttributeAndModes(new osg::Depth(
		osg::Depth::LEQUAL, 1.0f, 1.0f));
	ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	ss->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
	//ss->setRenderBinDetails(5, "RenderBin");
	ss->setRenderBinDetails(-1, "RenderBin");

}

void bimWorld::SkyBox::setEnvironmentMap(unsigned int unit,
	osg::Image* posX, osg::Image* negX,
	osg::Image* posY, osg::Image* negY,
	osg::Image* posZ, osg::Image* negZ)
{
	osg::ref_ptr<osg::TextureCubeMap> cubemap =
		new osg::TextureCubeMap;
	// <设置立方图的6个面的贴图>
	cubemap->setImage(osg::TextureCubeMap::POSITIVE_X, posX);
	cubemap->setImage(osg::TextureCubeMap::NEGATIVE_X, negX);
	cubemap->setImage(osg::TextureCubeMap::POSITIVE_Y, posY);
	cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Y, negY);
	cubemap->setImage(osg::TextureCubeMap::POSITIVE_Z, posZ);
	cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Z, negZ);
	// <设置纹理环绕模式>
	cubemap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	cubemap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
	cubemap->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);

	// <设置滤波：线性和 mipmap>
	cubemap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
	cubemap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);

	osg::ref_ptr<osg::TexGen> tg = new osg::TexGen;
	tg->setMode(osg::TexGen::NORMAL_MAP);

	//// <设置纹理矩阵>
	//osg::ref_ptr<osg::TexMat> tm = new osg::TexMat;
	//stateset->setTextureAttribute(0, tm.get());

	//osg::ref_ptr<osg::ClearNode> clearNode = new osg::ClearNode;
	//clearNode->setCullCallback(new TexMatCallback(*tm));
	//clearNode->addChild(geode.get());

	// Please find details in the source code
	cubemap->setResizeNonPowerOfTwoHint(false);
	getOrCreateStateSet()->setTextureAttributeAndModes(
		unit, cubemap.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
}

void bimWorld::SkyBox::setEnvironmentMap(unsigned int unit, osg::Image* hdr)
{
	osg::ref_ptr<osg::Texture2D> cubemap =
		new osg::Texture2D;
	// <设置纹理环绕模式>
	cubemap->setImage(hdr);
	cubemap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	cubemap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
	cubemap->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
	// <设置滤波：线性和 mipmap>
	cubemap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
	cubemap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);

	// <设置纹理映射方式，指定为替代方式，即纹理中的颜色代替原来的颜色>
	osg::ref_ptr<osg::TexEnv> te = new osg::TexEnv;
	te->setMode(osg::TexEnv::REPLACE);//REPLACE
	getOrCreateStateSet()->setTextureAttributeAndModes(
		unit, cubemap.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
}

bool bimWorld::SkyBox::computeLocalToWorldMatrix(osg::Matrix& matrix,
	osg::NodeVisitor* nv) const
{
	if (nv && nv->getVisitorType() ==
		osg::NodeVisitor::CULL_VISITOR)
	{
		osgUtil::CullVisitor* cv =
			static_cast<osgUtil::CullVisitor*>(nv);
		matrix.preMult(osg::Matrix::translate(cv->getEyeLocal()));
		return true;
	}
	else
		return osg::Transform::computeLocalToWorldMatrix(matrix, nv);
}

bool bimWorld::SkyBox::computeWorldToLocalMatrix(osg::Matrix& matrix,
	osg::NodeVisitor* nv) const
{
	if (nv && nv->getVisitorType() ==
		osg::NodeVisitor::CULL_VISITOR)
	{
		osgUtil::CullVisitor* cv =
			static_cast<osgUtil::CullVisitor*>(nv);
		matrix.postMult(osg::Matrix::translate(
			-cv->getEyeLocal()));
		return true;
	}
	else
		return osg::Transform::computeWorldToLocalMatrix(matrix, nv);
}

void bimWorld::SkyBox::setHDR(const std::string& filename, float radius)
{
	osg::ref_ptr<osg::Image> imageHDR = osgDB::readImageFile(filename);
	if (!imageHDR.get())
	{
		return;
	}
	setEnvironmentMap(0, imageHDR.get());
	createDrawable(radius);
}

void bimWorld::SkyBox::setCubeMap(const std::string& right, const std::string& left, const std::string& front, const std::string& back, const std::string& up, const std::string& down, float radius)
{
	osg::ref_ptr<osg::Image> imagePosX = osgDB::readImageFile(/*"right.jpg"*/right);
	osg::ref_ptr<osg::Image> imageNegX = osgDB::readImageFile(/*"left.jpg"*/left);
	osg::ref_ptr<osg::Image> imagePosY = osgDB::readImageFile(/*"front.jpg"*/front);
	osg::ref_ptr<osg::Image> imageNegY = osgDB::readImageFile(/*"back.jpg"*/back);
	osg::ref_ptr<osg::Image> imagePosZ = osgDB::readImageFile(/*"up.jpg"*/up);
	osg::ref_ptr<osg::Image> imageNegZ = osgDB::readImageFile(/*"down.jpg"*/down);
	if (!imagePosX.get() || !imageNegX.get() || !imagePosY.get() || !imageNegY.get() || !imagePosZ.get() || !imageNegZ.get())
	{
		osg::notify(osg::WARN) << "cube map has invalide picture, in SkyBox::setCubeMap()" << std::endl;
		return;
	}
	setEnvironmentMap(0, imagePosX.release(),
		imageNegX.release(),
		imagePosY.release(),
		imageNegY.release(),
		imagePosZ.release(),
		imageNegZ.release());

	createDrawable(radius);
}

void bimWorld::SkyBox::createDrawable(float radius)
{
	if (radius < 1)
	{
		radius = 1;
	}
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(new osg::ShapeDrawable(
		new osg::Sphere(osg::Vec3(), radius)));
	this->addChild(geode.get());
}
