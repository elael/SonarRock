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
#include <osg/Matrixd>
#include <math.h>
#include "Config.hpp"
#include "base/samples/sonar_beam.h"

namespace sonar_simple
{
  
  class SonarBeamGL{

  double beamradius_horizontal;
  double beamradius_vertical;
  float nearplane;
  float farplane;
  int pixel_resolution_y;
  int pixel_resolution_x;
  double aspect_ratio;
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
  
  osg::ref_ptr<postprocessing> postprocessor;
  base::samples::SonarBeam sonar_beam_buffer;
  base::samples::SonarBeam* sonar_beam_out;
  
  void initial_settings();
  
public:
  void setpixelresoltiony(int npixel){pixel_resolution_y = 2*floor(npixel/2.0)+1;};
  SonarBeamGL(const Config& config = Config(), osg::ref_ptr<osg::Node> scene = CreateSimpleScene());
  
  static osg::ref_ptr<osg::Node> CreateSimpleScene();
  void Configure(const Config& config);
  void getBeam(base::samples::SonarBeam &Beam);
  void setDataSender(void (*senderFunction)(float));
  void setBearing(double bearing);
  void addBearing(double degBearing);
  void Distanceto(osg::Vec3 lookto,base::samples::SonarBeam &Beam){setLookto(lookto);return getBeam(Beam);};
  void Distancefrom(osg::Vec3 center,base::samples::SonarBeam &Beam){setSonarPosition(center);return getBeam(Beam);};
  void setLookto(osg::Vec3 lookto){SonarLookto=lookto;};
  void setSonarPosition(osg::Vec3 center){SonarCenter=center;};
  
  
};


} // end namespace sonar_simple

#endif // _SONAR_SIMPLE_HPP_
