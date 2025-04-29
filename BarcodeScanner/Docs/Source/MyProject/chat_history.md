# Чат по разработке CameraPawn для Unreal Engine 5.4.4

## 1. Начальная реализация CameraPawn

### Задача
Реализация класса CameraPawn для управления камерой через touchscreen в Unreal Engine 5.4.4.

### Исходный код
#### CameraPawn.h
```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "CameraPawn.generated.h"

// Макросы для логирования
#define LOG_CAMERA(Verbosity, Format, ...) UE_LOG(LogTemp, Verbosity, TEXT("[CameraPawn] %s"), *FString::Printf(TEXT(Format), ##__VA_ARGS__))
#define LOG_CAMERA_INFO(Format, ...) LOG_CAMERA(Log, Format, ##__VA_ARGS__)
#define LOG_CAMERA_WARNING(Format, ...) LOG_CAMERA(Warning, Format, ##__VA_ARGS__)
#define LOG_CAMERA_ERROR(Format, ...) LOG_CAMERA(Error, Format, ##__VA_ARGS__)

UCLASS()
class YOURPROJECT_API ACameraPawn : public APawn
{
    GENERATED_BODY()

public:
    ACameraPawn();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
    // Компоненты камеры
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* CameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    USpringArmComponent* SpringArmComponent;

    // Параметры камеры
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float CameraSpeed = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float ZoomSpeed = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float MinZoomDistance = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float MaxZoomDistance = 1000.0f;

    // Функции обработки касаний
    void TouchPressed(ETouchIndex::Type FingerIndex, FVector Location);
    void TouchReleased(ETouchIndex::Type FingerIndex, FVector Location);
    void TouchMoved(ETouchIndex::Type FingerIndex, FVector Location);

private:
    // Состояние касаний
    bool bIsTouching;
    FVector2D TouchStart;
    FVector2D LastTouchLocation;
    float CurrentZoomDistance;
};
```

#### CameraPawn.cpp
```cpp
#include "CameraPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"

ACameraPawn::ACameraPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    // Создаем SpringArm компонент
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComponent->SetupAttachment(RootComponent);
    SpringArmComponent->TargetArmLength = 500.0f;
    SpringArmComponent->bUsePawnControlRotation = true;
    SpringArmComponent->bEnableCameraLag = true;
    SpringArmComponent->CameraLagSpeed = 3.0f;

    // Создаем Camera компонент
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
    CameraComponent->bUsePawnControlRotation = false;

    // Инициализация переменных
    bIsTouching = false;
    CurrentZoomDistance = SpringArmComponent->TargetArmLength;

    LOG_CAMERA_INFO("CameraPawn created with SpringArm length: %f", CurrentZoomDistance);
}

void ACameraPawn::BeginPlay()
{
    Super::BeginPlay();

    // Настраиваем ввод
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->bShowMouseCursor = true;
        PC->bEnableClickEvents = true;
        PC->bEnableTouchEvents = true;
        PC->bEnableMouseOverEvents = true;
    }

    LOG_CAMERA_INFO("CameraPawn started with controller: %s", 
        GetController() ? *GetController()->GetName() : TEXT("None"));
}

void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Привязываем функции обработки касаний
    PlayerInputComponent->BindTouch(IE_Pressed, this, &ACameraPawn::TouchPressed);
    PlayerInputComponent->BindTouch(IE_Released, this, &ACameraPawn::TouchReleased);
    PlayerInputComponent->BindTouch(IE_Repeat, this, &ACameraPawn::TouchMoved);

    LOG_CAMERA_INFO("Touch input bindings set up");
}

void ACameraPawn::TouchPressed(ETouchIndex::Type FingerIndex, FVector Location)
{
    bIsTouching = true;
    TouchStart = FVector2D(Location.X, Location.Y);
    LastTouchLocation = TouchStart;

    LOG_CAMERA_INFO("Touch pressed at location: X=%.2f, Y=%.2f", 
        TouchStart.X, TouchStart.Y);
}

void ACameraPawn::TouchReleased(ETouchIndex::Type FingerIndex, FVector Location)
{
    bIsTouching = false;
    FVector2D TouchEnd = FVector2D(Location.X, Location.Y);

    LOG_CAMERA_INFO("Touch released at location: X=%.2f, Y=%.2f", 
        TouchEnd.X, TouchEnd.Y);
}

void ACameraPawn::TouchMoved(ETouchIndex::Type FingerIndex, FVector Location)
{
    if (!bIsTouching) return;

    FVector2D CurrentTouch = FVector2D(Location.X, Location.Y);
    FVector2D Delta = CurrentTouch - LastTouchLocation;

    // Обновляем последнюю позицию касания
    LastTouchLocation = CurrentTouch;

    // Вычисляем изменение зума на основе вертикального движения
    float ZoomDelta = -Delta.Y * ZoomSpeed;
    CurrentZoomDistance = FMath::Clamp(
        CurrentZoomDistance + ZoomDelta,
        MinZoomDistance,
        MaxZoomDistance
    );

    LOG_CAMERA_INFO("Touch moved - Delta: X=%.2f, Y=%.2f, New zoom: %.2f",
        Delta.X, Delta.Y, CurrentZoomDistance);
}

void ACameraPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Плавное обновление зума
    if (SpringArmComponent->TargetArmLength != CurrentZoomDistance)
    {
        float NewLength = FMath::FInterpTo(
            SpringArmComponent->TargetArmLength,
            CurrentZoomDistance,
            DeltaTime,
            5.0f
        );
        SpringArmComponent->TargetArmLength = NewLength;
    }
}
```

