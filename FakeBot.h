// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007-2013 Istituto Italiano di Tecnologia iCub Facility
 * Authors: Lorenzo Natale and Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/dev/DeviceDriver.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IControlMode.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/Image.h>
#include <yarp/os/Time.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/Time.h>

namespace yarp {
    namespace dev {
      class FakeBot;
    }
}

const int ROBOT_REFRESH_PERIOD=20; //ms
const int ROBOT_NUMBER_OF_JOINTS=5;
const double ROBOT_POSITION_TOLERANCE=0.9;

class yarp::dev::FakeBot : public DeviceDriver,
            public IPositionControl, 
            public IVelocityControl,
            public IAmplifierControl,
            public IEncodersTimed, 
            public IControlCalibration2,
            public IControlLimits,
            public DeviceResponder,
            public IControlMode,
            private yarp::os::RateThread
{
private:
    int njoints;
    yarp::sig::Vector pos, vel, speed, acc, amp;
    yarp::sig::Vector ref_speed, ref_pos, ref_acc;
    yarp::sig::Vector max_pos, min_pos;
    yarp::sig::ImageOf<yarp::sig::PixelRgb> back, fore;

    bool *motion_done;
    int  *control_mode;

public:
    FakeBot():RateThread(ROBOT_REFRESH_PERIOD) {
        njoints = ROBOT_NUMBER_OF_JOINTS;
        pos.size(njoints);
        vel.size(njoints);
        speed.size(njoints);
        acc.size(njoints);
        amp.size(njoints);
        ref_speed.size(njoints);
        ref_pos.size(njoints);
        ref_acc.size(njoints);
        max_pos.size(njoints);
        min_pos.size(njoints);

        min_pos=-10;
        max_pos=10;

        pos = 0;
        vel = 0;
        speed = 0;
        ref_speed=0;
        ref_pos=0;
        ref_acc=0;
        acc = 0;
        amp = 1; // initially on - ok for simulator

        control_mode=new int[njoints];
        motion_done=new bool[njoints];

        for(int j=0; j<njoints; j++)
            control_mode[j]=VOCAB_CM_POSITION;
    }
    ~FakeBot()
    { 
        delete [] control_mode;
        delete [] motion_done;
    }

    // thread
    virtual bool threadInit(); 
    virtual void threadRelease(); 
    virtual void run();
   
    virtual bool open(yarp::os::Searchable& config);

    // IPositionControl etc.

    virtual bool getAxes(int *ax) {
        *ax = njoints;
        return true;
    }

    virtual bool setPositionMode() {
        return true;
    }

    virtual bool positionMove(int j, double ref) {
        if (j<njoints) {
            ref_pos[j] = ref;
        }
        return true;
    }

    virtual bool positionMove(const double *refs) {
        for (int i=0; i<njoints; i++) {
            ref_pos[i] = refs[i];
        }
        return true;
    }

    virtual bool relativeMove(int j, double delta) {
        if (j<njoints) {
            ref_pos[j] += delta;
        }
        return true;
    }


    virtual bool relativeMove(const double *deltas) {
        for (int i=0; i<njoints; i++) {
            ref_pos[i] += deltas[i];
        }
        return true;
    }


    virtual bool checkMotionDone(int j, bool *flag) {
        *flag=motion_done[j];
        return true;
    }


    virtual bool checkMotionDone(bool *flag) {
        *flag=true;
        for(int j=0; j<njoints; j++)
         { *flag&&motion_done[j]; }

        return true;
    }


    virtual bool setRefSpeed(int j, double sp) {
        if (j<njoints) {
            ref_speed[j] = sp;
        }
        return true;
    }


    virtual bool setRefSpeeds(const double *spds) {
        for (int i=0; i<njoints; i++) {
            ref_speed[i] = spds[i];
        }
        return true;
    }


    virtual bool setRefAcceleration(int j, double acc) {
        if (j<njoints) {
            ref_acc[j] = acc;
        }
        return true;
    }


    virtual bool setRefAccelerations(const double *accs) {
        for (int i=0; i<njoints; i++) {
            ref_acc[i] = accs[i];
        }
        return true;
    }


    virtual bool getRefSpeed(int j, double *ref) {
        if (j<njoints) {
            (*ref) = ref_speed[j];
        }
        return true;
    }


    virtual bool getRefSpeeds(double *spds) {
        for (int i=0; i<njoints; i++) {
            spds[i] = ref_speed[i];
        }
        return true;
    }


    virtual bool getRefAcceleration(int j, double *acc) {
        if (j<njoints) {
            (*acc) = ref_acc[j];
        }
        return true;
    }


    virtual bool getRefAccelerations(double *accs) {
        for (int i=0; i<njoints; i++) {
            accs[i] = ref_acc[i];
        }
        return true;
    }


    virtual bool stop(int j) {
        ref_pos[j]=pos[j];
        return true;
    }


    virtual bool stop() 
    {
        ref_pos=pos;
        return true;
    }


    virtual bool close() {
        return true;
    }

    virtual bool resetEncoder(int j) {
        if (j<njoints) {
            pos[j] = 0;
         }
        return true;
    }

    virtual bool resetEncoders() {
        for (int i=0; i<njoints; i++) {
            pos[i] = 0;
        }
        return true;
    }

    virtual bool setEncoder(int j, double val) {
        if (j<njoints) {
            pos[j] = val;
        }
        return true;
    }

    virtual bool setEncoders(const double *vals) {
        for (int i=0; i<njoints; i++) {
            pos[i] = vals[i];
        }
        return true;
    }

    virtual bool getEncoder(int j, double *v) {
       if (j<njoints) {
            (*v) = pos[j];
        }
        
        return true;
    }

    virtual bool getEncoderTimed(int j, double *v, double *t) {
        if (j<njoints) {
            (*v) = pos[j];
            (*t) = yarp::os::Time::now();
        }
        
        return true;
    }

    virtual bool getEncoders(double *encs) {
        for (int i=0; i<njoints; i++) {
            encs[i] = pos[i];
        }
        return true;
    }

    virtual bool getEncodersTimed(double *encs, double *times) {
        for (int i=0; i<njoints; i++) {
            encs[i] = pos[i];
            times[i]=yarp::os::Time::now();
        }
        return true;
    }

    virtual bool getEncoderSpeed(int j, double *sp) {
        if (j<njoints) {
            (*sp) = 0;
        }
        return true;
    }

    virtual bool getEncoderSpeeds(double *spds) {
        for (int i=0; i<njoints; i++) {
            spds[i] = 0;
        }
        return true;
    }
    
    virtual bool getEncoderAcceleration(int j, double *spds) {
        if (j<njoints) {
            (*spds) = 0;
        }
        return true;
    }

    virtual bool getEncoderAccelerations(double *accs) {
        for (int i=0; i<njoints; i++) {
            accs[i] = 0;
        }
        return true;
    }

    virtual bool velocityMove(int j, double sp) {
        if (j<njoints) {
            vel[j] = sp;
        }
        return true;
    }

    virtual bool velocityMove(const double *sp) {
        for (int i=0; i<njoints; i++) {
            vel[i] = sp[i];
        }
        return true;
    }

    virtual bool enableAmp(int j) {
        if (j<njoints) {
            amp[j] = 1;
            control_mode[j]=VOCAB_CM_POSITION;
        }
        return true;
    }

    virtual bool disableAmp(int j) {
        if (j<njoints) {
            amp[j] = 0;
            control_mode[j]=VOCAB_CM_IDLE;
        }
        return true;
    }

    virtual bool getCurrent(int j, double *val) {
        if (j<njoints) {
            val[j] = amp[j];
        }
        return true;
    }

    virtual bool getCurrents(double *vals) {
        for (int i=0; i<njoints; i++) {
            vals[i] = amp[i];
        }
        return true;
    }

    virtual bool setMaxCurrent(int j, double v) {
        return true;
    }

    virtual bool getAmpStatus(int *st) {
        *st = 0;
        return true;
    }

    virtual bool getAmpStatus(int k, int *v)
    {
        *v=0;
        return true;
    }

    virtual bool calibrate2(int j, unsigned int iv, double v1, double v2, double v3)
    {
        fprintf(stderr, "FakeBot: calibrating joint %d with parameters %u %lf %lf %lf\n", j, iv, v1, v2, v3);
        return true;
    }

    virtual bool done(int j)
    {
        fprintf(stderr , "FakeBot: calibration done on joint %d.\n", j);
        return true; 
    }

    // IControlLimits
    virtual bool getLimits(int axis, double *min, double *max)
    {
        *min=min_pos[axis];
        *max=max_pos[axis];
        return true;
    }
    
    virtual bool setLimits(int axis, double min, double max)
    {
        max_pos[axis]=max;
        min_pos[axis]=min;
        return true;
    }


    // IControlMode
    virtual bool setPositionMode(int j){control_mode[j]=VOCAB_CM_POSITION;}
    virtual bool setVelocityMode(int j){control_mode[j]=VOCAB_CM_VELOCITY;};

    virtual bool setVelocityMode() 
    {
       for(int j=0; j<njoints; j++)
           { control_mode[j]=VOCAB_CM_VELOCITY;}
    }
    
    virtual bool setTorqueMode(int j){ return false; };
    virtual bool setImpedancePositionMode(int j){return false;}
    virtual bool setImpedanceVelocityMode(int j){return false;}
    virtual bool setOpenLoopMode(int j){return false; }
    virtual bool getControlMode(int j, int *mode){*mode=control_mode[j];};
    virtual bool getControlModes(int *modes)
    {
        for(int j=0;j<njoints;j++)
            {modes[j]=control_mode[j];}
        return true;
    }
};

