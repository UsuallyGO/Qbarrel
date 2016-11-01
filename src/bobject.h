
#ifndef __BARREL_OBJECT_H__
#define __BARREL_OBJECT_H__

typedef struct bObject_t bObject;

struct bObject_t
{
    UInt m_type : 4; //bit field, 4 bits is enough to identify the type of the object
    UInt m_encoding : 4;
    Count m_refcount;

    VPtr m_ptr; //void* pointer, points the real object
};

#endif //!__BARREL_OBJECT_H__