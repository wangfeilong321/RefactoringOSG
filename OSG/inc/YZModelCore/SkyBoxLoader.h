#ifndef __SKY_BOX_LOADER_H__
#define __SKY_BOX_LOADER_H__
#include <osgDB/ReadFile>
#include "osg/Geometry"
#include "osg/Geode"
#include "osg/TexMat"
#include "osg/TexGen"
#include "osg/Matrix"
#include "osg/TexEnv"
#include "osg/TextureCubeMap"
#include "osg/Depth"
#include "osg/Quat"
#include "osg/ShapeDrawable"
#include <osg/Texture2D>

#include "osgUtil/CullVisitor"

class Exposure
{
private:
	float index, range, delta;
	bool gray;
	osg::Uniform * umin;
	osg::Uniform * umax;
	osg::Uniform * ugray;

	void update()
	{
		float min, max;
		min = index - (range / 2.0);
		max = index + (range / 2.0);

		fprintf(stderr, "Exposure %f, Range %f, Delta %f, Dynamic Range (%f, %f)\n",
			index, range, delta, min, max);

		umin->set(min);
		umax->set(max);
	}

public:
	Exposure()
		: index(0.5), range(10.0), delta(0.1), gray(false)
	{
		umin = new osg::Uniform("min", 0.0f);
		umax = new osg::Uniform("max", 0.0f);
		ugray = new osg::Uniform("gray", gray);
		update();
	}

	osg::Uniform * getUniformMin()
	{
		return umin;
	}
	osg::Uniform * getUniformMax()
	{
		return umax;
	}
	osg::Uniform * getUniformGrayScale()
	{
		return ugray;
	}

	void incIndex()
	{
		index += delta;
		update();
	}
	void decIndex()
	{
		index -= delta;
		update();
	}
	void incRange()
	{
		range += delta;
		update();
	}
	void decRange()
	{
		range -= delta;
		range = range < 0.0 ? 0.0 : range;
		update();
	}
	void incDelta()
	{
		delta += 0.01;
		update();
	}
	void decDelta()
	{
		delta -= 0.01;
		delta = delta < 0.0 ? 0.0 : delta;
		update();
	}
	void toogleGrayScale()
	{
		gray = !gray;
		ugray->set(gray);
	}
} exposure;

// <������������ͼ����>
struct TexMatCallback :public osg::NodeCallback
{
public:
	TexMatCallback(osg::TexMat& tm) :
		_texMat(tm)
	{

	}
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (cv)
		{
			// <�õ�ģ����ͼ����������ת�Ƕ�>
			const osg::Matrix& MV = *(cv->getModelViewMatrix());
			const osg::Matrix R = osg::Matrix::rotate(osg::DegreesToRadians(112.0f), 0.0f, 0.0f, 1.0f)*
				osg::Matrix::rotate(osg::DegreesToRadians(90.0f), 1.0f, 0.0f, 0.0f);

			osg::Quat q = MV.getRotate();
			const osg::Matrix C = osg::Matrix::rotate(q.inverse());

			// <�����������>
			_texMat.setMatrix(C*R);
		}
		traverse(node, nv);

	}

	// <�������>
	osg::TexMat& _texMat;

};

class SkyBoxLoader
{
public:
	SkyBoxLoader(const std::string& right, const std::string& left, const std::string& front, const std::string& back, const std::string& up, const std::string& down)
	{
		m_rightFile = right;
		m_leftFile = left;
		m_frontFile = front;
		m_backFile = back;
		m_upFile = up;
		m_downFile = down;
		m_useHDR = false;
		m_hdrFile = "";
	}
	SkyBoxLoader(const std::string& hdr)
	{
		m_rightFile = "";
		m_leftFile = "";
		m_frontFile = "";
		m_backFile = "";
		m_upFile = "";
		m_downFile = "";
		m_hdrFile = hdr;
		m_useHDR = true;
	}

