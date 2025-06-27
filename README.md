# 🌌 Dharma

> **Enlightenment through execution.**

**Dharma** is a dynamically typed, minimalist scripting language focused on clean syntax, expressive logic, and spiritual precision.

Currently under development — with the core interpreter (**Sutra**) taking its first breath.
Everything else is in the void, waiting to manifest.

---

## ⚙️ Core Components & Status

| 🧱 Component           | Name      | Description                                          | Status            |
| ---------------------- | --------- | ---------------------------------------------------- | ----------------- |
| 🔧 **Interpreter**     | `Sutra`   | Spiritual real-time execution.                       | 🛠️ *In Progress* |
| ❌ **Compiler**         | `Karma`   | Every line of code you write echoes here.            | ❌ Not started     |
| ❌ **VM**               | `Samsara` | Where code is reborn and executed in cycles.         | ❌ Not started     |
| ❌ **Standard Library** | `Mandala` | A web of sacred utilities.                           | ❌ Not started     |
| ❌ **Package Manager**  | `Ashram`  | Wisdom shared. Modules managed.                      | ❌ Not started     |
| ❌ **Build System**     | `Agni`    | Ignites and forges builds through fire.              | ❌ Not started     |
| ❌ **Debugger**         | `Drishti` | Sees into the soul of your program.                  | ❌ Not started     |
| ❌ **Formatter**        | `Bhava`   | Gives your code form, beauty, and expression.        | ❌ Not started     |
| ❌ **Linter**           | `Yama`    | Disciplines your code. No mercy.                     | ❌ Not started     |
| ❌ **REPL**             | `Mantra`  | Interactive chants of logic and return.              | ❌ Not started     |
| ❌ **IDE Plugin**       | `Mudra`   | Hand gesture to summon power in your editor.         | ❌ Not started     |
| ❌ **Doc Generator**    | `Veda`    | Sacred scrolls of wisdom from your comments.         | ❌ Not started     |
| ❌ **Profiler**         | `Tapa`    | Measures the heat of performance under ascetic fire. | ❌ Not started     |

---

## 🧠 Language Behavior & Features Demonstrated

Here’s what `sutra` — the interpreter — is already slinging like a yogi with a chainsaw:

### ⚡️ Variable Declaration Styles

Dharma supports both **C-style static declarations** and **dynamic `var`-based declarations**.

#### 🔹 C-style Static Typing

```dharma
int a = 10;
string name = "Yama";
boolean flag = true;
decimal pi = 3.14;
```

* You can optionally leave the value out:

  ```dharma
  int count;
  ```

* Once typed, the value **cannot change type**:

  ```dharma
  int x = 5;
  x = "boom"; // ❌ Type error
  ```

#### 🔹 Dynamic Typing with `var`

```dharma
var x = 100;
x = "string now";
x = false;
```

* Can change type during execution — this is **pure dynamic** mode.

#### 🔹 Hybrid Mode: `var: Type`

```dharma
var y: int = 100;
y = 200;     // ✅
y = true;    // ❌ Type error
```

* You declare with `var` for syntax consistency but **lock the type** with `: int`, `: string`, etc.
* Gives you the flexibility of dynamic syntax with static safety.

🛑 `var: var` is banned. So is `int a: int;` or `int a: string;`. Redundant or conflicting types will throw spiritual errors.

---

### 🔄 Block Scoping with Shadowing

```dharma
var a = 10
{
    var a = 50
    print a // prints 50
}
print a // prints 10
```

Inner blocks can **redefine** (`shadow`) outer variables, and they return to normal once the block closes.

---

### 🧘‍♂️ Semicolon Optionality

You want `;`? Fine. You don’t? Also fine.

```dharma
print 42;
print 42
```

Dharma don’t care — but you better be consistent with your chi.

---

### 🧮 Built-in Operations

* **Arithmetic:** `+`, `-`, `*`, `/`, `%`
* **Comparison:** `==`, `!=`, `<`, `<=`, `>`, `>=`
* **Logic:** `&&`, `||`, `!` (coming soon)
* **Concatenation:** `+` works for strings
* **Equality:** `==` is **type-safe**

---

## 🛣️ Planned Features

* Cross-platform bytecode execution (`Samsara`)
* REPL mode (`Mantra`)
* Package manager (`Ashram`)
* Formatter (`Bhava`) and linter (`Yama`)
* Doc generator (`Veda`)
* Debugger (`Drishti`) and profiler (`Tapa`)
* Build system (`Agni`) for scripting automation

---

## 🧪 Sample Dharma Script (`test.dh`)

```dharma
int a = 100;
var b: int = 200;
print a + b;
{
    int a = 200;
    print a * b;
}
print a;
{
    a = "500"
    print a;
    {
        a = true
        print a
    }
}
boolean c = true;
string d = 'hello'; print a == c
var e = " world"
print d + e
```

### Output

```
300
40000
100
500
true
true
hello world
```

---

## 🚀 Getting Started with Sutra

### Clone the Repo

```bash
git clone https://github.com/your-username/dharma.git
cd dharma
```

### Build with CMake

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

You'll get the `sutra` executable in the `build` directory.

### Run a Dharma Script

```bash
./sutra ../src/test.dh    # On Unix
.\sutra.exe ..\src\test.dh # On Windows
```

---

> *May your logic be clean. May your runtime be ruthless. May your bugs face Yama.*
