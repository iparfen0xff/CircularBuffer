#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
using namespace std;

#define uint8_t unsigned char
//#define debug

// дан циклический (кольцевой) буфер и некоторые функции работы с ним
#define BUFFER_SIZE 8

#if (BUFFER_SIZE & (BUFFER_SIZE - 1)) != 0
  #error "Incorrect buffer size"
#endif

uint8_t TestCount = 0;
uint8_t TestPassed = 0;
uint8_t memcpyRunCounter = 0;

typedef struct {
  size_t first;
  size_t last;
  uint8_t data[BUFFER_SIZE];
} CircularBuffer;

// ***********************************************************
// *** Local functions declaration
// ***********************************************************
size_t GetDataCount(CircularBuffer* pBuf);
bool IsBuff_Fragmented(CircularBuffer* pBuf);

// test func BufMoveSlow()
void TestCase_01(CircularBuffer* pBufA, CircularBuffer* pBufB);

// test func BufMoveFast()
void TestCase_02(CircularBuffer* pBufA, CircularBuffer* pBufB);
void TestCase_03(CircularBuffer* pBufA, CircularBuffer* pBufB);
void TestCase_04(CircularBuffer* pBufA, CircularBuffer* pBufB);
void TestCase_05(CircularBuffer* pBufA, CircularBuffer* pBufB);
void TestCase_06(CircularBuffer* pBufA, CircularBuffer* pBufB);
void TestCase_07(CircularBuffer* pBufA, CircularBuffer* pBufB);
void TestCase_08(CircularBuffer* pBufA, CircularBuffer* pBufB);
void TestCase_09(CircularBuffer* pBufA, CircularBuffer* pBufB);
void TestCase_10(CircularBuffer* pBufA, CircularBuffer* pBufB);
void TestCase_11(CircularBuffer* pBufA, CircularBuffer* pBufB);
void PrintTestStatus();
// ***********************************************************

// ClearBuf очищает буфер (может также использоваться для инициализации структуры CircularBuffer)
void ClearBuf(CircularBuffer* pBuf)
{
  pBuf->first = 0;
  pBuf->last = 0;
  
  for (uint8_t i = 0; i < BUFFER_SIZE; i++)
  {
    pBuf->data[i] = 0;
  }
}

// ReadByte читает байт из буфера.  если в буфере нет данных, возвращает -1.
int ReadByte(CircularBuffer* pBuf)
{
  if (pBuf->first == pBuf->last)
    return -1;
  int result = pBuf->data[pBuf->first];
  pBuf->first = (pBuf->first + 1) & (BUFFER_SIZE - 1);
  return result;
}

// пишет байт в буфер, возвращает true если запись прошла успешно
bool WriteByte(CircularBuffer* pBuf, uint8_t value)
{
  size_t next = (pBuf->last + 1) & (BUFFER_SIZE - 1);
  if (next == pBuf->first)
    return false;
  pBuf->data[pBuf->last] = value;
  pBuf->last = next;
  return true;
}

// функция IsEmpty возвращает true если буфер пуст, иначе false
// пустым являтся буфер в котором нет данных для чтения.
bool IsEmpty(CircularBuffer* pBuf)
{
 // TODO: напишите код этой фукнции
 return (GetDataCount(pBuf) > 0 ? false : true);
}

// функция IsFull возвращает true если буфер полон, иначе false
// попытка писать в полный буфер всегда будет завершаться неудачей.
bool IsFull(CircularBuffer* pBuf)
{
  // TODO: напишите код этой фукнции
  return (GetDataCount(pBuf) < (BUFFER_SIZE - 1) ? false : true);
}

// что возвращает функция GetSomething? переименуйте ее, чтобы название соответствоало возвращаемому значению 
size_t GetDataCount(CircularBuffer* pBuf)
{
  return (pBuf->last - pBuf->first) & (BUFFER_SIZE - 1);
}

// Get count of free bytes in selected buffer
size_t GetEmptyCount(CircularBuffer* pBuf)
{
  //return BUFFER_SIZE - GetDataCount(pBuf);
  return ((BUFFER_SIZE - 1) - GetDataCount(pBuf)); //?
}

//
bool IsBuff_Fragmented(CircularBuffer* pBuf)
{
   return pBuf->first > pBuf->last;
};

// нам нужна фукнция для перемещения данных из одного циклического буфера в другой
// мы решили ее объявить так
// size_t BufMove(CicrularBufffer* pDest, CicrularBufffer* pSource)
// предполагается что pDest и pSource указывают на разные буферы
// функция должна перемещать максимально возможное кол-во байт из Source в Dest
// и возвращать число скопированных байт.
// т.е если в Dest буфере не хватет места для всего содержимого из Source, 
// переместиться должны только те байты для которых есть место, остаток должен остаться в Source буфере 
// если же места хватает, то переместиться должно все, и буфер Source остаться пуст.

// программист написал вот такую фукнцию
// соответствует ли она описаню данному выше?
// какие у нее есть недостатки?
size_t BufMoveSlow(CircularBuffer* pDest, CircularBuffer* pSource)
{
  if (pDest == pSource)
    return 0;
  int value;
  size_t result = 0;
  while ((value = ReadByte(pSource)) != -1 && WriteByte(pDest, value))
    result++;
  return result;
}
// @Ivan Parfonov
// Відповідь про функцію BufMoveSlow() :
// - переміщює байти з одного буфера в інший (але є нюанс, про це далі).
// - робить перевірку що буфери pDest і pSource є різними (мають різні адреси).
// - вертає кількість скопійований байт.
// - не відповідає умові тех-завдання, при якій має залишати в буфері pSource всі байти, що не скопіювались в pDest
//    якщо pDest вже повний. Це пов'язано з тим, що функція ReadByte() читаючі перший байт "видаляє" його з буфера
//    збільшивши поле first на одиницю, тим самим переміщює початок буфера на наступний байт.
//    Цей еффект можна побачить запустивши функцію TestCase_01(), в якості доказового приклада.