	// <��ȡ����ͼ>
	osg::Texture* readCubeMap()
	{
		osg::ref_ptr<osg::Texture> result;

		if (m_useHDR)
		{
			if (m_hdrFile.empty())
			{
				return NULL;
			}
			auto cubemap = new osg::Texture2D();
			osg::ref_ptr<osg::Image> imageHDR = osgDB::readImageFile(m_hdrFile);
			if (!imageHDR.get())
			{
				return NULL;
			}
			//cubemap->setInternalFormat(GL_FLOAT_RGB32_NV);
			cubemap->setImage(imageHDR);
			//auto value = cubemap->getInternalFormat();
			// <����������ģʽ>
			cubemap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
			cubemap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
			cubemap->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
			result = cubemap;
		}
		else
		{
			if (m_rightFile.empty() || m_leftFile.empty() || m_frontFile.empty() || m_backFile.empty() || m_upFile.empty() || m_downFile.empty())
			{
				return NULL;
			}
			auto cubemap = new osg::TextureCubeMap();
			osg::ref_ptr<osg::Image> imagePosX = osgDB::readImageFile(/*"right.jpg"*/m_rightFile);
			osg::ref_ptr<osg::Image> imageNegX = osgDB::readImageFile(/*"left.jpg"*/m_leftFile);
			osg::ref_ptr<osg::Image> imagePosY = osgDB::readImageFile(/*"front.jpg"*/m_frontFile);
			osg::ref_ptr<osg::Image> imageNegY = osgDB::readImageFile(/*"back.jpg"*/m_backFile);
			osg::ref_ptr<osg::Image> imagePosZ = osgDB::readImageFile(/*"up.jpg"*/m_upFile);
			osg::ref_ptr<osg::Image> imageNegZ = osgDB::readImageFile(/*"down.jpg"*/m_downFile);
			if (!imagePosX.get() || !imageNegX.get() || !imagePosY.get() || !imageNegY.get() || !imagePosZ.get() || !imageNegZ.get())
			{
				return NULL;
			}

			// <��������ͼ��6�������ͼ>
			cubemap->setImage(osg::TextureCubeMap::POSITIVE_X, imagePosX.get());
			cubemap->setImage(osg::TextureCubeMap::POSITIVE_Y, imagePosY.get());
			cubemap->setImage(osg::TextureCubeMap::POSITIVE_Z, imagePosZ.get());
			cubemap->setImage(osg::TextureCubeMap::NEGATIVE_X, imageNegX.get());
			cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Y, imageNegY.get());
			cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Z, imageNegZ.get());

			// <����������ģʽ>
			cubemap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
			cubemap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
			cubemap->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
			result = cubemap;
		}

		// <�����˲������Ժ� mipmap>
		result->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
		result->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);

		return result.release();
	}

	osg::Program * shader(std::string filename)
	{

		osg::Shader* FragmentObject =
			new osg::Shader(osg::Shader::FRAGMENT);
		bool frag = FragmentObject->loadShaderSourceFromFile(filename);

		osg::Program* ProgramObject = new osg::Program;
		if (frag) ProgramObject->addShader(FragmentObject);

		return ProgramObject;
	}

	// <������պ�>
	osg::Node* createSkyBox()
	{
		osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();

		// <��������ӳ�䷽ʽ��ָ��Ϊ�����ʽ���������е���ɫ����ԭ������ɫ>
		osg::ref_ptr<osg::TexEnv> te = new osg::TexEnv;
		te->setMode(osg::TexEnv::REPLACE);//REPLACE
		stateset->setTextureAttributeAndModes(0, te.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

		// <�Զ������������꣬ ���䷽ʽ��REFLECTION_MAP��>
		/*
		NORMAL_MAP <��׼ģʽ��������ͼ����>
		REFLECTION_MAP <����ģʽ������������>
		SPHERE_MAP <����ģ�͡�����������>
		*/
		if (!m_useHDR)
		{
			osg::ref_ptr<osg::TexGen> tg = new osg::TexGen;
			tg->setMode(osg::TexGen::NORMAL_MAP);
			stateset->setTextureAttributeAndModes(0, tg.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

		}
		// <�����������>
		osg::ref_ptr<osg::TexMat> tm = new osg::TexMat;
		stateset->setTextureAttribute(0, tm.get());

		// <��������ͼ����>
		osg::ref_ptr<osg::Texture> skymap = readCubeMap();
		stateset->setTextureAttributeAndModes(0, skymap.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		stateset->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);

		//stateset->addUniform(exposure.getUniformMin());
		//stateset->addUniform(exposure.getUniformMax());
		//stateset->addUniform(exposure.getUniformGrayScale());
		//stateset->setAttributeAndModes(shader("E:\\WorkSpace\\VCplusplus\\OSG\\HDRTutorial\\osgsimplenvhdr\\x64\\Debug\\readtexture.frag"),
		//	osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

		// <���������ΪԶƽ��>
		osg::ref_ptr<osg::Depth> depth = new osg::Depth;
		depth->setFunction(osg::Depth::ALWAYS);
		depth->setRange(1.0, 1.0); // <Զƽ��>
		stateset->setAttributeAndModes(depth, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

		// <������Ⱦ˳��Ϊ-1�� ����Ⱦ>
		stateset->setRenderBinDetails(-1, "RenderBin");

		osg::ref_ptr<osg::Drawable> drawable = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), 100000));

		// <��������뵽Ҷ�ڵ�>
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		geode->setCullingActive(false);
		geode->setStateSet(stateset.get());
		geode->addDrawable(drawable.get());

		if (!m_useHDR)
		{
			osg::ref_ptr<osg::ClearNode> clearNode = new osg::ClearNode;
			clearNode->setCullCallback(new TexMatCallback(*tm));
			clearNode->addChild(geode.get());

			return clearNode.release();
		}
		else
		{
			return geode.release();
		}

	}

protected:
	std::string m_rightFile, m_leftFile, m_frontFile, m_backFile, m_upFile, m_downFile, m_hdrFile;
	bool m_useHDR;
};


#endif // __SKY_BOX_LOADER_H__