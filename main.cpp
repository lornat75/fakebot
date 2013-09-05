#include <stdio.h>

#include <yarp/os/Network.h>

#include "FakeBot.h"

#include <yarp/dev/Drivers.h>
#include <yarp/dev/PolyDriver.h>

using namespace yarp::os;
using namespace yarp::dev;

#include "getWorld.h"

int main()
{
    Network yarp;


    //create here pointer (in real life you call a function from Gazebo)
    void *world=(void *)(0x0A);
    //fill shared pointer
    World::setWorld(world);

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

    //here you are sure that the fakebot does not use the shared pointer
    //it is safe to delete it

    world=0; //in real life you call destroy/delete in Gazebo
         
    return 0;
}
