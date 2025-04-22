#include "ReflectionExample.h"

AReflectionExample::AReflectionExample()
{
    PrimaryActorTick.bCanEverTick = false;
    ExampleNumber = 0;
    PrivateNumber = 0;
    ReadOnlyString = TEXT("Это строка только для чтения");
}

void AReflectionExample::BeginPlay()
{
    Super::BeginPlay();
    
    // Вызываем событие при старте
    OnNumberChanged(0, ExampleNumber);
}

void AReflectionExample::SetExampleNumber(int32 NewNumber)
{
    // Сохраняем старое значение
    int32 OldNumber = ExampleNumber;
    
    // Устанавливаем новое значение
    ExampleNumber = NewNumber;
    
    // Вызываем событие
    OnNumberChanged(OldNumber, NewNumber);
} 