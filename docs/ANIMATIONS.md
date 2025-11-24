# System Animacji - Sprite Sheety

## Przegl¹d

System animacji w grze wykorzystuje **sprite sheety** - pojedyncze pliki PNG zawieraj¹ce wszystkie klatki animacji u³o¿one poziomo.

## Format Sprite Sheetów

### Wymagania

- **Format pliku:** PNG (z kana³em alpha)
- **Uk³ad:** Poziomy pasek (horizontal strip)
- **Rozmiar klatki:** 100×100 pikseli (domyœlnie)
- **Klatki:** U³o¿one od lewej do prawej

### Przyk³adowe rozmiary

| Animacja | Liczba klatek | Rozmiar pliku | Rozmiar klatki |
|----------|---------------|---------------|----------------|
| Walk     | 8             | 800×100 px | 100×100 px     |
| Idle     | 6     | 600×100 px    | 100×100 px     |
| Attack   | 4             | 400×100 px| 100×100 px   |
| Death    | 5             | 500×100 px    | 100×100 px|

## Struktura katalogów

```
assets/
??? animations/
    ??? player/
?   ??? Soldier-Walk-right.png   (800×100, 8 klatek)
    ?   ??? Soldier-Walk-left.png    (800×100, 8 klatek)
    ?   ??? Soldier-Idle.png  (600×100, 6 klatek)
    ?   ??? Soldier-Attack.png       (400×100, 4 klatki)
    ??? enemies/
     ??? Goblin-Walk.png          (600×100, 6 klatek)
   ??? Goblin-Idle.png          (400×100, 4 klatki)
```

## Tworzenie sprite sheetów

### Narzêdzia

1. **Aseprite** - Edytor sprite'ów z eksportem do sprite sheetów
2. **TexturePacker** - Pakowanie wielu obrazków do jednego sprite sheeta
3. **GIMP/Photoshop** - Rêczne tworzenie (wszystkie klatki w jednym warstwie)

### Krok po kroku (GIMP/Photoshop)

1. Stwórz nowy obraz o rozmiarze `(liczba_klatek × 100) × 100` px
   - Np. dla 8 klatek: 800×100 px
2. W³¹cz kana³ alpha (przezroczystoœæ)
3. U³ó¿ klatki animacji od lewej do prawej
4. Upewnij siê ¿e ka¿da klatka zajmuje dok³adnie 100×100 px
5. Wyeksportuj jako PNG z przezroczystoœci¹

### Krok po kroku (Aseprite)

1. Stwórz animacjê z wieloma klatkami
2. File ? Export Sprite Sheet
3. **Sheet Type:** Horizontal Strip
4. **Constraints:** None (wszystkie klatki)
5. **Output File:** `nazwa-animacji.png`
6. Export

## Implementacja w kodzie

### £adowanie sprite sheeta

```cpp
// W Assets.cpp - automatycznie dzieli na klatki
IconId baseId = assets_->loadSpriteSheet(
    "assets/animations/Soldier-Walk.png",  // Œcie¿ka do sprite sheeta
    8,       // Liczba klatek
  100,     // Szerokoœæ pojedynczej klatki
    100      // Wysokoœæ pojedynczej klatki
);
```

### Tworzenie animacji

```cpp
// W GameSetup.cpp
world.spawnPlayer(
  "assets/characters/hero.png",  // Fallback tekstura
    64, 64,               // Rozmiar gracza
    100.0f, 100.0f, // Pozycja startowa
    100,       // Max HP
    "assets/animations/Soldier-Walk-right.png", 8,  // Walk animation
    "assets/animations/Soldier-Idle.png", 6         // Idle animation
);
```

## Parametry animacji

### W AnimationController.h

```cpp
float frameDuration_ = 0.1f;  // 10 FPS (zmieñ na 0.05f dla 20 FPS)
```

### Typy animacji

```cpp
enum class AnimationType {
    Walk,   // Chodzenie
    Idle    // Stanie w miejscu
};
```

### Dodawanie nowych typów

1. Dodaj typ w `AnimationType` enum
2. Dodaj ³adowanie w `AnimationController` konstruktorze
3. Dodaj zmianê typu w `World::update()` na podstawie stanu gracza

## Cache i Optymalizacja

- Sprite sheety s¹ **cache'owane** po pierwszym za³adowaniu
- Ka¿da klatka tworzy **osobn¹ GPU teksturê** (VkImage)
- Ponowne wywo³anie `loadSpriteSheet` z tymi samymi parametrami zwraca **cache'owany wynik**
- Cache key: `œcie¿ka_spritesheet_liczba_klatek_x_szerokoœæ_x_wysokoœæ`

## Rozwi¹zywanie problemów

### Animacja nie wyœwietla siê

1. SprawdŸ czy plik PNG istnieje w podanej œcie¿ce
2. SprawdŸ rozmiar pliku (szerokoœæ = `liczba_klatek × 100`)
3. SprawdŸ logi - powinien byæ b³¹d z `Assets.cpp`

### Klatki s¹ przesuniête

- Upewnij siê ¿e **ka¿da klatka** ma dok³adnie 100×100 px
- SprawdŸ czy sprite sheet nie ma marginesów/paddingu

### Animacja za szybka/wolna

Zmieñ `frameDuration_` w `AnimationController.h`:
- **Szybciej:** Zmniejsz wartoœæ (np. `0.05f` = 20 FPS)
- **Wolniej:** Zwiêksz wartoœæ (np. `0.2f` = 5 FPS)

## Przyk³adowe sprite sheety (dla testów)

Mo¿esz u¿yæ darmowych sprite'ów z:
- **itch.io** - https://itch.io/game-assets/free/tag-sprites
- **OpenGameArt.org** - https://opengameart.org/
- **Kenney.nl** - https://www.kenney.nl/assets

Pamiêtaj ¿eby przekonwertowaæ je do formatu poziomego paska (horizontal strip) 100×100 px per klatka.
