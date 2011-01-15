/* Copyright (C) 2010-2011 Circuits At Home, LTD. All rights reserved.

This software may be distributed and modified under the terms of the GNU
General Public License version 2 (GPL2) as published by the Free Software
Foundation and appearing in the file GPL2.TXT included in the packaging of
this file. Please note that GPL2 Section 2[b] requires that all works based
on this software must also be made publicly available under the terms of
the GPL2 ("Copyleft").

Contact information
-------------------

Circuits At Home, LTD
Web      :  http://www.circuitsathome.com
e-mail   :  support@circuitsathome.com
*/
#if !defined(__SIMPLEFIFO_H__)
#define __SIMPLEFIFO_H__

template <class TYPE, const uint8_t SIZE>
class SimpleFIFO
{
    TYPE      theBuffer[SIZE];
    uint8_t   tail, head;
    
private: 
    void inc(uint8_t &val)
    {
        val ++;
        
        if (val >= SIZE)
            val = 0;
    };
public:
    SimpleFIFO() :
        tail(0),
        head(0)
    {
    };
    uint8_t Size()
    {
        if (tail == head)
            return 0;
            
        if (tail > head)
            return (tail - head);
        else
            return (SIZE - head + tail);
    };
    void Empty()
    {
        tail = head = 0;
    };
    void Push(TYPE val)
    {
        if (Size() >= SIZE-1)
            return;
            
        theBuffer[tail] = val;
        inc(tail);
//        Serial.print(">");
//        Serial.print(head,DEC);
//        Serial.print(":");
//        Serial.print(tail,DEC);
//        Serial.print(":");
//        Serial.println(Size(),DEC);
    };
    TYPE Pop()
    {
        if (head == tail) 
            return (TYPE)0;
        
        TYPE ret = theBuffer[head];
        inc(head);
//        Serial.print("<");
//        Serial.print(head,DEC);
//        Serial.print(":");
//        Serial.print(tail,DEC);
//        Serial.print(":");
//        Serial.println(Size(),DEC);
        return ret;
    };
};


#endif // __SIMPLEFIFO_H__