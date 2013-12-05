/**
 * Copyright (C) 2013 iCub Facility Istituto Italiano di Tecnologia 
 * Author Lorenzo Natale
 * email:   lorenzo.natale@iit.it
 * 
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/

#include <stdio.h>
#include <string.h>

#include <yarp/os/Network.h>

#include "FakeBot.h"

#include <yarp/dev/Drivers.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Wrapper.h>

using namespace yarp::os;
using namespace yarp::dev;

#include "getWorld.h"

using namespace std;

int main(int argc, const char **argv)
{
    Network yarp;

    Property p;
    p.fromCommand(argc, argv);

    string name;

    if (p.check("name"))
        name=p.find("name").asString().c_str();
    else
        name="robby";

    //create here pointer (in real life you call a function from Gazebo)
    void *world=(void *)(0x0A);
    //fill shared pointer
    World::setWorld(world);

    //Magic code starts. This is normally hidden, but I am making it
    //explicit to simplify porting as a plugin of Gazebo. 

    Drivers::factory().add(new DriverCreatorOf<FakeBot>("fakebot", 
        "controlboardwrapper2",
        "FakeBot"));

    printf("List of known devices (make sure fakebot2 and controlboardwrapper2 are listed):\n");
    printf("%s\n", yarp::dev::Drivers::factory().toString().c_str());
 
    PolyDriver driver;
    Property parameters;
    parameters.put("device", "fakebot");
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
    string tmpString;
    tmpString=name+"/"+"head";

    paramsHead.put("name", tmpString);
    tmp.fromString("(fakebot)");
    //head has 3 joints
    paramsHead.put("joints", 3);
    paramsHead.put("networks", tmp);
    //map joints 0-2 from fakebot to 0-2 of part robot/head
    tmp.fromString("(0 2 0 2)");  
    paramsHead.put("fakebot", tmp);

    //Init for torso part. Notice: this initialization usually goes to a file
    Property paramsTorso;
    paramsTorso.put("device", "controlboardwrapper2");
    
    tmpString=name+"/"+"torso";
    paramsTorso.put("name", tmpString);
    tmp.fromString("(fakebot)");
    //torso has 3 joints
    paramsTorso.put("joints", 2);
    paramsTorso.put("networks", tmp);
    //map joints 0-1 from fakebot to 3-4 of part robot/torso
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
