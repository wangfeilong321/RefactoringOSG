#include "BillboardFactory.h"
#include <osg/Depth>
#include <osg/ShapeDrawable>
using namespace BillboardExtension;
using namespace osg;

osg::ref_ptr<BillboardFactory> BillboardFactory::m_instance = NULL;

BillboardFactory* BillboardFactory::getInstance()
{
	if (m_instance == NULL)
		m_instance = new BillboardFactory();
	return m_instance.get();
}

osg::Geometry* BillboardExtension::createQuad(float sideLength)
{
	// create quad
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();

	// setup vertexes
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;
	geom->setVertexArray(v.get());
	v->push_back(osg::Vec3(-sideLength, 0.f, -sideLength));
	v->push_back(osg::Vec3(sideLength, 0.f, -sideLength));
	v->push_back(osg::Vec3(sideLength, 0.f, sideLength));
	v->push_back(osg::Vec3(-sideLength, 0.f, sideLength));

	// setup normals
	ref_ptr<Vec3Array> normal = new Vec3Array();
	normal->push_back(Vec3(sideLength, 0.0f, 0.0f) ^ Vec3(0.0f, 0.0f, sideLength));

	geom->setNormalArray(normal.get());
	geom->setNormalBinding(Geometry::BIND_OVERALL);

	// setup texture coordinates.
	ref_ptr<Vec2Array> vt = new Vec2Array();
	vt->push_back(Vec2(0.0f, 0.0f));
	vt->push_back(Vec2(1.0, 0.0f));
	vt->push_back(Vec2(1.0, 1.0));
	vt->push_back(Vec2(0.0f, 1.0));

	geom->setTexCoordArray(0, vt.get());

	// draw quad
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, v->size()));

	return geom.release();
}

osg::Billboard* BillboardFactory::createBillboard(osg::Geometry* geometry, osg::Image* image, osg::Vec3 position, osg::Billboard::Mode mode, osg::Vec3 axis)
{

	osg::ref_ptr<osg::Drawable> geom = geometry;
	if (!geom)
	{
		//if (!m_geometry)
		//	return NULL;
		geom = new osg::ShapeDrawable(new osg::Box(position, 60));
		//geom = createQuad();/*m_geometry.get();*/
	}
	// state property object
	ref_ptr<StateSet> stateset = new StateSet();

	if (image)
	{

		// create a texture2d property object
		ref_ptr<Texture2D> texture = new Texture2D();
		// relate image
		texture->setImage(image);

		// relate texture2d object, the default value of the third param is ON.
		stateset->setTextureAttributeAndModes(0, texture, StateAttribute::ON);

		// enable blending
		stateset->setMode(GL_BLEND, StateAttribute::ON);

		// set render hint, prepare to transparent.
		stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

		//// set depth range
		//osg::Depth* depth = new osg::Depth();
		//depth->setRange(0, 0.0001);
		//stateset->setAttribute(depth, StateAttribute::ON);

	}
	// enable depth test
	stateset->setMode(GL_DEPTH_TEST, StateAttribute::ON);

	// close lighting
	stateset->setMode(GL_LIGHTING, StateAttribute::OFF);

	geom->setStateSet(stateset.get());

	osg::ref_ptr<osg::Billboard> billboard = new osg::Billboard();
	billboard->setMode(mode);
	if (mode == Billboard::AXIAL_ROT)
		billboard->setAxis(axis);
	billboard->addDrawable(geom.get(), position);

	return billboard.release();
}

BillboardFactory::BillboardFactory()
{
	//m_geometry = createQuad();
}

BillboardExtension::BillboardFactory::~BillboardFactory()
{
	// ref_ptr needn't release operation.
}

#pragma region Override

BillboardExtension::BillboardFactory::BillboardFactory(const BillboardFactory & factory, const osg::CopyOp & copyop) :
osg::Object(factory, copyop)
{
	//m_geometry = dynamic_cast<osg::Geometry*>(copyop(factory.m_geometry));
}

/** Clone the type of an object, with Object* return type.
Must be defined by derived classes.*/
osg::Object* BillboardFactory::cloneType() const
{
	return new BillboardFactory();
}

/** Clone an object, with Object* return type.
Must be defined by derived classes.*/
osg::Object* BillboardFactory::clone(const osg::CopyOp& copyop) const
{
	return new BillboardFactory(*this, copyop);
}

/** return the name of the object's library. Must be defined
by derived classes. The OpenSceneGraph convention is that the
namespace of a library is the same as the library name.*/
const char* BillboardFactory::libraryName() const
{
	return "BillboardExtension";
}

/** return the name of the object's class type. Must be defined
by derived classes.*/
const char* BillboardFactory::className() const
{
	return "BillboardFactory";
}

#pragma endregion
