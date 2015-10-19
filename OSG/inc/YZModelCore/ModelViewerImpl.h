#ifndef __MODEL_VIEWER_IMPL_H__
#define __MODEL_VIEWER_IMPL_H__
#include "DataModel.h"
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgDB/DatabasePager>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osg/Material>
#include <osg/StateSet>
#include <osgUtil/Optimizer>
#include <osg/Group>
#include <osg/BlendFunc>
#include <boost/bimap.hpp>
#include <boost/unordered_map.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <vector>
#include <map>
#include <string>
#include "ManipulatorType.h"
#include "BillboardFactory.h"
//#include "ConstructMaterialSet.h"
#include "CameraAnimator.h"
#include <osgGA/FirstPersonManipulator>
#include "AnimationEditor.h"
#include "FixedPathAnimation.h"

namespace bimWorld
{
	class ModelViewerImpl;
	class CRenderingThread;

	osg::Material *constructMaterial(const MaterialSetting&);

	void constructMaterial(const MaterialSetting&, osg::Material* material);

	typedef std::vector<ModelViewerListener*> ListenerSet;


	class ModelViewerImpl
	{

		////////////////////////////////////////////////////////���ݽṹ����////////////////////////////////////////////////////////////////
		// ��ͼ����
		enum ViewDirection
		{
			Front,
			Back,
			Left,
			Right,
			Top,
			Bottom,
		};

		//�ڵ�ָ��id˫�����
		typedef boost::bimap<boost::bimaps::unordered_set_of<osg::Node*>, boost::bimaps::unordered_set_of<std::string> > NodeptrIdBimaps;

		//�ڵ�ָ��id˫�����
		typedef boost::bimap<boost::bimaps::unordered_set_of<osg::Node*>, boost::bimaps::unordered_set_of<std::string> > NodeptrUUIDBimaps;

		typedef std::map<YZString, osg::ref_ptr<osg::Node> > NodePtrNameBimaps;

		//����/ID��hash��
		//typedef boost::unordered_map<int, AttrConstruct> IdAttrHash;

		typedef boost::unordered_map<std::string, AttrConstruct> UuidAttrHash;
		 
		typedef boost::bimap<std::string,std::string> IdsToUuidBimaps;//ģ�����1 revite����ID&����ID��ģ�����2:����UUID 

		

		IdsToUuidBimaps		m_IdsToUuidBimaps;

		UuidAttrHash		m_UuidAttrHash;

		NodeptrUUIDBimaps	m_NodeptrUUIDBimaps;

	public:
		ModelViewerImpl();

		virtual ~ModelViewerImpl();

		ModelViewerImpl(const ModelViewerImpl&);

		ModelViewerImpl& operator=(const ModelViewerImpl&);

	public:


		//=============================cancelFixedFramerate()=============================
		/// @brief void cancelFixedFramerate();
		///
		/// ȡ���̶�֡��
		///
		/// @return void
		///
		/// @sa startFixedFramerate()
		//================================================================================
		void cancelFixedFramerate();

		//=============================startFixedFramerate()=============================
		/// @brief void startFixedFramerate();
		///
		/// ���ù̶�֡��
		///
		/// @return void
		///
		/// @sa cancelFixedFramerate()
		//================================================================================
		void startFixedFramerate();

		bool findAllParentsOfNode(osg::Node* n, std::vector<osg::Node*>& outVector);

		///////////////////////////////////////////////////////////////////�������//////////////////////////////////////////////////////////////////////////
		void completeCameraAnimator();

		void updateCameraAnimator(const CameraAnimator* ca);

		virtual bool zoomTo(void *node, bool withoutAnimation = true, ViewDirection direction = Front);

		virtual bool zoomTo(const std::string *node, int length, bool withoutAnimation = true, ViewDirection direction = Front);

		virtual bool zoomTo(void **node, int length, bool withoutAnimation = true, ViewDirection direction = Front);

		std::vector<bimWorld::CameraVector> createAnimationPath(osg::Vec3d pos, osg::Vec3d center, osg::Vec3d up, float delta_time = 50);