// напишите свой вариант функции перемещения данных
size_t BufMoveFast(CircularBuffer* pDest, CircularBuffer* pSource)
{
  // TODO: напишите код этой фукнции
  // фнукция должна соответствовать требованиям описаным в комментарии выше
  // для копирования данных между буферами используйте фукцнию memcpy
  // желательно чтобы число вызовов memcpy не превышало кол-во непрерывных блоков данных, которые нужно скопировать
  // т.е чтобы число вызовов memcpy было минимально возможным.
  
  //
  if (pDest == pSource)
  {
    return 0;
  }
  
  //
  if (IsFull(pDest))
    return 0;
  
  //
  if (IsEmpty(pSource))
    return 0;

  size_t count = 0;
  size_t cnt = 0;
  bool isSourceFrag = true; //true;
  //bool isDestFrag = true;

  while (isSourceFrag)
  {    
    if (IsBuff_Fragmented(pSource))
    {
      cnt = BUFFER_SIZE - pSource->first; // find how bytes from first to end of address of buff 
      isSourceFrag = true;
      
      #ifdef debug
      printf("*** Source buff fragmented ***\n");
      #endif
    }
    else
    {
      cnt = GetDataCount(pSource);
      isSourceFrag = false;
      
      #ifdef debug
      printf("*** Source buff NOT fragmented ***\n");
      #endif
    }    

    bool isDestFrag = true;
    while (isDestFrag)
    {
      if (IsBuff_Fragmented(pDest))
      {
        isDestFrag = true;
        
        #ifdef debug        
        printf("*** Destination buff fragmented ***\n");
        #endif
      }
      else
      {
        isDestFrag = false;

        #ifdef debug
        printf("*** Destination buff NOT fragmented ***\n");
        #endif
      }
      
      if (!isSourceFrag)
      {
        cnt = GetDataCount(pSource); 
      }

      #ifdef debug
      // want's to move, bytes:
      printf("(#1) cnt = %zu\n", cnt);
      #endif

      // if Source length data > Destination free space
      if (cnt > GetEmptyCount(pDest))
      {
        cnt = GetEmptyCount(pDest); // -GetDataCount
        isDestFrag = true;
        
        #ifdef debug
        // but will move no more, bytes:
        printf("(#2) cnt = %zu\n", cnt);
        #endif
      }
      else
      {
        isDestFrag = false;
      }

      // if Source length data > Destination free space to end of address
      if (cnt > (BUFFER_SIZE - pDest->last))
      {
        cnt = BUFFER_SIZE  - pDest->last;
        isDestFrag = true;
      }
      else
      {
        //cnt = GetDataCount(pSource);
        isDestFrag = false; 
      }

      #ifdef debug
      // but will move no more, bytes:
      printf("(#2) cnt = %zu\n", cnt);
      #endif

      if (cnt > 0)
      {
        memcpy(pDest->data+pDest->last, pSource->data+pSource->first, cnt);
        memcpyRunCounter ++;
        pDest->last = (pDest->last + cnt) & (BUFFER_SIZE - 1);
        pSource->first = (pSource->first + cnt) & (BUFFER_SIZE - 1);
        count += cnt;  
      }

      #ifdef debug
      // move in this itteration, bytes: 
      printf("(#3) cnt = %zu\n", cnt); // tmp
      #endif      
    };
  };

  return count;  
}

// вспомогательная функция для отладки
void PrintBuffer(CircularBuffer* pBuf) 
{ 
  if (pBuf->first == pBuf->last)
    printf(" Empty");
  size_t pos;    
  for (pos = pBuf->first; pos != pBuf->last; pos = (pos + 1) & (BUFFER_SIZE - 1))
    printf(" %02x", pBuf->data[pos]);
  printf("\n");
}


// код ниже можно менять по своему усмотрению для тестирования фукнций

CircularBuffer bufferA;
CircularBuffer bufferB;

int main(){
        
        //
        //TestCase_01(&bufferA, &bufferB);
        
        TestCase_02(&bufferA, &bufferA);
        TestCase_03(&bufferA, &bufferB);
        TestCase_04(&bufferA, &bufferB);
        TestCase_05(&bufferA, &bufferB);
        TestCase_06(&bufferA, &bufferB);
        TestCase_07(&bufferA, &bufferB);
        TestCase_08(&bufferA, &bufferB);
        TestCase_09(&bufferA, &bufferB);
        TestCase_10(&bufferA, &bufferB);
        TestCase_11(&bufferA, &bufferB);

        PrintTestStatus();

  return 0;
}

void PrintTestStatus()
{ 
  printf("\n***********************************\n"); 
  printf("Tests Count: %u \n", TestCount);
  printf("Tests PASS : %u \n", TestPassed);
  printf("Tests FAIL : %u \n", TestCount - TestPassed);
  printf("***********************************\n"); 
  if ((TestCount > 0) and (TestCount == TestPassed))
  {
    printf("All tests: PASS\n");
  }
  else
  {
    printf("All tests: FAIL\n");
  }  
}

