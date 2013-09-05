#ifndef __WORLD__
#define __WORLD__

class World
{
        static void *world_ptr;
public:
        static void *getWorld()
        {
                return world_ptr;
        }

        static void setWorld(void *p)
        {
                world_ptr=p;
        }

};

#endif

