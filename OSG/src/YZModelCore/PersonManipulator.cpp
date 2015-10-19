//#include "PersonManipulator.h" 
//#include <osgUtil/IntersectVisitor>
//#ifndef M_PI
//# define M_PI       3.14159265358979323846  /* pi */
//#endif
//#define FACTOR 35
//#include <fstream>
//
//PersonManipulator::PersonManipulator(bimWorld::ModelViewerImpl *m_pImpl, bool limitedSpace , double selectionPrecision):
// bimWorld::BIMCameraManipulator(m_pImpl,limitedSpace,selectionPrecision),m_shift(false),m_ctrl(false)	
//{
//	m_speedEpsilon=0.2;
//	m_speedFactor=0.2;
//	m_angleEpsilon=0.0001;
//	m_angleFactor=0.1;
//	m_upVector.set(0,0,1);
//	m_forwardSpeed=3;
//	m_sideSpeed=3;
//	m_upSpeed=3;
//	m_pitchAngle=0.07;
//	m_rollAngle=0.1;
//	m_yawAngle=0.1;
//	m_iLeftx=0;
//	m_iLefty=0;
//	m_bLeftDown=false;
//	m_pretime=0;
//}
//void PersonManipulator::_stop()
//{
//	//m_forwardSpeed=0.0;
//	//m_sideSpeed=0.0;
//	//m_upSpeed=0.0;
//	//m_pitchAngle=0.0;
//	//m_rollAngle=0.0;
//	//m_yawAngle=0.0;
//}
//const char* PersonManipulator::className() const
//{
//	return "PersonManipulator";
//}
//void PersonManipulator::getUsage(osg::ApplicationUsage &usage) const
//{
//	usage.addKeyboardMouseBinding("Walk Manipulator: <UpArrow>",                  "Move forward.");
//	usage.addKeyboardMouseBinding("Walk Manipulator: <DownArrow>",                "Move back.");
//	usage.addKeyboardMouseBinding("Walk Manipulator: <LeftArrow>",                "Rolate Left.");
//	usage.addKeyboardMouseBinding("Walk Manipulator: <RightArrow>",               "Rolate Right.");
//	usage.addKeyboardMouseBinding("Walk Manipulator: <Shift/UpArrow>",            "Move up.");
//	usage.addKeyboardMouseBinding("Walk Manipulator: <Shift/DownArrow>",          "Move down.");
//	usage.addKeyboardMouseBinding("Walk Manipulator: <Shift/LeftArrow>",          "Move left.");
//	usage.addKeyboardMouseBinding("Walk Manipulator: <Shift/RightArrow>",         "Move right.");
//	usage.addKeyboardMouseBinding("Walk Manipulator: <Ctrl/UpArrow>",             "Pitch up.");
//	usage.addKeyboardMouseBinding("Walk Manipulator: <Ctrl/DownArrow>",           "Pitch down.");
//	usage.addKeyboardMouseBinding("Walk Manipulator: <Ctrl/LeftArrow>",           "Roll left.");
//	usage.addKeyboardMouseBinding("Walk Manipulator: <Ctrl/RightArrow>",          "Roll Right.");
//}
//void PersonManipulator::setNode(osg::Node *node )
//{
//	m_pNode=node;
//    if (m_pNode.get())
//    {
//        const osg::BoundingSphere& boundingSphere=m_pNode->getBound();
//        m_modelScale = boundingSphere._radius;
//    }
//    
//}
//const osg::Node* PersonManipulator::getNode() const
//{
//    return m_pNode.get();
//}
//
//osg::Node* PersonManipulator::getNode()
//{
//    return m_pNode.get();
//}
//
//void PersonManipulator::setByInverseMatrix(const osg::Matrixd &matrix)
//{
//	osg::Matrixd mat		=matrix;
//	mat						=osg::Matrixd::inverse(mat);
//	m_eye					=mat.getTrans();
//	osg::CoordinateFrame cf	=getCoordinateFrame(m_eye);
//	m_frontVector			=getFrontVector(cf);
//	m_sideVector		    =getSideVector(cf);
//	m_upVector				=getUpVector(cf);
//}
//osg::Matrixd PersonManipulator::getMatrix() const
//{
//	osg::Matrix mat(m_sideVector[0],m_upVector[0],-m_frontVector[0],     0.0,
//                    m_sideVector[1],m_upVector[1],-m_frontVector[1],     0.0,
//                    m_sideVector[2],m_upVector[2],-m_frontVector[2],     0.0,
//                               0.0,         0.0,            0.0,     1.0);
//
//	return mat*osg::Matrix::translate(m_eye);
//}
//osg::Matrixd PersonManipulator::getInverseMatrix() const
//{
//	osg::Matrix mat(m_sideVector[0],m_upVector[0],-m_frontVector[0],     0.0,
//                    m_sideVector[1],m_upVector[1],-m_frontVector[1],     0.0,
//                    m_sideVector[2],m_upVector[2],-m_frontVector[2],     0.0,
//                               0.0,         0.0,            0.0,     1.0);
//
//	return osg::Matrix::translate(-m_eye)*mat;
//}
//void PersonManipulator::computeHomePosition()
//{
//	
//    if( !m_pNode.valid() )
//        return;
//
//    osg::BoundingSphere bs = m_pNode->getBound();
//
//    /*
//       * Find the ground - Assumption: The ground is the hit of an intersection
//       * from a line segment extending from above to below the database at its 
//       * horizontal center, that intersects the database closest to zero. */
//    osgUtil::IntersectVisitor iv;
//    iv.setTraversalMask(_intersectTraversalMask);
//
//    osg::CoordinateFrame cf( getCoordinateFrame(bs.center()) ); // not sure what position to use here
//    osg::Vec3d upVec( getUpVector(cf) );
//    osg::ref_ptr<osg::LineSegment> seg = new osg::LineSegment;
//    osg::Vec3 A = bs.center() + (upVec*(bs.radius()*2));
//    osg::Vec3 B = bs.center() + (-upVec*(bs.radius()*2));
//
//    if( (B-A).length() == 0.0)
//    {
//        return;
//    }
//
//    /*
//    seg->set( bs.center() + (upVec*(bs.radius()*2)), 
//              bs.center() + (-upVec*(bs.radius()*2)) );
//              */
//    seg->set( A, B );
//
//    iv.addLineSegment( seg.get() );
//    m_pNode->accept(iv);
//
//    // start with it high
//    double ground = bs.radius() * 3;
//
//    //if (iv.hits())
//    //{
//    //    osgUtil::IntersectVisitor::HitList& hitList = iv.getHitList(seg.get());
//    //    osg::Vec3d ip = hitList.front().getWorldIntersectPoint();
//    //    double d = ip.length();
//    //    if( d < ground )
//    //        ground = d;
//    //}
//    //else
//    //{
//        //osg::notify(osg::WARN)<<"UFOManipulator : I can't find the ground!"<<std::endl;
//        ground = 2.0;
//   // }
//    osg::Vec3 p(bs.center() + upVec*( ground + 2.0*1.25 ) );
//    setHomePosition( p, p + getFrontVector(cf), upVec );
//}
///*void PersonManipulator::home(double )
//{
//	if (getAutoComputeHomePosition()) computeHomePosition();
//	m_eye=_homeEye;
//	m_frontVector=_homeCenter-_homeEye;
//	m_frontVector.normalize();
//	m_upVector=_homeUp;
//	m_sideVector=m_frontVector^m_upVector;
//}*/
////void PersonManipulator::home(const  osgGA::GUIEventAdapter &ea,osgGA::GUIActionAdapter &aa)
////{
////	if (getAutoComputeHomePosition())
////        computeHomePosition();
////	m_eye=_homeEye;
////	m_frontVector=_homeCenter-_homeEye;
////	m_frontVector.normalize();
////	m_upVector=_homeUp;
////	m_sideVector=m_frontVector^m_upVector;
//
////}
//bool PersonManipulator::handle(const  osgGA::GUIEventAdapter &ea,osgGA::GUIActionAdapter &aa)
//{
//	
//	osgViewer::Viewer* viewer=dynamic_cast<osgViewer::Viewer*>(&aa);
//	if (ea.getHandled()) return false;
//	osgGA::GUIEventAdapter::ScrollingMotion sm = ea.getScrollingMotion();
//	switch(sm)
//	{
//	case osgGA::GUIEventAdapter::SCROLL_DOWN:
//		{
//			moveFront(false);
//		}
//		break;
//	case osgGA::GUIEventAdapter::SCROLL_UP:
//		{
//			moveFront(true);
//		}
//		break;
//	}
//	switch(ea.getEventType())
//	{
//	case osgGA::GUIEventAdapter::PUSH:
//		{
//			m_iLeftx=ea.getX();
//			m_iLefty=ea.getY();
//			m_bLeftDown=true;
//			return true;
//		}
//		break;
//	case osgGA::GUIEventAdapter::SCROLL_DOWN:
//		{
//			moveFront(false);
//		}
//		break;
//	case osgGA::GUIEventAdapter::DRAG:
//		{
//			if(m_bLeftDown)
//			{
//				int delx=ea.getX()-m_iLeftx;
//				m_iLeftx=ea.getX();
//				bool s=delx>0?true:false;
//				double distance=(double)m_eye.length();
//				//double radians=osg::DegreesToRadians((double)(delx/50));
//				//if(!s)
//					//radians=!radians;
//				rotateYaw(s,distance);
//			}
//			return true;
//		}
//		break;
//	case osgGA::GUIEventAdapter::RELEASE:
//		{
//			m_bLeftDown=false;
//			return true;
//		}
//		break;
//	case(osgGA::GUIEventAdapter::KEYUP):
//		{
//			_keyUp( ea, aa );
//			return false;
//		}
//		break;
//
//	case(osgGA::GUIEventAdapter::KEYDOWN):
//		{
//			_keyDown(ea, aa);
//			return false;
//		}
//		break;
//	}
//	return true;
//}
//void PersonManipulator::_keyUp(const  osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
//{
//	switch(ea.getKey())
//	{
//		case osgGA::GUIEventAdapter::KEY_Control_L:
//        case osgGA::GUIEventAdapter::KEY_Control_R:
//            m_ctrl = false;
//            break;
//
//        case osgGA::GUIEventAdapter::KEY_Shift_L :
//        case osgGA::GUIEventAdapter::KEY_Shift_R :
//            m_shift = false;
//            break;
//	}
//}
//void PersonManipulator::_keyDown(const  osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
//{
//	switch(ea.getKey())
//	{
//		case osgGA::GUIEventAdapter::KEY_Control_L:
//        case osgGA::GUIEventAdapter::KEY_Control_R:
//            m_ctrl = true;
//            break;
//
//        case osgGA::GUIEventAdapter::KEY_Shift_L :
//        case osgGA::GUIEventAdapter::KEY_Shift_R :
//            m_shift = true;
//            break;
//		case osgGA::GUIEventAdapter::KEY_Up :
//			if(m_ctrl)
//			{
//				rotatePitch(true);
//				aa.requestRedraw();
//				aa.requestContinuousUpdate(false);
//				break;
//			}
//			else
//			{
//				if(m_shift)
//				{
//					moveUp(true);
//					aa.requestRedraw();
//					aa.requestContinuousUpdate(false);
//					break;
//				}
//				else
//				{
//					moveFront(true);
//					aa.requestRedraw();
//					aa.requestContinuousUpdate(false);
//					break;
//				}
//			}
//		case osgGA::GUIEventAdapter::KEY_Down:
//			if(m_ctrl)
//			{
//				rotatePitch(false);
//				aa.requestRedraw();
//				aa.requestContinuousUpdate(false);
//				break;
//			}
//			else
//			{
//				if(m_shift)
//				{
//					moveUp(false);
//					aa.requestRedraw();
//					aa.requestContinuousUpdate(false);
//					break;
//				}
//				else
//				{
//					moveFront(false);
//					aa.requestRedraw();
//					aa.requestContinuousUpdate(false);
//					break;
//				}
//			}
//		case osgGA::GUIEventAdapter::KEY_Left:
//			if(m_ctrl)
//			{
//				rotateRoll(true);
//				aa.requestRedraw();
//				aa.requestContinuousUpdate(false);
//				break;
//			}
//			else
//			{
//				if(m_shift)
//				{
//					moveSide(false);
//					aa.requestRedraw();
//					aa.requestContinuousUpdate(false);
//					break;
//				}
//				else
//				{
//					double distance=(double)m_eye.length();
//					rotateYaw(true,distance);
//					aa.requestRedraw();
//					aa.requestContinuousUpdate(false);
//					break;
//				}
//			}
//		case osgGA::GUIEventAdapter::KEY_Right:
//			if(m_ctrl)
//			{
//				rotateRoll(false);
//				aa.requestRedraw();
//				aa.requestContinuousUpdate(false);
//				break;
//			}
//			else
//			{
//				if(m_shift)
//				{
//					moveSide(true);
//					aa.requestRedraw();
//					aa.requestContinuousUpdate(false);
//					break;
//				}
//				else
//				{
//					double distance=(double)m_eye.length();
//					rotateYaw(false,distance);
//					aa.requestRedraw();
//					aa.requestContinuousUpdate(false);
//					break;
//				}
//			}
//
//	}
//}
//void PersonManipulator::moveFront(bool positive)
//{
//    
//}
//void PersonManipulator::moveSide(bool positive)
//{
//	if(positive)
//	{
//		m_eye+=m_sideVector*m_forwardSpeed;
//	}
//	else
//	{
//		m_eye-=m_sideVector*m_forwardSpeed;
//	}
//    
//}
//void PersonManipulator::moveUp(bool positive)
//{
//	if(positive)
//	{
//		m_eye+=m_upVector*m_forwardSpeed;
//	}
//	else
//	{
//		m_eye-=m_upVector*m_forwardSpeed;
//	}
//}
//void PersonManipulator::rotateYaw(bool positive,double distance)
//{
//	
//	double facotr=FACTOR/distance;
//	if(positive)
//	{
//		m_frontVector=m_frontVector*osg::Matrixd::rotate(m_yawAngle*facotr,m_upVector);
//		m_sideVector=m_sideVector*osg::Matrixd::rotate(m_yawAngle*facotr,m_upVector);
//
//	}
//	else
//	{
//		m_frontVector=m_frontVector*osg::Matrixd::rotate(-m_yawAngle*facotr,m_upVector);
//		m_sideVector=m_sideVector*osg::Matrixd::rotate(-m_yawAngle*facotr,m_upVector);
//	}
//}
//void PersonManipulator::rotateYaw(bool positive,double distance,double Radians)
//{
//
//	double facotr=FACTOR/distance;
//	if(positive)
//	{
//		m_frontVector=m_frontVector*osg::Matrixd::rotate(Radians*facotr,m_upVector);
//		m_sideVector=m_sideVector*osg::Matrixd::rotate(Radians*facotr,m_upVector);
//
//	}
//	else
//	{
//		m_frontVector=m_frontVector*osg::Matrixd::rotate(-Radians*facotr,m_upVector);
//		m_sideVector=m_sideVector*osg::Matrixd::rotate(-Radians*facotr,m_upVector);
//	}
//}
//void PersonManipulator::rotatePitch(bool positive)
//{
//	if(positive)
//	{
//		m_frontVector=m_frontVector*osg::Matrixd::rotate(m_pitchAngle,m_sideVector);
//		m_upVector=m_upVector*osg::Matrixd::rotate(m_pitchAngle,m_sideVector);
//	}
//	else
//	{
//		m_frontVector=m_frontVector*osg::Matrixd::rotate(-m_pitchAngle,m_sideVector);
//		m_upVector=m_upVector*osg::Matrixd::rotate(-m_pitchAngle,m_sideVector);
//	}
//}
//void PersonManipulator::rotateRoll(bool positive)
//{
//	if(positive)
//	{
//		m_sideVector=m_sideVector*osg::Matrixd::rotate(m_rollAngle,m_frontVector);
//		m_upVector=m_upVector*osg::Matrixd::rotate(m_rollAngle,m_frontVector);
//	}
//	else
//	{
//		m_sideVector=m_sideVector*osg::Matrixd::rotate(-m_rollAngle,m_frontVector);
//		m_upVector=m_upVector*osg::Matrixd::rotate(-m_rollAngle,m_frontVector);
//	}
//}
