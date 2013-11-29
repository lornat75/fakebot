#include <stdio.h>

#include <yarp/os/Network.h>

#include "FakeBot.h"

#include <yarp/dev/Drivers.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Wrapper.h>

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

    printf("List of known devices (make sure fakebot2 and controlboardwrapper2 are listed):\n");
    printf("%s\n", yarp::dev::Drivers::factory().toString().c_str());
 
    PolyDriver driver;
    Property parameters;
    parameters.put("device", "fakebot2");
    driver.open(parameters);

    if (!driver.isValid())
    {
       fprintf(stderr, "Device did not open, returning\n");
       return -1;
    }


    PolyDriver wrapperHead;
    PolyDriver wrapperTorso;

    //Init for head part. Notice: this initialization usually goes to a file
    Property paramsHead;
    Value tmp;
    paramsHead.put("device", "controlboardwrapper2");
    paramsHead.put("rootName", "coman");
    paramsHead.put("name", "coman/head");
    tmp.fromString("(fakebot)");
    //head has 3 joints
    paramsHead.put("joints", 3);
    paramsHead.put("networks", tmp);
    //map joints 0-2 from fakebot to 0-2 of part coman/head
    tmp.fromString("(0 2 0 2)");  
    paramsHead.put("fakebot", tmp);

    //Init for torso part. Notice: this initialization usually goes to a file
    Property paramsTorso;
    paramsTorso.put("device", "controlboardwrapper2");
    paramsTorso.put("rootName", "coman");
    paramsTorso.put("name", "coman/torso");
    tmp.fromString("(fakebot)");
    //torso has 3 joints
    paramsTorso.put("joints", 2);
    paramsTorso.put("networks", tmp);
    //map joints 0-1 from fakebot to 3-4 of part coman/torso
    tmp.fromString("(0 1 3 4)");
    paramsTorso.put("fakebot", tmp);

    IMultipleWrapper *iwrapperHead, *iwrapperTorso;

    wrapperHead.open(paramsHead);

    if (!wrapperHead.isValid())
    {
       fprintf(stderr, "Head wrapper not open, returning\n");
       return -1;
    }

    wrapperTorso.open(paramsTorso);

    if (!wrapperTorso.isValid())
    {
       fprintf(stderr, "Device wrapper not open, returning\n");
       return -1;
    }

    wrapperHead.view(iwrapperHead);
    wrapperTorso.view(iwrapperTorso);
    
    PolyDriverList list;
    list.push(&driver, "fakebot");
    iwrapperHead->attachAll(list);
    iwrapperTorso->attachAll(list);
    
    printf("Device initialized correctly, now sitting and waiting\n");

    char c=0;

    while(c!='q')
    { 
      printf("Hit q+enter to quit\n");
      scanf(" %c", &c);
    }

    printf("Goodbye!\n");
    wrapperHead.close();
    wrapperTorso.close();

    driver.close();

    //here you are sure that the fakebot does not use the shared pointers
    //it is safe to delete it

    world=0; //in real life you call destroy/delete in Gazebo
         
    return 0;
}