## 2. Добавление эмуляции тач-событий через мышь

### Изменения в CameraPawn.h
```cpp
// ... existing code ...
    // Функции обработки мыши для эмуляции тач-событий
    void MousePressed();
    void MouseReleased();
    void MouseMoved();
// ... existing code ...
```

### Изменения в CameraPawn.cpp
```cpp
void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Привязываем функции обработки касаний
    PlayerInputComponent->BindTouch(IE_Pressed, this, &ACameraPawn::TouchPressed);
    PlayerInputComponent->BindTouch(IE_Released, this, &ACameraPawn::TouchReleased);
    PlayerInputComponent->BindTouch(IE_Repeat, this, &ACameraPawn::TouchMoved);

    // Привязываем функции обработки мыши для эмуляции тач-событий
    PlayerInputComponent->BindAction("MouseLeftClick", IE_Pressed, this, &ACameraPawn::MousePressed);
    PlayerInputComponent->BindAction("MouseLeftClick", IE_Released, this, &ACameraPawn::MouseReleased);
    PlayerInputComponent->BindAxis("MouseX", this, &ACameraPawn::MouseMoved);
    PlayerInputComponent->BindAxis("MouseY", this, &ACameraPawn::MouseMoved);

    LOG_CAMERA_INFO("Input bindings set up");
}

void ACameraPawn::MousePressed()
{
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        FVector2D MousePosition;
        PC->GetMousePosition(MousePosition.X, MousePosition.Y);
        TouchPressed(ETouchIndex::Touch1, FVector(MousePosition, 0));
    }
}

void ACameraPawn::MouseReleased()
{
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        FVector2D MousePosition;
        PC->GetMousePosition(MousePosition.X, MousePosition.Y);
        TouchReleased(ETouchIndex::Touch1, FVector(MousePosition, 0));
    }
}

void ACameraPawn::MouseMoved()
{
    if (bIsTouching && APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        FVector2D MousePosition;
        PC->GetMousePosition(MousePosition.X, MousePosition.Y);
        TouchMoved(ETouchIndex::Touch1, FVector(MousePosition, 0));
    }
}
```

## 3. Миграция на Enhanced Input System

### Изменения в CameraPawn.h
```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "CameraPawn.generated.h"

// ... existing code ...

protected:
    // Input Actions
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputAction* TouchPressAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputAction* TouchReleaseAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputAction* TouchMoveAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputMappingContext* DefaultMappingContext;

    // Функции обработки Enhanced Input
    void OnTouchPress(const FInputActionValue& Value);
    void OnTouchRelease(const FInputActionValue& Value);
    void OnTouchMove(const FInputActionValue& Value);

// ... existing code ...
```

