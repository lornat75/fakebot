// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007-2013 Istituto Italiano di Tecnologia iCub Facility
 * Authors: Lorenzo Natale and Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "FakeBot.h"

#include <yarp/sig/all.h>
#include <yarp/sig/ImageFile.h>
#include <yarp/os/all.h>


using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::draw;
using namespace yarp::sig::file;
using namespace yarp::dev;

#include "getWorld.h"

bool FakeBot::open(yarp::os::Searchable& config) {

    return RateThread::start();
}

bool FakeBot::threadInit()
{
    printf("FakeBot thread started with world:%p\n", World::getWorld());
    return true;    
}

void FakeBot::threadRelease()
{
    printf("FakeBot thread closed\n");
}

void FakeBot:: run()
{
    static int count=0;
    count++;
    if (count%100==0)
    {
        printf("======\nFakeBot thread looped [%d] times\n", count);
        printf("Current position: %s\n", pos.toString().c_str());
        printf("Current reference position: %s\n", ref_pos.toString().c_str());
        printf("Reference speed: %s\n", ref_speed.toString().c_str());
    }
    
    for(int j=0; j<njoints; j++)
    {
        // handle position mode
        if (control_mode[j]==VOCAB_CM_POSITION)
        {
            if ( (pos[j]-ref_pos[j]) < -ROBOT_POSITION_TOLERANCE)
            {
                pos[j]=pos[j]+ref_speed[j]*RateThread::getRate()/1000.0;
                motion_done[j]=false;
            }
            else if ( (pos[j]-ref_pos[j]) >ROBOT_POSITION_TOLERANCE)
            {
                pos[j]=pos[j]-ref_speed[j]*RateThread::getRate()/1000.0;
                motion_done[j]=false;
            }
            else
                motion_done[j]=true;
        }
        
        pos[j]=pos[j]+Random::normal(0,0.01);
    }
}


    