		std::vector<ConstructKey> getCategoryKeys();

		bool getCategoryKey(const std::string& id, ConstructKey& outKey);

		bool getVisibility(const std::string& id, bool& visible);

		virtual void hideAll();

		virtual void unHideAll();

		virtual void hide(void *node) { hide(&node, 1); }

		virtual void unHide(void *node) { unHide(&node, 1); }

		virtual void hideOthers(void* node);

		virtual void hideOthers(const std::string& id);

		virtual void unHideOthers(void* node);

		virtual void unHideOthers(const std::string& id);

		virtual void toggleHide(void *node) { toggleHide(&node, 1); }

		virtual bool isVisible(void *node);

		virtual void hide(void **nodes, int n);

		virtual void unHide(void **nodes, int n); 

		virtual long long getCurrentProgressInfo();

		virtual void unTransParentAll();

		virtual void transparentAll();

		virtual void toggleHide(void **nodes, int n);

		virtual bool getCameraView(CameraView& cameraView);

		virtual bool getCameraView(CameraView& cameraView, const char* strFileAllPath, unsigned char** buffer, int &width, int &height, int &channels);

		bool CaptureScreen(unsigned char** buffer, int & width, int & height, int & channels, int x = 0, int y = 0, const char* file = NULL);

		virtual bool getCameraPerspective(CameraPerspective& cameraPerspective);

		virtual bool setCameraView(const CameraView& cameraView);

		virtual bool setCameraPerspective(const CameraPerspective& cameraPerspective);

		virtual void setSteps(int steps = 30);

		//////////////////////////////////////////////////////// ������Ⱦ����¼��Ĳ��� //////////////////////////////////////////////////////////////////////////////////

#pragma region ---- Render When Needed Events ----

		/**********************************************************************************************//**
		 * @fn	virtual void ModelViewerImpl::setNeedUpdateAndDraw(bool value, int delay = 0);
		 *
		 * @brief	notify rendering thread to render a frame or not.
		 *
		 * @author	Sqh
		 * @date	2015/5/13
		 *
		 * @param	value	true to value.
		 * @param	delay	The delay.
		 *
		 * ### remarks	Sqh, 2015/5/6.
		 **************************************************************************************************/

		virtual void	setNeedUpdateAndDraw(bool value, int delay = 0);

		/**********************************************************************************************//**
		 * @fn	virtual bool ModelViewerImpl::getNeedUpdateAndDraw();
		 *
		 * @brief	get whether rendering thread will render a frame or not.
		 *
		 * @author	Sqh
		 * @date	2015/5/13
		 *
		 * @return	true if it succeeds, false if it fails.
		 *
		 * ### remarks	Sqh, 2015/5/6.
		 **************************************************************************************************/
		
		virtual bool	getNeedUpdateAndDraw();

		/**********************************************************************************************//**
		 * @fn	virtual void ModelViewerImpl::updateSeveralTimes(int times = 3);
		 *
		 * @brief	update and draw several times.
		 *
		 * @author	Sqh
		 * @date	2015/5/13
		 *
		 * @param	times	The times.
		 *
		 * ### remarks	Sqh, 2015/5/6.
		 **************************************************************************************************/

		virtual void	updateSeveralTimes(int times = 3);

		/**********************************************************************************************//**
		 * @fn	virtual void ModelViewerImpl::setIsExternalRendering(bool value);
		 *
		 * @brief	if there is any external rendering need, we must notify the rendering thread.
		 *
		 * @author	Sqh
		 * @date	2015/5/13
		 *
		 * @param	value	true to value.
		 *
		 * ### remarks	Sqh, 2015/5/6.
		 **************************************************************************************************/

		virtual void	setIsExternalRendering(bool value);

		/**********************************************************************************************//**
		 * @fn	void ModelViewerImpl::onlyUpdateFrame();
		 *
		 * @brief	update scene without rendering.
		 *
		 * @author	Sqh
		 * @date	2015/5/13
		 *
		 * ### remarks	Sqh, 2015/5/6.
		 **************************************************************************************************/

