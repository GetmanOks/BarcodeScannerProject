# Руководство по настройке плагина сканера штрих-кодов в Unreal Engine 5.4

## Создание проекта и базовой структуры

1. Создание Blueprint проекта:
   - Выбрать тип проекта "Blueprint"
   - Назвать проект
   - Создать структуру папок в Content Browser:
     - `Content/Blueprints`
     - `Content/UI`
     - `Content/Input`

2. Создание базовых классов:
   - `BarcodeScannerActor` (Blueprint Actor)
   - `BarcodeScannerWidget` (Widget Blueprint)
   - `BarcodeScannerController` (Blueprint Class -> Player Controller)
   - `BarcodeScannerGameMode` (Blueprint Class -> Game Mode Base)

## Настройка Enhanced Input System

1. Создание Input Actions:
   - `IA_StartScanner`
   - `IA_StopScanner`

2. Создание Input Mapping Context:
   - `IMC_BarcodeScanner`
   - Настройка маппингов:
     - S -> IA_StartScanner
     - X -> IA_StopScanner

3. Настройка Project Settings:
   - Engine -> Input
   - Default Input System = Enhanced Input
   - Default Mouse Capture Mode = No Capture
   - Default Viewport Mouse Capture Mode = No Capture
   - Default Viewport Mouse Lock Mode = Do Not Lock

4. Настройка World Settings:
   - Game Mode:
     - Enable Mouse Control = true
     - Enable Touch Events = true

## Настройка BarcodeScannerController

1. Переменные:
   ```cpp
   - IMC_BarcodeScanner (Input Mapping Context)
   - IA_StartScanner (Input Action)
   - IA_StopScanner (Input Action)
   - ScannerWidget (BarcodeScannerWidget)
   - ScannerActor (BarcodeScannerActor)
   ```

2. Событие BeginPlay:
   ```cpp
   1. Get Player Controller
   2. Set Show Mouse Cursor = true
   3. Set Input Mode Game and UI with:
      - In Widget to Focus = (пусто)
      - In mouse lock mode = Do Not Lock
      - hide cursor during capture = false
      - flush input = true
   4. Get Enhanced Input Local Player Subsystem
   5. Add Mapping Context IMC_BarcodeScanner with Priority 0
   ```

3. Функции обработки ввода:
   ```cpp
   StartScannerInput:
   1. Is Valid ScannerActor?
      - Yes: Call StartScanner on ScannerActor
      - No: Print String "No scanner actor found"

   StopScannerInput:
   1. Is Valid ScannerActor?
      - Yes: Call StopScanner on ScannerActor
      - No: Print String "No scanner actor found"
   ```

## Настройка BarcodeScannerWidget

1. Переменные:
   ```cpp
   - ScannerActor (BarcodeScannerActor)
   - ScannedCode (Text)
   - ScannerStatus (Text)
   ```

2. Функция UpdateUI:
   ```cpp
   1. Is Valid ScannerActor?
      - Yes:
        1. Set ScannedCode = ScannerActor.LastScannedCode
        2. Set ScannerStatus = ScannerActor.ScannerStatus
      - No:
        1. Set ScannedCode = "No scanner found"
        2. Set ScannerStatus = "Scanner not initialized"
   ```

3. Событие Construct:
   ```cpp
   1. Is Valid ScannerActor?
      - Yes:
        1. Call UpdateUI
        2. Set Timer by Function Name "UpdateUI" with 0.1 second interval
      - No:
        1. Set ScannedCode = "No scanner found"
        2. Set ScannerStatus = "Scanner not initialized"
   ```

## Настройка BarcodeScannerActor

1. Переменные:
   ```cpp
   - bIsScannerActive (Boolean)
   - LastScannedCode (String)
   - ScannerStatus (Text)
   - ScannerData (BarcodeScannerData)
   ```

2. Функции:
   ```cpp
   StartScanner:
   1. Set bIsScannerActive = true
   2. Set ScannerStatus = "Scanner active"
   3. Set ScannerData.IsConnected = true
   4. Set ScannerData.DeviceName = "Mindeo Scanner"
   5. Set ScannerData.DeviceID = "USB Scanner"
   6. Set ScannerData.ScanTime = Now

   StopScanner:
   1. Set bIsScannerActive = false
   2. Set ScannerStatus = "Scanner inactive"
   3. Set ScannerData.IsConnected = false
   4. Clear ScannerData.LastScannedCode

   OnBarcodeScanned:
   1. Set LastScannedCode = Input Code
   2. Set ScannerStatus = "Code scanned: " + Code
   3. Set ScannerData.LastScannedCode = Code
   4. Set ScannerData.ScanTime = Now
   ```

## Структура BarcodeScannerData

```cpp
- DeviceID (String)
- DeviceName (String)
- IsConnected (Boolean)
- LastScannedCode (String)
- ScanTime (DateTime)
```

## Следующие шаги

1. Настройка работы с реальным сканером
2. Добавление обработки ошибок
3. Улучшение UI
4. Добавление сохранения истории сканирований
5. Настройка автоматического сканирования

## Примечания

- Все настройки сохраняются в соответствующих файлах конфигурации
- Project Settings сохраняются в `Config/DefaultEngine.ini`
- World Settings сохраняются в файле уровня (`.umap`)
- При перезапуске редактора все настройки сохраняются 