void TestCase_01(CircularBuffer* pBufA, CircularBuffer* pBufB)
{
  printf("\n");
  printf("%s - for BufMoveSlow()\n", __func__ );

  ClearBuf(pBufA);
  ClearBuf(pBufB);

  WriteByte(pBufA, 4);
  WriteByte(pBufA, 5);
  WriteByte(pBufA, 6);
  WriteByte(pBufA, 7);
  WriteByte(pBufA, 8);

  WriteByte(pBufB, 1);
  WriteByte(pBufB, 2);
  WriteByte(pBufB, 3);

  printf("pBufA before move:");
  PrintBuffer(pBufA);
  printf("pBufB before move:");
  PrintBuffer(pBufB);

  size_t res = BufMoveSlow(pBufB, pBufA);
  printf("BufMoveSlow moved %zu item(s) from pBufA to pBufB\n", res);

  printf("pBufA after move:");
  PrintBuffer(pBufA);
  printf("pBufB after move:");
  PrintBuffer(pBufB);
}

void TestCase_02(CircularBuffer* pBufA, CircularBuffer* pBufB)
{ 
  TestCount += 1;
  bool isTestPass = false;
  uint8_t testBufA[] = {4, 5, 1, 2, 3};
  memcpyRunCounter = 0;

  printf("\n");
  printf("%s - for BufMoveFast() WHERE: BuffA and BuffB has common address\n", __func__ );

  ClearBuf(pBufA);
  ClearBuf(pBufB);

  WriteByte(pBufA, 4);
  WriteByte(pBufA, 5);  

  WriteByte(pBufB, 1);
  WriteByte(pBufB, 2);
  WriteByte(pBufB, 3);

  printf("Before move:\n");
  printf("BufA:");
  PrintBuffer(pBufA);
  printf("BufB:");
  PrintBuffer(pBufB);
  
  printf("BufA addres:");
  printf("%p\n", (void*)pBufA);
  printf("BufB addres:");
  printf("%p\n", (void*)pBufB);

  //pBufB = pBufA; //?
   
  printf("*** \n");
  size_t res = BufMoveFast(pBufB, pBufA);
  printf("BufMoveFast moved %zu item(s) from BufA to BufB\n", res);
  printf("*** \n");

  printf("After move:\n");
  printf("BufA:");
  PrintBuffer(pBufA);
  printf("BufB:");
  PrintBuffer(pBufB);

  printf("BufA addres:");
  printf("%p\n", (void*)pBufA);
  printf("BufB addres:");
  printf("%p\n", (void*)pBufB);

  // Test start
  uint8_t cnt = 0;
  for (uint8_t i = 0; i < GetDataCount(pBufA); i++)
  {
    if (pBufA->data[i] == testBufA[i])
    {
      cnt ++;
    }    
  }

  // Test conditions
  if ((res == 0) and (pBufA == pBufB) and (cnt == GetDataCount(pBufB)))
  {
    isTestPass = true;
    TestPassed += 1;
  };

  printf("*** memcpy() called %u times ***\n", memcpyRunCounter);

  // Test decision
  if (isTestPass)
  {    
    printf("******************\n");
    printf("Test: *** PASS ***\n");
  }
  else
  {
    printf("Test: *** FAIL ***\n");
  }  
}

void TestCase_03(CircularBuffer* pBufA, CircularBuffer* pBufB)
{ 
  TestCount += 1;
  bool isTestPass = false;
  uint8_t testBufA[] = {8};
  uint8_t testBufB[] = {1, 2, 3, 4, 5, 6, 7};
  memcpyRunCounter = 0;

  printf("\n");
  printf("%s - for BufMoveFast() WHERE: destination buffer full\n", __func__ );

  ClearBuf(pBufA);
  ClearBuf(pBufB);
  
  WriteByte(pBufA, 8);

  WriteByte(pBufB, 1);
  WriteByte(pBufB, 2);
  WriteByte(pBufB, 3);
  WriteByte(pBufB, 4);
  WriteByte(pBufB, 5);
  WriteByte(pBufB, 6);
  WriteByte(pBufB, 7);

  printf("Before move:\n");
  printf("BufA:");
  PrintBuffer(pBufA);
  printf("BufB:");
  PrintBuffer(pBufB);

  printf("Is BuffA empty: %s\n", IsEmpty(&bufferA) ? "true" : "false");
  printf("Is BuffB full: %s\n", IsFull(&bufferB) ? "true" : "false");

  printf("*** \n");
  size_t res = BufMoveFast(pBufB, pBufA);
  printf("BufMoveFast moved %zu item(s) from BufA to BufB\n", res);
  printf("*** \n");

  printf("After move:\n");
  printf("BufA:");
  PrintBuffer(pBufA);
  printf("BufB:");
  PrintBuffer(pBufB);

  printf("Is BuffA empty: %s\n", IsEmpty(&bufferA) ? "true" : "false");
  printf("Is BuffB full: %s\n", IsFull(&bufferB) ? "true" : "false");

  // Test start
  uint8_t cnt = 0;
  for (uint8_t i = 0; i < GetDataCount(pBufB); i++)
  {
    if (pBufB->data[i] == testBufB[i])
    {
      cnt ++;
    }    
  }

  // Test conditions
  if ((res == 0) and (!IsEmpty(&bufferA)) and (IsFull(&bufferB)) and (cnt == BUFFER_SIZE - 1) and (pBufA->data[0] == testBufA[0]))
  {
    isTestPass = true;
    TestPassed += 1;
  };

  printf("*** memcpy() called %u times ***\n", memcpyRunCounter);

  // Test decision
  if (isTestPass)
  {    
    printf("******************\n");
    printf("Test: *** PASS ***\n");
  }
  else
  {
    printf("Test: *** FAIL ***\n");
  }  
}

