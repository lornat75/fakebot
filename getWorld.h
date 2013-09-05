#ifndef __WORLD__
#define __WORLD__

/*
  This is an helper class that keeps a shared pointer.
*/
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

