/***************************************************************************

    file                 : ficos_continuous.cpp
    created              : Sat Dec 10 17:09:02 CST 2016
    copyright            : (C) 2002 YurongYou

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <tgf.h>
#include <track.h>
#include <car.h>
#include <raceman.h>
#include <robottools.h>
#include <robot.h>

static tTrack	*curTrack;

static void initTrack(int index, tTrack* track, void *carHandle, void **carParmHandle, tSituation *s);
static void newrace(int index, tCarElt* car, tSituation *s);
static void drive(int index, tCarElt* car, tSituation *s);
static void endrace(int index, tCarElt *car, tSituation *s);
static void shutdown(int index);
static int  InitFuncPt(int index, void *pt);


/*
 * Module entry point
 */
extern "C" int
ficos_continuous(tModInfo *modInfo)
{
    memset(modInfo, 0, 10*sizeof(tModInfo));

    modInfo->name    = strdup("ficos_continuous");		/* name of the module (short) */
    modInfo->desc    = strdup("");	/* description of the module (can be long) */
    modInfo->fctInit = InitFuncPt;		/* init function */
    modInfo->gfId    = ROB_IDENT;		/* supported framework version */
    modInfo->index   = 1;

    return 0;
}

/* Module interface initialization. */
static int
InitFuncPt(int index, void *pt)
{
    tRobotItf *itf  = (tRobotItf *)pt;

    itf->rbNewTrack = initTrack; /* Give the robot the track view called */
				 /* for every track change or new race */
    itf->rbNewRace  = newrace; 	 /* Start a new race */
    itf->rbDrive    = drive;	 /* Drive during race */
    itf->rbPitCmd   = NULL;
    itf->rbEndRace  = endrace;	 /* End of the current race */
    itf->rbShutdown = shutdown;	 /* Called before the module is unloaded */
    itf->index      = index; 	 /* Index used if multiple interfaces */
    return 0;
}

/* Called for every track change or new race. */
static void
initTrack(int index, tTrack* track, void *carHandle, void **carParmHandle, tSituation *s)
{
    curTrack = track;
    *carParmHandle = NULL;
}

static double clutchtime = 0;
static bool AutoReverseEngaged = false;

static double getAutoClutch(int gear, int newgear, tCarElt *car)
{
    if (newgear != 0 && newgear < car->_gearNb) {
        if (newgear != gear) {
            clutchtime = 0.332f - ((double) newgear / 65.0f);
        }

        if (clutchtime > 0.0f)
            clutchtime -= RCM_MAX_DT_ROBOTS;
        return 2.0f * clutchtime;
    }

    return 0.0f;
}

/* Start a new race. */
static void
newrace(int index, tCarElt* car, tSituation *s)
{
    AutoReverseEngaged = false;
    clutchtime = 0;
}


// /* Compute gear. */
// const float SHIFT = 0.85;         /* [-] (% of rpmredline) */
// const float SHIFT_MARGIN = 4.0;  /* [m/s] */

// int getGear(tCarElt *car)
// {
//     if (car->_gear <= 0)
//         return 1;
//     float gr_up = car->_gearRatio[car->_gear + car->_gearOffset];
//     float omega = car->_enginerpmRedLine/gr_up;
//     float wr = car->_wheelRadius(2);

//     if (omega*wr*SHIFT < car->_speed_x) {
//         return car->_gear + 1;
//     } else {
//         float gr_down = car->_gearRatio[car->_gear + car->_gearOffset - 1];
//         omega = car->_enginerpmRedLine/gr_down;
//         if (car->_gear > 1 && omega*wr*SHIFT > car->_speed_x + SHIFT_MARGIN) {
//             return car->_gear - 1;
//         }
//     }
//     return car->_gear;
// }


/* check if the car is stuck */
// const float MAX_UNSTUCK_SPEED = 5.0;   /* [m/s] */
// const float MIN_UNSTUCK_DIST = 3.0;    /* [m] */
// const float MAX_UNSTUCK_ANGLE = 20.0/180.0*PI;
// const int MAX_UNSTUCK_COUNT = 250;
// static int stuck = 0;

// bool isStuck(tCarElt* car)
// {
//     float angle = RtTrackSideTgAngleL(&(car->_trkPos)) - car->_yaw;
//     NORM_PI_PI(angle);

//     if (fabs(angle) > MAX_UNSTUCK_ANGLE &&
//         car->_speed_x < MAX_UNSTUCK_SPEED &&
//         fabs(car->_trkPos.toMiddle) > MIN_UNSTUCK_DIST) {
//         if (stuck > MAX_UNSTUCK_COUNT && car->_trkPos.toMiddle*angle < 0.0) {
//             return true;
//         } else {
//             stuck++;
//             return false;
//         }
//     } else {
//         stuck = 0;
//         return false;
//     }
// }


