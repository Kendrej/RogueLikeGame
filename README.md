# RogueLike Dungeon Crawler

Gra typu roguelike napisana w **C++20** z renderowaniem przez **Vulkan** i interfejsem **ImGui**.

[![C++](https://img.shields.io/badge/C++-20-blue.svg)](https://isocpp.org/)
[![Vulkan](https://img.shields.io/badge/Vulkan-1.3-red.svg)](https://www.vulkan.org/)
[![CMake](https://img.shields.io/badge/CMake-3.20+-green.svg)](https://cmake.org/)

---

## O projekcie

**RogueLike Dungeon Crawler** to gra akcji z widokiem z góry, w której gracz eksploruje lochy, walczy z przeciwnikami i zbiera przedmioty. Projekt demonstruje zaawansowane techniki programowania gier w C++.

### Główne funkcje

- **System walki** - ataki wręcz i dystansowe z animacjami
- **Edytor map** - poziomy tworzone w aplikacji [Tiled](https://www.mapeditor.org/) i ładowane z plików TMX
- **System ekwipunku** - zbieranie i używanie przedmiotów (mikstury, klucze)
- **Interaktywne elementy** - drzwi zamykane na klucz, skrzynie ze skarbami
- **Różnorodni przeciwnicy** - Orkowie, Szkielety-Łucznicy, Rycerze z własną AI
- **Roguelike mechaniki** - permadeath z możliwością odrodzenia

---

## Technologie

| Kategoria | Technologia |
|-----------|-------------|
| **Język** | C++20 |
| **Grafika** | Vulkan API |
| **UI** | Dear ImGui |
| **Okna** | GLFW |
| **Mapy** | TMXLite + Tiled Map Editor |
| **Matematyka** | GLM |
| **Build** | CMake + Ninja |
| **Zależności** | vcpkg |

---

## Architektura

```
src/
├── engine/           # Silnik gry
│   └── gfx/          # Rendering Vulkan, Assets
├── game/
│   ├── entities/     # Entity, Player, Npc, Projectile
│   │   └── animation/# System animacji
│   ├── factory/      # Fabryki (Player, Npc, Item)
│   ├── item/         # System przedmiotów i ekwipunku
│   ├── npc/          # AI przeciwników
│   └── world/        # Świat gry, mapy, kolizje
├── app/              # Główna pętla gry
└── utils/            # Narzędzia pomocnicze
```

### Kluczowe wzorce projektowe:
- **Entity Component System** - elastyczna architektura encji
- **Factory Pattern** - tworzenie graczy, NPC i przedmiotów
- **State Machine** - AI przeciwników i system animacji

---

## Uruchomienie

### Wymagania
- Windows 10/11
- Visual Studio 2022 (Desktop development with C++) lub Build Tools 2022
- CMake 3.20+
- Ninja

### Instalacja narzędzi

```powershell
# Przez winget
winget install -e --id Kitware.CMake
winget install -e --id Ninja-build.Ninja

# Lub przez Chocolatey
choco install cmake ninja -y
```

### Build (PowerShell)

```powershell
# Klonowanie z submodułami
git clone --recursive https://github.com/erybie222/RogueLikeGame.git
cd RogueLikeGame

# Konfiguracja i build
cmake --preset win-ninja-debug
cmake --build --preset win-ninja-debug-build

# Uruchomienie
.\build\win-ninja-debug\bin\RogueLikeGame.exe
```

### Rozwiązywanie problemów

Jeśli build używa złej architektury (x86 zamiast x64), załaduj środowisko VS:

```powershell
cmd /c '"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -host_arch=x64 -arch=x64 && set' |
  ForEach-Object {
    if ($_ -match '^(INCLUDE|LIB|LIBPATH|PATH)=') {
      $n,$v = $_ -split '=',2
      Set-Item -Path Env:$n -Value $v
    }
  }
```

---

## Tworzenie map

Mapy są tworzone w aplikacji **Tiled Map Editor** i zapisywane jako pliki `.tmx` w folderze `assets/maps/`.

Każda mapa może zawierać:
- Warstwy kafelków (podłogi, ściany)
- Obiekty (spawn NPC, skrzynie, drzwi, przejścia między mapami)
- Właściwości kafelków (solid, door, locked)

---

## Sterowanie

| Klawisz | Akcja |
|---------|-------|
| `W A S D` | Ruch |
| `Spacja` | Atak |
| `Q` | Zmiana trybu ataku (wręcz/dystans) |
| `E` | Interakcja (skrzynie, drzwi) |
| `1-9` | Użyj przedmiotu z ekwipunku |
| `ESC` | Pauza |

---

## TODO / Plany rozwoju

- [ ] Więcej typów przeciwników
- [ ] System umiejętności gracza
- [ ] Proceduralne generowanie lochów
- [ ] Zapisywanie postępów
- [ ] Efekty dźwiękowe i muzyka

---

## Autor

**[Twoje Imię]**
- GitHub: [@erybie222](https://github.com/erybie222)
- LinkedIn: [Twój profil]