		void			onlyUpdateFrame();

#pragma endregion


		//////////////////////////////////////////////////////// <��ͼ�л�����ز���> //////////////////////////////////////////////////////////////////////////////////

#pragma region ---- Switch View Related Operation ----

		/**********************************************************************************************//**
		 * @fn	virtual void ModelViewerImpl::switchMatrixManipulator(EMANIPULATOR emanip);
		 *
		 * @brief	<�л�������.>
		 *
		 * @author	Sqh
		 * @date	2015/5/13
		 *
		 * @param	emanip	The emanip.
		 *
		 * ### remarks	Sqh, 2015/5/6.
		 **************************************************************************************************/

		virtual void switchMatrixManipulator(EMANIPULATOR emanip);
        
        
        void enableFirstPersonControl();
        
        void disableFirstPersonControl();
        

		osgGA::CameraManipulator* getCameraManipulator();

		bimWorld::ViewerMode getCameraManipulatorMode();

		osg::Camera*            getCamera();

		/**********************************************************************************************//**
		 * @fn	virtual void bimWorld::ModelViewerImpl::switchToFrontView();
		 *
		 * @brief	<�л�������ͼ.>
		 *
		 * @author	Sqh
		 * @date	2015/5/13
		 *
		 * ### remarks	Sqh, 2015/5/6.
		 **************************************************************************************************/

		virtual void switchToFrontView();

		/**********************************************************************************************//**
		 * @fn	virtual void bimWorld::ModelViewerImpl::switchToBackView();
		 *
		 * @brief	<�л�������ͼ.>
		 *
		 * @author	Sqh
		 * @date	2015/5/13
		 *
		 * ### remarks	Sqh, 2015/5/6.
		 **************************************************************************************************/

		virtual void switchToBackView();

		/**********************************************************************************************//**
		 * @fn	virtual void ModelViewerImpl::switchToLeftView();
		 *
		 * @brief	<�л�������ͼ.>
		 *
		 * @author	Sqh
		 * @date	2015/5/13
		 *
		 * ### remarks	Sqh, 2015/5/6.
		 **************************************************************************************************/

		virtual void switchToLeftView();

		/**********************************************************************************************//**
		 * @fn	virtual void bimWorld::ModelViewerImpl::switchToRightView();
		 *
		 * @brief	<�л�������ͼ.>
		 *
		 * @author	Sqh
		 * @date	2015/5/13
		 *
		 * ### remarks	Sqh, 2015/5/6.
		 **************************************************************************************************/

		virtual void switchToRightView();

		/**********************************************************************************************//**
		 * @fn	virtual void bimWorld::ModelViewerImpl::switchToTopView();
		 *
		 * @brief	<�л�������ͼ.>
		 *
		 * @author	Sqh
		 * @date	2015/5/13
		 *
		 * ### remarks	Sqh, 2015/5/6.
		 **************************************************************************************************/

		virtual void switchToTopView();

		/**********************************************************************************************//**
		 * @fn	virtual void ModelViewerImpl::switchToBottomView();
		 *
		 * @brief	<�л�������ͼ.>
		 *
		 * @author	Sqh
		 * @date	2015/5/13
		 *
		 * ### remarks	Sqh, 2015/5/6.
		 **************************************************************************************************/

		virtual void switchToBottomView();

#pragma endregion


		/////////////////////////////////////////////////////////////////////////ģ�Ͳ���///////////////////////////////////////////////////////////////

		//��ȡȫ��ID�����Զ�Ӧ��ϵ

		
		// <��ȡȫ���ڵ�id>
		virtual void getAll(std::vector<std::string>& ids);

		virtual void getByFloor(std::vector<std::string>& ids, const Floor* floor);

		virtual void getByCategory(std::vector<std::string>& ids, const Category* category);

		bool setHighLightByCategory(const YZString& floor, const YZString& topCategory, const YZString& secondaryCategory, bool isHighLight);

		virtual void load(const char *file) { load(&file, 1); }

		virtual void load(const char **files, int fileNum);