/* Drive during race. */
extern double* psteerCmd;
extern double* paccelCmd;
extern double* pbrakeCmd;

extern double* pspeed;
extern double* pangle_in_rad;
extern int* pdamage;
extern double* ppos;
extern int* _pisEnd;

// Compute the length to the start of the segment.
float getDistToSegStart(tCarElt *ocar)
{
    if (ocar->_trkPos.seg->type == TR_STR) {
        return ocar->_trkPos.toStart;
    } else {
        return ocar->_trkPos.toStart*ocar->_trkPos.seg->radius;
    }
}

#define CMD_GEAR_R  4
#define CMD_GEAR_N  5
#define CMD_GEAR_1  6



/* Drive during race. */
static void
drive(int index, tCarElt* car, tSituation *s)
{
    memset(&car->ctrl, 0, sizeof(tCarCtrl));

    // if (isStuck(car)) {
    //     float angle = -RtTrackSideTgAngleL(&(car->_trkPos)) + car->_yaw;
    //     NORM_PI_PI(angle); // put the angle back in the range from -PI to PI

    //     car->ctrl.steer = angle / car->_steerLock;
    //     car->ctrl.gear = -1; // reverse gear
    //     car->ctrl.accelCmd = 0.9; // 90% accelerator pedal
    //     car->ctrl.brakeCmd = 0.0; // no brakes
    // }
    // else {
/////////////////////////////////////// by Yurong
    double angle = RtTrackSideTgAngleL(&(car->_trkPos)) - car->_yaw;
    NORM_PI_PI(angle);

    // car->ctrl.gear = getGear(car);


    car->ctrl.steer = *psteerCmd;
    car->ctrl.accelCmd = *paccelCmd;
    car->ctrl.brakeCmd = *pbrakeCmd;


    // auto gear
    int gear = car->_gear;
    gear += car->_gearOffset;
    car->_gearCmd = car->_gear;

    if (!AutoReverseEngaged) {
        tdble omega = car->_enginerpmRedLine * car->_wheelRadius(2) * 0.95;
        tdble shiftThld = 10000.0f;
        if (car->_gearRatio[gear] != 0) {
            shiftThld = omega / car->_gearRatio[gear];
        }

        if (car->pub.speed > shiftThld) {
            car->_gearCmd++;
        } else if (car->_gearCmd > 1) {
            if (car->pub.speed < (omega / car->_gearRatio[gear-1] - 4.0)) {
                car->_gearCmd--;
            }
        }

        if (car->_gearCmd <= 0) {
            car->_gearCmd++;
        }
    }

    // auto reverse
    if (AutoReverseEngaged) {
        /* swap brake and throttle */
        double brake = car->_brakeCmd;
        car->_brakeCmd = car->_accelCmd;
        car->_accelCmd = brake;
    }

    if (!AutoReverseEngaged) {
        if ((car->_brakeCmd > car->_accelCmd) && (car->_speed_x < 1.0)) {
            AutoReverseEngaged = 1;
            car->_gearCmd = -1;
        }
    } else {
        /* currently in autoreverse mode */
        if ((car->_brakeCmd > car->_accelCmd) && (car->_speed_x > -1.0) && (car->_speed_x < 1.0)) {
            AutoReverseEngaged = 0;
            car->_gearCmd = 1;
        } else {
            car->_gearCmd = -1;
        }
    }

    car->_clutchCmd = getAutoClutch(car->_gear, car->_gearCmd, car);

    *pspeed = car->_speed_x;
    *pangle_in_rad = angle;
    *pdamage = car->_dammage;
    *ppos = car->_trkPos.toMiddle;

    if (car->_state & (RM_CAR_STATE_ELIMINATED | RM_CAR_STATE_BROKEN | RM_CAR_STATE_OUTOFGAS | RM_CAR_STATE_FINISH)){
        *_pisEnd = 1;
    }
    // testing
    // printf("now angle: %lf\n", angle);
    // printf("now speed: %lf\n", car->_speed_x);
    // printf("now tangent speed: %lf\n", car->_speed_x * cos(angle));
    //printf("now _dammage: %d\n", car->_dammage);
    // printf("now steer: %lf\n", car->_steerCmd);
    // printf("now accel: %lf\n", car->_accelCmd);
    // printf("now brake: %lf\n", car->_brakeCmd);
    // printf("now gear: %d\n", car->_gearCmd);
/////////////////////////////////////// end by Yurong
    // }
}

/* End of the current race */
static void
endrace(int index, tCarElt *car, tSituation *s)
{
}

/* Called before the module is unloaded */
static void
shutdown(int index)
{
}

