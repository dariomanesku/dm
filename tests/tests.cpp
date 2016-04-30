#if 0
    g++ -I../include/ tests.cpp -o tests && ./tests && rm tests
    #g++ -g -I../include/ tests.cpp -o tests && gdb tests && rm tests
    exit
#endif

/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include <stdio.h>

#define DM_DEFINE_TYPES
#include <dm/ng/types.h>

#include <dm/ng/allocator.h>
#include <dm/ng/datastructures/array.h>
#include <dm/ng/datastructures/linkedlist.h>
#include <dm/ng/datastructures/handlealloc.h>
#include <dm/ng/datastructures/set.h>
#include <dm/ng/datastructures/list.h>
#include <dm/ng/datastructures/bitarray.h>
#include <dm/ng/datastructures/hashmap.h>
#include <dm/ng/datastructures/objhashmap.h>
#include <dm/ng/datastructures/common.h>

using namespace dm::ng;

struct Foo
{
    u32 m_a;
    u32 m_b;
};

template <typename ArrayTy>
void testArrayApi(ArrayTy& _array)
{
    _array.zero();
    _array.fillWith(1);

    u32* nums = _array.addNew(5);
    nums[0] = 12;
    nums[1] = 44;
    nums[2] = 66;
    nums[3] = 99;
    nums[4] = 553;

    _array.push(123);
    _array.pop();
    _array.pop(2);
    _array.remove(0);
    _array.removeSwap(1);

    u32 g0 = _array.get(0);
    _array[1]++;

    printf("Array out %d %d\n", g0, _array.count());
}

void testArrays()
{
    // Array with fixed size inline memory.
    typedef ArrayT<u32, 64> TestArrayT;
    TestArrayT array0;
    testArrayApi(array0);

    // Array with external memory.
    typedef ArrayExt<u32> TestArrayExt;
    TestArrayExt array1;
    u32 size = TestArrayExt::sizeFor(64);
    void* mem = dm_alloc(size, &::realloc);
    array1.init(64, (uint8_t*)mem);
    testArrayApi(array1);

    // Array with allocator.
    typedef Array<u32> TestArray;
    TestArray array2;
    array2.init(64, &::realloc);
    testArrayApi(array2);

    // Array as ptr.
    typedef ArrayH<u32> TestArrayH;
    TestArrayH* array3;
    array3 = create<TestArrayH>(64, &::realloc);
    testArrayApi(*array3);
    destroy(array3);
}

template <typename FooObjArrayTy>
void testObjArrayApi(FooObjArrayTy& _oa)
{
    _oa.zero();

    Foo* foo = _oa.addNew(5);
    foo[0].m_a = 12;
    foo[1].m_a = 44;
    foo[2].m_a = 66;
    foo[3].m_a = 99;
    foo[4].m_a = 553;

    Foo f0 = { 23, 67 };
    _oa.addCopy(&f0);

    _oa.pop();
    _oa.pop(2);
    _oa.remove(0);
    _oa.removeSwap(1);

    Foo* f1 = _oa.get(0);
    _oa[1].m_a++;

    printf("ObjArray out %d %d\n", f1->m_a, _oa.count());
}

void testObjArrays()
{
    // ObjArray with fixed size inline memory.
    typedef ObjArrayT<Foo, 64> TestObjArrayT;
    TestObjArrayT oa0;
    testObjArrayApi(oa0);

    // ObjArray with external memory.
    typedef ObjArrayExt<Foo> TestObjArrayExt;
    TestObjArrayExt oa1;
    u32 size = TestObjArrayExt::sizeFor(64);
    void* mem = dm_alloc(size, &::realloc);
    oa1.init(64, (uint8_t*)mem);
    testObjArrayApi(oa1);

    // ObjArray with allocator.
    typedef ObjArray<Foo> TestObjArray;
    TestObjArray oa2;
    oa2.init(64, &::realloc);
    testObjArrayApi(oa2);

    // ObjArray as ptr.
    typedef ObjArrayH<Foo> TestObjArrayH;
    TestObjArrayH* oa3;
    oa3 = create<TestObjArrayH>(64, &::realloc);
    testObjArrayApi(*oa3);
    destroy(oa3);
}

template <typename HandleAllocTy>
void testHandleAlloc(HandleAllocTy& _ha)
{
    _ha.alloc();
    _ha.alloc();
    _ha.alloc();
    _ha.alloc();
    u32 h0 = _ha.alloc();
    bool contains = _ha.contains(h0);
    _ha.free(h0);

    u32 h1 = _ha.getHandleAt(1);
    u32 h2 = _ha.getIdxOf(h1);
    u32 count = _ha.count();

    printf("HandleAlloc out %d %d %d %d %d\n", contains, h0, h1, h2, count);
}