		virtual  void	readNodeByMultiFle(const char **files, int fileNum);
		// <�������ģʽ,���ر��㲻��ж�ص�֮ǰ¥��>
		//return
		//����ֵ0��ʾ���سɹ������� -1��ʾ����ʧ�ܣ�����1��ʾ��¥���Ѿ����ع������ظ�����
		//files:ive�ļ�ȫ·��
		//dEngineeringLevel:��¥��Ĺ��̱��
		virtual int		loadBySingleFlr(const char *files, double dEngineeringLevel, long& outFloorNode);

		virtual bool unLoadSingleFlr(osg::Node* floorNode);

		virtual void unLoadAll();

		//�����������ģʽ
		//���ֵvec:¥�����뷵��ֵ��ֵ�ԣ�����ֵ0��ʾ���سɹ������� -1��ʾ����ʧ�ܣ�����1��ʾ��¥���Ѿ����ع������ظ�����
		//files:¥ive�ļ�ȫ·������
		//dEngineeringLevelvec:��¥��Ĺ��̱�߼�����filesһһ��Ӧ
		virtual int	loadByMultiFlr(	const char **files,
									int		  ifileSize,
									std::vector<double>& dEngineeringLevelvec,
									std::vector<std::pair<YZString, std::string> >&	vecresut);
		//����ģ����ͼ
		virtual	void	updateMode();

		virtual osg::Node* readNode(const char **files, int fileNum);

		virtual osg::ref_ptr<osg::Node> readFBXNode(const char **files, int fileNum);

		virtual osg::Node* readNode(const char *file);

		virtual osg::ref_ptr<osg::Node> readFBXNode(const char *file);

		virtual void writeNode(osg::Node *node, const char *file);
		//��������
		virtual void loadParam(const char *file);

		virtual void loadParam(const char **files, int fileNum);

		virtual osg::ref_ptr<osg::Group> reorganize();

		virtual osg::ref_ptr<osg::Group> reorganizeFbx();

		virtual osg::Node* readIveNode(const char *files, int fileNum );

		/////////////////////////////////////////////////////////////////////////��Ӧ�¼�///////////////////////////////////////////////////////////////
		virtual void initOSG();

		virtual void initGraphicContext(osg::GraphicsContext::Traits* traits) {}


		bool registerModelListener(ModelViewerListener *listener);

		bool removeModelListener(ModelViewerListener *listener);

		void initCamera(osg::Camera* camera);

		// <����һ���������>
		void createTopMostCamera(osg::Camera* camera);

		// <����Ĭ��ͼ���豸���Բ���>
		virtual osg::GraphicsContext::Traits* createDefaultTraits();
		
		// <��Ӹ������>
		void addSlaveCamera(osg::GraphicsContext::Traits* traits);

		void preFrameUpdate(void);

		void postFrameUpdate(void);

		void setDone(bool value) { m_bdone = value; }

		bool getDone(void) { return m_bdone; }

		void startRendering();

		std::string findIdByNode(osg::Node *node) const;

		osg::Node* findNodeById(const std::string& id) const;

		void findNodesByIds(const std::string *ids, int n, std::vector<void*>& nodes);

		// <����id��ȡ����>
		const AttrConstruct* getConstruct(int id) const;

		const AttrConstruct* getConstruct(std::string strUuid) const;
		

		virtual osg::StateSet* getSelectionRenderModel();


		virtual osg::BlendFunc* getBlendFunc();

		void	getClassNode(bimWorld::YZString nodename, std::vector<osg::Node*>& nodevec);

		// <��ȡ�����������ӽڵ�>
		void	getCategoryNodes(LevelName level, CategoryName category, std::vector<osg::Node*>& nodevec);

		// <��ȡ�������ڵ�>
		void getCategoryGroups(LevelName level, CategoryName category, std::vector<osg::Group*>& nodevec);

		osgViewer::ViewerBase* getViewer() const { return m_mViewer; }


		osg::MatrixTransform* getModelRoot();

		const char* getName(void* node);

