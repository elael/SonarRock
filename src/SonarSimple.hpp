#ifndef _SONAR_SIMPLE_HPP_
#define _SONAR_SIMPLE_HPP_

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
#include <math.h>

namespace sonar_simple
{
  
  class SonarBeamGL{

  static int beamRadius;
  static float nearplane;
  static float farplane;
  static float camera_high;
  
  
  /*Variable Declaration*/
  osg::Vec3   SonarCenter;
  osg::Vec3   SonarLookto;

  
  /*Node Pointer Declaration*/
  osg::ref_ptr<osg::Node> root;

  
  //Viewer importants
  osgViewer::Viewer viewer;
  osg::ref_ptr<osg::Camera> osgCam;
  
  
  //Save to images
  osg::ref_ptr<osg::Image> colorImage;
  osg::ref_ptr<osg::Image> zImage;
  osg::ref_ptr<osg::Image> zImageData;
  
  float z;
  
  
  //Export data
  void (*sendDistance)(float);
  
  class postprocessing: public osg::Camera::DrawCallback
  {
  public:
      SonarBeamGL* sonar;
      
      postprocessing(SonarBeamGL* thisSonar);
    
      void operator()(osg::RenderInfo& renderInfo) const;
  };
  friend class postprocessing;
  
  postprocessing* postprocessor;
  
public:
  SonarBeamGL(osg::ref_ptr<osg::Node> scene);
  
  static osg::ref_ptr<osg::Node> CreateSimpleScene();
  void Distance();
  void setDataSender(void (*senderFunction)(float));
  void Distanceto(osg::Vec3 lookto){setLookto(lookto);return Distance();};
  void Distancefrom(osg::Vec3 center){setSonarPosition(center);return Distance();};
  void setLookto(osg::Vec3 lookto){SonarLookto=lookto;};
  void setSonarPosition(osg::Vec3 center){SonarCenter=center;};
  
  
};


} // end namespace sonar_simple

#endif // _SONAR_SIMPLE_HPP_