void TestCase_04(CircularBuffer* pBufA, CircularBuffer* pBufB)
{ 
  TestCount += 1;
  bool isTestPass = false;
  uint8_t testBufB[] = {1, 2, 3};
  memcpyRunCounter = 0;

  printf("\n");
  printf("%s - for BufMoveFast() WHERE: source buffer empty\n", __func__ );

  ClearBuf(pBufA);
  ClearBuf(pBufB);
  
  WriteByte(pBufB, 1);
  WriteByte(pBufB, 2);
  WriteByte(pBufB, 3);
  
  printf("Before move:\n");
  printf("BufA:");
  PrintBuffer(pBufA);
  printf("BufB:");
  PrintBuffer(pBufB);

  printf("Is BuffA empty: %s\n", IsEmpty(&bufferA) ? "true" : "false");
  printf("Is BuffB empty: %s\n", IsEmpty(&bufferB) ? "true" : "false");

  printf("*** \n");
  size_t res = BufMoveFast(pBufB, pBufA);
  printf("BufMoveFast moved %zu item(s) from BufA to BufB\n", res);
  printf("*** \n");

  printf("After move:\n");
  printf("BufA:");
  PrintBuffer(pBufA);
  printf("BufB:");
  PrintBuffer(pBufB);

  printf("Is BuffA empty: %s\n", IsEmpty(&bufferA) ? "true" : "false");
  printf("Is BuffB empty: %s\n", IsEmpty(&bufferB) ? "true" : "false");

  // Test start
  uint8_t cnt = 0;
  for (uint8_t i = 0; i < GetDataCount(pBufB); i++)
  {
    if (pBufB->data[i] == testBufB[i])
    {
      cnt ++;
    }    
  }

  // Test conditions
  if ((res == 0) and (IsEmpty(&bufferA)) and (cnt == GetDataCount(pBufB)))
  {
    isTestPass = true;
    TestPassed += 1;
  };

  printf("*** memcpy() called %u times ***\n", memcpyRunCounter);

  // Test decision
  if (isTestPass)
  {    
    printf("******************\n");
    printf("Test: *** PASS ***\n");
  }
  else
  {
    printf("Test: *** FAIL ***\n");
  }
}

void TestCase_05(CircularBuffer* pBufA, CircularBuffer* pBufB)
{ 
  TestCount += 1;
  bool isTestPass = false;
  uint8_t testBufB[] = {3, 4, 5, 6, 1, 2};
  memcpyRunCounter = 0;

  printf("\n");
  printf("%s - for BufMoveFast() WHERE: source buffer less than destination\n", __func__ );
  printf(" AND: if destination buffer become full during BufMoveFast()\n");
  printf(" THEN: bytes from source buffer that not copied stay at sorce buffer\n");
  
  ClearBuf(pBufA);
  ClearBuf(pBufB);
  
  WriteByte(pBufA, 1);
  WriteByte(pBufA, 2);

  WriteByte(pBufB, 3);
  WriteByte(pBufB, 4);
  WriteByte(pBufB, 5);
  WriteByte(pBufB, 6);

  printf("Before move:\n");
  printf("BufA:");
  PrintBuffer(pBufA);
  printf("BufB:");
  PrintBuffer(pBufB);

  printf("*** \n");
  size_t res = BufMoveFast(pBufB, pBufA);
  printf("BufMoveFast moved %zu item(s) from BufA to BufB\n", res);
  printf("*** \n");

  printf("After move:\n");
  printf("BufA:");
  PrintBuffer(pBufA);
  printf("BufB:");
  PrintBuffer(pBufB);

  // Test start
  uint8_t cnt = 0;
  for (uint8_t i = 0; i < GetDataCount(pBufB); i++)
  {
    if (pBufB->data[i] == testBufB[i])
    {
      cnt ++;
    }    
  }
  
  // Test conditions
  if ((res > 0) and (IsEmpty(&bufferA)) and (cnt == GetDataCount(pBufB)))
  {
    isTestPass = true;
    TestPassed += 1;
  };

  printf("*** memcpy() called %u times ***\n", memcpyRunCounter);

  // Test decision
  if (isTestPass)
  {    
    printf("******************\n");
    printf("Test: *** PASS ***\n");
  }
  else
  {
    printf("Test: *** FAIL ***\n");
  }
}

void TestCase_06(CircularBuffer* pBufA, CircularBuffer* pBufB)
{ 
  TestCount += 1;
  bool isTestPass = false;
  uint8_t testBufA[] = {3};
  uint8_t testBufB[] = {4, 5, 6, 7, 8, 1, 2};
  memcpyRunCounter = 0;

  printf("\n");
  printf("%s - for BufMoveFast() WHERE: source buffer more than destination\n", __func__ );
  printf(" AND: source buffer not empty AND destination buffer not full\n");
  
  ClearBuf(pBufA);
  ClearBuf(pBufB);
  
  WriteByte(pBufA, 1);
  WriteByte(pBufA, 2);
  WriteByte(pBufA, 3);

  WriteByte(pBufB, 4);
  WriteByte(pBufB, 5);
  WriteByte(pBufB, 6);
  WriteByte(pBufB, 7);
  WriteByte(pBufB, 8);

  printf("Before move:\n");
  printf("BufA:");
  PrintBuffer(pBufA);
  printf("BufB:");
  PrintBuffer(pBufB);

  printf("*** \n");
  size_t res = BufMoveFast(pBufB, pBufA);
  printf("BufMoveFast moved %zu item(s) from BufA to BufB\n", res);
  printf("*** \n");

  printf("After move:\n");
  printf("BufA:");
  PrintBuffer(pBufA);
  printf("BufB:");
  PrintBuffer(pBufB);

  // Test start
  uint8_t cnt = 0;
  for (uint8_t i = 0; i < GetDataCount(pBufB); i++)
  {
    if (pBufB->data[i] == testBufB[i])
    {
      cnt ++;
    }    
  }

  // Test conditions
  if ((res > 0) and (!IsEmpty(&bufferA)) and (IsFull(&bufferB)) and
      (cnt == GetDataCount(pBufB)) and (pBufA->data[pBufA->first] == testBufA[0]))
  {
    isTestPass = true;
    TestPassed += 1;
  };

  printf("*** memcpy() called %u times ***\n", memcpyRunCounter);

  // Test decision
  if (isTestPass)
  {    
    printf("******************\n");
    printf("Test: *** PASS ***\n");
  }
  else
  {
    printf("Test: *** FAIL ***\n");
  }
}