		// <�������ƻ�ýڵ�>
		osg::Node* getNode(bimWorld::YZString nodename);

		void highlight(void* node);

		void unHighlight(void* node);

		void clearSelection();

		void setToTransparent(osg::Node* node);

		void setOtherNodeToTransparent(void *node);// <���ó�ѡ�нڵ����ڵ����ߵĽڵ�͸��>

		void setOtherNodeToTransparent(void **node, int length);// <���ó�ѡ�нڵ�Ľڵ�͸��>

		void unTransparent(void* node);

		void setStateColorToConstructs(void* node, int red, int green, int blue);

		bool revertColorOfNode(void* node);
		
		bool setLastAdditionalColor(void* ptr, int port = 0);

		bool getChildren(osg::Group* node, std::vector<void*>& children);

		const ListenerSet& getListeners() const { return m_modelViewerListeners; }

		static bool registerDefaultModelListener(ModelViewerListener *listener);

		static bool removeDefaultModelListener(ModelViewerListener *listener);

		virtual void setViewerMode(ViewerMode viewerMode);

		// draw one frame. the param indicates whether this function is called by rendering thread
		virtual void frame(const bool isRenderingThread = false);

		// for cross platform purpose, only bmp format is supported now.
		virtual void insertImage(double x, double y, double z, const char *imageFile);

		virtual osg::StateSet* createStateSetByMaterial(osg::Material* material);
		
		virtual void setCategoryMaterial(const std::vector<std::string>& ids, const MaterialSetting& materialSetting);
		
		virtual void setCategoryMaterial(const CategoryName& categoryName, const SystemType& systemType, const LevelName& levelName, const std::vector<std::string>& ids, const MaterialSetting& materialSetting);

		std::map<bimWorld::CategoryName, bimWorld::SystemType> xmlMap;

		virtual void setCategoryAndSystemTypes(const CategoryName& categoryName, const SystemType& systemType);

		//virtual osg::Material* getCategoryMaterial(const CategoryName& categoryName, const SystemType& systemType, const LevelName& levelName, int subMatNum) const;

		//virtual std::vector<std::pair<CategoryName, osg::ref_ptr<osg::Material> > > getSystemTypeMaterials(const SystemType& systemType) const;

		//virtual std::vector<std::pair<CategoryName, std::vector<osg::ref_ptr<osg::Group> > > > getSystemTypeGroups(const SystemType& systemType) const;

		osg::Camera* getWindowSpace();

		//�����켣��������
		virtual osgGA::CameraManipulator* createCameraManipulator();

		//������һ�˳�������
		virtual osgGA::CameraManipulator* createPersonManipulator();

		virtual void setAbortRendering() { m_babortRendering = true; }

		//////////////////////////////////////////////////// ��ӹ���� ///////////////////////////////////////////////////////
#pragma region ---- Add Billboard ----

		/**********************************************************************************************//**
		 * @fn	virtual osg::ref_ptr<osg::Node> ModelViewerImpl::createBillboard(osg::Vec3 position, osg::Geometry* geometry = NULL, osg::Image* image = NULL);
		 *
		 * @brief	Creates a billboard.
		 *
		 * @author	Sqh
		 * @date	2015/5/13
		 *
		 * @param	position			The position.
		 * @param [in,out]	geometry	(Optional) If non-null, the geometry.
		 * @param [in,out]	image   	(Optional) If non-null, the image.
		 *
		 * @return	The new billboard.
		 *
		 * ### remarks	Sqh, 2015/5/6.
		 **************************************************************************************************/

		virtual osg::Node* createBillboard(osg::Vec3 position, osg::Geometry* geometry = NULL, osg::Image* image = NULL);

		/**********************************************************************************************//**
		 * @fn	int ModelViewerImpl::addBillboard( int selectedConstruct[], const void(*callback)(BillboardData*, const void*), const char* img_path_name = NULL, const void* externalData = NULL);
		 *
		 * @brief	Adds a billboard.
		 *
		 * @author	Sqh
		 * @date	2015/5/13
		 *
		 * @param	selectedConstruct	The selected construct.
		 * @param [in,out]	callback 	If non-null, the callback.
		 * @param	img_path_name	 	Full pathname of the image file.
		 * @param	externalData	 	Information describing the external.
		 *
		 * @return	An int.
		 *
		 * ### remarks	Sqh, 2015/5/6.
		 **************************************************************************************************/

