#ifndef __PERSON_MANIPULATOR_H__
#define __PERSON_MANIPULATOR_H__
#include "config.h"
#include <osgGA/FirstPersonManipulator>
/*  
The Walk Manipulator allows the following movements with the listed
      Key combinations:
        \param UpArrow          ��ǰ�ƶ�
        \param DownArrow        ����ƶ�
        \param LeftArrow        ������ת
        \param RightArrow       ������ת
        \param SpaceBar         �ص�Ĭ��λ��
        \param Shift/UpArrow    �����ƶ�
        \param Shift/DownArrow  �����ƶ�
        \param Shift/LeftArrow  �����ƶ�
        \param Shift/RightArrow �����ƶ�
        \param Ctrl/UpArrow     ������ת
        \param Ctrl/DownArrow   ������ת
        \param Ctrl/LeftArrow   �����ת
        \param Ctrl/RightArrow  ���ҹ�ת
	*/



#include <iostream>
#include <osgGA/MultiTouchTrackballManipulator>
#include <osg/Node>
#include <osg/Matrix>
#include<osgGA/GUIEventAdapter>
#include<osgGA/GUIActionAdapter>
#include"CameraManipulators.h"
#include "CoordinateHelper.h"
#include <math.h>
#include <osg/Math>
//class YIZHU_EXPORT  PersonManipulator : public bimWorld::BIMCameraManipulator
//{
//	
//    public:
//        /** Default constructor */
//        PersonManipulator(bimWorld::ModelViewerImpl *m_pImpl, bool limitedSpace = false, double selectionPrecision = 1e-6);
//
//        /** return className */          
//        virtual const char*			className() const;
//
//        /** set the position of the matrix manipulator using a 4x4 Matrix.*/
////        virtual void				setByMatrix( const osg::Matrixd &matrix )
////									{ setByInverseMatrix(osg::Matrixd::inverse(matrix)); }
//
//        /** get the position of the manipulator as 4x4 Matrix.*/
//        virtual osg::Matrixd		getMatrix() const;
//
//		/** set the position of the matrix manipulator using a 4x4 Matrix.*/
//        virtual void				setByInverseMatrix(const osg::Matrixd& matrix);
//
//        /** get the position of the manipulator as a inverse matrix of the manipulator, typically used as a model view matrix.*/
//      virtual osg::Matrixd		getInverseMatrix() const;
//
//
//
//        /** Set the  subgraph this manipulator is driving the eye through. root of subgraph */
//        virtual void				setNode(osg::Node* node);
//
//        /** Get the root node of the subgraph this manipulator is driving the eye through (const)*/
//        virtual const osg::Node*	getNode() const;
//
//        /** Get the root node of the subgraph this manipulator is driving the eye through */
//        virtual osg::Node*			getNode();  
//
//        /** Sets the viewpoint matrix to the home position */
//        //virtual void				home(const  osgGA::GUIEventAdapter &ea,  osgGA::GUIActionAdapter &aa);
//		virtual void				computeHomePosition();
//		//virtual void				home(double);
//    //    virtual void init(const  osgGA::GUIEventAdapter &ea,  osgGA::GUIActionAdapter &aa);
//
//        /** Handles incoming osgGA events */
//        virtual bool				handle(const  osgGA::GUIEventAdapter &ea,  osgGA::GUIActionAdapter &aa);		
//
//        /** Reports Usage parameters to the application */
//        void						getUsage(osg::ApplicationUsage& usage) const;        
//
//        void						setForwardSpeed( double in_fs ) { m_forwardSpeed = in_fs; }
//        double						getForwardSpeed() const { return m_forwardSpeed; }
//
//        void						setSideSpeed( double in_ss ) { m_sideSpeed = in_ss; }
//        double						getSideSpeed() const { return m_sideSpeed; }
//
//        void						setUpSpeed( double in_us ) { m_upSpeed = in_us; }
//        double						getUpSpeed() const { return m_upSpeed; }
//        
//        void						setPitchAngle( double in_pa ) { m_pitchAngle = in_pa; }
//        double						getPitchAngle() const { return m_pitchAngle; }
//
//        void						setRollAngle( double in_ra ) { m_rollAngle = in_ra; }
//        double						getRollAngle() const { return m_rollAngle; }
//
//        void						setYawAngle( double in_ya ) { m_yawAngle = in_ya; }
//        double						getYawAngle() const { return m_yawAngle; }
//        
//        void						setModelScale( double in_ms ) { m_modelScale = in_ms; }
//        double						getModelScale() const { return m_modelScale; }
//
//		void						adjustFactor(bool accel);
//		osg::Vec3d					getEye() const { return m_eye; }
//
//		//	void computePosition(const osg::Vec3& eye, const osg::Vec3& lv, const osg::Vec3& up);
//
//		void						moveFront(bool positive);
//		void						moveSide(bool positive); 
//		void						moveUp(bool positive);
//
//		void						rotatePitch(bool positive);
//		void						rotateRoll(bool positive);
//		void						rotateYaw(bool positive,double distance);
//		void						rotateYaw(bool positive,double distance,double Radians);
//		void		_stop();// ʹ�ƶ��ٶȺ���ת�ǶȾ�Ϊ��
//		void		_keyDown(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
//		void		_keyUp(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
//	   //void		collisionDetect(osg::Vec3& delta);
//    protected:
//
//		virtual ~PersonManipulator() {}		
//
//		osg::ref_ptr<osg::Node>		m_pNode;
//        
//		// ģ�͵Ĵ�С
//		double						m_modelScale;
//        
//        // �ӵ��λ������̬
//		osg::Vec3d					m_eye;
//		
//		// ��ǰ�ӵ�ľֲ�����ϵ
//		osg::Vec3d					m_sideVector;
//		osg::Vec3d					m_frontVector;		
//		osg::Vec3d					m_upVector;
//		//��¼��ǰ���λ��
//		int                         m_iLeftx;
//		int                         m_iLefty;
//		bool                        m_bLeftDown;
//	//osg::Vec3d target;
//
//		double						m_speedEpsilon;		// �ٶȵľ���ֵ,С�ڸ�ֵ�ٶ�Ϊ��
//		double						m_forwardSpeed;		// ǰ���ƶ����ٶ�
//		double						m_sideSpeed;		// �����ƶ����ٶ�
//		double						m_upSpeed;			// �����ƶ����ٶ�
//		double						m_speedFactor;		// �ƶ��ٶȵ�������          
//
//		double						m_angleEpsilon;		// ��ת�Ƕȵľ���ֵ,С�ڸ�ֵ��ת�Ƕ�Ϊ��
//		double						m_pitchAngle;		// ������ת�ĽǶ�
//		double						m_rollAngle;		// ��ת��ת�ĽǶ�
//		double						m_yawAngle;			// ƫ����ת�ĽǶ�
//		double						m_angleFactor;		// ��ת�Ƕȵ�������
//		long                        m_pretime��;
//		bool						m_shift;			// �Ƿ���Shift��
//		bool						m_ctrl;				// �Ƿ���Ctrl��
//	//	bool						m_collision;        // �Ƿ�֧����ײ���
//};
class YIZHU_EXPORT  PersonManipulator : public osgGA::FirstPersonManipulator
{
public:
    PersonManipulator():m_Coordinatehelper(NULL){}
    void moveFront(double distance)
    {
        
       moveForward(distance);
        
    }

