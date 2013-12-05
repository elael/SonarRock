#include <iostream>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg/Geode>
#include <osg/Shape>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/Geometry>
#include <osg/ref_ptr>
#include <osg/ShapeDrawable>
#include <osg/Plane>
#include <osg/CullSettings>
#include <osg/PositionAttitudeTransform>
#include <osgDB/WriteFile>
#include <unistd.h>
#include <math.h>

int main(int argc, char** argv)
{
	/*Variable Declaration*/
    osg::Vec3   center;
    float      height;
	float	radius;

	center = osg::Vec3(1.0f, 0.0f, 0.0f);
	height = 2;
	radius = 0.1;
	
	/*Node Pointer Declaration*/
	osg::ref_ptr<osg::PositionAttitudeTransform> cylinderPlaced;
	osg::ref_ptr<osg::ShapeDrawable> boxDrawable;
	osg::ref_ptr<osg::PositionAttitudeTransform> boxPlaced;
	osg::ref_ptr<osg::ShapeDrawable> sphereDrawable;
	osg::ref_ptr<osg::Material> pMaterial;
	osg::ref_ptr<osg::Cylinder> cylinder;
	osg::ref_ptr<osg::Box> box;
	osg::ref_ptr<osg::Sphere> sphere;
	osg::ref_ptr<osg::ShapeDrawable> cylinderDrawable;
	osg::ref_ptr<osg::Geode> geode[4]; 	
	osg::ref_ptr<osg::Group> root;	
	osg::ref_ptr<osg::Node> somenode;
	osg::ref_ptr<osg::Texture2D> KLN89FaceTexture;
	osg::ref_ptr<osg::Image> klnFace;
	osg::ref_ptr<osg::StateSet> stateOne;
	osg::ref_ptr<osg::Geometry> planeGeo;
	
	// The Ground Plane
	planeGeo = new osg::Geometry();
	osg::Vec3 myCoords[] =
	{
	osg::Vec3(10.0f, 10.0f, 0.0f),
	osg::Vec3(-10.0f, 10.0f, 0.0),
	osg::Vec3(-10.0f, -10.0f, 0.0),
	osg::Vec3(10.0f, -10.0f, 0.0),
	};
        
	int numCoords = sizeof(myCoords)/sizeof(osg::Vec3);
        
	osg::Vec3Array* vertices = new osg::Vec3Array(numCoords,myCoords);
    
	planeGeo->setVertexArray(vertices);
	planeGeo->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,numCoords));


	//	A drawable cylinder
    cylinder = new osg::Cylinder(center,radius,height); 
	cylinderDrawable = new osg::ShapeDrawable(cylinder );

	//	A drawable box
    box = new osg::Box(center,0.7f*radius); 
	boxDrawable = new osg::ShapeDrawable(box);

	//	A drawable sphere
    sphere = new osg::Sphere(center,1.5f*radius); 
	sphereDrawable = new osg::ShapeDrawable(sphere );
	
	//   A geode to hold our figures
	geode[0] = new osg::Geode;
	geode[0]->addDrawable(cylinderDrawable); 
	geode[0]->addDrawable(sphereDrawable); 
	cylinderPlaced = new osg::PositionAttitudeTransform();
	cylinderPlaced->setPosition(osg::Vec3d(-2,0,1.5f));
	cylinderPlaced->addChild(geode[0]);

	geode[1] = new osg::Geode;
	geode[1]->addDrawable(boxDrawable); 
	boxPlaced = new osg::PositionAttitudeTransform();
	boxPlaced->setPosition(osg::Vec3d(0,-1,1));
	boxPlaced->addChild(geode[1]);

	geode[2] = new osg::Geode;
	geode[2]->addDrawable(sphereDrawable); 

	geode[3] = new osg::Geode;
	geode[3]->addDrawable(planeGeo); 


	   
	//  Add the cylinder to an existing group  
	root = new osg::Group();
	root->addChild(cylinderPlaced);
	root->addChild(boxPlaced); 
	root->addChild(geode[2]); 
	root->addChild(geode[3]); 
	//root->addChild(osgDB::readNodeFile("cessna.osg"));//"D:\\Documents\\3dsMax\\export\\wooddebri3.3DS")

	//boxDrawable = new osg::ShapeDrawable(osg::InfinitePlane(osg::Plane(0,0,1,0)));
	//root->addChild((geode[3] = new osg::Geode())->addDrawable(new osg::ShapeDrawable(osg::Plane(0,0,1,0))));
	







        // Simulation variables

	int beamRadius = 700;
	float nearplane = 30;
	float farplane = 200;

	osgViewer::Viewer viewer;
	viewer.setSceneData( root );


	//Set up camera

	osg::ref_ptr<osg::Camera> osgCam = new osg::Camera;
	
	osgCam = viewer.getCamera();
	
	osg::ref_ptr<osg::Image> colorImage= new osg::Image;
	osg::ref_ptr<osg::Image> zImage = new osg::Image;
	osg::ref_ptr<osg::Image> zImageData = new osg::Image;

	colorImage->allocateImage(720, 576, 1, GL_RGBA, GL_UNSIGNED_BYTE);
	zImage->allocateImage(720, 576, 1, GL_DEPTH_COMPONENT ,GL_UNSIGNED_BYTE); 
	zImageData->allocateImage(720, 576, 1, GL_DEPTH_COMPONENT ,GL_FLOAT); 
	
	osgCam->setViewport(new osg::Viewport(0,0,720,576)); 

	
	osgCam->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	osgCam->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
	osgCam->setProjectionMatrixAsPerspective(45.0, 1.0, nearplane, farplane); 
	osgCam->setViewMatrixAsLookAt(osg::Vec3(0,0,30.001), osg::Vec3(0,0,0), osg::Vec3(0,1,1) );
  
	//osgCam->setRenderTargetImplementation( osg::Camera::PIXEL_BUFFER );
	osgCam->attach(osg::Camera::COLOR_BUFFER, colorImage);
	osgCam->attach(osg::Camera::DEPTH_BUFFER, zImage); 
	osgCam->attach(osg::Camera::DEPTH_BUFFER, zImageData); 

	
	//pbuffer context begin

	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new
	osg::GraphicsContext::Traits;
	traits->x =0;
	traits->y = 0;
	traits->width = 720;
	traits->height = 576; 
	traits->windowDecoration = false;
	traits->doubleBuffer = false;
	traits->sharedContext = 0;
	traits->useMultiThreadedOpenGLEngine = false;
	//traits->pbuffer = true;

	osg::GraphicsContext* _gc = osg::GraphicsContext::createGraphicsContext(traits.get());

	osgCam->setGraphicsContext(_gc); 

	//pbuffer context end

	viewer.frame();

	usleep(10000);
	osgDB::writeImageFile(*colorImage,"color.bmp");
	osgDB::writeImageFile(*zImage,"depth.bmp"); 
	

	float z = ((float*)zImageData->data())[1];
	std::cout << "z value : " << z << std::endl; 
	//std::cout << "z value : " << (z/(pow(2.0,24)-1.0)) << std::endl; 

	float true_distance = farplane*nearplane/(farplane - z*(farplane-nearplane));
	std::cout << "z distance value : " <<  true_distance << std::endl; 


	return 0;
}