		int		addBillboard(std::string selectedConstruct[],
			const void(*callback)(BillboardData*, const void*),
			const char* img_path_name = NULL,
			const void* externalData = NULL);

		/**********************************************************************************************//**
		 * @fn	bool ModelViewerImpl::removeBillboard(int id);
		 *
		 * @brief	Removes the billboard described by ID.
		 *
		 * @author	Sqh
		 * @date	2015/5/13
		 *
		 * @param	id	The identifier.
		 *
		 * @return	true if it succeeds, false if it fails.
		 *
		 * ### remarks	Sqh, 2015/5/6.
		 **************************************************************************************************/

		bool removeBillboard(int id);

		/**********************************************************************************************//**
		 * @fn	bool ModelViewerImpl::isTopMostNode(osg::Node* node);
		 *
		 * @brief	Query if 'node' is top most node.
		 *
		 * @author	Sqh
		 * @date	2015/5/13
		 *
		 * @param [in,out]	node	If non-null, the node.
		 *
		 * @return	true if top most node, false if not.
		 *
		 * ### remarks	Sqh, 2015/5/6.
		 **************************************************************************************************/

		bool isTopMostNode(osg::Node* node);

		/**********************************************************************************************//**
		 * @fn	void ModelViewerImpl::triggerBillboardCallback(osg::Node* node);
		 *
		 * @brief	Callback, called when the trigger billboard.
		 *
		 * @author	Sqh
		 * @date	2015/5/13
		 *
		 * @param [in,out]	node	If non-null, the node.
		 *
		 * ### remarks	Sqh, 2015/5/6.
		 **************************************************************************************************/

		void triggerBillboardCallback(osg::Node* node);

#pragma endregion

		/**********************************************************************************************//**
		 * @fn	bool bimWorld::ModelViewerImpl::isLoaded();
		 *
		 * @brief	Query if the model is loaded.
		 *
		 * @author	Sqh
		 * @date	2015/5/13
		 *
		 * @return	true if loaded, false if not.
		 **************************************************************************************************/

		bool isLoaded();

		void setKeyFrame(int framePosition);

		void enableAnimationEditor();

		void disableAnimationEditor();

		void setAnimatingNodes(osg::Node* node);

		void setAnimatingNodes(std::vector<osg::Node*> nodes);

		void addActionToSequece(const std::function<void()>& action, double delayMilliseconds);

		void clearActionSequece();

		void play();

		void stop();

		void playKeyFrame(int framePosition);

		void setFrameMode(bool isFrameMode);

		Animation::FixedPathAnimation* FixedAnimationPath(){ return m_fixedAnimationPath; }

		void beginRecordAnimationControlPoint();

		void endRecordAnimationControlPoint();
		
		void applyAnimationPath(osg::Node* node);

		bool getBackgroundColor(osg::Vec4f& backColor);

		void setBackgroundColor(const osg::Vec4f& newColor);
		
		void revertBackgroundColor();
		
		void setSkyBox(const std::string& hdr);
		
		void setSkyBox(const std::string& right, const std::string& left, const std::string& front, const std::string& back, const std::string& up, const std::string& down);

		void setCurrentDBPath(const std::string& path);

	protected:

		void updateMaterials();

		//void initializeCategoryGroups1(osg::Node *root);
		void initializeCategoryGroups(osg::Node *root);

		//void updateCategoryMaterial(const CategoryName& categoryName, const SystemType& systemType, const LevelName& levelName, int subMaterialNum);

		//void setCategoryMaterial(const CategoryName& categoryName, const SystemType& systemType, const LevelName& levelName, int subMaterialNum, osg::StateSet* material);

#if SUPPORT_CPP11
		// <�����������>
		void traverseAllSubNodes(osg::Node* node, std::function<bool(osg::Node*)> callBack, bool includeItsSelf);

