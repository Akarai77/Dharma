# 🌌 Dharma

> **Enlightenment through execution.**

**Dharma** is a dynamically typed, minimalist scripting language focused on clean syntax, expressive logic, and spiritual precision.

Currently under development — with the core interpreter (**Sutra**) taking its first breath.
Everything else is in the void, waiting to manifest.

---

# Sutra — The Dharma Interpreter

**Sutra** is the spiritual real-time execution engine of Dharma — a **tree-walk interpreter** that evaluates Dharma scripts directly from their parsed syntax trees.

It supports:

- Dynamic and static typing (`var` and typed declarations)
- Block scoping with shadowing
- Functions with parameters and return values
- Classes with inheritance and methods
- Expression evaluation (arithmetic, comparisons, concatenation)

---

## Language Examples

### Variables & Types

```dharma
var x = 42
int y = 100
var z: string = "hello"
````

### Functions

```dharma
fun greet(name) {
    print "Hello, " + name + "!"
}

greet("Pothel")  // Hello, Pothel!
```

### Classes & Inheritance

```dharma
class Animal {
    fun speak() {
        print "Animal sound"
    }
}

class Dog extends Animal {
    fun speak() {
        print "Woof!"
    }
}

var dog = Dog()
dog.speak()  // Woof!
```

---

## Getting Started

Build and run the interpreter:

```bash
mkdir build && cd build
cmake ..
cmake --build .
./sutra ../src/test.dh
```

---

## Sample Dharma Script (`test.dh`)

```dharma
var a = 10

fun factorial(n) {
    if (n <= 1) return 1
    return n * factorial(n - 1)
}

print factorial(a)  // 3628800

class Counter {
    var count = 0

    fun increment() {
        count = count + 1
        print count
    }
}

var c = Counter()
c.increment()  // 1
c.increment()  // 2
```

---

*May your logic be clean. May your runtime be ruthless. May your bugs face Yama.*
