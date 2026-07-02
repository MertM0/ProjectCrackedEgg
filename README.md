# 🥚 Cracked Egg

> A third-person action-adventure dungeon crawler. Fight enemies, hatch your dragon, and escape the dungeon.

![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.7-313131?logo=unrealengine&logoColor=white)
![C++](https://img.shields.io/badge/C%2B%2B-Core%20Logic-00599C?logo=cplusplus&logoColor=white)
![Status](https://img.shields.io/badge/status-prototype-green)

---

## 📖 About

You play as a knight trapped in a dungeon. Along the way, you discover a mysterious egg and hatch a **Dragon Companion** (Fire or Lightning). Together, you must defeat enemies, level up, and unlock the dragon's **Level 2** ranged attacks to break sealed walls. Defeat all enemies to activate the Victory Portal and escape.

All core gameplay systems are built in **C++**, with Blueprints managing UI, visuals, and animations.

---

## ✨ Features

- ⚔️ **Combat & Evasion:** Combo system (Light/Heavy attacks) and directional evade roll.
- 🐉 **Dragon Companion:** Automatically follows and dynamically fights (melee claws or ranged projectiles).
- 🧗 **Priority Target:** Command your Level 2+ dragon to destroy sealed walls.
- ⚡ **Elemental Status Effects:** Burn (Fire) and Slow (Lightning) combat modifiers.
- 📈 **Progression & Stats:** XP/leveling system with manual stat upgrades (Health, Stamina, Damage, Speed).
- 🚪 **Progression Gating:** Key-locked doors, lever systems, and destructible obstacles.

---

## 🎮 Controls

| Action | Key |
|--------|-----|
| Move | `W` `A` `S` `D` |
| Look | `Mouse` |
| Jump | `Space` |
| Dash | `Left Ctrl` |
| Light Attack | `Left Mouse Button` |
| Heavy Attack | `Right Mouse Button` |
| Interact | `E` |
| Stat Upgrades UI | `U` |

---

## 🚀 Getting Started

### 🎮 Option 1: Play the Build (itch.io)
1. Download the packaged game from [itch.io](https://mertm.itch.io/project-cracked-egg).
2. Extract the ZIP file and run `ProjectCrackedEgg.exe`.

### 💻 Option 2: Clone & Run (GitHub)
1. Clone this repository into your Unreal Projects directory.
2. Download the required asset packages from [Asset Download Link] (Link will be added) and extract them into the `Content/` folder.
3. Right-click `ProjectCrackedEgg.uproject` and select **Generate Visual Studio project files**.
4. Open the `.sln` file and build the project in **Development Editor / Win64**.
5. Launch the project in Unreal Editor 5.7.