		// <�����������>
		bool traverseSubNodesAndFindFirst(osg::Node* node, std::function<bool(osg::Node*)> isGotCallBack, bool includeItsSelf);
#endif // SUPPORT_CPP11

		
		void setNodeState(osg::Node* node, osg::Vec4 color);

		void traverseChildren(osg::Node* n);
	protected:

		// ���ƣ�true�����£�false����ִ�� Camera ����֮ǰ���ڻ��ƽ׶λ����ڽ����½׶Ρ�
		bool origin_draw;

		// �Ƿ��Ѿ�����
		bool m_isLoaded;
		// �Ƿ����
		bool m_bdone;
		//�Ƿ��һ�μ���
		int m_zoomtimes;
		// �Ƿ�ȡ����Ⱦ
		//����
		int       m_steps;
		bool m_babortRendering;

		//OSG��ͼ
		osg::ref_ptr<osgViewer::Viewer> m_mViewer;
		//osg::ref_ptr<osgViewer::View> m_mainView;

		// ������ڵ�
		osg::ref_ptr<osg::Group> m_topRoot;

		// ������ڵ�Ļص���������
		std::map<osg::ref_ptr<osg::Node>, const void(*)()> m_topMostCallback;

		// ����Ƶ����ݼ���
		std::map<osg::ref_ptr<osg::Node>, BillboardData*> m_billboardDataSet;

		// ����ƵĶ������ݼ���
		std::map<osg::ref_ptr<osg::Node>, const void*> m_billboardExternalDataSet;

		std::map<int, osg::ref_ptr<osg::Node> > m_topMostSet;

		osg::ref_ptr<util::CoordinateHelper> m_coordHelper;

		//��Ļ��ͼ
		osg::ref_ptr<osg::Group> m_sceneRoot;

		//ģ����ͼ
		osg::ref_ptr<osg::MatrixTransform> m_modelRoot;

		//ʰȡ״̬
		osg::ref_ptr<osg::StateSet> m_selectionRenderMode;

		//��ɫ���
		osg::ref_ptr<osg::BlendFunc> m_blendFunc;

		// <�����>
		osg::ref_ptr<osg::Camera> m_windowSpace;

		// <��ͼ����������>
		ListenerSet m_modelViewerListeners;

		//// <�ڵ�ָ����ID˫��map>
		//NodeptrIdBimaps m_nodeptrToIdBimaps;

		//// <�ڵ�ָ����ڵ�����˫��map>
		//NodePtrNameBimaps m_nodePtrNameBimaps;

		// <�ڵ�ID������hash��>
		//UuidAttrHash m_elementParameterHash; m_UuidAttrHash

		osg::ref_ptr<osgGA::CameraManipulator> m_d, m_p;

		// <���ڵ�idֵ>
		int m_newId;

		// <��Ⱦ�߳�>
		osg::ref_ptr<CRenderingThread> m_rendering;

		osg::ref_ptr<CameraAnimator> m_cameraAnimator;

		// <�����Ͳ������ݴ洢����>
		//osg::ref_ptr<ConstructMaterialSet> m_constructMaterialSet;

		// <�������л�>
		osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> m_ptrKeySwitchMatrixManipulator;

		// <�̻߳�����>
		OpenThreads::Mutex m_mutex;

		// <animation>
		osg::ref_ptr<Animation::AnimationEditor> m_animaEditor;

		// BIMCameraManipulator mode.
		bimWorld::ViewerMode m_cameraManipulatorMode;

		osg::ref_ptr<Animation::FixedPathAnimation> m_fixedAnimationPath;

		// <��պ�>
		osg::ref_ptr<osg::Node> m_skyBox;

		std::string m_currentPrjDBPath;
		
		//// <uuid ��ָ���Ӧ��ϵ>
		//std::map<std::string, int> m_idNodeptr;
		std::map<std::string, osg::Node*> m_idNodeptr;

	};
}
#endif // __MODEL_VIEWER_IMPL_H__