void testHandleAllocs()
{
    // HandleAlloc with fixed size inline memory.
    typedef HandleAllocT<64> TestHandleAllocT;
    TestHandleAllocT ha0;
    testHandleAlloc(ha0);

    // HandleAlloc with external memory.
    typedef HandleAllocExt<u16> TestHandleAllocExt;
    TestHandleAllocExt ha1;
    u32 size = TestHandleAllocExt::sizeFor(64);
    void* mem = dm_alloc(size, &::realloc);
    ha1.init(64, (uint8_t*)mem);
    testHandleAlloc(ha1);

    // HandleAlloc with allocator.
    typedef HandleAlloc<u16> TestHandleAlloc;
    TestHandleAlloc ha2;
    ha2.init(64, &::realloc);
    testHandleAlloc(ha2);

    // HandleAlloc as ptr.
    typedef HandleAllocH<u16> TestHandleAllocH;
    TestHandleAllocH* ha3;
    ha3 = create<TestHandleAllocH>(64, &::realloc);
    testHandleAlloc(*ha3);
    destroy(ha3);
}

template <typename BitArrayTy>
void testBitArrayApi(BitArrayTy& _ba)
{
    _ba.set(0);
    _ba.set(1);
    _ba.set(22);
    _ba.set(23);

    _ba.unset(1);
    _ba.unset(22);

    _ba.toggle(1);
    _ba.toggle(1);
    _ba.toggle(1);

    _ba.toggle(0);
    _ba.toggle(63);
    _ba.toggle(0);

    u32 first = _ba.setFirst();
    u32 any   = _ba.setAny();

    u32 firstSet   = _ba.getFirstSetBit();
    u32 firstUnset = _ba.getFirstSetBit();

    u32 lastSet   = _ba.getLastSetBit();
    u32 lastUnset = _ba.getLastUnsetBit();

    bool b0 = _ba.isSet(0);
    bool b1 = _ba.isSet(1);
    bool b2 = _ba.isSet(22);
    bool b3 = _ba.isSet(23);
    bool b4 = _ba.isSet(63);

    u32 count = _ba.doCount();

    printf("Bit Array out %d %d | %d %d %d %d %d | %d %d %d %d | %d\n"
          , first, any
          , b0, b1, b2, b3, b4
          , firstSet
          , firstUnset
          , lastSet
          , lastUnset
          , count
          );
}

void testBitArrays()
{
    // BitArray with fixed size inline memory.
    typedef BitArrayT<64> TestBitArrayT;
    TestBitArrayT ba0;
    testBitArrayApi(ba0);

    // BitArray with external memory.
    typedef BitArrayExt TestBitArrayExt;
    TestBitArrayExt ba1;
    u32 size = TestBitArrayExt::sizeFor(64);
    void* mem = dm_alloc(size, &::realloc);
    ba1.init(64, (uint8_t*)mem);
    testBitArrayApi(ba1);

    // BitArray with allocator.
    typedef BitArray TestBitArray;
    TestBitArray ba2;
    ba2.init(64, &::realloc);
    testBitArrayApi(ba2);

    // BitArray as ptr.
    typedef BitArrayH TestBitArrayH;
    TestBitArrayH* ba3;
    ba3 = create<TestBitArrayH>(64, &::realloc);
    testBitArrayApi(*ba3);
    destroy(ba3);
}

template <typename SetTy>
void testSetApi(SetTy& _set)
{
    _set.insert(12);
    _set.insert(22);
    _set.insert(44);
    _set.insert(8);

    _set.remove(44);
    _set.remove(55);

    _set.safeInsert(234);
    _set.safeInsert(20394);
    _set.safeInsert(11);

    bool s0 = _set.contains(12);
    bool s1 = _set.contains(22);
    bool s2 = _set.contains(0);
    bool s3 = _set.contains(1);
    bool s4 = _set.contains(22);

    u32 count = _set.count();
    u32 idx = _set.indexOf(22);
    u32 val = _set.getValueAt(2);

    printf("Set output %d %d %d %d %d | %d %d %d\n"
          , s0, s1, s2, s3, s4
          , idx
          , val
          , count
          );
}

