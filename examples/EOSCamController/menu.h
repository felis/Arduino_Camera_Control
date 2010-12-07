#if !defined(__MENU_H__)
#define __MENU_H__

#include <inttypes.h>
#include "screenitem.h"
#include "screen.h"
#include "controls.h"

typedef void (*MenuFunctionPtr)();

struct MenuItem
{
      ScreenItem        *screenItem;
      MenuFunctionPtr    ptrFunction;
};

class Menu : public ControlEvents
{
    uint8_t             numItems;
    uint8_t             itemSelected; 
    uint8_t             screenId;    
    MenuItem            *menuItems;
    StateMachine        *returnState;
  
public:
    Menu(uint8_t scr, uint8_t num_items, MenuItem *items, uint8_t sel = 0, StateMachine *s = NULL) : 
        screenId(scr), 
        numItems(num_items), 
        menuItems(items), 
        itemSelected(sel),
        returnState(s) 
        {};
    
    void SetReturnState(StateMachine *s) { returnState = s; };
      
    virtual bool OnInitialState(); 
    virtual bool OnEncoderChanged(int8_t value); 
    virtual bool OnEncButtonDown(); 
    virtual bool OnExtButtonDown(); 
};

typedef void (*SpinFunction)(DataItemBase *data_item);

template <class ITEM_TYPE, class VALUE_TYPE>
class IntSpin : public ControlEvents
{
      VALUE_TYPE  minValue;
      VALUE_TYPE  maxValue;
      VALUE_TYPE  incValue;
      
      ITEM_TYPE         *dataItem;
      StateMachine      *returnState;
      SpinFunction      pFunction;

public:
      IntSpin(VALUE_TYPE min_val, VALUE_TYPE max_val, VALUE_TYPE inc_val, ITEM_TYPE *item, SpinFunction f) :
            minValue(min_val),
            maxValue(max_val),
            incValue(inc_val),
            dataItem(item),
            returnState(NULL),
            pFunction(f)
            {
            };
            
      void SetConstraints(VALUE_TYPE min_val, VALUE_TYPE max_val, VALUE_TYPE inc_val)
      {
            minValue  = min_val;
            maxValue  = max_val;
            incValue  = inc_val;
      };
      
      void SetReturnState(StateMachine *s) { returnState = s; };
      
      virtual bool OnEncoderChanged(int8_t value) 
      {
          int16_t    new_val = dataItem->Get() + value * incValue;
          
          if (new_val > maxValue)
              new_val = maxValue;
          else if (new_val < minValue)
              new_val = minValue;
          
          dataItem->Set((VALUE_TYPE)new_val);
          dataItem->SetUpdated(true);  
          
          return true;
      };

      virtual bool OnEncButtonDown() 
      {
          if (pFunction)
              pFunction((DataItemBase*)dataItem);
              
          if (returnState)
              StateMachine::SetState(returnState);
          return true; 
      };
};

template <class ITEM_TYPE, class VALUE_TYPE>
class EEPROMListIntSpin : public ControlEvents
{
      EEPROMByteList    *valueList;
      ITEM_TYPE         *dataItem;
      StateMachine      *returnState;
      SpinFunction      pFunction;
      
public:
      EEPROMListIntSpin(EEPROMByteList *list, ITEM_TYPE *item, SpinFunction pf) :
            valueList(list),
            dataItem(item),
            returnState(NULL),
            pFunction(pf)
            {
            };
            
      void SetReturnState(StateMachine *s) { returnState = s; };
      
      virtual bool OnEncoderChanged(int8_t value) 
      {
          if (valueList->GetSize() < 1)
              return true;
              
          VALUE_TYPE  new_value;
          
          if (value < 0)
               new_value = valueList->GetPrev(dataItem->Get(), -value);
          else
               new_value = valueList->GetNext(dataItem->Get(), value);
          
          dataItem->Set(new_value);
          dataItem->SetUpdated(true);  
          
          return true;
      };

      virtual bool OnEncButtonDown() 
      {
          if (pFunction)
              pFunction((DataItemBase*)dataItem);
              
          if (returnState)
              StateMachine::SetState(returnState);
              
          return true; 
      };
};


template <class ITEM_TYPE, class VALUE_TYPE, class VALUE_LIST_TYPE>
class SRAMListIntSpin : public ControlEvents
{
      VALUE_LIST_TYPE   *valueList;
      ITEM_TYPE         *dataItem;
      StateMachine      *returnState;
      SpinFunction      pFunction;
      
public:
      SRAMListIntSpin(VALUE_LIST_TYPE *list, ITEM_TYPE *item, SpinFunction pf) :
            valueList(list),
            dataItem(item),
            returnState(NULL),
            pFunction(pf)
            {
            };
            
      void SetReturnState(StateMachine *s) { returnState = s; };
      
      virtual bool OnEncoderChanged(int8_t value) 
      {
          if (valueList->GetSize() < 1)
              return true;
              
          VALUE_TYPE  new_value;
          
          if (value < 0)
               new_value = valueList->GetPrev(dataItem->Get(), -value);
          else
               new_value = valueList->GetNext(dataItem->Get(), value);
          
          dataItem->Set(new_value);
          dataItem->SetUpdated(true);  
          
          return true;
      };

      virtual bool OnEncButtonDown() 
      {
          if (pFunction)
              pFunction((DataItemBase*)dataItem);
              
          if (returnState)
              StateMachine::SetState(returnState);
              
          return true; 
      };
};

#endif // __MENU_H__
