#ifndef __BILLBOARD_FACTORY_H__
#define __BILLBOARD_FACTORY_H__

#include <osg/Object>
#include <osg/Billboard>
#include <osg/Geometry>
#include <osg/Texture2D>

#include <osg/NodeCallback>
#include <osg/Node>
#include <osg/NodeVisitor>
#include <osg/MatrixTransform>
#include <osg/UserDataContainer>
#include "CoordinateHelper.h"
#include <osg/Camera>

namespace BillboardExtension
{
	///-------------------------------------------------------------------------------------------------
	/// <summary>	An update billboard position. </summary>
	///
	/// <remarks>	Sqh, 2015/5/6. </remarks>
	///-------------------------------------------------------------------------------------------------

	class UpdateBillboardPosition : public osg::NodeCallback
	{
	public:

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Constructor. </summary>
		///
		/// <remarks>	Sqh, 2015/5/6. </remarks>
		///
		/// <param name="helper">	The helper. </param>
		///-------------------------------------------------------------------------------------------------

		UpdateBillboardPosition(osg::ref_ptr<util::CoordinateHelper> helper)
		{
			m_coordHelper = helper;
		}
		//~UpdateBillboardPosition()
		//{
		//	delete m_coordHelper;
		//}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Function call operator. </summary>
		///
		/// <remarks>	Sqh, 2015/5/6. </remarks>
		///-------------------------------------------------------------------------------------------------

		virtual void operator () (osg::Node *node, osg::NodeVisitor* nv)
		{
			auto container = dynamic_cast<osg::DefaultUserDataContainer*>(node->getUserDataContainer());
			// �Զ��ģ�͵�������ƽ��ֵ����ƽ��������Ϊ���Ƶ�λ��
			auto tcenter = osg::Vec3(0, 0, 0);
			auto length = container->getNumUserObjects();
			for (unsigned int i = 0; i < length; i++)
			{
				auto construct = dynamic_cast<osg::Node*>(container->getUserObject(i));
				if (!construct)
					continue;
				tcenter += construct->getBound().center();
			}
			auto pos = tcenter / length;
			auto currentNode = dynamic_cast<osg::Billboard*>(node);
			if (!currentNode)
			{
				osg::notify(osg::FATAL) << "Update billboard position failed. The node is not a Billboard." << std::endl;
			}
			else
			{
				auto worldMatrix = node->getWorldMatrices()[0];
				osg::Vec3 world = pos * worldMatrix;
				currentNode->setPosition(0, m_coordHelper->WorldToScreen(world));
				//currentNode->setMatrix(osg::Matrix::translate(world));
			}
			traverse(node, nv);
		}
	protected:
		osg::ref_ptr<util::CoordinateHelper> m_coordHelper;
	};

	///-------------------------------------------------------------------------------------------------
	/// <summary>	Creates a quad. </summary>
	///
	/// <remarks>	Sqh, 2015/5/6. </remarks>
	///
	/// <param name="sideLength">	Length of the side. </param>
	///
	/// <returns>	The new quad. </returns>
	///-------------------------------------------------------------------------------------------------

	osg::Geometry* createQuad(float sideLength = 60);

	///-------------------------------------------------------------------------------------------------
	/// <summary>	A billboard factory. </summary>
	///
	/// <remarks>	Sqh, 2015/5/6. </remarks>
	///-------------------------------------------------------------------------------------------------

	class BillboardFactory : public osg::Object
	{
	public:

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Gets the singleton instance. </summary>
		///
		/// <remarks>	Sqh, 2015/5/6. </remarks>
		///
		/// <returns>	The instance. </returns>
		///-------------------------------------------------------------------------------------------------

		static BillboardFactory* getInstance();


		///-------------------------------------------------------------------------------------------------
		/// <summary>	���������. </summary>
		///
		/// <remarks>	Sqh, 2015/5/6. </remarks>
		///
		/// <param name="geometry">	[in,out] (Optional) If non-null, ����Ƶļ���ͼ��. 
		/// 						Ĭ��Ϊ�߳�Ϊ2�ķ�Ƭ��
		/// 						����һ�κ󣬺������ô˺���ʱ��ֱ��ʹ����һ�εļ���ͼ�Ρ�
		/// </param>
		/// <param name="image">   	[in,out] (Optional) If non-null, ���������.
		/// 						����һ�κ󣬺������ô˺���ʱ��ֱ��ʹ����һ�εļ���ͼ�Ρ�
		/// </param>
		/// <param name="position">	�����λ��. </param>
		/// <param name="mode">	   	���������. </param>
		/// <param name="axis">	   	�����Χ���ĸ�����ת���� Billboard::AXIAL_ROT ��Ч��. </param>
		///
		/// <returns>	The new billboard. </returns>
		///-------------------------------------------------------------------------------------------------

		osg::Billboard* createBillboard(osg::Geometry* geometry = NULL, osg::Image *image = NULL, osg::Vec3 position = osg::Vec3(0, 0, 0), osg::Billboard::Mode mode = osg::Billboard::POINT_ROT_EYE, osg::Vec3 axis = osg::Vec3(0.0f, 0.0f, 0.0f));


#pragma region override

		/** Copy constructor using CopyOp to manage deep vs shallow copy. */
		BillboardFactory(const BillboardFactory& geometry, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

		/** Clone the type of an object, with Object* return type.
		Must be defined by derived classes.*/
		virtual Object* cloneType() const override;

		/** Clone an object, with Object* return type.
		Must be defined by derived classes.*/
		virtual Object* clone(const osg::CopyOp&) const override;

		/** return the name of the object's library. Must be defined
		by derived classes. The OpenSceneGraph convention is that the
		namespace of a library is the same as the library name.*/
		virtual const char* libraryName() const override;

		/** return the name of the object's class type. Must be defined
		by derived classes.*/
		virtual const char* className() const override;

#pragma endregion

	protected:
		BillboardFactory();

		~BillboardFactory();

		static osg::ref_ptr<BillboardFactory> m_instance;

		//osg::ref_ptr<osg::Geometry> m_geometry;
	};
}
#endif // __BILLBOARD_FACTORY_H__