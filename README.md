# 🌌 Dharma

> **Enlightenment through execution.**

**Dharma** is a dynamically typed, minimalist scripting language focused on clean syntax, expressive logic, and spiritual precision.

Currently under development — with the core interpreter (**Sutra**) taking its first breath.  
Everything else is in the void, waiting to manifest.

---

## ⚙️ Core Components & Status

| 🧱 Component         | Name       | Description                                           | Status           |
|----------------------|------------|-------------------------------------------------------|------------------|
| 🔧 **Interpreter**   | `Sutra`    | Spiritual real-time execution.                       | 🛠️ *In Progress* |
| ❌ **Compiler**      | `Karma`    | Every line of code you write echoes here.            | ❌ Not started    |
| ❌ **VM**            | `Samsara`  | Where code is reborn and executed in cycles.         | ❌ Not started    |
| ❌ **Standard Library** | `Mandala`  | A web of sacred utilities.                           | ❌ Not started    |
| ❌ **Package Manager** | `Ashram`   | Wisdom shared. Modules managed.                      | ❌ Not started    |
| ❌ **Build System**  | `Agni`     | Ignites and forges builds through fire.              | ❌ Not started    |
| ❌ **Debugger**      | `Drishti`  | Sees into the soul of your program.                  | ❌ Not started    |
| ❌ **Formatter**     | `Bhava`    | Gives your code form, beauty, and expression.        | ❌ Not started    |
| ❌ **Linter**        | `Yama`     | Disciplines your code. No mercy.                     | ❌ Not started    |
| ❌ **REPL**          | `Mantra`   | Interactive chants of logic and return.              | ❌ Not started    |
| ❌ **IDE Plugin**    | `Mudra`    | Hand gesture to summon power in your editor.         | ❌ Not started    |
| ❌ **Doc Generator** | `Veda`     | Sacred scrolls of wisdom from your comments.         | ❌ Not started    |
| ❌ **Profiler**      | `Tapa`     | Measures the heat of performance under ascetic fire. | ❌ Not started    |

---

## 🧠 Language Behavior & Features Demonstrated

Here’s what `sutra` — the interpreter — is already slinging like a yogi with a chainsaw:

### Dynamic Typing with Optional Static Annotations

- `var` declares a variable with dynamic type — it can change its type during execution:

  ```dharma
  var x = 100
  x = "a string now"
  x = true

* But if you use type annotations like `var y: int`, it **locks** the type. Any assignment of a different type will raise an error:

  ```dharma
  var y: int = 100
  y = "nah"  // ❌ Invalid
  ```

---

### Block Scoping with Shadowing

* Inner blocks can redefine (`shadow`) outer variables temporarily:

  ```dharma
  var a = 10
  {
      var a = 50
      print a // prints 50
  }
  print a // prints 10
  ```

---

### Semicolon Optionality

* You can use `;` or ignore it. Both styles are valid:

  ```dharma
  print 42;
  print 42
  ```

---

### Built-in Operations

* Arithmetic: `+`, `-`, `*`, `/`
* Logical: `==` supports type-safe equality comparison
* String concatenation using `+`
* Dynamic output using `print`

---

## 🛣️ Planned Features

* Cross-platform bytecode execution (`Samsara`)
* REPL mode (`Mantra`) for interactive scripting
* Package manager (`Ashram`) to share & manage Dharma modules
* Formatter (`Bhava`) and linter (`Yama`) for code discipline
* Documentation generator (`Veda`) from comments
* Debugger (`Drishti`) and profiler (`Tapa`) for deep introspection
* Build system (`Agni`) for project-level scripting automation

---

## 🧪 Sample Dharma Script (`test.dh`)

```dharma
var a = 100;
var b : int = 200;
print a + b;
{
    var a = 200;
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
var c : boolean = true;
var d : string = 'hello'; print a == c
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

You’ll get the `sutra` executable inside the `build` directory.

### Run a Dharma Script

```bash
./sutra ../src/test.dh    # On Unix
.\sutra.exe ..\src\test.dh # On Windows
```

*“May your logic be clean. May your runtime be ruthless. May your bugs face Yama.”*