void TestCase_07(CircularBuffer* pBufA, CircularBuffer* pBufB)
{ 
  TestCount += 1;
  bool isTestPass = false;  
  uint8_t testBufB[] = {6, 7, 1, 2, 3, 4, 5};
  memcpyRunCounter = 0;

  printf("\n");
  printf("%s - for BufMoveFast() WHERE: source buffer less than destination\n", __func__ );
  printf(" AND: source buffer fragmented (index of first byte more then index of last byte)\n");
  
  ClearBuf(pBufA);
  ClearBuf(pBufB);
  
  WriteByte(pBufA, 0);
  WriteByte(pBufA, 0);
  WriteByte(pBufA, 0);
  WriteByte(pBufA, 0);
  WriteByte(pBufA, 0);
  ReadByte(pBufA);
  ReadByte(pBufA);
  ReadByte(pBufA);
  ReadByte(pBufA);
  ReadByte(pBufA);
  
  #ifdef debug
  printf("Is BuffA empty: %s\n", IsEmpty(&bufferA) ? "true" : "false");
  printf("BufA first index: %zu \n", pBufA->first);
  printf("BufA last index: %zu \n", pBufA->last);
  #endif

  WriteByte(pBufA, 1);
  WriteByte(pBufA, 2);
  WriteByte(pBufA, 3);
  WriteByte(pBufA, 4);
  WriteByte(pBufA, 5);

  WriteByte(pBufB, 6);
  WriteByte(pBufB, 7);
  
  printf("Before move:\n");
  printf("BufA:");
  PrintBuffer(pBufA);
  printf("BufB:");
  PrintBuffer(pBufB);
  
  printf("BufA first index: %zu \n", pBufA->first);
  printf("BufA last index: %zu \n", pBufA->last);
  printf("BufB first index: %zu \n", pBufB->first);
  printf("BufB last index: %zu \n", pBufB->last);

  printf("*** \n");
  size_t res = BufMoveFast(pBufB, pBufA);
  printf("BufMoveFast moved %zu item(s) from BufA to BufB\n", res);
  printf("*** \n");

  printf("After move:\n");
  printf("BufA:");
  PrintBuffer(pBufA);
  printf("BufB:");
  PrintBuffer(pBufB);

  #ifdef debug
  printf("A[0] = %u B[0] = %u\n", pBufA->data[0], pBufB->data[0]);
  printf("A[1] = %u B[1] = %u\n", pBufA->data[1], pBufB->data[1]);
  printf("A[2] = %u B[2] = %u\n", pBufA->data[2], pBufB->data[2]);
  printf("A[3] = %u B[3] = %u\n", pBufA->data[3], pBufB->data[3]);
  printf("A[4] = %u B[4] = %u\n", pBufA->data[4], pBufB->data[4]);
  printf("A[5] = %u B[5] = %u\n", pBufA->data[5], pBufB->data[5]);
  printf("A[6] = %u B[6] = %u\n", pBufA->data[6], pBufB->data[6]);
  printf("A[7] = %u B[7] = %u\n", pBufA->data[7], pBufB->data[7]);
  #endif

  printf("BufA first index: %zu \n", pBufA->first);
  printf("BufA last index: %zu \n", pBufA->last);
  printf("BufB first index: %zu \n", pBufB->first);
  printf("BufB last index: %zu \n", pBufB->last);

  // Test start
  uint8_t cnt = 0;
  for (uint8_t i = 0; i < GetDataCount(pBufB); i++)
  {
    if (pBufB->data[i] == testBufB[i])
    {
      cnt ++;
    }
  }

  #ifdef debug
  printf("cnt...%u\n", cnt);
  printf("GetDataCount(pBufB)...%zu\n", GetDataCount(pBufB));
  printf("res...%zu\n", res);
  //printf("...%u\n", pBufA == pBufB);
  #endif

  // Test conditions
  if ((res > 0) and (IsEmpty(&bufferA)) and (IsFull(&bufferB)) and (cnt == GetDataCount(pBufB)))
  {
    isTestPass = true;
    TestPassed += 1;
  };

  printf("*** memcpy() called %u times ***\n", memcpyRunCounter);

  // Test decision
  if (isTestPass)
  {    
    printf("******************\n");
    printf("Test: *** PASS ***\n");
  }
  else
  {
    printf("Test: *** FAIL ***\n");
  }
}

