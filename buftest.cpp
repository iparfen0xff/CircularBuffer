#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
using namespace std;

#define uint8_t unsigned char

// дан циклический (кольцевой) буфер и некоторые функции работы с ним
#define BUFFER_SIZE 8

#if (BUFFER_SIZE & (BUFFER_SIZE - 1)) != 0
  #error "Incorrect buffer size"
#endif

typedef struct {
  size_t first;
  size_t last;
  uint8_t data[BUFFER_SIZE];
} CircularBuffer;

// ***********************************************************
// *** Local functions declaration
// ***********************************************************
size_t GetDataCount(CircularBuffer* pBuf);

// test func BufMoveSlow()
void TestCase_01(CircularBuffer* pBufA, CircularBuffer* pBufB);

// test func BufMoveFast()
void TestCase_02(CircularBuffer* pBufA, CircularBuffer* pBufB);
void TestCase_03(CircularBuffer* pBufA, CircularBuffer* pBufB);
void TestCase_04(CircularBuffer* pBufA, CircularBuffer* pBufB);
void TestCase_05(CircularBuffer* pBufA, CircularBuffer* pBufB);
void TestCase_06(CircularBuffer* pBufA, CircularBuffer* pBufB);
// ***********************************************************

// ClearBuf очищает буфер (может также использоваться для инициализации структуры CircularBuffer)
void ClearBuf(CircularBuffer* pBuf)
{
  pBuf->first = 0;
  pBuf->last = 0;
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
//  but not used yet
size_t GetEmptyCount(CircularBuffer* pBuf)
{
  return (BUFFER_SIZE - GetDataCount(pBuf));
}

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

  size_t count = 0; // init
  if ((GetDataCount(pSource)) < (BUFFER_SIZE - GetDataCount(pDest)))
  {
    //
    count = GetDataCount(pSource);
    memcpy(pDest->data+pDest->last, pSource->data+pSource->first, count);
    ClearBuf(pSource);
    pDest->last += count; 
    return count;
  }
  else
  {
    //
    count = (BUFFER_SIZE - 1) - GetDataCount(pDest);
    memcpy(pDest->data+pDest->last, pSource->data+pSource->first, count);
    pDest->last += count;
    pSource->first += count;
    return count;
  }
  
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
        ClearBuf(&bufferA);
        ClearBuf(&bufferB);

        WriteByte(&bufferA, 4);
        WriteByte(&bufferA, 5);
        WriteByte(&bufferA, 6);
        WriteByte(&bufferA, 7);        

        WriteByte(&bufferB, 1);
        WriteByte(&bufferB, 2);
        WriteByte(&bufferB, 3);

        printf("BufferA before move:");
        PrintBuffer(&bufferA);
        printf("BufferB before move:");
        PrintBuffer(&bufferB);

        size_t res = BufMoveSlow(&bufferB, &bufferA);
        printf("BufMoveSlow moved %zu item(s) from BufferA to BufferB\n", res);

        printf("BufferA after move:");
        PrintBuffer(&bufferA);
        printf("BufferB after move:");
        PrintBuffer(&bufferB);
        
        //WriteByte(&bufferB, 8);
        printf("add 8: %s\n", WriteByte(&bufferB, 8) ? "true" : "false");

        ReadByte(&bufferB);
        ReadByte(&bufferB); //
  
        printf("BufferB after read:");
        PrintBuffer(&bufferB);

        WriteByte(&bufferB, 1);

        printf("BufferB after write:");
        PrintBuffer(&bufferB);

        printf("bufferB->first: %zu \n", bufferB.first);
        printf("bufferB->last: %zu \n", bufferB.last);
        printf("GetDataCount B: %zu \n", GetDataCount(&bufferB));
        printf("last - first: %zu \n", (bufferB.last - bufferB.first) & (BUFFER_SIZE - 1));

        printf("IsEmpty A: %s\n", IsEmpty(&bufferA) ? "true" : "false");
        printf("IsEmpty B: %s\n", IsEmpty(&bufferB) ? "true" : "false");

        printf("IsFull A: %s\n", IsFull(&bufferA) ? "true" : "false");
        printf("IsFull B: %s\n", IsFull(&bufferB) ? "true" : "false");

        printf("GetDataCount A: %zu\n", GetDataCount(&bufferA));
        printf("GetDataCount B: %zu\n", GetDataCount(&bufferB));

        //
        TestCase_01(&bufferA, &bufferB);
        TestCase_02(&bufferA, &bufferA);
        TestCase_03(&bufferA, &bufferB);
        TestCase_04(&bufferA, &bufferB);
        TestCase_05(&bufferA, &bufferB);
        TestCase_06(&bufferA, &bufferB);

  return 0;
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
  CircularBuffer* tmp = pBufB; 
  printf("\n");
  printf("%s - for BufMoveFast() where BuffA and BuffB has common address\n", __func__ );

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

  pBufB = pBufA;
   
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

  printf("Restore addresses:\n");
  pBufB = tmp; // or use const params

  printf("BufA addres:");
  printf("%p\n", (void*) &pBufA);
  printf("BufB addres:");
  printf("%p\n", (void*) &pBufB);
}

void TestCase_03(CircularBuffer* pBufA, CircularBuffer* pBufB)
{ 
  printf("\n");
  printf("%s - for BufMoveFast() where destination buffer full\n", __func__ );

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

  printf("Is BuffA full: %s\n", IsFull(&bufferA) ? "true" : "false");
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

  printf("Is BuffA full: %s\n", IsFull(&bufferA) ? "true" : "false");
  printf("Is BuffB full: %s\n", IsFull(&bufferB) ? "true" : "false");
}

void TestCase_04(CircularBuffer* pBufA, CircularBuffer* pBufB)
{ 
  printf("\n");
  printf("%s - for BufMoveFast() where source buffer empty\n", __func__ );

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
}

void TestCase_05(CircularBuffer* pBufA, CircularBuffer* pBufB)
{ 
  printf("\n");
  printf("%s - for BufMoveFast() WHERE source buffer less than destination\n", __func__ );
  printf(" AND source buffer not empty AND destination buffer not full\n");
  
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
}

void TestCase_06(CircularBuffer* pBufA, CircularBuffer* pBufB)
{ 
  printf("\n");
  printf("%s - for BufMoveFast() WHERE source buffer more than destination\n", __func__ );
  printf(" AND source buffer not empty AND destination buffer not full\n");
  
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
}