void testSets()
{
    // Set with fixed size inline memory.
    typedef SetT<64> TestSetT;
    TestSetT set0;
    testSetApi(set0);

    // Set with external memory.
    typedef SetExt TestSetExt;
    TestSetExt set1;
    u32 size = TestSetExt::sizeFor(64);
    void* mem = dm_alloc(size, &::realloc);
    set1.init(64, (uint8_t*)mem);
    testSetApi(set1);

    // Set with allocator.
    typedef Set TestSet;
    TestSet set2;
    set2.init(64, &::realloc);
    testSetApi(set2);

    // Set as ptr.
    typedef SetH TestSetH;
    TestSetH* set3;
    set3 = create<TestSetH>(64, &::realloc);
    testSetApi(*set3);
    destroy(set3);
}

template <typename ListTy>
void testListApi(ListTy& _list)
{
    Foo foo = { 23, 55 };
    _list.fillWith(&foo);
    _list.addCopy(&foo);

    Foo* f0 = _list.addNew();
    f0->m_a = 111;
    f0->m_b = 222;

    u16 handle = _list.getHandleOf(f0);
    bool b0 = _list.contains(handle);
    bool b1 = _list.contains(0);
    bool b2 = _list.containsObj(f0);
    bool b3 = _list.containsObj(NULL);

    u16 at = _list.getHandleAt(0);

    Foo* f1 = _list.get(handle);
    Foo* f2 = _list.getAt(0);
    _list[0].m_a = 22;

    _list.remove(handle);
    _list.removeObj(f0);
    _list.removeAt(0);

    u32 cnt = _list.count();

    (void)f0;
    (void)f1;
    (void)f2;

    printf("List out %d %d | %d %d %d %d | %d\n"
         , handle, at
         , b0, b1, b2, b3
         , cnt
         );
}

void testLists()
{
    // List with fixed size inline memory.
    typedef ListT<Foo, 64> TestListT;
    TestListT list0;
    testListApi(list0);

    // List with external memory.
    typedef ListExt<Foo> TestListExt;
    TestListExt list1;
    u32 size = TestListExt::sizeFor(64);
    void* mem = dm_alloc(size, &::realloc);
    list1.init(64, (uint8_t*)mem);
    testListApi(list1);

    // List with allocator.
    typedef List<Foo> TestList;
    TestList list2;
    list2.init(64, &::realloc);
    testListApi(list2);

    // List as ptr.
    typedef ListH<Foo> TestListH;
    TestListH* list3;
    list3 = create<TestListH>(64, &::realloc);
    testListApi(*list3);
    destroy(list3);
}

template <typename LinkedListTy>
void testLinkedListApi(LinkedListTy& _ll)
{
    Foo foo = { 22, 33 };

    Foo* f0 = _ll.addNew();
    u16 handle = _ll.getHandle(f0);

    Foo* f1 = _ll.insertAfter(handle);
    f1->m_a = 55;
    f1->m_b = 2;

    Foo* f2 = _ll.insertAfter(f1);
    f2->m_a = 77;
    f2->m_b = 123;

    Foo* f3 = _ll.next(f1);
    Foo* f4 = _ll.prev(f1);

    u16 f5 = _ll.next(handle);
    u16 f6 = _ll.prev(handle);

    Foo* f7 = _ll.lastElem();
    Foo* f8 = _ll.firstElem();

    u16 h0 = _ll.firstHandle();
    u16 h1 = _ll.lastHandle();

    Foo* f9  = _ll.getObj(h0);
    Foo* f10 = _ll.getObjAt(0);

    _ll[0]->m_a = 63;
    u32 val = _ll[0]->m_a;

    (void)f0; (void)f1; (void)f2; (void)f3; (void)f4;
    (void)f5; (void)f6; (void)f7; (void)f8; (void)f9;
    (void)f10; (void)val; (void)foo; (void)h0; (void)h1;

    _ll.remove(handle);

    bool b0 = _ll.contains(handle);
    bool b1 = _ll.contains(f0);
    u32 count = _ll.count();

    printf("Linked List out %d %d %d %d %d\n", f1->m_a, val, count, b0, b1);
}

void testLinkedLists()
{
    // LinkedList with fixed size inline memory.
    typedef LinkedListT<Foo, 64> TestLinkedListT;
    TestLinkedListT list0;
    testLinkedListApi(list0);

    // LinkedList with external memory.
    typedef LinkedListExt<Foo> TestLinkedListExt;
    TestLinkedListExt list1;
    u32 size = TestLinkedListExt::sizeFor(64);
    void* mem = dm_alloc(size, &::realloc);
    list1.init(64, (uint8_t*)mem);
    testLinkedListApi(list1);

    // LinkedList with allocator.
    typedef LinkedList<Foo> TestLinkedList;
    TestLinkedList list2;
    list2.init(64, &::realloc);
    testLinkedListApi(list2);

    // LinkedList as ptr.
    typedef LinkedListH<Foo> TestLinkedListH;
    TestLinkedListH* list3;
    list3 = create<TestLinkedListH>(64, &::realloc);
    testLinkedListApi(*list3);
    destroy(list3);
}