void TestCase_08(CircularBuffer* pBufA, CircularBuffer* pBufB)
{ 
  TestCount += 1;
  bool isTestPass = false;  
  uint8_t testBufB[] = {6, 1, 2, 3, 0, 0, 4, 5};
  memcpyRunCounter = 0;

  printf("\n");
  printf("%s - for BufMoveFast() WHERE: source buffer less than destination\n", __func__ );
  printf(" AND: source buffer NOT fragmented \n");
  printf(" AND: dectination buffer fragmented\n");
  
  ClearBuf(pBufA);
  ClearBuf(pBufB);
  
  #ifdef debug
  //printf("BufA first index: %zu \n", pBufA->first);
  //printf("BufA last index: %zu \n", pBufA->last);
  //printf("BufB first index: %zu \n", pBufB->first);
  //printf("BufB last index: %zu \n", pBufB->last);
  #endif

  WriteByte(pBufA, 0);
  WriteByte(pBufA, 0);  
  ReadByte(pBufA);
  ReadByte(pBufA);
  WriteByte(pBufA, 1);
  WriteByte(pBufA, 2);
  WriteByte(pBufA, 3);
  

  WriteByte(pBufB, 0);
  WriteByte(pBufB, 0);
  WriteByte(pBufB, 0);
  WriteByte(pBufB, 0);
  WriteByte(pBufB, 0);
  WriteByte(pBufB, 0);
  ReadByte(pBufB); ReadByte(pBufB); ReadByte(pBufB);
  ReadByte(pBufB); ReadByte(pBufB); ReadByte(pBufB);
  WriteByte(pBufB, 4);
  WriteByte(pBufB, 5);
  WriteByte(pBufB, 6);

  #ifdef debug
  //printf("BufA first index: %zu \n", pBufA->first);
  //printf("BufA last index: %zu \n", pBufA->last);
  #endif

  printf("Before move:\n");
  printf("BufA:");
  PrintBuffer(pBufA);
  printf("BufB:");
  PrintBuffer(pBufB);
  
  printf("BufA first index: %zu \n", pBufA->first);
  printf("BufA last index: %zu \n", pBufA->last);
  printf("BufB first index: %zu \n", pBufB->first);
  printf("BufB last index: %zu \n", pBufB->last);

  printf("*** \n");
  size_t res = BufMoveFast(pBufB, pBufA);
  printf("BufMoveFast moved %zu item(s) from BufA to BufB\n", res);
  printf("*** \n");

  printf("After move:\n");
  printf("BufA:");
  PrintBuffer(pBufA);
  printf("BufB:");
  PrintBuffer(pBufB);

  #ifdef debug
  printf("A[0] = %u B[0] = %u\n", pBufA->data[0], pBufB->data[0]);
  printf("A[1] = %u B[1] = %u\n", pBufA->data[1], pBufB->data[1]);
  printf("A[2] = %u B[2] = %u\n", pBufA->data[2], pBufB->data[2]);
  printf("A[3] = %u B[3] = %u\n", pBufA->data[3], pBufB->data[3]);
  printf("A[4] = %u B[4] = %u\n", pBufA->data[4], pBufB->data[4]);
  printf("A[5] = %u B[5] = %u\n", pBufA->data[5], pBufB->data[5]);
  printf("A[6] = %u B[6] = %u\n", pBufA->data[6], pBufB->data[6]);
  printf("A[7] = %u B[7] = %u\n", pBufA->data[7], pBufB->data[7]);
  #endif

  printf("BufA first index: %zu \n", pBufA->first);
  printf("BufA last index: %zu \n", pBufA->last);
  printf("BufB first index: %zu \n", pBufB->first);
  printf("BufB last index: %zu \n", pBufB->last);

  // Test start
  uint8_t cnt = 0;
  for (uint8_t i = 0; i < GetDataCount(pBufB); i++)
  {
    if (pBufB->data[i] == testBufB[i])
    {
      cnt ++;
    }
  }

  #ifdef debug
  //printf("cnt...%u\n", cnt);
  //printf("GetDataCount(pBufB)...%zu\n", GetDataCount(pBufB));
  //printf("res...%zu\n", res);
  //printf("...%u\n", pBufA == pBufB);
  #endif

  // Test conditions
  if ((res > 0) and (IsEmpty(&bufferA)) and (!IsFull(&bufferB)) and (cnt == GetDataCount(pBufB)))
  {
    isTestPass = true;
    TestPassed += 1;
  };

  printf("*** memcpy() called %u times ***\n", memcpyRunCounter);

  // Test decision
  if (isTestPass)
  {    
    printf("******************\n");
    printf("Test: *** PASS ***\n");
  }
  else
  {
    printf("Test: *** FAIL ***\n");
  }
}

