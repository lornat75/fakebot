#include <stdio.h>

#include <yarp/os/Network.h>

#include "FakeBot.h"

#include <yarp/dev/Drivers.h>
#include <yarp/dev/PolyDriver.h>

using namespace yarp::os;
using namespace yarp::dev;

int main()
{
    Network yarp;

    //Magic code starts. This is normally hidden, but I am making it
    //explicit to simplify porting as a plugin of Gazebo. 
    Drivers::factory().add(new DriverCreatorOf<FakeBot>("fakebot2", 
        "controlboardwrapper2",
        "FakeBot"));

    // printf("List of known devices (make sure fakebot2 is listed):\n");
    // printf("%s\n", yarp::dev::Drivers::factory().toString().c_str());
 
    PolyDriver driver;
    Property parameters;
    parameters.put("device", "controlboard");
    parameters.put("subdevice", "fakebot2");
    parameters.put("name", "/fakebot/head");
    
    driver.open(parameters);

    if (!driver.isValid())
    {
       fprintf(stderr, "Device did not open, returning\n");
       return -1;
    }
    
    printf("Device initialized correctly, now sitting and waiting\n");

    char c=0;

    while(c!='q')
    { 
      printf("Hit q+enter to quit\n");
      scanf(" %c", &c);
    }

    printf("Goodbye!\n");
    driver.close();
        
    return 0;
}
