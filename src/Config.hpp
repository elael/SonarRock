#ifndef _SONAR_SIMPLE_CONFIG_HPP_
#define _SONAR_SIMPLE_CONFIG_HPP_
#include <math.h>

inline double DegreetoRadian(double degree){return degree*M_PIl/180;}
inline double RadiantoDegree(double radians){return radians*180/M_PIl;}

namespace sonar_simple
{

/**
* This configuration struct is a simple example of what you
* can do in order to wrap multiple configuration properties
* into a single object
* 
* This way you can manage configuration properties by grouping
* them into struct, and you don't have to change the oroGen 
* components interface when your configuration object changes
*/
struct Config
{
  
  //sampling interval of each range bin in secs
  double sampling_interval;

  //speed of sound
  //this takes the medium into account
  float speed_of_sound;

  //horizontal beamwidth of the sonar beam in radians
  float beamwidth_horizontal;

  //vertical beamwidth of the sonar beam in radians
  float beamwidth_vertical;

  //vertical beamwidth of the sonar beam in radians
  float maximum_range;

  //vertical beamwidth of the sonar beam in radians
  float minimum_range;

  Config()
    : sampling_interval(0.00002), speed_of_sound(1500), beamwidth_horizontal(DegreetoRadian(3)), beamwidth_vertical(DegreetoRadian(35)), maximum_range(75),minimum_range(0.3)
  {   
    
  }   

};

}
#endif // _SONAR_SIMPLE_CONFIG_HPP_