### Изменения в CameraPawn.cpp
```cpp
#include "CameraPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// ... existing code ...

void ACameraPawn::BeginPlay()
{
    Super::BeginPlay();

    // Настраиваем Enhanced Input
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    LOG_CAMERA_INFO("CameraPawn started with controller: %s", 
        GetController() ? *GetController()->GetName() : TEXT("None"));
}

void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Настраиваем Enhanced Input
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(TouchPressAction, ETriggerEvent::Started, this, &ACameraPawn::OnTouchPress);
        EnhancedInputComponent->BindAction(TouchReleaseAction, ETriggerEvent::Completed, this, &ACameraPawn::OnTouchRelease);
        EnhancedInputComponent->BindAction(TouchMoveAction, ETriggerEvent::Triggered, this, &ACameraPawn::OnTouchMove);
    }

    LOG_CAMERA_INFO("Enhanced Input bindings set up");
}

void ACameraPawn::OnTouchPress(const FInputActionValue& Value)
{
    bIsTouching = true;
    const FVector2D TouchPosition = Value.Get<FVector2D>();
    TouchStart = TouchPosition;
    LastTouchLocation = TouchPosition;

    LOG_CAMERA_INFO("Touch pressed at location: X=%.2f, Y=%.2f", 
        TouchStart.X, TouchStart.Y);
}

void ACameraPawn::OnTouchRelease(const FInputActionValue& Value)
{
    bIsTouching = false;
    const FVector2D TouchPosition = Value.Get<FVector2D>();

    LOG_CAMERA_INFO("Touch released at location: X=%.2f, Y=%.2f", 
        TouchPosition.X, TouchPosition.Y);
}

void ACameraPawn::OnTouchMove(const FInputActionValue& Value)
{
    if (!bIsTouching) return;

    const FVector2D TouchPosition = Value.Get<FVector2D>();
    FVector2D Delta = TouchPosition - LastTouchLocation;
    LastTouchLocation = TouchPosition;

    // Вычисляем изменение зума на основе вертикального движения
    float ZoomDelta = -Delta.Y * ZoomSpeed;
    CurrentZoomDistance = FMath::Clamp(
        CurrentZoomDistance + ZoomDelta,
        MinZoomDistance,
        MaxZoomDistance
    );

    LOG_CAMERA_INFO("Touch moved - Delta: X=%.2f, Y=%.2f, New zoom: %.2f",
        Delta.X, Delta.Y, CurrentZoomDistance);
}
```

## 4. Настройка Enhanced Input в редакторе

### Создание Input Actions
1. Создать папку `Input` в Content Browser
2. Создать три Input Action:
   - `IA_TouchPress` (Value Type: Digital, Trigger Events: Started)
   - `IA_TouchRelease` (Value Type: Digital, Trigger Events: Completed)
   - `IA_TouchMove` (Value Type: Axis2D, Trigger Events: Triggered)

### Создание Input Mapping Context
1. Создать `IMC_CameraControls`
2. Настроить маппинги:
   ```
   IA_TouchPress:
   - Left Mouse Button (для тестирования)
   - Touch (для реального тачскрина)

   IA_TouchRelease:
   - Left Mouse Button (для тестирования)
   - Touch (для реального тачскрина)

   IA_TouchMove:
   - Mouse XY (для тестирования)
   - Touch (для реального тачскрина)
   ```

### Настройка Blueprint
1. Открыть Blueprint CameraPawn
2. В Details панели найти секцию "Input"
3. Присвоить созданные Input Actions и Input Mapping Context соответствующим полям

## 5. Тестирование
- Нажатие левой кнопки мыши = тач-событие
- Перетаскивание мыши = перемещение камеры
- Вертикальное движение мыши = зум камеры

## 6. Преимущества нового подхода
- Более гибкая система ввода
- Лучшая поддержка разных устройств
- Возможность легко добавлять новые типы ввода
- Более чистая и понятная структура кода

## 7. Важные замечания
- Для работы Enhanced Input нужно включить соответствующий плагин в настройках проекта:
  1. Открыть Project Settings (Edit -> Project Settings)
  2. Перейти в раздел Plugins
  3. Найти "Enhanced Input"
  4. Включить плагин
  5. Перезапустить редактор
- Ошибки линтера, связанные с заголовочными файлами Unreal Engine, можно игнорировать
- Рекомендуется использовать Blueprint для быстрой настройки параметров

## 8. Дополнительные настройки проекта
Для корректной работы Enhanced Input также рекомендуется:
1. В Project Settings -> Input:
   - Убедиться, что "Default Input Component Class" установлен как "EnhancedInputComponent"
   - Проверить, что "Default Player Input Class" установлен как "EnhancedPlayerInput"
2. В Project Settings -> Engine - Input:
   - Включить "Enable Enhanced Input Support"
   - Установить "Default Input Component Class" как "EnhancedInputComponent"
   - Установить "Default Player Input Class" как "EnhancedPlayerInput"

## 9. Советы по настройке Input Actions

### IA_TouchMove (движение)
- **Value Type:** Axis2D (Vector2D)
- **Triggers:** None (оставь пустым)
- **Почему так:**
    - Для осевых действий (движение, зум, поворот) триггер обычно не нужен, потому что действие должно срабатывать при любом изменении значения.
    - Если выбрать Pressed/Released, действие будет срабатывать только при нажатии/отпускании, а не при движении.
- **Итог:**
    - Просто оставь Triggers пустым, и твой Move Action будет срабатывать каждый раз, когда есть движение мыши или тач-событие.

---

Если нужно — могу добавить ASCII-схему или дополнительные пояснения! 😊 