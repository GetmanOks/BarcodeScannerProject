# Документация по системе камеры в проекте

## Оглавление
1. [Общее описание](#общее-описание)
2. [Основные компоненты](#основные-компоненты)
3. [Настройка камеры](#настройка-камеры)
4. [Автоматическое вращение](#автоматическое-вращение)
5. [Позиционирование камеры](#позиционирование-камеры)
6. [Переключение между уровнями](#переключение-между-уровнями)
7. [Рекомендации по настройке](#рекомендации-по-настройке)
8. [Обработка смены уровней](#обработка-смены-уровней)

## Общее описание
Система камеры реализована в классе `CameraPawn` и обеспечивает:
- Автоматическое вращение вокруг объектов
- Умное позиционирование с учетом размеров объектов
- Плавное перемещение между позициями
- Автоматическую настройку при смене уровней

## Основные компоненты
- **Spring Arm Component**: Обеспечивает плавное движение камеры
- **Camera Component**: Основной компонент камеры
- **Enhanced Input System**: Система ввода для управления камерой

## Настройка камеры

### Основные параметры камеры
```cpp
// Базовые настройки
float CameraSpeed = 5.0f;        // Скорость движения камеры
float ZoomSpeed = 50.0f;         // Скорость зума
float MinZoomDistance = 100.0f;  // Минимальное расстояние зума
float MaxZoomDistance = 1000.0f; // Максимальное расстояние зума
```

### Параметры автоматического вращения
```cpp
bool bEnableAutoRotation = false;   // Включение/выключение вращения
float AutoRotationSpeed = 10.0f;    // Скорость вращения
float AutoRotationRadius = 500.0f;  // Радиус вращения
float AutoRotationHeight = 200.0f;  // Высота камеры
```

### Параметры умного позиционирования
```cpp
float RadiusMultiplier = 0.5f;    // Множитель радиуса (0.1 - 2.0)
float HeightMultiplier = 0.3f;    // Множитель высоты (0.1 - 2.0)
float PaddingPercent = 5.0f;      // Отступ от границ объектов (0 - 50%)
float MinRotationRadius = 500.0f;  // Минимальный радиус
float MaxRotationRadius = 2000.0f; // Максимальный радиус
float MaxHeight = 1000.0f;        // Максимальная высота
```

## Автоматическое вращение

### Управление вращением
```cpp
void StartAutoRotation();  // Включить вращение
void StopAutoRotation();   // Выключить вращение
```

### Настройка в Blueprint
1. Установите `bEnableAutoRotation = true`
2. Настройте `AutoRotationSpeed` (рекомендуется 0.1 - 1.0)
3. `AutoRotationRadius` и `AutoRotationHeight` устанавливаются автоматически

## Позиционирование камеры

### Функции позиционирования
```cpp
void CalculateOptimalCameraPosition();  // Расчет оптимальной позиции
void SetRotationCenter(const FVector& NewCenter);  // Установка центра вращения
```

### Алгоритм расчета позиции
1. Находит все целевые объекты через `GetTargetActors()`
2. Вычисляет границы объектов через `GetTargetActorsBounds()`
3. Рассчитывает оптимальный радиус и высоту
4. Применяет множители и ограничения
5. Устанавливает камеру в рассчитанную позицию

## Переключение между уровнями

### Система позиций камеры
```cpp
TMap<FString, FVector> CameraPositions;  // Карта позиций для уровней
```

### Функции переключения
```cpp
void MoveCameraToPosition(const FString& LevelName);  // Перемещение камеры
void NotifyOnLevelChange();  // Обработчик смены уровня
```

### Процесс смены уровня
1. При загрузке уровня вызывается `NotifyOnLevelChange`
2. Проверяется наличие предустановленной позиции
3. Если позиция найдена - камера перемещается в неё
4. Если нет - вычисляется оптимальная позиция

## Рекомендации по настройке

### Начальные значения
```cpp
RadiusMultiplier = 0.5f    // Половина базового радиуса
HeightMultiplier = 0.3f    // 30% от базовой высоты
PaddingPercent = 5.0f      // Небольшой отступ
MinRotationRadius = 500.0f // Минимальная дистанция
MaxRotationRadius = 2000.0f // Максимальная дистанция
MaxHeight = 1000.0f        // Максимальная высота
```

### Тонкая настройка
1. Если камера слишком далеко:
   - Уменьшите `RadiusMultiplier` и `HeightMultiplier`
   - Уменьшите `PaddingPercent`

2. Если камера слишком близко:
   - Увеличьте множители
   - Увеличьте `MinRotationRadius`

3. Для плавности движения:
   - Настройте `CameraSpeed`
   - Включите `bEnableCameraLag` на Spring Arm
   - Настройте `CameraLagSpeed`

### Blueprint настройка
1. Откройте BP_CameraPawn
2. Найдите секцию "Camera|SceneObjects"
3. Настройте параметры под ваши нужды
4. Протестируйте на разных уровнях

### Советы по отладке
- Используйте `LOG_CAMERA_INFO` для отслеживания изменений
- Проверяйте границы объектов через визуализацию
- Тестируйте на уровнях разного размера 

## Обработка смены уровней

### События смены уровня
В Blueprint камеры (`BP_CameraPawn`) доступны следующие события:
- **Level Loaded**: Вызывается при загрузке уровня
- **On Level Changed**: Вызывается при смене уровня
- **Post Load Map**: Вызывается после загрузки карты

### Реализация в Blueprint
1. Откройте `BP_CameraPawn`
2. В Event Graph добавьте следующие ноды:
   ```
   Event Level Loaded
   ├── Call NotifyOnLevelChange
   │   └── Set Camera Position
   │       └── Calculate Optimal Position
   ```

3. Настройте порядок вызова:
   ```
   Level Loaded -> On Level Changed -> Post Load Map
   ```

### Когда вызывать события
1. **Level Loaded**:
   - При первой загрузке уровня
   - После загрузки всех объектов
   - Когда все компоненты инициализированы

2. **On Level Changed**:
   - При переходе между уровнями
   - После загрузки нового уровня
   - Перед началом геймплея

3. **Post Load Map**:
   - После полной загрузки карты
   - Когда все объекты размещены
   - Перед активацией игровой логики

### Пример реализации в Blueprint
```cpp
// В BP_CameraPawn
Event Level Loaded
{
    // 1. Проверяем готовность камеры
    if (IsCameraReady)
    {
        // 2. Вызываем NotifyOnLevelChange
        NotifyOnLevelChange();
        
        // 3. Устанавливаем позицию
        if (HasPresetPosition)
        {
            MoveCameraToPosition(CurrentLevelName);
        }
        else
        {
            CalculateOptimalCameraPosition();
        }
    }
}
```

### Рекомендации по использованию
1. Всегда проверяйте готовность камеры перед вызовом событий
2. Используйте задержки при необходимости:
   ```cpp
   Delay 0.1s -> NotifyOnLevelChange
   ```
3. Добавьте проверки на валидность:
   ```cpp
   if (IsValid(CameraComponent))
   {
       NotifyOnLevelChange();
   }
   ```

### Отладка событий
1. Добавьте логирование:
   ```cpp
   LOG_CAMERA_INFO("Level Loaded: %s", *CurrentLevelName);
   ```
2. Используйте визуализацию:
   ```cpp
   DrawDebugSphere(GetWorld(), CameraLocation, 100.0f, 12, FColor::Green);
   ```
3. Проверяйте состояние:
   ```cpp
   if (GEngine)
   {
       GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, 
           FString::Printf(TEXT("Camera State: %s"), *GetCameraState()));
   }
   ```

### Частые ошибки
1. Вызов событий до инициализации камеры
2. Отсутствие проверки на валидность компонентов
3. Неправильный порядок вызова событий
4. Отсутствие обработки ошибок

### Советы по оптимизации
1. Используйте кэширование позиций:
   ```cpp
   if (CachedPositions.Contains(LevelName))
   {
       MoveCameraToPosition(CachedPositions[LevelName]);
   }
   ```
2. Добавьте плавные переходы:
   ```cpp
   SetCameraLocationWithLerp(NewPosition, 1.0f);
   ```
3. Оптимизируйте проверки:
   ```cpp
   if (ShouldUpdateCameraPosition())
   {
       NotifyOnLevelChange();
   }
   ``` 