template <typename HashMapTy>
void testHashMapApi(HashMapTy& _hm)
{
    _hm.insert(1.5f, 111);
    _hm.insert("asdf", 222);
    _hm.insertHandleDup(1.7f, 333);
    _hm.insertHandleDup("qwer", 444);

    u32 handle = _hm.findHandleOf(1.5f);
    u32 val0 = _hm.getValueOf(handle);

    u32 val1 = _hm.find(1.5f);
    u32 val2 = _hm.find("asdf");
    bool rem = _hm.remove(1.5f);

    printf("Hash Map out: %d %d %d %d %d\n", handle, val0, val1, val2, rem);
}

void testHashMaps()
{
    // HashMap with fixed size inline memory.
    typedef HashMapT<sizeof(float), uint32_t, 256> TestHashMapT;
    TestHashMapT hm0;
    testHashMapApi(hm0);

    // HashMap with external memory.
    typedef HashMapExt<sizeof(float), uint32_t> TestHashMapExt;
    TestHashMapExt hm1;
    u32 size = TestHashMapExt::sizeFor(256);
    void* mem = dm_alloc(size, &::realloc);
    hm1.init(256, (uint8_t*)mem);
    testHashMapApi(hm1);

    // HashMap with allocator.
    typedef HashMap<sizeof(float), uint32_t> TestHashMap;
    TestHashMap hm2;
    hm2.init(256, &::realloc);
    testHashMapApi(hm2);

    // HashMap as ptr.
    typedef HashMapH<sizeof(float), uint32_t> TestHashMapH;
    TestHashMapH* hm3;
    hm3 = create<TestHashMapH>(256, &::realloc);
    testHashMapApi(*hm3);
    destroy(hm3);
}

template <typename FooHashMapTy>
void testObjHashMapApi(FooHashMapTy& _ohm)
{
    Foo* f0 = _ohm.insert(1.5f); f0->m_a = 111;
    Foo* f1 = _ohm.insert("asdf"); f1->m_a = 222;

    Foo* f2 = _ohm.insertHandleDup(1.7f).m_obj; f2->m_a = 333;
    Foo* f3 = _ohm.insertHandleDup("qwer").m_obj; f3->m_a = 444;

    Foo* f4 = _ohm.find(1.5f);
    Foo* f5 = _ohm.find("asdf");
    u32 val0 = f4->m_a;
    u32 val1 = f5->m_a;

    bool rem = _ohm.remove(1.5f);

    printf("Obj Hash Map out: %d %d %d\n", val0, val1, rem);
}

void testObjHashMaps()
{
    // ObjHashMap with fixed size inline memory.
    typedef ObjHashMapT<sizeof(float), Foo, 256> TestObjHashMapT;
    TestObjHashMapT ohm0;
    testObjHashMapApi(ohm0);

    // ObjHashMap with external memory.
    typedef ObjHashMapExt<sizeof(float), Foo> TestObjHashMapExt;
    TestObjHashMapExt ohm1;
    u32 size = TestObjHashMapExt::sizeFor(256);
    void* mem = dm_alloc(size, &::realloc);
    ohm1.init(256, (uint8_t*)mem);
    testObjHashMapApi(ohm1);

    // ObjHashMap with allocator.
    typedef ObjHashMap<sizeof(float), Foo> TestObjHashMap;
    TestObjHashMap ohm2;
    ohm2.init(256, &::realloc);
    testObjHashMapApi(ohm2);

    // ObjHashMap as ptr.
    typedef ObjHashMapH<sizeof(float), Foo> TestObjHashMapH;
    TestObjHashMapH* ohm3;
    ohm3 = create<TestObjHashMapH>(256, &::realloc);
    testObjHashMapApi(*ohm3);
    destroy(ohm3);
}

void testApi()
{
    testArrays();
    testObjArrays();
    testHandleAllocs();
    testBitArrays();
    testSets();
    testLists();
    testLinkedLists();
    testHashMaps();
    testObjHashMaps();
}

int main()
{
    u32 macka = 0;
    u32c macka2 = macka;
    testApi();
    return macka2;
}
