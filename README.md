# 🐍 Terminal Snake Game

*Terminal Snake Game* is a classic implementation of the snake game written in C using the `ncurses` library. It features real-time controls, growing snake mechanics, and a simple score system — all playable directly in the terminal.

---

## 🎯 Purpose

This game provides a nostalgic, terminal-based experience of the iconic Snake game.  
It’s a fun way to explore C programming concepts such as pointers, linked lists, threads, and real-time input.

---

## 🧩 App Architecture

Designed with modularity and clarity in mind:

| 🧱 Component      | 📌 Description                            |
|------------------|--------------------------------------------|
| *SnakeSegment*   | Linked list nodes representing snake body  |
| *Food*           | Struct for food location                   |
| *init_game()*    | Initializes game, snake, UI, and food      |
| *update_snake()* | Threaded logic for snake movement and growth |
| *draw_game()*    | Renders the game board and UI in terminal  |

---

## 🚀 Features

<ul>
  <li>🎮 <strong>Real-time Controls</strong>: Use <code>WASD</code> to control the snake.</li>
  <li>🍏 <strong>Random Food Placement</strong>: Eat food to grow and increase score.</li>
  <li>🚧 <strong>Wall Collision Detection</strong>: Game ends on collision.</li>
  <li>🐍 <strong>Self-Collision Detection</strong>: Avoid biting yourself!</li>
  <li>🔢 <strong>Score Display</strong>: Track your performance after each game.</li>
</ul>

---

## 📸 Screenshots

> Run the game in your terminal to enjoy the experience!

<p align="center">
  <img src="https://github.com/bsaltalii/snake_game/blob/main/screenshots/game_1.png" width="800"/>
  <img src="https://github.com/bsaltalii/snake_game/blob/main/screenshots/game_2.png" width="800"/>
</p>

---

## 🖥️ Platform

•⁠  ⁠Linux / Unix-based Terminals  
•⁠  ⁠Compatible with macOS terminal  

---

## 📦 Installation

### Prerequisites

•⁠  ⁠GCC compiler  
•⁠  ⁠`ncurses` library  
•⁠  ⁠Make or terminal access

### Steps

```bash
# Clone the repository
git clone https://github.com/bsaltalii/terminal-snake-game.git

# Navigate to the project directory
cd terminal-snake-game

# Compile the game
gcc snakegame.c -lncurses -lpthread -o snakegame

# Run the game
./snakegame
```

> Press `q` to quit the game at any time.

---

## 👤 Developer

<table>
  <tbody>
    <tr>
      <td align="center">
        <a href="https://github.com/bsaltalii">
          <img src="https://avatars.githubusercontent.com/u/96692734?v=4" width="100px;" alt="Developer Avatar"/>
          <br /><sub><b>Burak Saltalı</b></sub>
        </a>
      </td>
    </tr>
  </tbody>
</table>

---

## 📝 License

This project is licensed under the *MIT License*.  
See the [LICENSE](LICENSE) file for more information.