void TestCase_09(CircularBuffer* pBufA, CircularBuffer* pBufB)
{ 
  TestCount += 1;
  bool isTestPass = false;  
  uint8_t testBufB[] = {5, 6, 1, 2, 3, 4};
  memcpyRunCounter = 0;

  printf("\n");
  printf("%s - for BufMoveFast() WHERE: source buffer less than destination\n", __func__ );
  printf(" AND: source buffer fragmented \n");
  printf(" AND: dectination buffer NOT fragmented\n");
  
  ClearBuf(pBufA);
  ClearBuf(pBufB);
  
  #ifdef debug
  //printf("BufA first index: %zu \n", pBufA->first);
  //printf("BufA last index: %zu \n", pBufA->last);
  //printf("BufB first index: %zu \n", pBufB->first);
  //printf("BufB last index: %zu \n", pBufB->last);
  #endif

  WriteByte(pBufA, 0);
  WriteByte(pBufA, 0);
  WriteByte(pBufA, 0);
  WriteByte(pBufA, 0);
  WriteByte(pBufA, 0);  
  ReadByte(pBufA);
  ReadByte(pBufA);
  ReadByte(pBufA);
  ReadByte(pBufA);
  ReadByte(pBufA);
  WriteByte(pBufA, 1);
  WriteByte(pBufA, 2);
  WriteByte(pBufA, 3);
  WriteByte(pBufA, 4);
  

  WriteByte(pBufB, 0);
  ReadByte(pBufB);  
  WriteByte(pBufB, 5);
  WriteByte(pBufB, 6);

  #ifdef debug
  //printf("BufA first index: %zu \n", pBufA->first);
  //printf("BufA last index: %zu \n", pBufA->last);
  #endif

  printf("Before move:\n");
  printf("BufA:");
  PrintBuffer(pBufA);
  printf("BufB:");
  PrintBuffer(pBufB);
  
  printf("BufA first index: %zu \n", pBufA->first);
  printf("BufA last index: %zu \n", pBufA->last);
  printf("BufB first index: %zu \n", pBufB->first);
  printf("BufB last index: %zu \n", pBufB->last);

  printf("*** \n");
  size_t res = BufMoveFast(pBufB, pBufA);
  printf("BufMoveFast moved %zu item(s) from BufA to BufB\n", res);
  printf("*** \n");

  printf("After move:\n");
  printf("BufA:");
  PrintBuffer(pBufA);
  printf("BufB:");
  PrintBuffer(pBufB);

  #ifdef debug
  printf("A[0] = %u B[0] = %u\n", pBufA->data[0], pBufB->data[0]);
  printf("A[1] = %u B[1] = %u\n", pBufA->data[1], pBufB->data[1]);
  printf("A[2] = %u B[2] = %u\n", pBufA->data[2], pBufB->data[2]);
  printf("A[3] = %u B[3] = %u\n", pBufA->data[3], pBufB->data[3]);
  printf("A[4] = %u B[4] = %u\n", pBufA->data[4], pBufB->data[4]);
  printf("A[5] = %u B[5] = %u\n", pBufA->data[5], pBufB->data[5]);
  printf("A[6] = %u B[6] = %u\n", pBufA->data[6], pBufB->data[6]);
  printf("A[7] = %u B[7] = %u\n", pBufA->data[7], pBufB->data[7]);
  #endif

  printf("BufA first index: %zu \n", pBufA->first);
  printf("BufA last index: %zu \n", pBufA->last);
  printf("BufB first index: %zu \n", pBufB->first);
  printf("BufB last index: %zu \n", pBufB->last);

  // Test start
  uint8_t cnt = 0;
  for (uint8_t i = 0; i < GetDataCount(pBufB); i++)
  {
    if (pBufB->data[i] == testBufB[i])
    {
      cnt ++;
    }
  }

  #ifdef debug
  //printf("cnt...%u\n", cnt);
  //printf("GetDataCount(pBufB)...%zu\n", GetDataCount(pBufB));
  //printf("res...%zu\n", res);
  //printf("...%u\n", pBufA == pBufB);
  #endif

  // Test conditions
  if ((res > 0) and (IsEmpty(&bufferA)) and (!IsFull(&bufferB)))
  {
    isTestPass = true;
    TestPassed += 1;
  };

  printf("*** memcpy() called %u times ***\n", memcpyRunCounter);

  // Test decision
  if (isTestPass)
  {    
    printf("******************\n");
    printf("Test: *** PASS ***\n");
  }
  else
  {
    printf("Test: *** FAIL ***\n");
  }
}

void TestCase_10(CircularBuffer* pBufA, CircularBuffer* pBufB)
{ 
  TestCount += 1;
  bool isTestPass = false;  
  uint8_t testBufA[] = {5};
  uint8_t testBufB[] = {6, 7, 8, 1, 2, 3, 4};
  memcpyRunCounter = 0;

  printf("\n");
  printf("%s - for BufMoveFast() WHERE: source buffer more than destination\n", __func__ );
  printf(" AND: source buffer fragmented \n");
  printf(" AND: dectination buffer fragmented\n");
  
  ClearBuf(pBufA);
  ClearBuf(pBufB);
  
  #ifdef debug
  //printf("BufA first index: %zu \n", pBufA->first);
  //printf("BufA last index: %zu \n", pBufA->last);
  //printf("BufB first index: %zu \n", pBufB->first);
  //printf("BufB last index: %zu \n", pBufB->last);
  #endif

  WriteByte(pBufA, 0);
  WriteByte(pBufA, 0);
  WriteByte(pBufA, 0);
  WriteByte(pBufA, 0);
  WriteByte(pBufA, 0);  
  ReadByte(pBufA);
  ReadByte(pBufA);
  ReadByte(pBufA);
  ReadByte(pBufA);
  ReadByte(pBufA);
  WriteByte(pBufA, 1);
  WriteByte(pBufA, 2);
  WriteByte(pBufA, 3);
  WriteByte(pBufA, 4);
  WriteByte(pBufA, 5);
  

  WriteByte(pBufB, 0);  WriteByte(pBufB, 0);
  WriteByte(pBufB, 0);  WriteByte(pBufB, 0);
  WriteByte(pBufB, 0);  WriteByte(pBufB, 0);
  ReadByte(pBufB);      ReadByte(pBufB);
  ReadByte(pBufB);      ReadByte(pBufB);
  ReadByte(pBufB);      ReadByte(pBufB);
  WriteByte(pBufB, 6);
  WriteByte(pBufB, 7);
  WriteByte(pBufB, 8);

  #ifdef debug
  //printf("BufA first index: %zu \n", pBufA->first);
  //printf("BufA last index: %zu \n", pBufA->last);
  #endif

  printf("Before move:\n");
  printf("BufA:");
  PrintBuffer(pBufA);
  printf("BufB:");
  PrintBuffer(pBufB);
  
  printf("BufA first index: %zu \n", pBufA->first);
  printf("BufA last index: %zu \n", pBufA->last);
  printf("BufB first index: %zu \n", pBufB->first);
  printf("BufB last index: %zu \n", pBufB->last);

  printf("*** \n");
  size_t res = BufMoveFast(pBufB, pBufA);
  printf("BufMoveFast moved %zu item(s) from BufA to BufB\n", res);
  printf("*** \n");

  printf("After move:\n");
  printf("BufA:");
  PrintBuffer(pBufA);
  printf("BufB:");
  PrintBuffer(pBufB);

  #ifdef debug
  printf("A[0] = %u B[0] = %u\n", pBufA->data[0], pBufB->data[0]);
  printf("A[1] = %u B[1] = %u\n", pBufA->data[1], pBufB->data[1]);
  printf("A[2] = %u B[2] = %u\n", pBufA->data[2], pBufB->data[2]);
  printf("A[3] = %u B[3] = %u\n", pBufA->data[3], pBufB->data[3]);
  printf("A[4] = %u B[4] = %u\n", pBufA->data[4], pBufB->data[4]);
  printf("A[5] = %u B[5] = %u\n", pBufA->data[5], pBufB->data[5]);
  printf("A[6] = %u B[6] = %u\n", pBufA->data[6], pBufB->data[6]);
  printf("A[7] = %u B[7] = %u\n", pBufA->data[7], pBufB->data[7]);
  #endif

  printf("BufA first index: %zu \n", pBufA->first);
  printf("BufA last index: %zu \n", pBufA->last);
  printf("BufB first index: %zu \n", pBufB->first);
  printf("BufB last index: %zu \n", pBufB->last);

  // Test start
  uint8_t cnt = 0;
  for (uint8_t i = 0; i < GetDataCount(pBufB); i++)
  {
    if (pBufB->data[i] == testBufB[i])
    {
      cnt ++;
    }
  }

  #ifdef debug
  //printf("cnt...%u\n", cnt);
  //printf("GetDataCount(pBufB)...%zu\n", GetDataCount(pBufB));
  //printf("res...%zu\n", res);
  //printf("...%u\n", pBufA == pBufB);
  #endif

  // Test conditions
  if (res > 0 and !IsEmpty(&bufferA) and IsFull(&bufferB) and pBufA->data[1] == 5)
  {
    isTestPass = true;
    TestPassed += 1;
  };

  printf("*** memcpy() called %u times ***\n", memcpyRunCounter);

  // Test decision
  if (isTestPass)
  {    
    printf("******************\n");
    printf("Test: *** PASS ***\n");
  }
  else
  {
    printf("Test: *** FAIL ***\n");
  }
}

