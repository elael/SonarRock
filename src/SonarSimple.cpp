#include "SonarSimple.hpp"

#include <iostream>

namespace sonar_simple{
  
  
float SonarBeamGL::camera_high = 5;

void SonarBeamGL::addBearing(double degBearing)
{
sonar_beam_buffer.bearing=base::Angle.fromDeg(sonar_beam_buffer.bearing.getDeg()+degBearing);
}



SonarBeamGL::postprocessing::postprocessing(SonarBeamGL* thisSonar): sonar(thisSonar) {} 

void SonarBeamGL::postprocessing::operator()(osg::RenderInfo& renderInfo) const
{
       //copy data into SonarBeam
      sonar->sonar_beam_buffer.time = base::Time::now();
      
      sonar->sonar_beam_buffer.getSpatialResolution();
      
      sonar->z = *(float*)(sonar->zImageData->data((sonar->pixel_resolution_x-1)/2,(sonar->pixel_resolution_y-1)/2));//30,360
      
      std::cout << sonar->pixel_resolution_x << " " << sonar->pixel_resolution_y << std::endl;
      
     // float true_distance = farplane*nearplane/(farplane - z*(farplane-nearplane));
      
      (*sonar->sendDistance)( sonar->farplane*sonar->nearplane/(sonar->farplane - sonar->z*(sonar->farplane-sonar->nearplane)));
      
}

void SonarBeamGL::Configure(const Config& config)
{
  //OSG configuration
  setpixelresoltiony(720);
  nearplane=config.minimum_range;
  farplane=config.maximum_range;
  aspect_ratio = tan(config.beamwidth_horizontal/2.0)/tan(config.beamwidth_vertical/2.0);
  pixel_resolution_x = (int)(2*floor(round(aspect_ratio*pixel_resolution_y)/2.0)+1);

  
  //SonarBeam configuration
  sonar_beam_buffer.sampling_interval = config.sampling_interval;
  sonar_beam_buffer.speed_of_sound = config.speed_of_sound;
  sonar_beam_buffer.beamwidth_vertical = config.beamwidth_vertical;
  sonar_beam_buffer.beamwidth_horizontal = config.beamwidth_horizontal;
  sonar_beam_buffer.bearing = base::Angle.fromDeg(0);
  
  std::cout << "config beamwidth h" << RadiantoDegree(config.beamwidth_horizontal) << " v" << RadiantoDegree(config.beamwidth_vertical) << std::endl;
  
//   sonar_beam_buffer.beamwidth_vertical = 35;
}



SonarBeamGL::SonarBeamGL(const Config& config, osg::ref_ptr< osg::Node > scene)
{
	Configure(config);

	root=scene;
	
	initial_settings();

}

void SonarBeamGL::initial_settings()
{
        // Simulation variables
	viewer.setSceneData( root );

	
	//Save to images
	
	
	colorImage= new osg::Image;
	zImageData = new osg::Image;
// 	zImage = new osg::Image;

	colorImage->allocateImage(pixel_resolution_x, pixel_resolution_y, 1, GL_RGB, GL_UNSIGNED_BYTE);
	zImageData->allocateImage(pixel_resolution_x, pixel_resolution_y, 1, GL_DEPTH_COMPONENT ,GL_FLOAT); 
// 	zImage->allocateImage(720, 576, 1, GL_DEPTH_COMPONENT ,GL_UNSIGNED_BYTE); 
	
	
	//Set up camera


	osgCam = new osg::Camera;
	
	osgCam = viewer.getCamera();
	
	osgCam->setClearColor(osg::Vec4(0.1f,0.1f,0.3f,1.0f));
	osgCam->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	
	
	osgCam->setViewport(new osg::Viewport(0,0,pixel_resolution_x,pixel_resolution_y)); //720, 576
	
	osgCam->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	osgCam->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
	osgCam->setProjectionMatrixAsPerspective(RadiantoDegree(sonar_beam_buffer.beamwidth_vertical), aspect_ratio, nearplane, farplane); 
  
	//osgCam->setRenderTargetImplementation( osg::Camera::PIXEL_BUFFER );
	osgCam->attach(osg::Camera::COLOR_BUFFER, colorImage);
 	osgCam->attach(osg::Camera::DEPTH_BUFFER, zImageData); //CANNOT HAVE TWO DEPTH_BUFFER ATTACHMENTS
//	osgCam->attach(osg::Camera::DEPTH_BUFFER, zImage); 
	
	SonarCenter = osg::Vec3(0,0,camera_high);
	SonarLookto = osg::Vec3(0,0,0); //Used in Distance()

	
	//pbuffer context begin

	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new
	osg::GraphicsContext::Traits;
	traits->x =0;
	traits->y = 0;
	traits->width = pixel_resolution_x;
	traits->height = pixel_resolution_y; 
	traits->windowDecoration = false;
	traits->doubleBuffer = false;
	traits->sharedContext = 0;
	traits->useMultiThreadedOpenGLEngine = false;
	traits->pbuffer = true;

	osg::GraphicsContext* _gc = osg::GraphicsContext::createGraphicsContext(traits.get());

	osgCam->setGraphicsContext(_gc); 

	//pbuffer context end

	postprocessor = new postprocessing(this);
	
	osgCam->setPostDrawCallback(postprocessor);
	
	viewer.setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
}


osg::ref_ptr<osg::Node> SonarBeamGL::CreateSimpleScene()
{
  
	osg::ref_ptr<osg::PositionAttitudeTransform> cylinderPlaced;
	osg::ref_ptr<osg::ShapeDrawable> boxDrawable;
	osg::ref_ptr<osg::PositionAttitudeTransform> boxPlaced;
	osg::ref_ptr<osg::ShapeDrawable> sphereDrawable;
	osg::ref_ptr<osg::Cylinder> cylinder;
	osg::ref_ptr<osg::Box> box;
	osg::ref_ptr<osg::Sphere> sphere;
	osg::ref_ptr<osg::ShapeDrawable> cylinderDrawable;
	osg::ref_ptr<osg::Geode> geode[4]; 	
	osg::ref_ptr<osg::Geometry> planeGeo;
  
	/*Variable Declaration*/
	osg::Vec3   center;
	float      height;
	float	radius;

	center = osg::Vec3(0.0f, 0.0f, 0.0f);
	height = 2;
	radius = 0.1;
	
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
	
	//   A geode to hold our figures:
	
	// The Cylinder with Sphere
	geode[0] = new osg::Geode;
	geode[0]->addDrawable(cylinderDrawable); 
	geode[0]->addDrawable(sphereDrawable); 
	cylinderPlaced = new osg::PositionAttitudeTransform();
	cylinderPlaced->setPosition(osg::Vec3d(-2,0,1.5f));
	cylinderPlaced->addChild(geode[0]);

	// The box
	geode[1] = new osg::Geode;
	geode[1]->addDrawable(boxDrawable); 
	boxPlaced = new osg::PositionAttitudeTransform();
  	boxPlaced->setPosition(osg::Vec3d(0,-0.5,1));
// 	boxPlaced->setPosition(osg::Vec3d(0,0,1));
	boxPlaced->addChild(geode[1]);

	// The Sphere
	geode[2] = new osg::Geode;
	geode[2]->addDrawable(sphereDrawable); 

	// The floor
	geode[3] = new osg::Geode;
	geode[3]->addDrawable(planeGeo); 


	   
	//  Add the cylinder to an existing group  
	osg::ref_ptr<osg::Group> scene = new osg::Group();
	scene->addChild(cylinderPlaced);
	scene->addChild(boxPlaced); 
	scene->addChild(geode[2]); 
	scene->addChild(geode[3]); 
	
	return scene;

}

void SonarBeamGL::getBeam(base::samples::SonarBeam& Beam)
{
//   std::cout << SonarLookto.x() << SonarLookto.y() << SonarLookto.z() << std::endl; //Cout Lookto
//   osg::Matrixd camrotate;
//   camrotate.makeRotate();
  
  osgCam->setViewMatrixAsLookAt(SonarCenter, SonarLookto, osg::Vec3(0,1,0) );
  sonar_beam_out = &Beam;
  viewer.frame();
  
  
  //For test purposes:
  
  osgDB::writeImageFile(*(colorImage),"color.bmp");
    
  std::cout << "sonar bearing : " << sonar_beam_buffer.bearing << std::endl;
  
  //osgDB::writeImageFile(*(zImage),"depth.bmp");  //Cannot be used at the same time as zImageData
  
  std::cout << "z value : " << z << std::endl;
  
  //std::cout << "z value : " << (z/(pow(2.0,24)-1.0)) << std::endl; 

}

void SonarBeamGL::setDataSender(void (*senderFunction)(float))
{
  sendDistance=senderFunction;  
}

 
}

using namespace sonar_simple;

void coutData(float data){
  std::cout << "z distance value : " <<  data << std::endl;
}

int main(int argc, char** argv)
{
	SonarBeamGL sonar;
	
	sonar.setDataSender(coutData);
	
	base::samples::SonarBeam test;
	
	sonar.getBeam(test);
		
	return 0;//viewer.run();
}