	void moveRight(const double distance)
	{
		osgGA::FirstPersonManipulator::moveRight(distance);
	}

	void moveUp(const double distance)
	{
		osgGA::FirstPersonManipulator::moveUp(distance);
	}
  
    void moveRound(double distance)
    {
        auto mat = getMatrix() * osg::Matrix::inverse(osg::Matrix::translate(_homeCenter)) * osg::Matrix::inverse(osg::Matrix::rotate(_rotation));
        auto trans = mat.getTrans();
        //auto matDistance = trans.z();
        
        //double eyeDistance=(double)_eye.length2();
        double angle,x,y,z;
        _rotation.getRotate(angle,x,y,z);
        _rotation = osg::Matrix::rotate(angle,osg::Vec3d(x,y,z)).getRotate();
        if(_eye[2] >= 3)
            rotateYawPitch(_rotation,(distance/_eye[2])*3,0);
        else
            rotateYawPitch(_rotation,distance,0);
      
    }
    virtual void setByMatrix(const osg::Matrix& mat) override
    {
        osgGA::FirstPersonManipulator::setByMatrix(mat);
    }
    
    void setPosition(const osg::Vec3& eye, const osg::Vec3& center, const osg::Vec3& up, const osg::Matrix& matrix)
    {
        setHomePosition(eye, center, up);
        home(1);
        _eye = matrix.getTrans();
        auto mat = matrix * osg::Matrix::inverse(osg::Matrix::translate(_eye));
        _rotation = mat.getRotate();
    }
    
    void setScreenRotation(const osg::Quat& rotation)
    {
        m_screenRotation = rotation;
    }
    
    void setScreenDistance(double distance)
    {
        m_screenDistance = osg::Vec3(0,0,distance);
    }
    
    void translation(double startx,double starty,double endx,double endy,osg::Camera* camera)
    {
        if(!m_Coordinatehelper)
			m_Coordinatehelper = new  util::CoordinateHelper(camera);
        osg::Vec3 startWndCoordination(startx,starty,0);
        osg::Vec3 endtWndCoordination(endx,endy,0);
        osg::Vec3 startWorldCoordination;
        osg::Vec3 endWorldCoordination;
        if(!m_Coordinatehelper)
            return;
        startWorldCoordination=m_Coordinatehelper->ScreenToWorld(startWndCoordination);
        endWorldCoordination=m_Coordinatehelper->ScreenToWorld(endtWndCoordination);
        osg::Vec3 moveVector=startWorldCoordination-endWorldCoordination;
        moveVector=(moveVector/std::abs(m_screenDistance.z()*atan(osg::PI/180.0*15))*300)*osg::Matrix::rotate(m_screenRotation.inverse());//* osg::Matrix::inverse(osg::Matrix::translate(m_screenDistance))
        
        _eye+=_rotation*(osg::Vec3d(moveVector[1]*1.58,moveVector[0]*1.58,moveVector[2]*1.58));
        
    }
protected:
    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& /*aa*/)
    {
        return true;
    }
private:
    util::CoordinateHelper*m_Coordinatehelper;
    osg::Quat m_screenRotation;
    osg::Vec3 m_screenDistance;
};

#endif // __PERSON_MANIPULATOR_H__