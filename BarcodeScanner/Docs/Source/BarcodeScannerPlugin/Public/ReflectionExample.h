#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReflectionExample.generated.h"

UCLASS(Blueprintable) // Blueprintable позволяет создавать Blueprint на основе этого класса
class BARCODESCANNERPLUGIN_API AReflectionExample : public AActor
{
    GENERATED_BODY()

public:
    AReflectionExample();

    // Свойство, которое можно редактировать в редакторе
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reflection Example")
    int32 ExampleNumber;

    // Свойство, которое можно редактировать только в Blueprint
    UPROPERTY(BlueprintReadOnly, Category = "Reflection Example")
    FString ReadOnlyString;

    // Функция, которую можно вызвать из Blueprint
    UFUNCTION(BlueprintCallable, Category = "Reflection Example")
    void SetExampleNumber(int32 NewNumber);

    // Событие, которое можно реализовать в Blueprint
    UFUNCTION(BlueprintImplementableEvent, Category = "Reflection Example")
    void OnNumberChanged(int32 OldNumber, int32 NewNumber);

protected:
    virtual void BeginPlay() override;

private:
    // Приватное свойство, недоступное извне
    int32 PrivateNumber;
}; 