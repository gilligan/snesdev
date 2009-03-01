#ifndef TILEPTR_LIST_H
#define TILEPTR_LIST_H

#include <qptrlist.h>

template <class t>
class TilePtrList : public QPtrList<t>
{
    public:

    TilePtrList() : QPtrList<t>() {}
    TilePtrList( const QPtrList<t> & list ) : QPtrList<t>( list ) {}

    ~TilePtrList(){}

    protected:

    int compareItems( QPtrCollection::Item item1, QPtrCollection::Item item2 )
    {
        if ( item1 == item2 ) return 0;
        else return 1;
    }
    
    
};
#endif // TILEPTR_LIST_H