void TestCase_11(CircularBuffer* pBufA, CircularBuffer* pBufB)
{
  TestCount += 1;
  bool isTestPass = false;    
  uint8_t testBufB[] = {1, 5, 6, 7, 8, 9};
  memcpyRunCounter = 0;

  printf("\n");
  printf("%s - for BufMoveFast() WHERE: destination buffer not fragmented before data mooving\n", __func__ );
  printf(" AND: destination buffer fragmented after mooving data from source buffer \n");
  
  ClearBuf(pBufA);
  ClearBuf(pBufB);

  WriteByte(pBufA, 5);
  WriteByte(pBufA, 6);
  WriteByte(pBufA, 7);
  WriteByte(pBufA, 8);
  WriteByte(pBufA, 9);

// work with buff B
  for (int i = 0; i < BUFFER_SIZE - 2; i++) {
    WriteByte(pBufB, 0);
    ReadByte(pBufB);
  }
  
  WriteByte(pBufB, 1);
  // buf A - 5 6 7 8 9 
  // buf B - 1

  //  expected result: 1 5 6 7 8 9
  printf("pBufA before move:");
  PrintBuffer(pBufA);
  printf("pBufB before move:");
  PrintBuffer(pBufB);

  printf("BufA first index: %zu \n", pBufA->first);
  printf("BufA last index: %zu \n", pBufA->last);
  printf("BufB first index: %zu \n", pBufB->first);
  printf("BufB last index: %zu \n", pBufB->last);
  printf("*** \n");

  size_t res = BufMoveFast(pBufB, pBufA);
  printf("BufMoveSlow moved %zu item(s) from pBufA to pBufB\n", res);
  printf("*** \n");

  printf("pBufA after move:");
  PrintBuffer(pBufA);
  printf("pBufB after move:");
  PrintBuffer(pBufB);

  #ifdef debug
  printf("A[0] = %u B[0] = %u\n", pBufA->data[0], pBufB->data[0]);
  printf("A[1] = %u B[1] = %u\n", pBufA->data[1], pBufB->data[1]);
  printf("A[2] = %u B[2] = %u\n", pBufA->data[2], pBufB->data[2]);
  printf("A[3] = %u B[3] = %u\n", pBufA->data[3], pBufB->data[3]);
  printf("A[4] = %u B[4] = %u\n", pBufA->data[4], pBufB->data[4]);
  printf("A[5] = %u B[5] = %u\n", pBufA->data[5], pBufB->data[5]);
  printf("A[6] = %u B[6] = %u\n", pBufA->data[6], pBufB->data[6]);
  printf("A[7] = %u B[7] = %u\n", pBufA->data[7], pBufB->data[7]);
  #endif

  printf("BufA first index: %zu \n", pBufA->first);
  printf("BufA last index: %zu \n", pBufA->last);
  printf("BufB first index: %zu \n", pBufB->first);
  printf("BufB last index: %zu \n", pBufB->last);

  // Test start
  uint8_t cnt = 0;
  for (uint8_t i = 0; i < GetDataCount(pBufB); i++)
  {
    if (pBufB->data[i] == testBufB[i])
    {
      cnt ++;
    }
  }

  #ifdef debug
  //printf("cnt...%u\n", cnt);
  //printf("GetDataCount(pBufB)...%zu\n", GetDataCount(pBufB));
  //printf("res...%zu\n", res);
  //printf("...%u\n", pBufA == pBufB);
  #endif

  // Test conditions
  if (res > 0 and IsEmpty(&bufferA) and !IsFull(&bufferB))
  {
    isTestPass = true;
    TestPassed += 1;
  };

  printf("*** memcpy() called %u times ***\n", memcpyRunCounter);

  // Test decision
  if (isTestPass)
  {    
    printf("******************\n");
    printf("Test: *** PASS ***\n");
  }
  else
  {
    printf("Test: *** FAIL ***\n");
  }
}